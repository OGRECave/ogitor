/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2011 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
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

#include "OgreTechnique.h"

#include <QtGui/QColorDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QApplication>
#include <QtCore/QObject>

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
MaterialTextEditorCodec::MaterialTextEditorCodec(QPlainTextEdit* textEdit, QString docName, QString documentIcon) : 
ITextEditorCodec(textEdit, docName, documentIcon)
{
    int pos = docName.lastIndexOf(".material");

    if(pos != -1)
        mMaterialName = docName.left(pos);
    else
        mMaterialName = docName;

    //    QSettings settings;
    //    settings.beginGroup(mMaterialPrefsEditor->getPrefsSectionName().c_str());
    //
    //    if(settings.value("lineWrapping", false).toBool() == false)
    //        setLineWrapMode(QPlainTextEdit::NoWrap);
    //    else
    //        setLineWrapMode(QPlainTextEdit::WidgetWidth);
    //
    //    settings.endGroup();
}
//-----------------------------------------------------------------------------------------
QString MaterialTextEditorCodec::prepareForDisplay(QString docName, QString text)
{
    Ogre::ScriptCompilerManager::getSingletonPtr()->setListener(this);
    
    return text;
}
//-----------------------------------------------------------------------------------------
bool MaterialTextEditorCodec::save()
{
    //    //bool reLoad = false;
    //
    //    //Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(mFilePath.toStdString(), mResourceGroup.toStdString());				
    //    //QString contents = stream->getAsString().c_str();
    //    //stream.setNull();
    //
    //    //int pos = contents.indexOf(mLastMaterialSource);
    //    //contents.erase(pos, mLastMaterialSource.size());
    //    //contents.insert(pos, document()->toPlainText().toAscii());
    //
    //    //Ogre::Material* material = static_cast<Ogre::Material*>(mMaterialViewWidget->getMaterialEditor()->getHandle());
    //
    //    //Ogre::FileInfoListPtr fileInfoList;
    //    //QString matOrigin = material->getOrigin();
    //
    //    //try
    //    //{
    //    //    fileInfoList = Ogre::ResourceGroupManager::getSingletonPtr()->findResourceFileInfo(material->getGroup(), material->getOrigin());
    //
    //    //    stream = fileInfoList.getPointer()->at(0).archive->create(matOrigin);
    //
    //    //    if(!stream.isNull())
    //    //    {
    //    //        int content_size = contents.length();
    //    //        stream->write(contents.c_str(), content_size);
    //    //        stream->close();
    //    //        reLoad = true;
    //    //    }
    //    //}
    //    //catch(...)
    //    //{
    //    //}
    //
    //
    //    //if(reLoad)
    //    //{
    //    //    QString matName = material->getName();
    //    //    material->unload();
    //    //    Ogre::MaterialManager::getSingletonPtr()->remove(matName);
    //
    //    //    stream = fileInfoList.getPointer()->at(0).archive->open(matOrigin);
    //
    //    //    if(!stream.isNull())
    //    //    {	
    //    //        mScriptError = false;
    //    //        Ogre::MaterialManager::getSingleton().parseScript(stream, material->getGroup());
    //
    //    //        if(!mScriptError)
    //    //        {
    //    //            Ogre::MaterialPtr reloadedMaterial = Ogre::MaterialManager::getSingleton().getByName(matName);
    //    //            if (!reloadedMaterial.isNull())
    //    //            {
    //    //                reloadedMaterial->compile();
    //    //                reloadedMaterial->load();
    //    //                mMaterialViewWidget->getMaterialEditor()->setHandle(reloadedMaterial);
    //    //            }
    //
    //    //            stream->close();
    //    //            setTextModified(false);
    //    //            setWindowModified(false);
    //    //            //return matName;
    //    //        }
    //    //    }
    //    //}
    //    //else
    //    //{
    //    //    QMessageBox::information(QApplication::activeWindow(),"qtOgitor", tr("Error saving material script"));
    //    //}
    //
    //    ////return QString("");
    //
    //    return true;
    return true;
}
//-----------------------------------------------------------------------------------------
void MaterialTextEditorCodec::contextMenu(QContextMenuEvent* event)
{
    QTextCursor cursor = mTextEdit->textCursor();
    cursor.select(QTextCursor::LineUnderCursor);
    mTextEdit->setTextCursor(cursor);

    if(mTextEdit->textCursor().hasSelection())
    {
        QString lineText = mTextEdit->textCursor().selectedText();
        int lineNumber = mTextEdit->textCursor().blockNumber();

        cursor = mTextEdit->textCursor();
        cursor.setPosition(0);
        cursor.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor, lineNumber);
        int startPos = cursor.position();

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
            cursor = mTextEdit->textCursor();
            cursor.setPosition(startPos + index + length - expression.cap(1).length());
            cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, expression.cap(1).length());
            mTextEdit->setTextCursor(cursor);

            Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingletonPtr()->getByName(mMaterialName.toStdString());
            
            Ogre::ColourValue colour;
            if(colourType == AMBIENT)
                colour = mat->getBestTechnique(0)->getPass(0)->getAmbient();
            else if(colourType == DIFFUSE)
                colour = mat->getBestTechnique(0)->getPass(0)->getDiffuse();
            else if(colourType == SPECULAR)
                colour = mat->getBestTechnique(0)->getPass(0)->getSpecular();
            else if(colourType == EMISSIVE)
                colour = mat->getBestTechnique(0)->getPass(0)->getSelfIllumination();

            QColor oldColour = QColor(255 * colour.r, 255 * colour.g, 255 * colour.b, 255 * colour.a);
            QColor newColour = QColorDialog::getColor(oldColour, mTextEdit);

            // If the user pressed cancel, use our previous colour
            if(!newColour.isValid())
                return;

            cursor.removeSelectedText();
            char temp[128];
            sprintf(temp, "%.4f %.4f %.4f %.4f", newColour.redF(), newColour.greenF(), newColour.blueF(), newColour.alphaF());
            cursor.insertText(QString(temp));
            if(colourType == AMBIENT)
                mat->getTechnique(0)->getPass(0)->setAmbient(Ogre::ColourValue(newColour.redF(), newColour.greenF(), newColour.blueF(), newColour.alphaF()));
            else if(colourType == DIFFUSE)
                mat->getTechnique(0)->getPass(0)->setDiffuse(Ogre::ColourValue(newColour.redF(), newColour.greenF(), newColour.blueF(), newColour.alphaF()));
            else if(colourType == SPECULAR)
                mat->getTechnique(0)->getPass(0)->setSpecular(Ogre::ColourValue(newColour.redF(), newColour.greenF(), newColour.blueF(), newColour.alphaF()));
            else if(colourType == EMISSIVE)
                mat->getTechnique(0)->getPass(0)->setSelfIllumination(Ogre::ColourValue(newColour.redF(), newColour.greenF(), newColour.blueF(), newColour.alphaF()));

            return;
        }

        expression = QRegExp("texture[^_]([a-zA-Z0-9\\.\\-\\_]+)\\s*([a-zA-Z0-9-]*)");
        index = lineText.indexOf(expression);
        if(index >= 0)
        {
            int length = expression.matchedLength();
            cursor = mTextEdit->textCursor();
            cursor.setPosition(startPos + index + length - expression.cap(1).length());
            cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, expression.cap(1).length());
            mTextEdit->setTextCursor(cursor);
            return;
        }
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
void MaterialTextEditorCodec::addHighlighter(GenericTextEditorDocument* document)
{
    new MaterialHighlighter(GenericTextEditor::getSingletonPtr()->modelFromFile(":/syntax_highlighting/material.txt"), document->document());
}
//-----------------------------------------------------------------------------------------
void MaterialTextEditorCodec::handleError(Ogre::ScriptCompiler *compiler, Ogre::uint32 code, const QString &file, int line, const QString &msg)
{
    mScriptError = true;
    QList<QTextEdit::ExtraSelection> extraSelections;
    QTextEdit::ExtraSelection selection;
    int currentLine = mTextEdit->textCursor().blockNumber() + 1;
    QTextCursor cursor = mTextEdit->textCursor();
    int lineDiff = line - currentLine;
    if(lineDiff > 0)
        cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, lineDiff);
    else if(lineDiff < 0)
        cursor.movePosition(QTextCursor::Up, QTextCursor::MoveAnchor, lineDiff);
    mTextEdit->setTextCursor(cursor);

    QColor lineColor = QColor(Qt::red).lighter(160);
    selection.format.setBackground(lineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = mTextEdit->textCursor();
    selection.cursor.clearSelection();
    extraSelections.append(selection);
    mTextEdit->setExtraSelections(extraSelections);

    QString message = QString("Script Compiler error: %1\nLine: %2").arg(msg).arg(line);
    QMessageBox::information(QApplication::activeWindow(), "qtOgitor", QObject::tr(message.toAscii()));
}
//-----------------------------------------------------------------------------------------
void MaterialTextEditorCodec::keyPressEvent( QKeyEvent *event )
{
    //    if((event->key() == Qt::Key_F5 || event->key() == Qt::Key_F6) && event->modifiers() == Qt::AltModifier)
    //    {
    //        //QString newMaterialName = saveMaterial();
    //
    //        //if(event->key() == Qt::Key_F5)
    //            //OgitorsRoot::getSingletonPtr()->UpdateMaterialInScene(newMaterialName);
    //    }
    //
    //    if(mCompleter && mCompleter->popup()->isVisible()) 
    //    {
    //        switch (event->key()) 
    //        {
    //        case Qt::Key_Enter:
    //        case Qt::Key_Return:
    //        case Qt::Key_Escape:
    //        case Qt::Key_Tab:
    //        case Qt::Key_Backtab:
    //            event->ignore();
    //            return;
    //        default:
    //            break;
    //        }
    //    }
    //
    //    // Auto start next line with the indentation of previous line...
    //    if(event->key() == Qt::Key_Return)
    //    {
    //        QTextCursor tc = textCursor();
    //        int savePos = tc.position();
    //        //Get Previous bracket position
    //        tc.movePosition(QTextCursor::Start, QTextCursor::KeepAnchor);
    //        QString tmpStr = tc.selectedText();
    //
    //        int count = calculateIndentation(tmpStr);
    //
    //        tc.setPosition(savePos);
    //        QString txt = "\n";
    //        for(int z = 0;z < count;z++)
    //            txt += " ";
    //        tc.insertText(txt);
    //        tc.setPosition(savePos + count + 1);
    //        setTextCursor(tc);
    //        event->accept();
    //        return;
    //    }
    //
    //    //Insert 4 spaces instead of tab
    //    if(event->key() == Qt::Key_Tab)
    //    {
    //        QTextCursor tc = textCursor();
    //        int savePos = tc.position();
    //        QString txt = "    ";
    //        tc.insertText(txt);
    //        tc.setPosition(savePos + 4);
    //        setTextCursor(tc);
    //        event->accept();
    //        return;
    //    }
    //
    //    bool isShortcut = ((event->modifiers() & Qt::ControlModifier) && event->key() == Qt::Key_Space); 
    //    if(!mCompleter || !isShortcut) 
    //        QPlainTextEdit::keyPressEvent(event);
    //
    //    const bool ctrlOrShift = event->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
    //    if(!mCompleter || (ctrlOrShift && event->text().isEmpty()))
    //        return;
    //
    //    static QString eow("~!@#$%^&*()+{}|:\"<>?,./;'[]\\-= "); // end of word
    //    bool hasModifier = (event->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    //    QString completionPrefix = textUnderCursor();
    //
    //    if(!isShortcut && (hasModifier || event->text().isEmpty() || completionPrefix.length() < 1
    //        || eow.contains(event->text().right(1))))
    //    {
    //        mCompleter->popup()->hide();
    //        return;
    //    }
    //
    //    if(completionPrefix != mCompleter->completionPrefix()) 
    //    {
    //        mCompleter->setCompletionPrefix(completionPrefix);
    //        mCompleter->popup()->setCurrentIndex(mCompleter->completionModel()->index(0, 0));
    //    }
    //    QRect cr = cursorRect();
    //    cr.setWidth(mCompleter->popup()->sizeHintForColumn(0) + mCompleter->popup()->verticalScrollBar()->sizeHint().width());
    //    mCompleter->complete(cr);
}
//-----------------------------------------------------------------------------------------
ITextEditorCodec* MaterialTextEditorCodecFactory::create(QPlainTextEdit* textEdit, QString docName)
{
    return new MaterialTextEditorCodec(textEdit, docName, ":/icons/material.svg");
}
//-----------------------------------------------------------------------------------------