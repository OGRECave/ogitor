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

#include <QtGui/QtGui>
#include "genericimageeditor.hxx"
#include "heightimageeditorcodec.hxx"

#include "Ogitors.h"
#include "OgitorsDefinitions.h"
#include "DefaultEvents.h"
#include "EventManager.h"
#include "OFSDataStream.h"

//-----------------------------------------------------------------------------------------

template<> GenericImageEditor* Ogre::Singleton<GenericImageEditor>::msSingleton = 0;
ImageCodecExtensionFactoryMap GenericImageEditor::mRegisteredCodecFactories = ImageCodecExtensionFactoryMap();

//-----------------------------------------------------------------------------------------
GenericImageEditor::GenericImageEditor(QString editorName, QWidget *parent) : QMdiArea(parent)
{
    mParentTabWidget = static_cast<QTabWidget*>(parent->parent());
    setObjectName(editorName);
    setViewMode(QMdiArea::TabbedView);
    
    mTabBar = findChildren<QTabBar*>().at(0);
    mTabBar->setTabsClosable(true);

    connect(mTabBar, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));

    mActZoomIn = new QAction(tr("Zoom In"), this);
    mActZoomIn->setStatusTip(tr("Zoom In"));
    mActZoomIn->setIcon(QIcon(":/icons/zoom_in.svg"));
    mActZoomIn->setEnabled(false);

    mActZoomOut = new QAction(tr("Zoom Out"), this);
    mActZoomOut->setStatusTip(tr("Zoom Out"));
    mActZoomOut->setIcon(QIcon(":/icons/zoom_out.svg"));
    mActZoomOut->setEnabled(false);

    mMainToolBar = new QToolBar();
    mMainToolBar->setObjectName("renderwindowtoolbar");
    mMainToolBar->setIconSize(QSize(20, 20));
    mMainToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    mMainToolBar->addAction(mActZoomIn);
    mMainToolBar->addAction(mActZoomOut);

    QMainWindow *mw = static_cast<QMainWindow*>(this->parentWidget());
    mw->addToolBar(Qt::TopToolBarArea, mMainToolBar);

    mActiveDocument = 0;

//    connect(mActEditCut,                SIGNAL(triggered()),    this, SLOT(pasteAvailable()));
//    connect(mActEditCopy,               SIGNAL(triggered()),    this, SLOT(pasteAvailable()));
//    connect(mActSave,                   SIGNAL(triggered()),    this, SLOT(onSave()));
//    connect(QApplication::clipboard(),  SIGNAL(dataChanged()),  this, SLOT(onClipboardChanged()));

    // Register the standard generic text editor codec extensions
    GenericImageEditorCodecFactory* genCodecFactory = new GenericImageEditorCodecFactory();
    HeightImageEditorCodecFactory* heightCodecFactory = new HeightImageEditorCodecFactory();

    GenericImageEditor::registerCodecFactory("png",         genCodecFactory);
    GenericImageEditor::registerCodecFactory("jpg",         genCodecFactory);
    GenericImageEditor::registerCodecFactory("gif",         genCodecFactory);
    GenericImageEditor::registerCodecFactory("bmp",         genCodecFactory);
    GenericImageEditor::registerCodecFactory("jpeg",        genCodecFactory);
    GenericImageEditor::registerCodecFactory("pbm",         genCodecFactory);
    GenericImageEditor::registerCodecFactory("pgm",         genCodecFactory);
    GenericImageEditor::registerCodecFactory("ppm",         genCodecFactory);
    GenericImageEditor::registerCodecFactory("tiff",        genCodecFactory);
    GenericImageEditor::registerCodecFactory("xbm",         genCodecFactory);
    GenericImageEditor::registerCodecFactory("xpm",         genCodecFactory);

    // Below are files generated via render target. Perhaps only make these read only.
    GenericImageEditor::registerCodecFactory("dds",         genCodecFactory);

    GenericImageEditor::registerCodecFactory("f32",         heightCodecFactory);

    Ogitors::EventManager::getSingletonPtr()->connectEvent(Ogitors::EventManager::MODIFIED_STATE_CHANGE, this, true, 0, true, 0, EVENT_CALLBACK(GenericImageEditor, onModifiedStateChanged));
    Ogitors::EventManager::getSingletonPtr()->connectEvent(Ogitors::EventManager::LOAD_STATE_CHANGE, this, true, 0, true, 0, EVENT_CALLBACK(GenericImageEditor, onLoadStateChanged));
}
//-----------------------------------------------------------------------------------------
GenericImageEditor::~GenericImageEditor()
{
    Ogitors::EventManager::getSingletonPtr()->disconnectEvent(Ogitors::EventManager::MODIFIED_STATE_CHANGE, this);
    Ogitors::EventManager::getSingletonPtr()->disconnectEvent(Ogitors::EventManager::LOAD_STATE_CHANGE, this);
}
//-----------------------------------------------------------------------------------------
/*void GenericImageEditor::pasteAvailable()
{
    mActEditPaste->setEnabled(true);
}*/
//-----------------------------------------------------------------------------------------
void GenericImageEditor::onSave()
{
    mActSave->setEnabled(false);
}
//-----------------------------------------------------------------------------------------
void GenericImageEditor::registerCodecFactory(QString extension, IImageEditorCodecFactory* codec)
{
    mRegisteredCodecFactories.insert(ImageCodecExtensionFactoryMap::value_type(extension.toStdString(), codec));
}
//-----------------------------------------------------------------------------------------
void GenericImageEditor::unregisterCodecFactory(QString extension)
{
    ImageCodecExtensionFactoryMap::iterator it = mRegisteredCodecFactories.end();
    it = mRegisteredCodecFactories.find(extension.toStdString());

    if(it != mRegisteredCodecFactories.end())
        mRegisteredCodecFactories.erase(it);
}
//-----------------------------------------------------------------------------------------
bool GenericImageEditor::displayImageFromFile(QString filePath)
{
    IImageEditorCodecFactory* codecFactory = GenericImageEditor::findMatchingCodecFactory(filePath);

    if(codecFactory == 0)
       return false;  
    
    GenericImageEditorDocument* document = 0;
    if(!isPathAlreadyShowing(filePath, document) || isAllowDoubleDisplay())
    {
        document = new GenericImageEditorDocument(this);
        IImageEditorCodec* codec = codecFactory->create(document, filePath);
        document->setCodec(codec);
        document->displayImageFromFile(QFile(filePath).fileName(), filePath);
    
        addTab(document, codec);
    }
    else
    {
        setActiveDocument(document);
    }

    moveToForeground();

    return true;
}
//-----------------------------------------------------------------------------------------
bool GenericImageEditor::isPathAlreadyShowing(QString filePath, GenericImageEditorDocument*& document)
{
    foreach(QMdiSubWindow *window, subWindowList()) 
    {
        document = qobject_cast<GenericImageEditorDocument*>(window->widget());
        if(document != 0 && document->getFilePath() == filePath)
            return true;
    }

    document = 0;
    return false;
}
//-----------------------------------------------------------------------------------------
bool GenericImageEditor::isDocAlreadyShowing(QString docName, GenericImageEditorDocument*& document)
{
    foreach(QMdiSubWindow *window, subWindowList()) 
    {
        document = qobject_cast<GenericImageEditorDocument*>(window->widget());
        if(document != 0 && document->getDocName() == docName)
            return true;
    }

    document = 0;
    return false;
}
//-----------------------------------------------------------------------------------------
void GenericImageEditor::tabContentChange()
{
    mActSave->setEnabled(true);
}
//-----------------------------------------------------------------------------------------
void GenericImageEditor::closeTab(int index)
{
    QList<QMdiSubWindow*> list = subWindowList();

    GenericImageEditorDocument* document = static_cast<GenericImageEditorDocument*>(list[index]->widget());
    setActiveDocument(document);

    if (!list[index]->close())
        return;
        
    if (document == mActiveDocument)
        closeActiveDocument();
}
//-----------------------------------------------------------------------------------------
void GenericImageEditor::addTab(GenericImageEditorDocument* newDocument, IImageEditorCodec* codec)
{
    QMdiSubWindow* subWindow = new QMdiSubWindow;
    subWindow->setWidget(newDocument);
    subWindow->setAttribute(Qt::WA_DeleteOnClose);
    subWindow->setWindowIcon(QIcon(codec->getDocumentIcon()));

    // [*] is special qt thing to show the file as modified
    QFileInfo pathInfo( newDocument->getDocName()+"[*]" );

    subWindow->setWindowTitle(pathInfo.fileName());
    addSubWindow(subWindow);

    newDocument->showMaximized();
}
//-----------------------------------------------------------------------------------------
void GenericImageEditor::setActiveDocument(GenericImageEditorDocument* document)
{
    closeActiveDocument();
    
    mActiveDocument = document;

    //connect(mActSave, SIGNAL(triggered()), mActiveDocument, SLOT(save()));
    //connect(mActEditCut, SIGNAL(triggered()), mActiveDocument, SLOT(cut()));
    //connect(mActEditCopy, SIGNAL(triggered()), mActiveDocument, SLOT(copy()));
    //connect(mActEditPaste, SIGNAL(triggered()), mActiveDocument, SLOT(paste()));

//    connect(mActiveDocument, SIGNAL(textChanged()), this, SLOT(tabContentChange()));
//    connect(mActiveDocument, SIGNAL(textChanged()), mActiveDocument, SLOT(documentWasModified()));

    //connect(mActiveDocument, SIGNAL(copyAvailable(bool)), mActEditCopy, SLOT(setEnabled(bool)));
    //connect(mActiveDocument, SIGNAL(copyAvailable(bool)), mActEditCut, SLOT(setEnabled(bool)));

    
    //mActSave->setEnabled(mActiveDocument->isModified());
    //mActEditCut->setEnabled(false);
    //mActEditCopy->setEnabled(false);

    QToolBar *tb = document->getCodec()->getCustomToolBar();
    if(tb != 0)
    {
        QMainWindow *mw = static_cast<QMainWindow*>(this->parentWidget());
        mw->addToolBar(Qt::TopToolBarArea, tb);
        tb->show();
    }

    mActiveDocument->setFocus(Qt::ActiveWindowFocusReason);
}
//-----------------------------------------------------------------------------------------
void GenericImageEditor::closeActiveDocument()
{
    if (mActiveDocument == 0)
        return;

    /*
    TODO: I have commented out functionality however it should be implemented later. Keeping it
    commented out so you can see where you need to implement features.
     */

    //disconnect(mActSave, SIGNAL(triggered()), mActiveDocument, SLOT(save()));
    //disconnect(mActEditCut, SIGNAL(triggered()), mActiveDocument, SLOT(cut()));
    //disconnect(mActEditCopy, SIGNAL(triggered()), mActiveDocument, SLOT(copy()));
    //disconnect(mActEditPaste, SIGNAL(triggered()), mActiveDocument, SLOT(paste()));
    //disconnect(mActiveDocument, SIGNAL(textChanged()), this, SLOT(tabContentChange()));
    //disconnect(mActiveDocument, SIGNAL(copyAvailable(bool)), mActEditCopy, SLOT(setEnabled(bool)));
    //disconnect(mActiveDocument, SIGNAL(copyAvailable(bool)), mActEditCut, SLOT(setEnabled(bool)));

    QToolBar *tb = mActiveDocument->getCodec()->getCustomToolBar();
    if(tb != 0)
    {
        QMainWindow *mw = static_cast<QMainWindow*>(this->parentWidget());
        mw->removeToolBar(tb);
    }

    //mActSave->setEnabled(false);
    //mActEditCut->setEnabled(false);
    //mActEditCopy->setEnabled(false);
    //mActEditPaste->setEnabled(false);

    mActiveDocument = 0;
}
//-----------------------------------------------------------------------------------------
void GenericImageEditor::closeEvent(QCloseEvent *event)
{
    QList<QMdiSubWindow*> list = subWindowList();
    for(int i = 0; i < list.size(); i++)
        closeTab(0);
}
//-----------------------------------------------------------------------------------------
IImageEditorCodecFactory* GenericImageEditor::findMatchingCodecFactory(QString extensionOrFileName)
{
    int pos = extensionOrFileName.lastIndexOf(".");
    QString extension;

    if(pos != -1)
        extension = extensionOrFileName.right(extensionOrFileName.size() - pos - 1);
    else
        extension = extensionOrFileName;
    
    ImageCodecExtensionFactoryMap::const_iterator it = mRegisteredCodecFactories.end();
    it = mRegisteredCodecFactories.find(extension.toStdString());

    if(it != mRegisteredCodecFactories.end())
        return it->second;
    else 
        return 0;
}
//-----------------------------------------------------------------------------------------
void GenericImageEditor::moveToForeground()
{
    mParentTabWidget->setCurrentIndex(mParentTabWidget->indexOf(this->parentWidget()));
}
//-----------------------------------------------------------------------------------------
void GenericImageEditor::saveAll()
{
    GenericImageEditorDocument* document;
    QList<QMdiSubWindow*> list = subWindowList();
    for(int i = 0; i < list.size(); i++)
    { 
        document = static_cast<GenericImageEditorDocument*>(list[i]->widget());
        document->save();
    }
}
//-----------------------------------------------------------------------------------------
void GenericImageEditor::onModifiedStateChanged(Ogitors::IEvent* evt)
{
    Ogitors::SceneModifiedChangeEvent *change_event = Ogitors::event_cast<Ogitors::SceneModifiedChangeEvent*>(evt);

    if(!change_event)
        return;

    bool state = change_event->getState();

    // If scene is not modified anymore, the user saved it, so we need to save the
    // documents as well.
    if(!state)
        saveAll();
}
//-----------------------------------------------------------------------------------------
void GenericImageEditor::onLoadStateChanged(Ogitors::IEvent* evt)
{
    Ogitors::LoadStateChangeEvent *change_event = Ogitors::event_cast<Ogitors::LoadStateChangeEvent*>(evt);

    if(!change_event)
        return;
    
    Ogitors::LoadState state = change_event->getType();

    if(state == Ogitors::LS_UNLOADED)
    {
        closeAllSubWindows();
    }
}
//-----------------------------------------------------------------------------------------
/*void GenericImageEditor::onClipboardChanged()
{
    const QClipboard* clipboard = QApplication::clipboard();
    const QMimeData* mimeData = clipboard->mimeData();

    if(mimeData->hasText())
        emit pasteAvailable();
}
*/
