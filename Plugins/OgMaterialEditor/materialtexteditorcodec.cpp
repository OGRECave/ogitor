/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2012 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
//
/// The MIT License
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////*/

#include "materialtexteditorcodec.hxx"
#include "generictexteditor.hxx"
#include "materialhighlighter.hxx"
#include "materialeditorprefseditor.hxx"

#include "OgitorsRoot.h"
#include "OgreTechnique.h"

#include <QtGui/QColorDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QApplication>
#include <QtCore/QObject>
#include <QtCore/QSettings>

//-----------------------------------------------------------------------------------------
enum ColorType
{
    NONE = 0,
    AMBIENT,
    DIFFUSE,
    SPECULAR,
    EMISSIVE
};
//-----------------------------------------------------------------------------------------

MaterialTextEditorCodecToolBar* MaterialTextEditorCodec::mToolBar   = 0;
QStringListModel* MaterialTextEditorCodec::mKeyHighlightList        = 0;
QStringListModel* MaterialTextEditorCodec::mEnumHighlightList       = 0;
QStringListModel* MaterialTextEditorCodec::mDataTypeHighlightList   = 0;
QStringListModel* MaterialTextEditorCodec::mCombinedHighlightList   = 0;

//-----------------------------------------------------------------------------------------
MaterialTextEditorCodecToolBar::MaterialTextEditorCodecToolBar(const QString& name) : QToolBar(name), mActRefresh(0)
{
    mActRefresh = new QAction("Refresh Material", this);
    mActRefresh->setStatusTip("Refresh Material");
    mActRefresh->setIcon(QIcon(":/icons/refresh.svg"));
    mActRefresh->setEnabled(true);
    connect(mActRefresh, SIGNAL(triggered()), this, SLOT(onRefresh()));

    setObjectName("renderwindowtoolbar");
    setIconSize(QSize(20,20));
    setToolButtonStyle(Qt::ToolButtonIconOnly);
    addAction(mActRefresh);
}
//-----------------------------------------------------------------------------------------
MaterialTextEditorCodecToolBar::~MaterialTextEditorCodecToolBar()
{
}
//-----------------------------------------------------------------------------------------
void MaterialTextEditorCodecToolBar::onRefresh()
{
    GenericTextEditorDocument* document = GenericTextEditor::getSingletonPtr()->getActiveDocument();
    if(document != 0)
    {
        static_cast<MaterialTextEditorCodec*>(document->getCodec())->onRefresh();
    }
}
//-----------------------------------------------------------------------------------------
MaterialTextEditorCodec::MaterialTextEditorCodec(GenericTextEditorDocument* genTexEdDoc, QString docName, QString documentIcon) : 
ITextEditorCodec(genTexEdDoc, docName, documentIcon)
{
    Ogre::ScriptCompilerManager::getSingletonPtr()->setListener(this);

    mScriptError = false;

    QSettings settings;
    settings.beginGroup(mMaterialPrefsEditor->getPrefsSectionName().c_str());

    if(settings.value("lineWrapping", false).toBool() == false)
        mGenTexEdDoc->setLineWrapMode(QPlainTextEdit::NoWrap);
    else
        mGenTexEdDoc->setLineWrapMode(QPlainTextEdit::WidgetWidth);

    settings.endGroup();
}
//-----------------------------------------------------------------------------------------
QString MaterialTextEditorCodec::onBeforeDisplay(QString text)
{
    return text;
}
//-----------------------------------------------------------------------------------------
void MaterialTextEditorCodec::onContextMenu(QContextMenuEvent* event)
{
    QTextCursor cursor = mGenTexEdDoc->textCursor();
    cursor.select(QTextCursor::LineUnderCursor);
    mGenTexEdDoc->setTextCursor(cursor);

    QString lineText = mGenTexEdDoc->textCursor().selectedText();
    int lineNumber = mGenTexEdDoc->textCursor().blockNumber();

    cursor = mGenTexEdDoc->textCursor();
    cursor.setPosition(0);
    cursor.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor, lineNumber);
    int startPos = cursor.position();
    
    QString materialName;

    // There can be multiple materials within one material file, so we
    // need to figure out in which material the context menu event occurred.
    QTextCursor tc = mGenTexEdDoc->textCursor();
    int savePos = tc.position();
    tc.movePosition(QTextCursor::Start, QTextCursor::KeepAnchor);
    QString tmpStr = tc.selectedText();

    // Cut to content from the material, the click occurred in and cut "material" keyword
    int pos =  tmpStr.lastIndexOf("material");
    tmpStr = tmpStr.mid(pos + 9);               // 8 for "material" + 1 for blank after the keyword 

    // Cut to content in material name line
    pos = tmpStr.indexOf("{");
    tmpStr = tmpStr.left(pos); 

    // There might be a ":" from material inheritance...get rid of it
    pos = tmpStr.indexOf(":");
    tmpStr = tmpStr.left(pos);
    materialName = tmpStr;
    
    // Offer color dialog help for the light terms
    int colourType = NONE;
    int index;
    QRegExp expression;
    expression.setPattern("ambient[^_](.+)$");
    index = lineText.indexOf(expression);
    if(index >= 0)
        colourType = AMBIENT;
    else
    {
        expression.setPattern("diffuse[^_](.+)$");
        index = lineText.indexOf(expression);
        if(index >= 0)
            colourType = DIFFUSE;
        else
        {
            expression.setPattern("specular[^_](.+)$");
            index = lineText.indexOf(expression);
            if(index >= 0)
                colourType = SPECULAR;
            else
            {
                expression.setPattern("emissive[^_](.+)$");
                index = lineText.indexOf(expression);
                if(index >= 0)
                    colourType = EMISSIVE;
            }
        }
    }
        
    if(colourType != NONE)
    {         
        int length = expression.matchedLength();
        cursor = mGenTexEdDoc->textCursor();
        cursor.setPosition(startPos + index + length - expression.cap(1).length());
        cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, expression.cap(1).length());
        mGenTexEdDoc->setTextCursor(cursor);

        QColor oldColour;
        Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingletonPtr()->getByName(materialName.toStdString());
        
        // If null then the material was probably not made a scene asset yet.
        if(!mat.isNull())
        {
            Ogre::ColourValue colour;
            if(colourType == AMBIENT)
                colour = mat->getBestTechnique(0)->getPass(0)->getAmbient();
            else if(colourType == DIFFUSE)
                colour = mat->getBestTechnique(0)->getPass(0)->getDiffuse();
            else if(colourType == SPECULAR)
                colour = mat->getBestTechnique(0)->getPass(0)->getSpecular();
            else if(colourType == EMISSIVE)
                colour = mat->getBestTechnique(0)->getPass(0)->getSelfIllumination();

            oldColour = QColor(255 * colour.r, 255 * colour.g, 255 * colour.b, 255 * colour.a);
        }
        else
            oldColour = QColor();

        QColor newColour = QColorDialog::getColor(oldColour, mGenTexEdDoc);

        // If the user pressed cancel, use our previous colour
        if(!newColour.isValid())
            return;

        cursor.removeSelectedText();
        char temp[128];
        sprintf(temp, "%.4f %.4f %.4f %.4f", newColour.redF(), newColour.greenF(), newColour.blueF(), newColour.alphaF());
        cursor.insertText(QString(temp));
        
        if(!mat.isNull())
        {
            if(colourType == AMBIENT)
                mat->getTechnique(0)->getPass(0)->setAmbient(Ogre::ColourValue(newColour.redF(), newColour.greenF(), newColour.blueF(), newColour.alphaF()));
            else if(colourType == DIFFUSE)
                mat->getTechnique(0)->getPass(0)->setDiffuse(Ogre::ColourValue(newColour.redF(), newColour.greenF(), newColour.blueF(), newColour.alphaF()));
            else if(colourType == SPECULAR)
                mat->getTechnique(0)->getPass(0)->setSpecular(Ogre::ColourValue(newColour.redF(), newColour.greenF(), newColour.blueF(), newColour.alphaF()));
            else if(colourType == EMISSIVE)
                mat->getTechnique(0)->getPass(0)->setSelfIllumination(Ogre::ColourValue(newColour.redF(), newColour.greenF(), newColour.blueF(), newColour.alphaF()));
        }

        return;
    }

    expression = QRegExp("texture[^_]([a-zA-Z0-9\\.\\-\\_]+)\\s*([a-zA-Z0-9-]*)");
    index = lineText.indexOf(expression);
    if(index >= 0)
    {
        int length = expression.matchedLength();
        cursor = mGenTexEdDoc->textCursor();
        cursor.setPosition(startPos + index + length - expression.cap(1).length());
        cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, expression.cap(1).length());
        mGenTexEdDoc->setTextCursor(cursor);
        return;
    }
}
//-----------------------------------------------------------------------------------------
QString MaterialTextEditorCodec::getMaterialText(const Ogre::String& input, const Ogre::String& find)
{
    Ogre::String buffer = input;
    char *curpos = const_cast<char *>(buffer.c_str());
    char *start = curpos;
    char *end = curpos;
    const char *curposend = curpos + buffer.size();

    // Fill all comment lines with SPACE so they don't interfere with our search, example: //material X
    while(curpos < curposend)
    {
        if(curpos[0] == '/' && (curpos + 1) < curposend && curpos[1] == '/')
        {
            while(curpos < curposend && curpos[0] != '\n')
            {
                *curpos = ' ';
                curpos++;
            }
        }
        curpos++;
    }

    int ps = buffer.find("material " + find);
    if(ps != -1)
    {
        start += ps;
        end += ps;

        curpos = start;

        while(curpos < curposend && curpos[0] != '{')
            curpos++;

        if((curpos + 1) >= curposend)
            return "";

        curpos++;
        int brackets = 1;

        while(brackets && (curpos < curposend))
        {
            if(curpos[0] == '{')
                ++brackets;
            else if(curpos[0] == '}')
                --brackets;
            curpos++;
        }

        if(curpos > curposend)
            return "";

        end = curpos;
        Ogre::String res = input.substr(start - buffer.c_str(), end - start);
        return res.c_str();
    }
    else
        return "";
}
//-----------------------------------------------------------------------------------------
void MaterialTextEditorCodec::onAddHighlighter()
{
    new MaterialHighlighter(MaterialTextEditorCodec::mKeyHighlightList,
                            MaterialTextEditorCodec::mEnumHighlightList,
                            MaterialTextEditorCodec::mDataTypeHighlightList, 
                            mGenTexEdDoc->document());
}
//-----------------------------------------------------------------------------------------
void MaterialTextEditorCodec::handleError(Ogre::ScriptCompiler *compiler, Ogre::uint32 code, const Ogre::String &file, int line, const Ogre::String &msg)
{
    mScriptError = true;
    QList<QTextEdit::ExtraSelection> extraSelections;
    QTextEdit::ExtraSelection selection;
    int currentLine = mGenTexEdDoc->textCursor().blockNumber() + 1;
    QTextCursor cursor = mGenTexEdDoc->textCursor();
    int lineDiff = line - currentLine;
    if(lineDiff > 0)
        cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, lineDiff);
    else if(lineDiff < 0)
        cursor.movePosition(QTextCursor::Up, QTextCursor::MoveAnchor, -lineDiff);
    mGenTexEdDoc->setTextCursor(cursor);

    QColor lineColor = QColor(Qt::red).lighter(160);
    selection.format.setBackground(lineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = mGenTexEdDoc->textCursor();
    selection.cursor.clearSelection();
    extraSelections.append(selection);
    mGenTexEdDoc->setExtraSelections(extraSelections);

    QString message = QString("Script Compiler error: %1\nLine: %2").arg(msg.c_str()).arg(line);
    QMessageBox::information(QApplication::activeWindow(), "qtOgitor", QObject::tr(message.toAscii()));
}
//-----------------------------------------------------------------------------------------
void MaterialTextEditorCodec::onKeyPressEvent(QKeyEvent *event)
{
    if((event->key() == Qt::Key_F5 || event->key() == Qt::Key_F6) && event->modifiers() == Qt::AltModifier)
    {
        mGenTexEdDoc->save();

        if(event->key() == Qt::Key_F5)
        {
            QStringList materialList = findMaterialNames();

            // Check if we are dealing with actual loaded Ogre materials here. If we previously found out
            // that there is a script error proceed anyway.
            Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingletonPtr()->getByName(materialList[0].toStdString());
            if(mat.isNull() && !mScriptError)
            {
                QMessageBox::information(QApplication::activeWindow(), "qtOgitor", QObject::tr("This material file seems to not be an active Ogre material. Please make sure it is declared as a scene asset and properly loaded."));      
                return;
            }

            reloadMaterials(materialList);

            for(int i = 0; i < materialList.size(); i++)
                Ogitors::OgitorsRoot::getSingletonPtr()->UpdateMaterialInScene(materialList[i].toStdString());
        }
    }
}
//-----------------------------------------------------------------------------------------
void MaterialTextEditorCodec::onRefresh()
{
    mGenTexEdDoc->save();

    QStringList materialList = findMaterialNames();

    Ogre::HighLevelGpuProgramManager::getSingletonPtr();
    Ogre::ScriptCompilerManager::getSingletonPtr();

    // Check if we are dealing with actual loaded Ogre materials here. If we previously found out
    // that there is a script error proceed anyway.
    Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingletonPtr()->getByName(materialList[0].toStdString());
    if(mat.isNull() && !mScriptError)
    {
        QMessageBox::information(QApplication::activeWindow(), "qtOgitor", QObject::tr("This material file seems to not be an active Ogre material. Please make sure it is declared as a scene asset and properly loaded."));      
        return;
    }

    reloadMaterials(materialList);

    for(int i = 0; i < materialList.size(); i++)
        Ogitors::OgitorsRoot::getSingletonPtr()->UpdateMaterialInScene(materialList[i].toStdString());
}
//-----------------------------------------------------------------------------------------
void MaterialTextEditorCodec::onAddCompleter()
{
    mGenTexEdDoc->addCompleter(MaterialTextEditorCodec::mCombinedHighlightList->stringList());
}
//-----------------------------------------------------------------------------------------
void MaterialTextEditorCodec::onAfterDisplay()
{
    // If the optionalData field is filled, we interpret it as the material that is to be shown,
    // so we move the cursor to its first line and ensure that it is visible.
    if(!mOptionalData.isEmpty())
    {
        QString mat = mGenTexEdDoc->toPlainText();
        int pos = mat.indexOf(QRegExp("\\s" + mOptionalData + "\\s"));

        if(pos > 0)
        {
            QTextCursor tc = mGenTexEdDoc->textCursor();
            tc.setPosition(pos);
            mGenTexEdDoc->setTextCursor(tc);
            mGenTexEdDoc->ensureCursorVisible();
        }
    }
}
//-----------------------------------------------------------------------------------------
void MaterialTextEditorCodec::onDisplayRequest()
{
    onAfterDisplay();
}
//-----------------------------------------------------------------------------------------
QStringList MaterialTextEditorCodec::findMaterialNames()
{
    QRegExp reg("material\\s+([a-zA-Z.0-9/_]+)\\s");
    int pos = 0;
    QStringList result;

    while((pos = reg.indexIn(mGenTexEdDoc->toPlainText(), pos)) != -1) 
    {
        result << reg.cap(1);
        pos += reg.matchedLength();
    }

    return result;
}
//-----------------------------------------------------------------------------------------
void MaterialTextEditorCodec::reloadMaterials(QStringList materialList)
{
    for(int i = 0; i < materialList.size(); i++)
    {
        QString matName = materialList[i];
        Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingletonPtr()->getByName(matName.toStdString());
        mat->unload();
        Ogre::MaterialManager::getSingletonPtr()->remove(matName.toStdString());

        Ogre::FileInfoListPtr fileInfoList;
        fileInfoList = Ogre::ResourceGroupManager::getSingletonPtr()->findResourceFileInfo(mat->getGroup(), mat->getOrigin());
        Ogre::DataStreamPtr stream = fileInfoList.getPointer()->at(0).archive->open(mat->getOrigin());

        if(!stream.isNull())
        {	
            mScriptError = false;
            Ogre::MaterialManager::getSingleton().parseScript(stream, mat->getGroup());

            if(!mScriptError)
            {
                Ogre::MaterialPtr reloadedMaterial = Ogre::MaterialManager::getSingleton().getByName(matName.toStdString());
                if(!reloadedMaterial.isNull())
                {
                    reloadedMaterial->compile();
                    reloadedMaterial->load();
                }

                stream->close();
            }
        }
    }
}
//-----------------------------------------------------------------------------------------
ITextEditorCodec* MaterialTextEditorCodecFactory::create(GenericTextEditorDocument* genTexEdDoc, QString docName)
{
    return new MaterialTextEditorCodec(genTexEdDoc, docName, ":/icons/material.svg");
}
//-----------------------------------------------------------------------------------------
