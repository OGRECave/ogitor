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

#include "generictexteditor.hxx"
#include "xmltexteditorcodec.hxx"

#include <QtGui/QtGui>

#include "DefaultEvents.h"
#include "EventManager.h"


//-----------------------------------------------------------------------------------------

template<> GenericTextEditor* Ogre::Singleton<GenericTextEditor>::msSingleton = 0;
TextCodecExtensionFactoryMap GenericTextEditor::mRegisteredCodecFactories = TextCodecExtensionFactoryMap();

//-----------------------------------------------------------------------------------------
GenericTextEditor::GenericTextEditor(QString editorName, QWidget *parent) : QMdiArea(parent)
{
    mParentTabWidget = static_cast<QTabWidget*>(parent->parent());
    setObjectName(editorName);
    setViewMode(QMdiArea::TabbedView);

    mTabBar = findChildren<QTabBar*>().at(0);
    mTabBar->setTabsClosable(true);

    connect(mTabBar, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    connect(mTabBar, SIGNAL(currentChanged(int)),    this, SLOT(tabChanged(int)));
    connect(this,   SIGNAL(currentChanged(int)),    this, SLOT(tabChanged(int)));

    mActSave = new QAction(tr("Save"), this);
    mActSave->setStatusTip(tr("Save"));
    mActSave->setIcon(QIcon(":/icons/filesave.svg" ));
    mActSave->setEnabled(false);
    
    mActEditCopy = new QAction(tr("Copy"), this);
    mActEditCopy->setStatusTip(tr("Copy Selected"));
    mActEditCopy->setIcon(QIcon(":/icons/editcopy.svg"));
    mActEditCopy->setEnabled(false);

    mActEditCut = new QAction(tr("Cut"), this);
    mActEditCut->setStatusTip(tr("Cut Selected"));
    mActEditCut->setIcon(QIcon(":/icons/editcut.svg"));
    mActEditCut->setEnabled(false);

    mActEditPaste = new QAction(tr("Paste"), this);
    mActEditPaste->setStatusTip(tr("Paste From Clipboard"));
    mActEditPaste->setIcon(QIcon(":/icons/editpaste.svg"));
    mActEditPaste->setEnabled(false);

    mMainToolBar = new QToolBar();
    mMainToolBar->setObjectName("renderwindowtoolbar");
    mMainToolBar->setIconSize(QSize(20, 20));
    mMainToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    mMainToolBar->addAction(mActSave);
    mMainToolBar->addSeparator();
    mMainToolBar->addAction(mActEditCut);
    mMainToolBar->addAction(mActEditCopy);
    mMainToolBar->addAction(mActEditPaste);

    QMainWindow *mw = static_cast<QMainWindow*>(this->parentWidget());
    mw->addToolBar(Qt::TopToolBarArea, mMainToolBar);

    mActiveDocument = 0;

    connect(mActEditCut,                SIGNAL(triggered()),    this, SLOT(pasteAvailable()));
    connect(mActEditCopy,               SIGNAL(triggered()),    this, SLOT(pasteAvailable()));
    connect(mActSave,                   SIGNAL(triggered()),    this, SLOT(onSave()));
    connect(QApplication::clipboard(),  SIGNAL(dataChanged()),  this, SLOT(onClipboardChanged()));

    // Register the standard generic text editor codec extensions
    GenericTextEditorCodecFactory* genCodecFactory = new GenericTextEditorCodecFactory();
    GenericTextEditor::registerCodecFactory("txt",         genCodecFactory);
    GenericTextEditor::registerCodecFactory("cfg",         genCodecFactory);
    GenericTextEditor::registerCodecFactory("log",         genCodecFactory);

    // Register the XML generic text editor codec extensions
    XMLTextEditorCodecFactory* XMLCodecFactory = new XMLTextEditorCodecFactory();
    GenericTextEditor::registerCodecFactory("xml",         XMLCodecFactory);
    GenericTextEditor::registerCodecFactory("ogscene",     XMLCodecFactory);
    GenericTextEditor::registerCodecFactory("html",        XMLCodecFactory);
    GenericTextEditor::registerCodecFactory("htm",         XMLCodecFactory);
    GenericTextEditor::registerCodecFactory("scene",       XMLCodecFactory);

    Ogitors::EventManager::getSingletonPtr()->connectEvent(Ogitors::EventManager::MODIFIED_STATE_CHANGE, this, true, 0, true, 0, EVENT_CALLBACK(GenericTextEditor, onModifiedStateChanged));
    Ogitors::EventManager::getSingletonPtr()->connectEvent(Ogitors::EventManager::LOAD_STATE_CHANGE, this, true, 0, true, 0, EVENT_CALLBACK(GenericTextEditor, onLoadStateChanged));
}
//-----------------------------------------------------------------------------------------
GenericTextEditor::~GenericTextEditor()
{
    Ogitors::EventManager::getSingletonPtr()->disconnectEvent(Ogitors::EventManager::MODIFIED_STATE_CHANGE, this);
    Ogitors::EventManager::getSingletonPtr()->disconnectEvent(Ogitors::EventManager::LOAD_STATE_CHANGE, this);
}
//-----------------------------------------------------------------------------------------
void GenericTextEditor::pasteAvailable()
{
    mActEditPaste->setEnabled(true);
}
//-----------------------------------------------------------------------------------------
void GenericTextEditor::onSave()
{
    mActSave->setEnabled(false);
}
//-----------------------------------------------------------------------------------------
void GenericTextEditor::registerCodecFactory(QString extension, ITextEditorCodecFactory* codec)
{
    mRegisteredCodecFactories.insert(TextCodecExtensionFactoryMap::value_type(extension.toStdString(), codec));
}
//-----------------------------------------------------------------------------------------
void GenericTextEditor::unregisterCodecFactory(QString extension)
{
    TextCodecExtensionFactoryMap::iterator it = mRegisteredCodecFactories.end();
    it = mRegisteredCodecFactories.find(extension.toStdString());

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

        QMdiSubWindow* subWindow = new QMdiSubWindow;
        subWindow->setWidget(document);
        subWindow->setAttribute(Qt::WA_DeleteOnClose);
        subWindow->setWindowIcon(QIcon(codec->getDocumentIcon()));
        addSubWindow(subWindow);

        document->showMaximized();
        mTabBar->setTabToolTip(findChildren<QMdiSubWindow*>().size() - 1, QFile(filePath).fileName()); 

        QList<QMdiSubWindow*> list = subWindowList();
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
    
    mActSave->setEnabled(false);

    document->setInitialDisplay(false);

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

        QMdiSubWindow* subWindow = new QMdiSubWindow;
        subWindow->setWidget(document);
        subWindow->setAttribute(Qt::WA_DeleteOnClose);
        subWindow->setWindowIcon(QIcon(codec->getDocumentIcon()));
        addSubWindow(subWindow);

        document->showMaximized();
        mTabBar->setTabToolTip(findChildren<QMdiSubWindow*>().size() - 1, docName); 
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
    mActSave->setEnabled(false);

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
void GenericTextEditor::tabContentChange()
{
    if(!mActiveDocument->isIntialDisplay())
        mActSave->setEnabled(true);
}
//-----------------------------------------------------------------------------------------
void GenericTextEditor::closeTab(int index)
{
    QList<QMdiSubWindow*> list = subWindowList();

    std::cout << "closeTab: " << index << std::endl;

    GenericTextEditorDocument* document = static_cast<GenericTextEditorDocument*>(list[index]->widget());
    if (!document->close())
        return;

    mTabBar->removeTab(index);

    if (mTabBar->count() == 0)
        removeActiveDocument();

    emit currentChanged(subWindowList().indexOf(activeSubWindow()));
}
//-----------------------------------------------------------------------------------------
void GenericTextEditor::switchActiveDocument(GenericTextEditorDocument* newActiveDocument)
{
    removeActiveDocument();

    if (mTabBar->count() == 0)
        return;

    connect(mActSave, SIGNAL(triggered()), newActiveDocument, SLOT(save()));
    connect(mActEditCut, SIGNAL(triggered()), newActiveDocument, SLOT(cut()));
    connect(mActEditCopy, SIGNAL(triggered()), newActiveDocument, SLOT(copy()));
    connect(mActEditPaste, SIGNAL(triggered()), newActiveDocument, SLOT(paste()));
    connect(newActiveDocument, SIGNAL(textChanged()), this, SLOT(tabContentChange()));
    connect(newActiveDocument, SIGNAL(copyAvailable(bool)), mActEditCopy, SLOT(setEnabled(bool)));
    connect(newActiveDocument, SIGNAL(copyAvailable(bool)), mActEditCut, SLOT(setEnabled(bool)));
    mActSave->setEnabled(newActiveDocument->isTextModified());
    mActEditCut->setEnabled(false);
    mActEditCopy->setEnabled(false);

    mActiveDocument = newActiveDocument;

    QToolBar *tb = mActiveDocument->getCodec()->getCustomToolBar();
    if(tb != 0)
    {
        QMainWindow *mw = static_cast<QMainWindow*>(this->parentWidget());
        mw->addToolBar(Qt::TopToolBarArea, tb);
        tb->show();
    }
}
//-----------------------------------------------------------------------------------------
void GenericTextEditor::removeActiveDocument()
{
    if (mActiveDocument == 0)
        return;

    disconnect(mActSave, SIGNAL(triggered()), mActiveDocument, SLOT(save()));
    disconnect(mActEditCut, SIGNAL(triggered()), mActiveDocument, SLOT(cut()));
    disconnect(mActEditCopy, SIGNAL(triggered()), mActiveDocument, SLOT(copy()));
    disconnect(mActEditPaste, SIGNAL(triggered()), mActiveDocument, SLOT(paste()));
    disconnect(mActiveDocument, SIGNAL(textChanged()), this, SLOT(tabContentChange()));
    disconnect(mActiveDocument, SIGNAL(copyAvailable(bool)), mActEditCopy, SLOT(setEnabled(bool)));
    disconnect(mActiveDocument, SIGNAL(copyAvailable(bool)), mActEditCut, SLOT(setEnabled(bool)));

    QToolBar *tb = mActiveDocument->getCodec()->getCustomToolBar();
    if(tb != 0)
    {
        QMainWindow *mw = static_cast<QMainWindow*>(this->parentWidget());
        mw->removeToolBar(tb);
    }

    mActiveDocument = 0;
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
    
    TextCodecExtensionFactoryMap::const_iterator it = mRegisteredCodecFactories.end();
    it = mRegisteredCodecFactories.find(extension.toStdString());

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
    while(!file.atEnd()) 
    {
        QByteArray line = file.readLine();
        if(!line.trimmed().isEmpty())
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
    mParentTabWidget->setCurrentIndex(mParentTabWidget->indexOf(this->parentWidget()));
}
//-----------------------------------------------------------------------------------------
void GenericTextEditor::saveAll()
{
    GenericTextEditorDocument* document;
    QList<QMdiSubWindow*> list = subWindowList();
    for(int i = 0; i < list.size(); i++)
    { 
        document = static_cast<GenericTextEditorDocument*>(list[i]->widget());
        document->save();
    }
}
//-----------------------------------------------------------------------------------------
void GenericTextEditor::tabChanged(int index)
{
    if(mTabBar->count() == 0)
    {
        GenericTextEditorDocument* document;
        QList<QMdiSubWindow*> list = subWindowList();
        document = static_cast<GenericTextEditorDocument*>(list[index]->widget());
        document->getCodec()->onTabChange();
        switchActiveDocument(document);
    }
    else
    {
        mActSave->setEnabled(false);
        mActEditCut->setEnabled(false);
        mActEditCopy->setEnabled(false);
        mActEditPaste->setEnabled(false);
        mActiveDocument = 0;
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
        {
            removeActiveDocument();
            closeAllSubWindows();
        }
    }
}
//-----------------------------------------------------------------------------------------
void GenericTextEditor::onClipboardChanged()
{
    const QClipboard* clipboard = QApplication::clipboard();
    const QMimeData* mimeData = clipboard->mimeData();

    if(mimeData->hasText())
        emit pasteAvailable();
}

