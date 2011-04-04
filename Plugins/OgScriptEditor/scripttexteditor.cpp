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
#include "OgitorsRoot.h"
#include "OgitorsScriptConsole.h"
#include "BaseEditor.h"
#include "scripttexteditor.hxx"
#include "scriptview.hxx"

using namespace Ogitors;

ScriptTextEditor *mScriptTextEditor = 0;
//-----------------------------------------------------------------------------------------
ScriptTextEditor::ScriptTextEditor(QWidget *parent) : QMdiArea(parent)
{
    setObjectName("ScriptTextEditor");
    setViewMode(QMdiArea::TabbedView);

    foreach(QTabBar* tab, findChildren<QTabBar*>())
    {
        tab->setTabsClosable(true);
        connect(tab, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
        connect(tab, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
    }
    connect(this, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
}
//-----------------------------------------------------------------------------------------
void ScriptTextEditor::displayScriptFromFile(Ogre::String scriptName, Ogre::String fileName)
{
    bool alreadyShowing = false;
    ScriptTextEditorDocument* document;

    foreach(QMdiSubWindow *window, subWindowList()) 
    {
        document = qobject_cast<ScriptTextEditorDocument*>(window->widget());
        if(document->getScriptFileName() == fileName)
        {
            alreadyShowing = true;
            break;
        }
    }

    if(!alreadyShowing)
    {
        ScriptTextEditorDocument* document = new ScriptTextEditorDocument(this);
        document->displayScriptFromFile(scriptName, fileName);
        QMdiSubWindow *window = addSubWindow(document);
        window->setToolTip(fileName.c_str());
        window->setWindowIcon(QIcon(":/icons/script.svg"));
        document->showMaximized();
        QTabBar* tabBar = findChildren<QTabBar*>().at(0);
        tabBar->setTabToolTip(findChildren<QMdiSubWindow*>().size() - 1, scriptName.c_str());
    }
    else
    {
        setActiveSubWindow(qobject_cast<QMdiSubWindow*>(document->window()));
        document->setFocus(Qt::ActiveWindowFocusReason);
    }
}
//-----------------------------------------------------------------------------------------
void ScriptTextEditor::closeTab(int index)
{
    QMdiSubWindow *sub = subWindowList()[index];
    setActiveSubWindow(sub);
    ScriptTextEditorDocument* document = static_cast<ScriptTextEditorDocument*>(sub->widget());
    if(document->isTextModified())
    {	
        int result = QMessageBox::information(QApplication::activeWindow(),"qtOgitor", "Document has been modified. Should the changes be saved?", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        switch(result)
        {
        case QMessageBox::Yes:	document->saveScript(); break;
        case QMessageBox::No: break;
        case QMessageBox::Cancel: return;
        }
    }
    sub->close();
    document->close();
    
    emit currentChanged(subWindowList().indexOf(activeSubWindow()));
}
//-----------------------------------------------------------------------------------------
void ScriptTextEditor::tabChanged(int index)
{
    QTreeWidget *tree = mScriptViewWidget->getTreeWidget();
    
    if(index == -1)
    {
        tree->selectionModel()->clearSelection();
    }
    else
    {
        QMdiSubWindow *wnd = subWindowList()[index];
        ScriptTextEditorDocument *document = static_cast<ScriptTextEditorDocument*>(wnd->widget());
        if(document)
        {
            QList<QTreeWidgetItem*> list = tree->findItems(document->getScriptName().c_str(), Qt::MatchRecursive | Qt::MatchCaseSensitive);
            if(list.size())
            {
                tree->setCurrentItem(list[0]);
            }
        }
    }
}
//-----------------------------------------------------------------------------------------
void ScriptTextEditor::closeEvent(QCloseEvent *event)
{
    QList<QMdiSubWindow*> list = subWindowList();
    for(int i = 0; i < list.size(); i++)
        closeTab(i);
}
//-----------------------------------------------------------------------------------------
ScriptTextEditorDocument::ScriptTextEditorDocument(QWidget *parent) : QPlainTextEdit(parent), 
mCompleter(0), mHighlighter(0), mScriptName(""), mScriptFileName(""), mTextModified(false)
{
    QFont fnt = font();
    fnt.setFamily("Courier New");
    setFont(fnt);

    QFontMetrics fm(fnt);
    setTabStopWidth(fm.width("abcd"));
    mLineNumberArea = new LineNumberArea(this);

    mHighlighter = new ScriptHighlighter(modelFromFile(":/syntax_highlightning/script.txt"), document());

    mCompleter = new QCompleter(modelFromFile(":/syntax_highlightning/script.txt")->stringList(), this);
    mCompleter->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    mCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    mCompleter->setCompletionMode(QCompleter::PopupCompletion);
    mCompleter->setWrapAround(false);
    mCompleter->setWidget(this);

    connect(mCompleter, SIGNAL(activated(const QString&)), this, SLOT(insertCompletion(const QString&)));
    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(const QRect &, int)), this, SLOT(updateLineNumberArea(const QRect &, int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}
//-----------------------------------------------------------------------------------------
void ScriptTextEditorDocument::displayScriptFromFile(Ogre::String scriptName, Ogre::String fileName)
{
    mScriptName = scriptName;
    mScriptFileName = fileName;
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
int ScriptTextEditorDocument::lineNumberAreaWidth()
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
void ScriptTextEditorDocument::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}
//-----------------------------------------------------------------------------------------
void ScriptTextEditorDocument::updateLineNumberArea(const QRect &rect, int dy)
{
    if(dy)
        mLineNumberArea->scroll(0, dy);
    else
        mLineNumberArea->update(0, rect.y(), mLineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}
//-----------------------------------------------------------------------------------------
void ScriptTextEditorDocument::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    mLineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}
//-----------------------------------------------------------------------------------------
void ScriptTextEditorDocument::highlightCurrentLine()
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
void ScriptTextEditorDocument::lineNumberAreaPaintEvent(QPaintEvent *event)
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
void ScriptTextEditorDocument::insertCompletion(const QString& completion)
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
QString ScriptTextEditorDocument::textUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}
//-----------------------------------------------------------------------------------------
void ScriptTextEditorDocument::focusInEvent(QFocusEvent *event)
{
    if(mCompleter)
        mCompleter->setWidget(this);
    QPlainTextEdit::focusInEvent(event);
}
//-----------------------------------------------------------------------------------------
QStringListModel* ScriptTextEditorDocument::modelFromFile(const QString& fileName)
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
void ScriptTextEditorDocument::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_F5 && event->modifiers() == Qt::AltModifier)
    {
        Ogre::String source = document()->toPlainText().toStdString();
        OgitorsScriptConsole::getSingletonPtr()->testScript(source.c_str());
    }
    else if(event->key() == Qt::Key_F6 && event->modifiers() == Qt::AltModifier)
    {
        Ogre::String newScriptName = saveScript();

        Ogre::String script;
        Ogitors::ObjectVector list;
        Ogitors::OgitorsRoot::getSingletonPtr()->GetObjectList(0, list);

        for(unsigned int i = 0;i < list.size();i++)
        {
            script = list[i]->getUpdateScript();
            
            if(script == mScriptName)
            {
                list[i]->setUpdateScript("");
                list[i]->setUpdateScript(mScriptName);
            }
        }
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
void ScriptTextEditorDocument::contextMenuEvent(QContextMenuEvent *event)
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
    }
}
//-----------------------------------------------------------------------------------------
void ScriptTextEditorDocument::mousePressEvent(QMouseEvent *event)
{
    // Rewrite the mouse event to a left button event so the cursor is moved to the location of the pointer
    if(event->button() == Qt::RightButton)
        event = new QMouseEvent(QEvent::MouseButtonPress, event->pos(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QPlainTextEdit::mousePressEvent(event);
}
//-----------------------------------------------------------------------------------------
Ogre::String ScriptTextEditorDocument::saveScript()
{
    QFile file(mScriptFileName.c_str());
    if(!file.exists() || !file.open(QIODevice::ReadWrite | QIODevice::Truncate)) 
    {
        QMessageBox::information(QApplication::activeWindow(),"qtOgitor", tr("Error saving script"));
    }
    else
    {
        file.write(document()->toPlainText().toAscii());
        file.close();

        setTextModified(false);
        setWindowModified(false);

        return mScriptFileName;
    }

    return Ogre::String("");
}
//-----------------------------------------------------------------------------------------
void ScriptTextEditorDocument::documentWasModified()
{
    setTextModified(true);
    setWindowModified(isTextModified());
}
//-----------------------------------------------------------------------------------------
