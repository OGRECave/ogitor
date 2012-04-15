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

#pragma once

#include <QtCore/QFile>
#include <QtCore/QTextStream>

#include <QtGui/QScrollArea>
#include <QtGui/QWidget>
#include <QtGui/QScrollBar>
#include <QtGui/QMdiArea>
#include <QtGui/QMdiSubWindow>
#include <QtGui/QLabel>
#include <QtGui/QToolBar>
#include <QtGui/QAction>

#include "OgreSingleton.h"
#include "Ogitors.h"

#include "genericimageeditorcodec.hxx"
#include "genericimageeditordocument.hxx"
#include "ofs.h"

class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;
class QPainter;
class QRectF;
class QSizeF;

class GenericImageEditorDocument;
class ToolTipLabel;

#if defined( __WIN32__ ) || defined( _WIN32 )
   #ifdef GENERICIMAGEEDITOR_EXPORT
     #define GIEExport __declspec (dllexport)
   #else
     #define GIEExport __declspec (dllimport)
   #endif
#else
   #define GIEExport
#endif

typedef std::map<std::string, IImageEditorCodecFactory*> ImageCodecExtensionFactoryMap;

//-----------------------------------------------------------------------------------------

class GIEExport GenericImageEditor : public QMdiArea, public Ogre::Singleton<GenericImageEditor>
{
    Q_OBJECT

public:
    GenericImageEditor(QString editorName, QWidget *parent = 0);
    virtual ~GenericImageEditor();

    bool displayImageFromFile(QString filePath);
    void moveToForeground();
    void saveAll();

    static void registerCodecFactory(QString extension, IImageEditorCodecFactory* codec);
    static void unregisterCodecFactory(QString extension);
    static IImageEditorCodecFactory* findMatchingCodecFactory(QString extensionOrFileName);

    inline void setAllowDoubleDisplay(bool allow) {mAllowDoubleDisplay = allow;}
    inline bool isAllowDoubleDisplay() {return mAllowDoubleDisplay;}

    void onModifiedStateChanged(Ogitors::IEvent* evt);
    void onLoadStateChanged(Ogitors::IEvent* evt);

    GenericImageEditorDocument* getActiveDocument() { return mActiveDocument; }
    void setActiveDocument(GenericImageEditorDocument* document);

signals:
    void currentChanged(int);

public slots:
    void tabContentChange();
//    void pasteAvailable();
    void onSave();
//    void onClipboardChanged();

protected:
    bool isPathAlreadyShowing(QString filePath, GenericImageEditorDocument*& document);
    bool isDocAlreadyShowing(QString docName, GenericImageEditorDocument*& document);
    void closeEvent(QCloseEvent *event);

private slots:
    void closeTab(int index);

private:
    void closeActiveDocument();
    void addTab(GenericImageEditorDocument* newDocument, IImageEditorCodec* codec);

    static ImageCodecExtensionFactoryMap mRegisteredCodecFactories;

    QTabWidget *mParentTabWidget;
    QTabBar *mTabBar;
    GenericImageEditorDocument *mActiveDocument;    
    
    bool mAllowDoubleDisplay;
    
    QToolBar   *mMainToolBar;
    QAction    *mActZoomIn;
    QAction    *mActZoomOut;
    QAction    *mActSave;
//    QAction    *mActEditCut;
//    QAction    *mActEditCopy;
//    QAction    *mActEditPaste;
};

