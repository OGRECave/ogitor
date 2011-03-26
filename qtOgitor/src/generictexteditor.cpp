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

#include "generictexteditor.hxx"

using namespace Ogitors;

//-----------------------------------------------------------------------------------------
GenericTextEditor::GenericTextEditor(QString documentIcon, QWidget *parent) : QMdiArea(parent)
{
    mDocumentIcon = documentIcon;
    mAllowDoubleDisplay = false;
    
    setObjectName("GenericTextEditor");
    setViewMode(QMdiArea::TabbedView);

    QTabBar* tabBar = findChildren<QTabBar*>().at(0);
    tabBar->setTabsClosable(true);

    connect(tabBar, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    connect(tabBar, SIGNAL(currentChanged(int)),    this, SLOT(tabChanged(int)));
    connect(this,   SIGNAL(currentChanged(int)),    this, SLOT(tabChanged(int)));
}
//-----------------------------------------------------------------------------------------
void GenericTextEditor::displayTextFromFile(Ogre::String filePath)
{
    bool alreadyShowing = false;
    GenericTextEditorDocument* document;

    foreach(QMdiSubWindow *window, subWindowList()) 
    {
        document = qobject_cast<GenericTextEditorDocument*>(window->widget());
        if(document->getFilePath() == filePath)
        {
            alreadyShowing = true;
            break;
        }
    }

    if(!alreadyShowing || mAllowDoubleDisplay)
    {
        GenericTextEditorDocument* document = new GenericTextEditorDocument(this);
        document->displayTextFromFile(OgitorsUtils::ExtractFileName(filePath), filePath);
        QMdiSubWindow *window = addSubWindow(document);
        window->setWindowIcon(QIcon(mDocumentIcon));
        document->showMaximized();
        QTabBar* tabBar = findChildren<QTabBar*>().at(0);
        tabBar->setTabToolTip(findChildren<QMdiSubWindow*>().size() - 1, OgitorsUtils::ExtractFileName(filePath).c_str());
    }
    else
    {
        setActiveSubWindow(qobject_cast<QMdiSubWindow*>(document->window()));
        document->setFocus(Qt::ActiveWindowFocusReason);
    }
}
//-----------------------------------------------------------------------------------------
void GenericTextEditor::displayText(Ogre::String docName, Ogre::String text)
{
    bool alreadyShowing = false;
    GenericTextEditorDocument* document;

    foreach(QMdiSubWindow *window, subWindowList()) 
    {
        document = qobject_cast<GenericTextEditorDocument*>(window->widget());
        if(document->getDocName() == docName)
        {
            alreadyShowing = true;
            break;
        }
    }

    if(!alreadyShowing || mAllowDoubleDisplay)
    {
        GenericTextEditorDocument* document = new GenericTextEditorDocument(this);
        document->displayText(docName, text);
        QMdiSubWindow *window = addSubWindow(document);
        window->setWindowIcon(QIcon(mDocumentIcon));
        document->showMaximized();
        QTabBar* tabBar = findChildren<QTabBar*>().at(0);
        tabBar->setTabToolTip(findChildren<QMdiSubWindow*>().size() - 1, docName.c_str());
    }
    else
    {
        setActiveSubWindow(qobject_cast<QMdiSubWindow*>(document->window()));
        document->setFocus(Qt::ActiveWindowFocusReason);
    }
}
//-----------------------------------------------------------------------------------------
void GenericTextEditor::closeTab(int index)
{
    QMdiSubWindow *sub = subWindowList()[index];
    setActiveSubWindow(sub);
    GenericTextEditorDocument* document = static_cast<GenericTextEditorDocument*>(sub->widget());
    if(document->isTextModified())
    {	
        int result = QMessageBox::information(QApplication::activeWindow(), "qtOgitor", "Document has been modified. Should the changes be saved?", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        switch(result)
        {
        case QMessageBox::Yes:	document->saveFile(); break;
        case QMessageBox::No: break;
        case QMessageBox::Cancel: return;
        }
    }
    sub->close();
    document->close();
    
    emit currentChanged(subWindowList().indexOf(activeSubWindow()));
}
//-----------------------------------------------------------------------------------------
void GenericTextEditor::closeEvent(QCloseEvent *event)
{
    QList<QMdiSubWindow*> list = subWindowList();
    for(int i = 0; i < list.size(); i++)
        closeTab(i);
}
//-----------------------------------------------------------------------------------------
GenericTextEditorDocument::GenericTextEditorDocument(QWidget *parent) : QPlainTextEdit(parent), 
mCompleter(0), mHighlighter(0), mDocName(""), mFilePath(""), mTextModified(false)
{
    QFont fnt = font();
    fnt.setFamily("Courier New");
    fnt.setPointSize(10);
    setFont(fnt);
    
    setLineWrapMode(QPlainTextEdit::WidgetWidth);

    QFontMetrics fm(fnt);
    setTabStopWidth(fm.width("abcd"));
    mLineNumberArea = new LineNumberArea(this);

    //mHighlighter = new QSyntaxHighlighter(modelFromFile(":/syntax_highlightning/material.txt"), document());

    mCompleter = new QCompleter(modelFromFile(":/syntax_highlightning/material.txt")->stringList(), this);
    mCompleter->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    mCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    mCompleter->setCompletionMode(QCompleter::PopupCompletion);
    mCompleter->setWrapAround(false);
    mCompleter->setWidget(this);

    connect(mCompleter, SIGNAL(activated(const QString&)),          this, SLOT(insertCompletion(const QString&)));
    connect(this,       SIGNAL(blockCountChanged(int)),             this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this,       SIGNAL(updateRequest(const QRect &, int)),  this, SLOT(updateLineNumberArea(const QRect &, int)));
    connect(this,       SIGNAL(cursorPositionChanged()),            this, SLOT(highlightCurrentLine()));

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}
//-----------------------------------------------------------------------------------------
void GenericTextEditorDocument::displayTextFromFile(Ogre::String docName, Ogre::String filePath)
{
    mDocName = docName;
    mFilePath = filePath;
    QFile* pFile = new QFile(filePath.c_str());
    pFile->open(QIODevice::ReadOnly);
    setPlainText(pFile->readAll().data());

    Ogre::String tabTitle = OgitorsUtils::ExtractFileName(filePath);
    if(tabTitle.length() > 25)
        tabTitle = tabTitle.substr(0, 12) + "..." + tabTitle.substr(tabTitle.length() - 10, 10);
    setWindowTitle(tabTitle.c_str() + QString("[*]"));
    
    connect(this, SIGNAL(textChanged()), this, SLOT(documentWasModified()));
}
//-----------------------------------------------------------------------------------------
void GenericTextEditorDocument::displayText(Ogre::String docName, Ogre::String text)
{
    mDocName = docName;
    setPlainText(text.c_str());

    Ogre::String tabTitle = docName;
    if(tabTitle.length() > 25)
        tabTitle = tabTitle.substr(0, 12) + "..." + tabTitle.substr(tabTitle.length() - 10, 10);
    setWindowTitle(tabTitle.c_str() + QString("[*]"));

    connect(this, SIGNAL(textChanged()), this, SLOT(documentWasModified()));
}
//-----------------------------------------------------------------------------------------
int GenericTextEditorDocument::lineNumberAreaWidth()
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
void GenericTextEditorDocument::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}
//-----------------------------------------------------------------------------------------
void GenericTextEditorDocument::updateLineNumberArea(const QRect &rect, int dy)
{
    if(dy)
        mLineNumberArea->scroll(0, dy);
    else
        mLineNumberArea->update(0, rect.y(), mLineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}
//-----------------------------------------------------------------------------------------
void GenericTextEditorDocument::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    mLineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}
//-----------------------------------------------------------------------------------------
void GenericTextEditorDocument::highlightCurrentLine()
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
void GenericTextEditorDocument::lineNumberAreaPaintEvent(QPaintEvent *event)
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
void GenericTextEditorDocument::insertCompletion(const QString& completion)
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
QString GenericTextEditorDocument::textUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}
//-----------------------------------------------------------------------------------------
void GenericTextEditorDocument::focusInEvent(QFocusEvent *event)
{
    if(mCompleter)
        mCompleter->setWidget(this);
    QPlainTextEdit::focusInEvent(event);
}
//-----------------------------------------------------------------------------------------
QStringListModel* GenericTextEditorDocument::modelFromFile(const QString& fileName)
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
void GenericTextEditorDocument::keyPressEvent(QKeyEvent *event)
{
    QPlainTextEdit::keyPressEvent(event);
}
//-----------------------------------------------------------------------------------------
void GenericTextEditorDocument::contextMenuEvent(QContextMenuEvent *event)
{
    QPlainTextEdit::contextMenuEvent(event);
}
//-----------------------------------------------------------------------------------------
void GenericTextEditorDocument::mousePressEvent(QMouseEvent *event)
{
    // Rewrite the mouse event to a left button event so the cursor is moved to the location of the pointer
    if(event->button() == Qt::RightButton)
        event = new QMouseEvent(QEvent::MouseButtonPress, event->pos(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QPlainTextEdit::mousePressEvent(event);
}
//-----------------------------------------------------------------------------------------
void GenericTextEditorDocument::documentWasModified()
{
    setTextModified(true);
    setWindowModified(isTextModified());
}
//-----------------------------------------------------------------------------------------
bool GenericTextEditorDocument::saveFile()
{
    return true;
}
//-----------------------------------------------------------------------------------------