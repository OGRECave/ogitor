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

#include "OgitorsDefinitions.h"
#include "DefaultEvents.h"
#include "EventManager.h"

//-----------------------------------------------------------------------------------------

template<> GenericTextEditor* Ogre::Singleton<GenericTextEditor>::ms_Singleton = 0;
TextCodecExtensionFactoryMap GenericTextEditor::mRegisteredCodecFactories = TextCodecExtensionFactoryMap();

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

    Ogitors::EventManager::getSingletonPtr()->connectEvent(Ogitors::EventManager::MODIFIED_STATE_CHANGE, this, true, 0, true, 0, EVENT_CALLBACK(GenericTextEditor, onModifiedStateChanged));
    Ogitors::EventManager::getSingletonPtr()->connectEvent(Ogitors::EventManager::LOAD_STATE_CHANGE, this, true, 0, true, 0, EVENT_CALLBACK(GenericTextEditor, onLoadStateChanged));
}
//-----------------------------------------------------------------------------------------
void GenericTextEditor::registerCodecFactory(QString extension, ITextEditorCodecFactory* codec)
{
    mRegisteredCodecFactories.insert(TextCodecExtensionFactoryMap::value_type(extension, codec));
}
//-----------------------------------------------------------------------------------------
void GenericTextEditor::unregisterCodecFactory(QString extension)
{
    TextCodecExtensionFactoryMap::const_iterator it = mRegisteredCodecFactories.end();
    it = mRegisteredCodecFactories.find(extension);

    if(it != mRegisteredCodecFactories.end())
        mRegisteredCodecFactories.erase(it);
}
//-----------------------------------------------------------------------------------------
bool GenericTextEditor::displayTextFromFile(QString filePath, QString optionalData)
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
        document->displayTextFromFile(QFile(filePath).fileName(), filePath, optionalData);
        QMdiSubWindow *window = addSubWindow(document);
        window->setWindowIcon(QIcon(codec->getDocumentIcon()));
        document->showMaximized();
        QTabBar* tabBar = findChildren<QTabBar*>().at(0);
        tabBar->setTabToolTip(findChildren<QMdiSubWindow*>().size() - 1, QFile(filePath).fileName());   
    }
    else
    {
        document->getCodec()->setOptionalData(optionalData);
        document->getCodec()->onDisplayRequest();
        setActiveSubWindow(qobject_cast<QMdiSubWindow*>(document->window()));
        document->setFocus(Qt::ActiveWindowFocusReason);
    }

    moveToForeground();   

    connect(document, SIGNAL(textChanged()), document, SLOT(documentWasModified()));

    return true;
}
//-----------------------------------------------------------------------------------------
bool GenericTextEditor::displayText(QString docName, QString text, QString extension, QString optionalData)
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
        document->displayText(docName, text, optionalData);
        QMdiSubWindow *window = addSubWindow(document);
        window->setWindowIcon(QIcon(codec->getDocumentIcon()));
        document->showMaximized();
        QTabBar* tabBar = findChildren<QTabBar*>().at(0);
        tabBar->setTabToolTip(findChildren<QMdiSubWindow*>().size() - 1, docName);    
    }
    else
    {
        document->getCodec()->setOptionalData(optionalData);
        document->getCodec()->onDisplayRequest();
        setActiveSubWindow(qobject_cast<QMdiSubWindow*>(document->window()));
        document->setFocus(Qt::ActiveWindowFocusReason);
    }

    moveToForeground();

    connect(document, SIGNAL(textChanged()), document, SLOT(documentWasModified()));

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
        case QMessageBox::Yes:      document->save(); break;
        case QMessageBox::No:       break;
        case QMessageBox::Cancel:   return;
        }
    }
    sub->close();
    document->getCodec()->onClose();
    document->releaseFile();
    document->close();

    emit currentChanged(subWindowList().indexOf(activeSubWindow()));
}
//-----------------------------------------------------------------------------------------
void GenericTextEditor::closeEvent(QCloseEvent *event)
{
    QList<QMdiSubWindow*> list = subWindowList();
    for(int i = 0; i < list.size(); i++)
        closeTab(0);
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
    
    TextCodecExtensionFactoryMap::const_iterator it = mRegisteredCodecFactories.end();
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
        document->save();
    }
}
//-----------------------------------------------------------------------------------------
void GenericTextEditor::tabChanged(int index)
{
    // -1 means that the last tab was just closed and so there is no one left anymore to switch to
    if(index != -1)
    {
        GenericTextEditorDocument* document;
        QList<QMdiSubWindow*> list = subWindowList();
        document = static_cast<GenericTextEditorDocument*>(subWindowList()[index]->widget());
        document->getCodec()->onTabChange();
    }
}
//-----------------------------------------------------------------------------------------
void GenericTextEditor::onModifiedStateChanged(Ogitors::IEvent* evt)
{
    Ogitors::SceneModifiedChangeEvent *change_event = Ogitors::event_cast<Ogitors::SceneModifiedChangeEvent*>(evt);

    if(change_event)
    {
        bool state = change_event->getState();

        // If scene is not modified anymore, the user saved it, so we need to save the
        // documents as well.
        if(!state)
            saveAll();
    }
}
//-----------------------------------------------------------------------------------------
void GenericTextEditor::onLoadStateChanged(Ogitors::IEvent* evt)
{
    Ogitors::LoadStateChangeEvent *change_event = Ogitors::event_cast<Ogitors::LoadStateChangeEvent*>(evt);

    if(change_event)
    {
        Ogitors::LoadState state = change_event->getType();

        if(state == Ogitors::LS_UNLOADED)
            close();
    }
}
//-----------------------------------------------------------------------------------------
GenericTextEditorDocument::GenericTextEditorDocument(QWidget *parent) : QPlainTextEdit(parent), 
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

        unsigned int cont_len = 0;
        mOfsPtr->getFileSize(mOfsFileHandle, cont_len);

        char* buf = new char[cont_len + 1];
        buf[cont_len] = 0;

        mOfsPtr->read(mOfsFileHandle, buf, cont_len);
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

    setWindowModified(false);

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
    char c;

    for(int i = 0; i < str_len; i++)
    {
        c = str.at(i).toAscii();
        
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
    Ogitors::OgitorsRoot::getSingletonPtr()->SetSceneModified(modified);
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
        int pos = mFilePath.indexOf("::");
        if(pos > 0)
            mFilePath = mFilePath.remove(0, pos + 2);

        OFS::OfsResult res = mOfsPtr->openFile(mOfsFileHandle, mFilePath.toAscii(), OFS::OFS_WRITE);

        if(res != OFS::OFS_OK)
            return false;

        QString text = toPlainText();
        res = mOfsPtr->write(mOfsFileHandle, text.toAscii(), text.toAscii().length());    
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
        if(mFile.write(toPlainText().toAscii()) == -1)
            return false;
    }

    return true;
}
//-----------------------------------------------------------------------------------------