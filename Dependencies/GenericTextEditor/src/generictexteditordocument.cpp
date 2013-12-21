/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2013 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
///
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

#include "generictexteditor.hxx"
#include "generictexteditordocument.hxx"
#include "xmltexteditorcodec.hxx"

#include <QtWidgets/QAbstractItemView>
#include <QtWidgets/QMessageBox>
#include <QtCore/QSettings>
#include <QtGui/QPainter>

#include "Ogitors.h"
#include "OgreSingleton.h"
#include "OgitorsDefinitions.h"
#include "DefaultEvents.h"
#include "EventManager.h"

//-----------------------------------------------------------------------------------------

GenericTextEditorDocument::GenericTextEditorDocument(QWidget *parent) : QPlainTextEdit(parent), 
mCodec(0), mCompleter(0), mDocName(""), mFilePath(""), mTextModified(false), mFile(0), mIsOfsFile(false),
mInitialDisplay(true), mCloseEvtAlreadyProcessed(false)
{
    QSettings settings;

    QFont fnt = font();
    fnt.setFamily("Courier New");
    fnt.setPointSize(settings.value("preferences/fontSize").toUInt());
    setFont(fnt);   

    QPlainTextEdit::LineWrapMode mode;

    if(settings.value("preferences/lineWrapping").toBool())
        mode = QPlainTextEdit::WidgetWidth;
    else
        mode = QPlainTextEdit::NoWrap;

    setLineWrapMode(mode);
    setAttribute(Qt::WA_DeleteOnClose);

    QFontMetrics fm(fnt);
    setTabStopWidth(fm.width("abcd"));
    mLineNumberArea = new LineNumberArea(this);

    connect(this,       SIGNAL(blockCountChanged(int)),             this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this,       SIGNAL(updateRequest(const QRect &, int)),  this, SLOT(updateLineNumberArea(const QRect &, int)));
    connect(this,       SIGNAL(cursorPositionChanged()),            this, SLOT(highlightCurrentLine()));

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}
//-----------------------------------------------------------------------------------------
GenericTextEditorDocument::~GenericTextEditorDocument()
{
    mFile.close();
    mOfsPtr.unmount();
    delete mCodec;
    mCodec = 0;
}
//-----------------------------------------------------------------------------------------
void GenericTextEditorDocument::displayTextFromFile(QString docName, QString filePath, QString optionalData = "")
{
    mDocName = docName;
    mFilePath = filePath;
    mCodec->setOptionalData(optionalData);

    int pos = filePath.indexOf("::");
    if(pos > 0)
    {
        QString ofsFile = filePath.mid(0, pos);
        filePath.remove(0, pos + 2);
        
        if(mOfsPtr.mount(ofsFile.toStdString().c_str()) != OFS::OFS_OK)
            return;

        if(mOfsPtr->openFile(mOfsFileHandle, filePath.toStdString().c_str(), OFS::OFS_READ) != OFS::OFS_OK)
        {
            mOfsPtr.unmount();
            return;
        }

        mIsOfsFile = true;

        OFS::ofs64 cont_len = 0;
        mOfsPtr->getFileSize(mOfsFileHandle, cont_len);

        char* buf = new char[(unsigned int)cont_len + 1];
        buf[cont_len] = 0;

        mOfsPtr->read(mOfsFileHandle, buf, (unsigned int)cont_len);
        mOfsPtr->closeFile(mOfsFileHandle);
        
        displayText(filePath, buf, optionalData);
        delete[] buf;
    }
    else
    {
        mIsOfsFile = false;

        mFile.setFileName(filePath);
        mFile.open(QIODevice::ReadOnly);
        displayText(docName, mFile.readAll(), optionalData);
    }
}
//-----------------------------------------------------------------------------------------
void GenericTextEditorDocument::displayText(QString docName, QString text, QString optionalData = "")
{
    mDocName = docName;
    mCodec->setOptionalData(optionalData);

    setPlainText(mCodec->onBeforeDisplay(text));

    QString tabTitle = docName;
    if(tabTitle.length() > 25)
        tabTitle = tabTitle.left(12) + "..." + tabTitle.right(10);
    setWindowTitle(tabTitle + QString("[*]"));

    mCodec->onAddCompleter();
    mCodec->onAddHighlighter();

    setTextModified(false);

    mCodec->onAfterDisplay();
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
void GenericTextEditorDocument::keyPressEvent(QKeyEvent *event)
{
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

    // Auto start next line with the indentation of previous line...
    if(event->key() == Qt::Key_Return)
    {
        QTextCursor tc = textCursor();
        int savePos = tc.position();
        //Get Previous bracket position
        tc.movePosition(QTextCursor::Start, QTextCursor::KeepAnchor);
        QString tmpStr = tc.selectedText();

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

    // Insert 4 spaces instead of tab
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
    {
        mCodec->onKeyPressEvent(event);
        QPlainTextEdit::keyPressEvent(event);
    }

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

    mCodec->onKeyPressEvent(event);
}
//-----------------------------------------------------------------------------------------
void GenericTextEditorDocument::contextMenuEvent(QContextMenuEvent *event)
{
    mCodec->onContextMenu(event);
}
//-----------------------------------------------------------------------------------------
void GenericTextEditorDocument::mousePressEvent(QMouseEvent *event)
{
    // Rewrite the right click mouse event to a left button event so the cursor is moved to the location of the click
    if(event->button() == Qt::RightButton)
        event = new QMouseEvent(QEvent::MouseButtonPress, event->pos(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QPlainTextEdit::mousePressEvent(event);
}
//-----------------------------------------------------------------------------------------
void GenericTextEditorDocument::documentWasModified()
{
    // Check if the underlying QTextDocument also reports back the modified flag, 
    // to ignore highlighting changes
    if(document()->isModified())
        setTextModified(true);
}
//-----------------------------------------------------------------------------------------
void GenericTextEditorDocument::addCompleter(const QString keywordListFilePath)
{
    addCompleter(GenericTextEditor::getSingletonPtr()->modelFromFile(keywordListFilePath)->stringList());
}
//-----------------------------------------------------------------------------------------
void GenericTextEditorDocument::addCompleter(const QStringList stringList)
{
    mCompleter = new QCompleter(stringList, this);
    mCompleter->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    mCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    mCompleter->setCompletionMode(QCompleter::PopupCompletion);
    mCompleter->setWrapAround(false);
    mCompleter->setWidget(this);

    connect(mCompleter, SIGNAL(activated(const QString&)), this, SLOT(insertCompletion(const QString&)));
}
//-----------------------------------------------------------------------------------------
int GenericTextEditorDocument::calculateIndentation(const QString& str)
{
    int indent = 0;
    int str_len = str.length();
    char c;

    for(int i = 0; i < str_len; i++)
    {
        c = str.at(i).toLatin1();
        
        if(c == '{')
            ++indent;
        else if(c == '}')
            --indent;
    }

    if(indent < 0)
        indent = 0;

    return (indent * 4);
}
//-----------------------------------------------------------------------------------------
void GenericTextEditorDocument::setTextModified(bool modified)
{
    mTextModified = modified; 
    setWindowModified(modified);
    Ogitors::OgitorsRoot::getSingletonPtr()->ChangeSceneModified(modified);
    GenericTextEditor::getSingletonPtr()->enableSaveButton(modified);
}
//-----------------------------------------------------------------------------------------
void GenericTextEditorDocument::save()
{
    if(isTextModified())
    {
        if(!mCodec->isUseDefaultSaveLogic())
            mCodec->onSave();
        else
            if(!saveDefaultLogic())
                QMessageBox::information(QApplication::activeWindow(), "qtOgitor", QObject::tr("Error saving file %1").arg(mFilePath));

        mCodec->onAfterSave();
    }

    setTextModified(false);
}
//-----------------------------------------------------------------------------------------
bool GenericTextEditorDocument::saveDefaultLogic()
{
    if(mIsOfsFile)
    {
        QString savePath = mFilePath;

        int pos = savePath.indexOf("::");
        if(pos > 0)
            savePath = savePath.remove(0, pos + 2);

        OFS::OfsResult res = mOfsPtr->openFile(mOfsFileHandle, savePath.toUtf8(), OFS::OFS_WRITE);

        if(res != OFS::OFS_OK)
            return false;

        QString text = toPlainText();
        res = mOfsPtr->write(mOfsFileHandle, text.toUtf8(), text.toUtf8().length());    
        mOfsPtr->closeFile(mOfsFileHandle);

        if(res != OFS::OFS_OK)
            return false;
    }
    else
    {
        // First close it, since it is still open from loading (will never get closed as long as it is
        // used within Ogitor to prevent manipulation from outside).
        mFile.close();
        mFile.open(QIODevice::ReadWrite | QIODevice::Truncate);
        QString text = toPlainText();
        if(mFile.write(text.toUtf8()) == -1)
            return false;

        mFile.close();
        mFile.open(QIODevice::ReadOnly);
    }

    return true;
}
//-----------------------------------------------------------------------------------------
void GenericTextEditorDocument::closeEvent(QCloseEvent* event)
{
    if(mCloseEvtAlreadyProcessed == true)
        return;
    
    if(isTextModified())
    {
        int result = QMessageBox::information(QApplication::activeWindow(), "qtOgitor", "Document has been modified. Should the changes be saved?", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        switch(result)
        {
        case QMessageBox::Yes:      save(); break;
        case QMessageBox::No:       break;
        case QMessageBox::Cancel:   event->ignore(); return;
        }
    }

    event->accept();
    getCodec()->onClose();

    mCloseEvtAlreadyProcessed = true;
}
//-----------------------------------------------------------------------------------------
void GenericTextEditorDocument::showEvent(QShowEvent* event)
{
    GenericTextEditor* editor = (GenericTextEditor*) ((QMdiSubWindow*)parent())->mdiArea();
    editor->setActiveDocument(this);
}
//-----------------------------------------------------------------------------------------


