/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (mCompleter) 2008-2010 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
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

#include <QtGui/QtGui>
#include <QtGui/QColorDialog>
#include <QtGui/QMessageBox>
#include <QtSvg>

#include "OgitorsPrerequisites.h"
#include "BaseEditor.h"
#include "MaterialEditor.h"

#include "materialtexteditor.hxx"
#include "materialview.hxx"
#include "materialeditorprefseditor.hxx"

using namespace Ogitors;

MaterialTextEditor *mMaterialTextEditor = 0;
//-----------------------------------------------------------------------------------------
Ogre::String getMaterialText(const Ogre::String& input, const Ogre::String& find)
{
    Ogre::String buffer = input;

    char *curpos = const_cast<char *>(buffer.c_str());
    char *start = curpos;
    char *end = curpos;

    const char *curposend = curpos + buffer.size();

    // Fill all comment lines with SPACE so they dont interfere with our search, example: //material X
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

        return input.substr(start - buffer.c_str(), end - start);
    }
    else
        return "";
}
//-----------------------------------------------------------------------------------------
MaterialTextEditor::MaterialTextEditor(QWidget *parent) : QMdiArea(parent)
{
    setObjectName("MaterialTextEditor");
    setViewMode(QMdiArea::TabbedView);

    QTabBar* tabBar = findChildren<QTabBar*>().at(0);
    tabBar->setTabsClosable(true);
    connect(tabBar, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    connect(tabBar, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
    connect(this, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
}
//-----------------------------------------------------------------------------------------
void MaterialTextEditor::displayMaterialFromFile(Ogre::String fileName)
{
    bool alreadyShowing = false;
    MaterialTextEditorDocument* document;

    foreach(QMdiSubWindow *window, subWindowList()) 
    {
        document = qobject_cast<MaterialTextEditorDocument*>(window->widget());
        if(document->getMaterialFileName() == fileName)
        {
            alreadyShowing = true;
            break;
        }
    }

    if(!alreadyShowing)
    {
        MaterialTextEditorDocument* document = new MaterialTextEditorDocument(this);
        document->displayMaterialFromFile(fileName);
        QMdiSubWindow *window = addSubWindow(document);
        window->setWindowIcon(QIcon(":/icons/material.svg"));
        document->showMaximized();
        QTabBar* tabBar = findChildren<QTabBar*>().at(0);
        tabBar->setTabToolTip(findChildren<QMdiSubWindow*>().size() - 1, OgitorsUtils::ExtractFileName(fileName).c_str());
    }
    else
    {
        setActiveSubWindow(qobject_cast<QMdiSubWindow*>(document->window()));
        document->setFocus(Qt::ActiveWindowFocusReason);
    }
}
//-----------------------------------------------------------------------------------------
void MaterialTextEditor::displayMaterial(Ogre::String materialName, Ogre::String resourceGroup, Ogre::String materialFileName)
{
    bool alreadyShowing = false;
    MaterialTextEditorDocument* document;

    foreach(QMdiSubWindow *window, subWindowList()) 
    {
        document = qobject_cast<MaterialTextEditorDocument*>(window->widget());
        if(document->getMaterialName() == materialName)
        {
            alreadyShowing = true;
            break;
        }
    }

    if(!alreadyShowing)
    {
        MaterialTextEditorDocument* document = new MaterialTextEditorDocument(this);
        document->displayMaterial(materialName, resourceGroup, materialFileName);
        QMdiSubWindow *window = addSubWindow(document);
        window->setWindowIcon(QIcon(":/icons/material.svg"));
        document->showMaximized();
        QTabBar* tabBar = findChildren<QTabBar*>().at(0);
        tabBar->setTabToolTip(findChildren<QMdiSubWindow*>().size() - 1, materialName.c_str());
    }
    else
    {
        setActiveSubWindow(qobject_cast<QMdiSubWindow*>(document->window()));
        document->setFocus(Qt::ActiveWindowFocusReason);
    }
}
//-----------------------------------------------------------------------------------------
void MaterialTextEditor::closeTab(int index)
{
    QMdiSubWindow *sub = subWindowList()[index];
    setActiveSubWindow(sub);
    MaterialTextEditorDocument* document = static_cast<MaterialTextEditorDocument*>(sub->widget());
    if(document->isTextModified())
    {	
        int result = QMessageBox::information(QApplication::activeWindow(),"qtOgitor", "Document has been modified. Should the changes be saved?", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        switch(result)
        {
        case QMessageBox::Yes:	document->saveMaterial(); break;
        case QMessageBox::No: break;
        case QMessageBox::Cancel: return;
        }
    }
    sub->close();
    document->close();
    
    emit currentChanged(subWindowList().indexOf(activeSubWindow()));
}
//-----------------------------------------------------------------------------------------
void MaterialTextEditor::tabChanged(int index)
{
    QTreeWidget *tree = mMaterialViewWidget->getTreeWidget();
    
    if(index == -1)
    {
        tree->selectionModel()->clearSelection();
    }
    else
    {
        QMdiSubWindow *wnd = subWindowList()[index];
        MaterialTextEditorDocument *document = static_cast<MaterialTextEditorDocument*>(wnd->widget());
        if(document)
        {
            QList<QTreeWidgetItem*> list = tree->findItems(document->getMaterialName().c_str(), Qt::MatchRecursive | Qt::MatchCaseSensitive);
            if(list.size())
            {
                tree->setCurrentItem(list[0]);
            }
        }
    }
}
//-----------------------------------------------------------------------------------------
void MaterialTextEditor::closeEvent(QCloseEvent *event)
{
    QList<QMdiSubWindow*> list = subWindowList();
    for(int i = 0; i < list.size(); i++)
        closeTab(i);
}
//-----------------------------------------------------------------------------------------
MaterialTextEditorDocument::MaterialTextEditorDocument(QWidget *parent) : QPlainTextEdit(parent), 
mCompleter(0), mHighlighter(0), mMaterialName(""), mMaterialFileName(""), mLastMaterialSource(""), 
mResourceGroup(""), mTextModified(false)
{
   QSettings settings;
   settings.beginGroup(mMaterialPrefsEditor->getPrefsSectionName().c_str());

    QFont fnt = font();
    fnt.setFamily("Courier New");
    fnt.setPointSize(settings.value("fontSize", 10).toUInt());
    setFont(fnt);
    
    if(settings.value("lineWrapping", false).toBool() == false)
        setLineWrapMode(QPlainTextEdit::NoWrap);
    else
        setLineWrapMode(QPlainTextEdit::WidgetWidth);

    settings.endGroup();

    QFontMetrics fm(fnt);
    setTabStopWidth(fm.width("abcd"));
    mLineNumberArea = new LineNumberArea(this);

    mHighlighter = new MaterialHighlighter(modelFromFile(":/syntax_highlightning/material.txt"), document());

    mCompleter = new QCompleter(modelFromFile(":/syntax_highlightning/material.txt")->stringList(), this);
    mCompleter->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    mCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    mCompleter->setCompletionMode(QCompleter::PopupCompletion);
    mCompleter->setWrapAround(false);
    mCompleter->setWidget(this);

    Ogre::ScriptCompilerManager::getSingletonPtr()->setListener(this);

    connect(mCompleter, SIGNAL(activated(const QString&)), this, SLOT(insertCompletion(const QString&)));
    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(const QRect &, int)), this, SLOT(updateLineNumberArea(const QRect &, int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}
//-----------------------------------------------------------------------------------------
void MaterialTextEditorDocument::displayMaterialFromFile(Ogre::String fileName)
{
    mMaterialFileName = fileName;
    QFile* pFile = new QFile(fileName.c_str());
    pFile->open(QIODevice::ReadOnly);
    setPlainText(pFile->readAll().data());

    Ogre::String tabTitle = OgitorsUtils::ExtractFileName(fileName);
    if(tabTitle.length() > 25)
        tabTitle = tabTitle.substr(0, 12) + "..." + tabTitle.substr(tabTitle.length() - 10, 10);
    setWindowTitle(tabTitle.c_str() + QString("[*]"));
    
    connect(this, SIGNAL(textChanged()), this, SLOT(documentWasModified()));
}
//-----------------------------------------------------------------------------------------
void MaterialTextEditorDocument::displayMaterial(Ogre::String materialName, Ogre::String resourceGroup, Ogre::String materialFileName)
{
    mResourceGroup = resourceGroup;
    mMaterialName = materialName;
    mMaterialFileName = materialFileName;
    Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(materialFileName, resourceGroup);                
    mLastMaterialSource = getMaterialText(stream->getAsString().c_str(), materialName);
    setPlainText(mLastMaterialSource.c_str());

    Ogre::String tabTitle = materialName;
    if(tabTitle.length() > 25)
        tabTitle = tabTitle.substr(0, 12) + "..." + tabTitle.substr(tabTitle.length() - 10, 10);
    setWindowTitle(tabTitle.c_str() + QString("[*]"));

    connect(this, SIGNAL(textChanged()), this, SLOT(documentWasModified()));
}
//-----------------------------------------------------------------------------------------
int MaterialTextEditorDocument::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while(max >= 10) 
    {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

    return space;
}
//-----------------------------------------------------------------------------------------
void MaterialTextEditorDocument::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}
//-----------------------------------------------------------------------------------------
void MaterialTextEditorDocument::updateLineNumberArea(const QRect &rect, int dy)
{
    if(dy)
        mLineNumberArea->scroll(0, dy);
    else
        mLineNumberArea->update(0, rect.y(), mLineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}
//-----------------------------------------------------------------------------------------
void MaterialTextEditorDocument::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    mLineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}
//-----------------------------------------------------------------------------------------
void MaterialTextEditorDocument::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if(!isReadOnly()) 
    {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::yellow).lighter(160);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}
//-----------------------------------------------------------------------------------------
void MaterialTextEditorDocument::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(mLineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while(block.isValid() && top <= event->rect().bottom()) 
    {
        if(block.isVisible() && bottom >= event->rect().top()) 
        {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, mLineNumberArea->width(), fontMetrics().height(),
                Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}
//-----------------------------------------------------------------------------------------
void MaterialTextEditorDocument::insertCompletion(const QString& completion)
{
    if(mCompleter->widget() != this)
        return;
    QTextCursor tc = textCursor();
    int extra = completion.length() - mCompleter->completionPrefix().length();
    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completion.right(extra));
    setTextCursor(tc);
}
//-----------------------------------------------------------------------------------------
QString MaterialTextEditorDocument::textUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}
//-----------------------------------------------------------------------------------------
void MaterialTextEditorDocument::focusInEvent(QFocusEvent *event)
{
    if(mCompleter)
        mCompleter->setWidget(this);
    QPlainTextEdit::focusInEvent(event);
}
//-----------------------------------------------------------------------------------------
QStringListModel* MaterialTextEditorDocument::modelFromFile(const QString& fileName)
{
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly))
        return new QStringListModel(mCompleter);

    QStringList words;
    while (!file.atEnd()) 
    {
        QByteArray line = file.readLine();
        if(!line.isEmpty())
            words << line.trimmed();
    }
    QMap<QString, QString> strMap;
    foreach(QString str, words) 
        strMap.insert(str.toLower(), str);
    return new QStringListModel(strMap.values(), mCompleter);
}
//-----------------------------------------------------------------------------------------
int calculateIndentation(const Ogre::String& str)
{
    int indent = 0;
    int str_len = str.length();
    const char *chr = str.c_str();

    for(int i = 0;i < str_len;i++)
    {
        if(chr[i] == '{')
            ++indent;
        else if(chr[i] == '}')
            --indent;
    }

    if(indent < 0)
        indent = 0;

    return (indent * 4);
}
//-----------------------------------------------------------------------------------------
void MaterialTextEditorDocument::keyPressEvent(QKeyEvent *event)
{
    if((event->key() == Qt::Key_F5 || event->key() == Qt::Key_F6) && event->modifiers() == Qt::AltModifier)
    {
        Ogre::String newMaterialName = saveMaterial();

        if(event->key() == Qt::Key_F5)
            OgitorsRoot::getSingletonPtr()->UpdateMaterialInScene(newMaterialName);
    }

    if(mCompleter && mCompleter->popup()->isVisible()) 
    {
        switch (event->key()) 
        {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
            event->ignore();
            return;
        default:
            break;
        }
    }

    //Auto start next line with the indetation of previous line...
    if(event->key() == Qt::Key_Return)
    {
        QTextCursor tc = textCursor();
        int savePos = tc.position();
        //Get Previous bracket position
        tc.movePosition(QTextCursor::Start, QTextCursor::KeepAnchor);
        Ogre::String tmpStr = tc.selectedText().toStdString();

        int count = calculateIndentation(tmpStr);

        tc.setPosition(savePos);
        QString txt = "\n";
        for(int z = 0;z < count;z++)
            txt += " ";
        tc.insertText(txt);
        tc.setPosition(savePos + count + 1);
        setTextCursor(tc);
        event->accept();
        return;
    }

    //Insert 4 spaces instead of tab
    if(event->key() == Qt::Key_Tab)
    {
        QTextCursor tc = textCursor();
        int savePos = tc.position();
        QString txt = "    ";
        tc.insertText(txt);
        tc.setPosition(savePos + 4);
        setTextCursor(tc);
        event->accept();
        return;
    }

    bool isShortcut = ((event->modifiers() & Qt::ControlModifier) && event->key() == Qt::Key_Space); 
    if(!mCompleter || !isShortcut) 
        QPlainTextEdit::keyPressEvent(event);

    const bool ctrlOrShift = event->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
    if(!mCompleter || (ctrlOrShift && event->text().isEmpty()))
        return;

    static QString eow("~!@#$%^&*()+{}|:\"<>?,./;'[]\\-= "); // end of word
    bool hasModifier = (event->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    QString completionPrefix = textUnderCursor();

    if(!isShortcut && (hasModifier || event->text().isEmpty() || completionPrefix.length() < 1
        || eow.contains(event->text().right(1))))
    {
        mCompleter->popup()->hide();
        return;
    }

    if(completionPrefix != mCompleter->completionPrefix()) 
    {
        mCompleter->setCompletionPrefix(completionPrefix);
        mCompleter->popup()->setCurrentIndex(mCompleter->completionModel()->index(0, 0));
    }
    QRect cr = cursorRect();
    cr.setWidth(mCompleter->popup()->sizeHintForColumn(0) + mCompleter->popup()->verticalScrollBar()->sizeHint().width());
    mCompleter->complete(cr);
}
//-----------------------------------------------------------------------------------------
void MaterialTextEditorDocument::contextMenuEvent(QContextMenuEvent *event)
{
    QTextCursor cursor = textCursor();
    cursor.select(QTextCursor::LineUnderCursor);
    setTextCursor(cursor);

    if(textCursor().hasSelection())
    {
        QString lineText = textCursor().selectedText();
        int lineNumber = textCursor().blockNumber();

        cursor = textCursor();
        cursor.setPosition(0);
        cursor.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor, lineNumber);
        int startPos = cursor.position();

        // Offer color dialog help for the ambient light term
        QRegExp expression("ambient[^_](.+)$");
        int index = lineText.indexOf(expression);
        if(index >= 0)
        {
            int length = expression.matchedLength();
            cursor = textCursor();
            cursor.setPosition(startPos + index + length - expression.cap(1).length());
            cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, expression.cap(1).length());
            setTextCursor(cursor);

            Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingletonPtr()->getByName(mMaterialName);
            Ogre::ColourValue colour = mat->getBestTechnique(0)->getPass(0)->getAmbient();

            QColor oldColour = QColor(255 * colour.r, 255 * colour.g, 255 * colour.b, 255 * colour.a);
            QColor newColour = QColorDialog::getColor(oldColour, this);

            // If the user pressed cancel, use our previous colour
            if(!newColour.isValid())
                newColour = oldColour;

            cursor.removeSelectedText();
            char temp[128];
            sprintf(temp, "%.4f %.4f %.4f %.4f", newColour.redF(), newColour.greenF(), newColour.blueF(), newColour.alphaF());
            cursor.insertText(QString(temp));
            mat->getTechnique(0)->getPass(0)->setAmbient(Ogre::ColourValue(newColour.redF(), newColour.greenF(), newColour.blueF(), newColour.alphaF()));
            return;
        }

        // Offer color dialog help for the diffuse light term
        expression = QRegExp("diffuse[^_](.+)$");
        index = lineText.indexOf(expression);
        if(index >= 0)
        {
            int length = expression.matchedLength();
            cursor = textCursor();
            cursor.setPosition(startPos + index + length - expression.cap(1).length());
            cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, expression.cap(1).length());
            setTextCursor(cursor);

            Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingletonPtr()->getByName(mMaterialName);
            Ogre::ColourValue colour = mat->getBestTechnique(0)->getPass(0)->getDiffuse();

            QColor oldColour = QColor(255 * colour.r, 255 * colour.g, 255 * colour.b, 255 * colour.a);
            QColor newColour = QColorDialog::getColor(oldColour, this);

            // If the user pressed cancel, use our previous colour
            if(!newColour.isValid())
                newColour = oldColour;

            cursor.removeSelectedText();
            char temp[128];
            sprintf(temp, "%.4f %.4f %.4f %.4f", newColour.redF(), newColour.greenF(), newColour.blueF(), newColour.alphaF());
            cursor.insertText(QString(temp));
            mat->getTechnique(0)->getPass(0)->setDiffuse(Ogre::ColourValue(newColour.redF(), newColour.greenF(), newColour.blueF(), newColour.alphaF()));
            return;
        }

        // Offer color dialog help for the specular light term
        expression = QRegExp("specular[^_](.+)$");
        index = lineText.indexOf(expression);
        if(index >= 0)
        {
            int length = expression.matchedLength();
            cursor = textCursor();
            cursor.setPosition(startPos + index + length - expression.cap(1).length());
            cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, expression.cap(1).length());
            setTextCursor(cursor);

            Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingletonPtr()->getByName(mMaterialName);
            Ogre::ColourValue colour = mat->getBestTechnique(0)->getPass(0)->getSpecular();

            QColor oldColour = QColor(255 * colour.r, 255 * colour.g, 255 * colour.b, 255 * colour.a);
            QColor newColour = QColorDialog::getColor(oldColour, this);

            // If the user pressed cancel, use our previous colour
            if(!newColour.isValid())
                newColour = oldColour;

            cursor.removeSelectedText();
            char temp[128];
            sprintf(temp, "%.4f %.4f %.4f %.4f", newColour.redF(), newColour.greenF(), newColour.blueF(), newColour.alphaF());
            cursor.insertText(QString(temp));
            mat->getTechnique(0)->getPass(0)->setSpecular(Ogre::ColourValue(newColour.redF(), newColour.greenF(), newColour.blueF(), newColour.alphaF()));
            return;
        }

        // Offer color dialog help for the emissive light term   
        expression = QRegExp("emissive[^_](.+)$");
        index = lineText.indexOf(expression);
        if(index >= 0)
        {
            int length = expression.matchedLength();
            cursor = textCursor();
            cursor.setPosition(startPos + index + length - expression.cap(1).length());
            cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, expression.cap(1).length());
            setTextCursor(cursor);

            Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingletonPtr()->getByName(mMaterialName);
            Ogre::ColourValue colour = mat->getBestTechnique(0)->getPass(0)->getSelfIllumination();

            QColor oldColour = QColor(255 * colour.r, 255 * colour.g, 255 * colour.b, 255 * colour.a);
            QColor newColour = QColorDialog::getColor(oldColour, this);

            // If the user pressed cancel, use our previous colour
            if(!newColour.isValid())
                newColour = oldColour;

            cursor.removeSelectedText();
            char temp[128];
            sprintf(temp, "%.4f %.4f %.4f %.4f", newColour.redF(), newColour.greenF(), newColour.blueF(), newColour.alphaF());
            cursor.insertText(QString(temp));
            mat->getTechnique(0)->getPass(0)->setSelfIllumination(Ogre::ColourValue(newColour.redF(), newColour.greenF(), newColour.blueF(), newColour.alphaF()));
            return;
        }

        expression = QRegExp("texture[^_]([a-zA-Z0-9\\.\\-\\_]+)\\s*([a-zA-Z0-9-]*)");
        index = lineText.indexOf(expression);
        if(index >= 0)
        {
            int length = expression.matchedLength();
            cursor = textCursor();
            cursor.setPosition(startPos + index + length - expression.cap(1).length());
            cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, expression.cap(1).length());
            setTextCursor(cursor);
            return;
        }
    }
}
//-----------------------------------------------------------------------------------------
void MaterialTextEditorDocument::mousePressEvent(QMouseEvent *event)
{
    // Rewrite the mouse event to a left button event so the cursor is moved to the location of the pointer
    if(event->button() == Qt::RightButton)
        event = new QMouseEvent(QEvent::MouseButtonPress, event->pos(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QPlainTextEdit::mousePressEvent(event);
}
//-----------------------------------------------------------------------------------------
Ogre::String MaterialTextEditorDocument::saveMaterial()
{
    bool reLoad = false;

    Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(mMaterialFileName, mResourceGroup);				
    Ogre::String contents = stream->getAsString();
    stream.setNull();

    int pos = contents.find(mLastMaterialSource);
    contents.erase(pos, mLastMaterialSource.size());
    contents.insert(pos, document()->toPlainText().toAscii());

    Ogre::Material* material = static_cast<Ogre::Material*>(mMaterialViewWidget->getMaterialEditor()->getHandle());

    Ogre::FileInfoListPtr fileInfoList;
    Ogre::String matOrigin = material->getOrigin();

    try
    {
        fileInfoList = Ogre::ResourceGroupManager::getSingletonPtr()->findResourceFileInfo(material->getGroup(), material->getOrigin());

        stream = fileInfoList.getPointer()->at(0).archive->create(matOrigin);

        if(!stream.isNull())
        {
            int content_size = contents.length();
            stream->write(contents.c_str(), content_size);
            stream->close();
            reLoad = true;
        }
    }
    catch(...)
    {
    }


    if(reLoad)
    {
        Ogre::String matName = material->getName();
        material->unload();
        Ogre::MaterialManager::getSingletonPtr()->remove(matName);

        stream = fileInfoList.getPointer()->at(0).archive->open(matOrigin);

        if(!stream.isNull())
        {	
            mScriptError = false;
            Ogre::MaterialManager::getSingleton().parseScript(stream, material->getGroup());

            if(!mScriptError)
            {
                Ogre::MaterialPtr reloadedMaterial = Ogre::MaterialManager::getSingleton().getByName(matName);
                if (!reloadedMaterial.isNull())
                {
                    reloadedMaterial->compile();
                    reloadedMaterial->load();
                    mMaterialViewWidget->getMaterialEditor()->setHandle(reloadedMaterial);
                }

                stream->close();
                setTextModified(false);
                setWindowModified(false);
                return matName;
            }
        }
    }
    else
    {
        QMessageBox::information(QApplication::activeWindow(),"qtOgitor", tr("Error saving material script"));
    }

    return Ogre::String("");
}
//-----------------------------------------------------------------------------------------
void MaterialTextEditorDocument::handleError(Ogre::ScriptCompiler *compiler, Ogre::uint32 code, const Ogre::String &file, int line, const Ogre::String &msg)
{
    mScriptError = true;
    QList<QTextEdit::ExtraSelection> extraSelections;
    QTextEdit::ExtraSelection selection;
    int currentLine = textCursor().blockNumber() + 1;
    QTextCursor cursor = textCursor();
    int lineDiff = line - currentLine;
    if(lineDiff > 0)
        cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, lineDiff);
    else if(lineDiff < 0)
        cursor.movePosition(QTextCursor::Up, QTextCursor::MoveAnchor, lineDiff);
    setTextCursor(cursor);

    QColor lineColor = QColor(Qt::red).lighter(160);
    selection.format.setBackground(lineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = textCursor();
    selection.cursor.clearSelection();
    extraSelections.append(selection);
    setExtraSelections(extraSelections);

    QString message = QString("Script Compiler error: %1\nLine: %2").arg(QString::fromStdString(msg)).arg(line);
    QMessageBox::information(QApplication::activeWindow(),"qtOgitor", tr(message.toAscii()));
}
//-----------------------------------------------------------------------------------------
void MaterialTextEditorDocument::documentWasModified()
{
    setTextModified(true);
    setWindowModified(isTextModified());
}
//-----------------------------------------------------------------------------------------
