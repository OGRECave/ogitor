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

#ifndef GENERIC_IMAGE_EDITOR_HXX
#define GENERIC_IMAGE_EDITOR_HXX

#include <QtCore/QFile>
#include <QtCore/QTextStream>

#include <QtGui/QScrollArea>
#include <QtGui/QWidget>
#include <QtGui/QPainter>
#include <QtGui/QScrollBar>
#include <QtGui/QMdiArea>
#include <QtGui/QMdiSubWindow>

#include "OgreSingleton.h"

#include "genericimageeditorcodec.hxx"
#include "ofs.h"

class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;
class QPainter;
class QRectF;
class QSizeF;

class GenericImageEditorDocument;

#if defined( __WIN32__ ) || defined( _WIN32 )
   #ifdef GENERICIMAGEEDITOR_EXPORT
     #define GIEExport __declspec (dllexport)
   #else
     #define GIEExport __declspec (dllimport)
   #endif
#else
   #define GIEExport
#endif

typedef std::map<QString, IImageEditorCodecFactory*> ImageCodecExtensionFactoryMap;

//-----------------------------------------------------------------------------------------

class GIEExport GenericImageEditor : public QMdiArea, public Ogre::Singleton<GenericImageEditor>
{
    Q_OBJECT

public:
    GenericImageEditor(QString editorName, QWidget *parent = 0);

    bool                            displayImageFromFile(QString filePath);
    void                            moveToForeground();
    void                            saveAll();

    static void                     registerCodecFactory(QString extension, IImageEditorCodecFactory* codec);
    static void                     unregisterCodecFactory(QString extension);
    static IImageEditorCodecFactory* findMatchingCodecFactory(QString extensionOrFileName);

    inline void                     setAllowDoubleDisplay(bool allow) {mAllowDoubleDisplay = allow;}
    inline bool                     isAllowDoubleDisplay() {return mAllowDoubleDisplay;}

signals:
    void    currentChanged(int);

protected:
    bool    isPathAlreadyShowing(QString filePath, GenericImageEditorDocument*& document);
    bool    isDocAlreadyShowing(QString docName, GenericImageEditorDocument*& document);
    void    closeEvent(QCloseEvent *event);  

protected slots:
	void	tabChanged(int index);

private slots:
    void    closeTab(int index);

private:
     static ImageCodecExtensionFactoryMap   mRegisteredCodecFactories;
     QTabWidget*                            mParentTabWidget;
     bool                                   mAllowDoubleDisplay;
};

//-----------------------------------------------------------------------------------------

class GIEExport GenericImageEditorDocument : public QScrollArea
{
    Q_OBJECT

public:
    GenericImageEditorDocument(QWidget *parent = 0);
    ~GenericImageEditorDocument();

    void displayImageFromFile(QString docName, QString filePath);
    void releaseFile();
    
    inline QString getDocName(){return mDocName;}
    inline QString getFilePath(){return mFilePath;}
    inline IImageEditorCodec* getCodec(){return mCodec;}
    inline void setCodec(IImageEditorCodec* codec){mCodec = codec;}

protected:
    void contextMenuEvent(QContextMenuEvent *event);
    void mousePressEvent(QMouseEvent *event);

protected:
    IImageEditorCodec*  mCodec;
    bool                mIsOfsFile;
    QString             mDocName;
    QString             mFilePath;
    QFile               mFile;
    OFS::OfsPtr         mOfsPtr;
    OFS::OFSHANDLE      mOfsFileHandle;
};

//-----------------------------------------------------------------------------------------

#endif

//-----------------------------------------------------------------------------------------
