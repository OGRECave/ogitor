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

#include <QtGui/QtGui>
#include "generictexteditor.hxx"

#include "Ogitors.h"

//-----------------------------------------------------------------------------------------

template<> GenericTextEditor* Ogre::Singleton<GenericTextEditor>::ms_Singleton = 0;
CodecExtensionFactoryMap GenericTextEditor::mRegisteredCodecFactories = CodecExtensionFactoryMap();

//-----------------------------------------------------------------------------------------
GenericTextEditor::GenericTextEditor(QString editorName, QWidget *parent) : QMdiArea(parent)
{
    mParentTabWidget = static_cast<QTabWidget*>(parent);
    setObjectName(editorName);
    setViewMode(QMdiArea::TabbedView);

    QTabBar* tabBar = findChildren<QTabBar*>().at(0);
    tabBar->setTabsClosable(true);

    connect(tabBar, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    connect(tabBar, SIGNAL(currentChanged(int)),    this, SLOT(tabChanged(int)));
    connect(this,   SIGNAL(currentChanged(int)),    this, SLOT(tabChanged(int)));

    // Register the standard generic text editor codec extensions
    GenericTextEditorCodecFactory* genCodecFactory = new GenericTextEditorCodecFactory();
    GenericTextEditor::registerCodecFactory("txt",         genCodecFactory);
    GenericTextEditor::registerCodecFactory("xml",         genCodecFactory);
    GenericTextEditor::registerCodecFactory("ogscene",     genCodecFactory);
    GenericTextEditor::registerCodecFactory("html",        genCodecFactory);
    GenericTextEditor::registerCodecFactory("htm",         genCodecFactory);
    GenericTextEditor::registerCodecFactory("scene",       genCodecFactory);
    GenericTextEditor::registerCodecFactory("cfg",         genCodecFactory);
    GenericTextEditor::registerCodecFactory("log",         genCodecFactory);
}
//-----------------------------------------------------------------------------------------
void GenericTextEditor::registerCodecFactory(QString extension, ITextEditorCodecFactory* codec)
{
    mRegisteredCodecFactories.insert(CodecExtensionFactoryMap::value_type(extension, codec));
}
//-----------------------------------------------------------------------------------------
void GenericTextEditor::unregisterCodecFactory(QString extension)
{
    CodecExtensionFactoryMap::const_iterator it = mRegisteredCodecFactories.end();
    it = mRegisteredCodecFactories.find(extension);

    if(it != mRegisteredCodecFactories.end())
        mRegisteredCodecFactories.erase(it);
}
//-----------------------------------------------------------------------------------------
bool GenericTextEditor::displayTextFromFile(QString filePath)
{
    ITextEditorCodecFactory* codecFactory = GenericTextEditor::findMatchingCodecFactory(filePath);

    if(codecFactory == 0)
       return false;    
    
    GenericTextEditorDocument* document = 0;
    if(!isPathAlreadyShowing(filePath, document) || isAllowDoubleDisplay())
    {
        document = new GenericTextEditorDocument(this);
        ITextEditorCodec* codec = codecFactory->create(document, filePath);
        document->setCodec(codec);
        document->displayTextFromFile(QFile(filePath).fileName(), filePath);
        QMdiSubWindow *window = addSubWindow(document);
        window->setWindowIcon(QIcon(codec->getDocumentIcon()));
        document->showMaximized();
        QTabBar* tabBar = findChildren<QTabBar*>().at(0);
        tabBar->setTabToolTip(findChildren<QMdiSubWindow*>().size() - 1, QFile(filePath).fileName());
    }
    else
    {
        setActiveSubWindow(qobject_cast<QMdiSubWindow*>(document->window()));
        document->setFocus(Qt::ActiveWindowFocusReason);
    }

    moveToForeground();

    return true;
}
//-----------------------------------------------------------------------------------------
bool GenericTextEditor::displayText(QString docName, QString text, QString extension = "")
{
    // If there is no extra extension passed, then try to find the matching one based on the doc name
    ITextEditorCodecFactory* codecFactory;
    if(extension == "")    
        codecFactory = GenericTextEditor::findMatchingCodecFactory(docName);
    else
        codecFactory = GenericTextEditor::findMatchingCodecFactory(extension);

    if(codecFactory == 0)
        return false;

    GenericTextEditorDocument* document = 0;
    if(!isDocAlreadyShowing(docName, document) || isAllowDoubleDisplay())
    {
        document = new GenericTextEditorDocument(this);
        ITextEditorCodec* codec = codecFactory->create(document, docName);
        document->setCodec(codec);
        document->displayText(docName, text);
        QMdiSubWindow *window = addSubWindow(document);
        window->setWindowIcon(QIcon(codec->getDocumentIcon()));
        document->showMaximized();
        QTabBar* tabBar = findChildren<QTabBar*>().at(0);
        tabBar->setTabToolTip(findChildren<QMdiSubWindow*>().size() - 1, docName);
    }
    else
    {
        setActiveSubWindow(qobject_cast<QMdiSubWindow*>(document->window()));
        document->setFocus(Qt::ActiveWindowFocusReason);
    }

    moveToForeground();

    return true;
}
//-----------------------------------------------------------------------------------------
bool GenericTextEditor::isPathAlreadyShowing(QString filePath, GenericTextEditorDocument*& document)
{
    foreach(QMdiSubWindow *window, subWindowList()) 
    {
        document = qobject_cast<GenericTextEditorDocument*>(window->widget());
        if(document->getFilePath() == filePath)
            return true;
    }

    return false;
}
//-----------------------------------------------------------------------------------------
bool GenericTextEditor::isDocAlreadyShowing(QString docName, GenericTextEditorDocument*& document)
{
    foreach(QMdiSubWindow *window, subWindowList()) 
    {
        document = qobject_cast<GenericTextEditorDocument*>(window->widget());
        if(document->getDocName() == docName)
            return true;
    }

    return false;
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
        case QMessageBox::Yes: document->getCodec()->save(); break;
        case QMessageBox::No: break;
        case QMessageBox::Cancel: return;
        }
    }
    sub->close();
    document->releaseFile();
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
ITextEditorCodecFactory* GenericTextEditor::findMatchingCodecFactory(QString extensionOrFileName)
{
    int pos = extensionOrFileName.lastIndexOf(".");
    QString extension;

    if(pos != -1)
        extension = extensionOrFileName.right(extensionOrFileName.size() - pos - 1);
    else
        extension = extensionOrFileName;
    
    CodecExtensionFactoryMap::const_iterator it = mRegisteredCodecFactories.end();
    it = mRegisteredCodecFactories.find(extension);

    if(it != mRegisteredCodecFactories.end())
        return it->second;
    else 
        return 0;
}
//-----------------------------------------------------------------------------------------
QStringListModel* GenericTextEditor::modelFromFile(const QString& filePath)
{
    QFile file(filePath);
    if(!file.open(QFile::ReadOnly))
        return new QStringListModel();

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

    file.close();
    return new QStringListModel(strMap.values());
}
//-----------------------------------------------------------------------------------------
void GenericTextEditor::moveToForeground()
{
    mParentTabWidget->setCurrentIndex(mParentTabWidget->indexOf(this));
}
//-----------------------------------------------------------------------------------------
void GenericTextEditor::saveAll()
{
    GenericTextEditorDocument* document;
    QList<QMdiSubWindow*> list = subWindowList();
    for(int i = 0; i < list.size(); i++)
    { 
        document = static_cast<GenericTextEditorDocument*>(subWindowList()[i]->widget());
    
        if(document->isTextModified())
            document->getCodec()->save();
    }
}
//-----------------------------------------------------------------------------------------
GenericTextEditorDocument::GenericTextEditorDocument( QWidget *parent) : QPlainTextEdit(parent), 
mCodec(0), mCompleter(0), mDocName(""), mFilePath(""), mTextModified(false), mFile(0), mIsOfsFile(false)
{
    QFont fnt = font();
    fnt.setFamily("Courier New");
    fnt.setPointSize(10);
    setFont(fnt);

    setLineWrapMode(QPlainTextEdit::WidgetWidth);

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
    mOfsPtr.unmount();
}
//-----------------------------------------------------------------------------------------
void GenericTextEditorDocument::displayTextFromFile(QString docName, QString filePath)
{
    mDocName = docName;
    mFilePath = filePath;

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

        unsigned int cont_len = 0;
        mOfsPtr->getFileSize(mOfsFileHandle, cont_len);

        char* buf = new char[cont_len + 1];
        buf[cont_len] = 0;

        mOfsPtr->read(mOfsFileHandle, buf, cont_len);
        mOfsPtr->closeFile(mOfsFileHandle);
        
        displayText(filePath, buf);
        delete[] buf;
    }
    else
    {
        mIsOfsFile = false;

        mFile.setFileName(filePath);
        mFile.open(QIODevice::ReadOnly);
        displayText(docName, mFile.readAll());
    }
   
}
//-----------------------------------------------------------------------------------------
void GenericTextEditorDocument::displayText(QString docName, QString text)
{
    mDocName = docName;

    setPlainText(mCodec->prepareForDisplay(docName, text));

    QString tabTitle = docName;
    if(tabTitle.length() > 25)
        tabTitle = tabTitle.left(12) + "..." + tabTitle.right(10);
    setWindowTitle(tabTitle + QString("[*]"));

    mCodec->addCompleter(this);
    mCodec->addHighlighter(this);

    connect(this, SIGNAL(textChanged()), this, SLOT(documentWasModified()));

    setWindowModified(false);
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
    QPlainTextEdit::keyPressEvent(event);
}
//-----------------------------------------------------------------------------------------
void GenericTextEditorDocument::contextMenuEvent(QContextMenuEvent *event)
{
    mCodec->contextMenu(event);
}
//-----------------------------------------------------------------------------------------
void GenericTextEditorDocument::mousePressEvent(QMouseEvent *event)
{
    // Rewrite the right clcick mouse event to a left button event so the cursor is moved to the location of the click
    if(event->button() == Qt::RightButton)
        event = new QMouseEvent(QEvent::MouseButtonPress, event->pos(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QPlainTextEdit::mousePressEvent(event);
}
//-----------------------------------------------------------------------------------------
void GenericTextEditorDocument::documentWasModified()
{
    setTextModified(true);
    setWindowModified(isTextModified());

    Ogitors::OgitorsRoot::getSingletonPtr()->SetSceneModified(true);
}
//-----------------------------------------------------------------------------------------
void GenericTextEditorDocument::releaseFile()
{
    mFile.close();
}
//-----------------------------------------------------------------------------------------
void GenericTextEditorDocument::addCompleter(const QString keywordListFilePath)
{
    mCompleter = new QCompleter(GenericTextEditor::getSingletonPtr()->modelFromFile(keywordListFilePath)->stringList(), this);
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
    const char *chr = str.toStdString().c_str();

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