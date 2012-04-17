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

//----------------------------------------------------------------------------------------

#if defined( __WIN32__ ) || defined( _WIN32 )
   #ifdef GENERICIMAGEEDITOR_EXPORT
     #define GIEExport __declspec (dllexport)
   #else
     #define GIEExport __declspec (dllimport)
   #endif
#else
   #define GIEExport
#endif

//----------------------------------------------------------------------------------------

class GIEExport GenericImageEditorDocument : public QScrollArea
{
    Q_OBJECT

public:
    GenericImageEditorDocument(QWidget *parent = 0);
    virtual ~GenericImageEditorDocument();

    void displayImageFromFile(QString docName, QString filePath);
    void displayImage(QString docName, Ogre::DataStreamPtr stream);
    bool saveDefaultLogic();
    inline QString getDocName(){return mDocName;}
    inline QString getFilePath(){return mFilePath;}
    inline bool isModified(){return mModified;}
    inline void setModified(bool modified);
    
    inline IImageEditorCodec* getCodec(){return mCodec;}
    inline void setCodec(IImageEditorCodec* codec){mCodec = codec;}
    inline bool isOfsFile(){return mIsOfsFile;};
    inline OFS::OfsPtr getOfsPtr(){return mOfsPtr;};
    inline OFS::OFSHANDLE getOfsFileHandle(){return mOfsFileHandle;};
    ToolTipLabel* getLabel(){return mLabel;};

public slots:
    void onZoomIn();
    void onZoomOut();
    void save();

protected:
    void contextMenuEvent(QContextMenuEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void closeEvent(QCloseEvent* event);
    void showEvent(QShowEvent* event);
    void wheelEvent(QWheelEvent* event);

    void scaleImage(float factor);

protected:
    IImageEditorCodec*  mCodec;
    bool                mIsOfsFile;
    bool                mModified;
    QString             mDocName;
    QString             mFilePath;
    QFile               mFile;
    OFS::OfsPtr         mOfsPtr;
    OFS::OFSHANDLE      mOfsFileHandle;
    ToolTipLabel*       mLabel;
};

//-----------------------------------------------------------------------------------------

class GIEExport ToolTipLabel : public QLabel
{
    Q_OBJECT

public:
    ToolTipLabel(GenericImageEditorDocument* genImgEdDoc, QWidget *parent = 0);

    void mouseMoveEvent(QMouseEvent *event);

private:
    GenericImageEditorDocument* mGenImgEdDoc;
};

