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
#include "genericimageeditor.hxx"

#include "Ogitors.h"

//-----------------------------------------------------------------------------------------

template<> GenericImageEditor* Ogre::Singleton<GenericImageEditor>::ms_Singleton = 0;
ImageCodecExtensionFactoryMap GenericImageEditor::mRegisteredCodecFactories = ImageCodecExtensionFactoryMap();

//-----------------------------------------------------------------------------------------
GenericImageEditor::GenericImageEditor(QString editorName, QWidget *parent) : QMdiArea(parent)
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
    GenericImageEditorCodecFactory* genCodecFactory = new GenericImageEditorCodecFactory();
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
}
//-----------------------------------------------------------------------------------------
void GenericImageEditor::registerCodecFactory(QString extension, IImageEditorCodecFactory* codec)
{
    mRegisteredCodecFactories.insert(ImageCodecExtensionFactoryMap::value_type(extension, codec));
}
//-----------------------------------------------------------------------------------------
void GenericImageEditor::unregisterCodecFactory(QString extension)
{
    ImageCodecExtensionFactoryMap::const_iterator it = mRegisteredCodecFactories.end();
    it = mRegisteredCodecFactories.find(extension);

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
bool GenericImageEditor::isPathAlreadyShowing(QString filePath, GenericImageEditorDocument*& document)
{
    foreach(QMdiSubWindow *window, subWindowList()) 
    {
        document = qobject_cast<GenericImageEditorDocument*>(window->widget());
        if(document->getFilePath() == filePath)
            return true;
    }

    return false;
}
//-----------------------------------------------------------------------------------------
bool GenericImageEditor::isDocAlreadyShowing(QString docName, GenericImageEditorDocument*& document)
{
    foreach(QMdiSubWindow *window, subWindowList()) 
    {
        document = qobject_cast<GenericImageEditorDocument*>(window->widget());
        if(document->getDocName() == docName)
            return true;
    }

    return false;
}
//-----------------------------------------------------------------------------------------
void GenericImageEditor::closeTab(int index)
{
    QMdiSubWindow *sub = subWindowList()[index];
    setActiveSubWindow(sub);
    GenericImageEditorDocument* document = static_cast<GenericImageEditorDocument*>(sub->widget());
    sub->close();
    document->releaseFile();
    document->close();

    emit currentChanged(subWindowList().indexOf(activeSubWindow()));
}
//-----------------------------------------------------------------------------------------
void GenericImageEditor::closeEvent(QCloseEvent *event)
{
    QList<QMdiSubWindow*> list = subWindowList();
    for(int i = 0; i < list.size(); i++)
        closeTab(i);
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
    it = mRegisteredCodecFactories.find(extension);

    if(it != mRegisteredCodecFactories.end())
        return it->second;
    else 
        return 0;
}
//-----------------------------------------------------------------------------------------
void GenericImageEditor::moveToForeground()
{
    mParentTabWidget->setCurrentIndex(mParentTabWidget->indexOf(this));
}
//-----------------------------------------------------------------------------------------
void GenericImageEditor::saveAll()
{
    GenericImageEditorDocument* document;
    QList<QMdiSubWindow*> list = subWindowList();
    for(int i = 0; i < list.size(); i++)
    { 
        document = static_cast<GenericImageEditorDocument*>(subWindowList()[i]->widget());
    }
}
//-----------------------------------------------------------------------------------------
void GenericImageEditor::tabChanged(int index)
{
}
//-----------------------------------------------------------------------------------------
GenericImageEditorDocument::GenericImageEditorDocument(QWidget *parent) : QScrollArea(parent), 
mCodec(0), mDocName(""), mFilePath(""), mFile(0), mIsOfsFile(false)
{
}
//-----------------------------------------------------------------------------------------
GenericImageEditorDocument::~GenericImageEditorDocument()
{
    mOfsPtr.unmount();
}
//-----------------------------------------------------------------------------------------
void GenericImageEditorDocument::displayImageFromFile(QString docName, QString filePath)
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
        
        Ogre::DataStreamPtr stream(new Ogre::MemoryDataStream(buf, cont_len + 1));
        Ogre::Image ogreImage;
        ogreImage.load(stream);

        QLabel* label = new QLabel();
        QImage qImage;
        bool res = qImage.loadFromData((uchar*)buf, cont_len + 1);

        label->setPixmap(QPixmap::fromImage(qImage));
        label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        
        setWidget(label);

        QString tabTitle = docName;
        if(tabTitle.length() > 25)
            tabTitle = tabTitle.left(12) + "..." + tabTitle.right(10);
        setWindowTitle(tabTitle + QString("[*]"));
        setWindowModified(false);

        delete[] buf;
    }
    else
    {
        mIsOfsFile = false;

        mFile.setFileName(filePath);
        mFile.open(QIODevice::ReadOnly);
    }   
}
//-----------------------------------------------------------------------------------------
void GenericImageEditorDocument::contextMenuEvent(QContextMenuEvent *event)
{
    mCodec->contextMenu(event);
}
//-----------------------------------------------------------------------------------------
void GenericImageEditorDocument::mousePressEvent(QMouseEvent *event)
{
    // Rewrite the right click mouse event to a left button event so the cursor is moved to the location of the click
    if(event->button() == Qt::RightButton)
        event = new QMouseEvent(QEvent::MouseButtonPress, event->pos(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QScrollArea::mousePressEvent(event);
}
//-----------------------------------------------------------------------------------------
void GenericImageEditorDocument::releaseFile()
{
    mFile.close();
}
//-----------------------------------------------------------------------------------------