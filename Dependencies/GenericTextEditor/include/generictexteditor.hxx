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

#ifndef GENERIC_TEXT_EDITOR_HXX
#define GENERIC_TEXT_EDITOR_HXX

#include <QtCore/QFile>
#include <QtCore/QTextStream>

#include <QtGui/QPlainTextEdit>
#include <QtGui/QWidget>
#include <QtGui/QPainter>
#include <QtGui/QCompleter>
#include <QtGui/QStringListModel>
#include <QtGui/QScrollBar>
#include <QtGui/QMdiArea>
#include <QtGui/QMdiSubWindow>
#include <QtGui/QSyntaxHighlighter>

#include "Ogitors.h"
#include "OgreSingleton.h"

#include "generictexteditorcodec.hxx"
#include "ofs.h"

class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;
class QTextDocument;
class QTextFormat;
class QPainter;
class QRectF;
class QSizeF;

class GenericTextEditorDocument;
class LineNumberArea;

#if defined( __WIN32__ ) || defined( _WIN32 )
   #ifdef GENERICTEXTEDITOR_EXPORT
     #define GTEExport __declspec (dllexport)
   #else
     #define GTEExport __declspec (dllimport)
   #endif
#else
   #define GTEExport
#endif

typedef std::map<QString, ITextEditorCodecFactory*> TextCodecExtensionFactoryMap;

//-----------------------------------------------------------------------------------------

class GTEExport GenericTextEditor : public QMdiArea, public Ogre::Singleton<GenericTextEditor>
{
    Q_OBJECT

public:
    GenericTextEditor(QString editorName, QWidget *parent = 0);

    bool                            displayTextFromFile(QString filePath, QString optionalData = "");
    bool                            displayText(QString docName, QString text, QString extension = "", QString optionalData = "");
    void                            moveToForeground();
    void                            saveAll();

    static void                     registerCodecFactory(QString extension, ITextEditorCodecFactory* codec);
    static void                     unregisterCodecFactory(QString extension);
    static ITextEditorCodecFactory* findMatchingCodecFactory(QString extensionOrFileName);

    static QStringListModel*        modelFromFile(const QString& fileName);

    inline void                     setAllowDoubleDisplay(bool allow) {mAllowDoubleDisplay = allow;}
    inline bool                     isAllowDoubleDisplay() {return mAllowDoubleDisplay;}

    void                            onModifiedStateChanged(Ogitors::IEvent* evt);
    void                            onLoadStateChanged(Ogitors::IEvent* evt);

signals:
    void                            currentChanged(int);

protected:
    bool                            isPathAlreadyShowing(QString filePath, GenericTextEditorDocument*& document);
    bool                            isDocAlreadyShowing(QString docName, GenericTextEditorDocument*& document);
    void                            closeEvent(QCloseEvent *event);  

protected slots:
	void	                        tabChanged(int index);

private slots:
    void                            closeTab(int index);

private:
     static TextCodecExtensionFactoryMap    mRegisteredCodecFactories;
     QTabWidget*                        mParentTabWidget;
     bool                               mAllowDoubleDisplay;
};

//-----------------------------------------------------------------------------------------

class GTEExport GenericTextEditorDocument : public QPlainTextEdit
{
    Q_OBJECT

public:
    GenericTextEditorDocument(QWidget *parent = 0);
    ~GenericTextEditorDocument();

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int  lineNumberAreaWidth();

    void displayTextFromFile(QString docName, QString filePath, QString optionalData);
    void displayText(QString docName, QString text, QString optionalData);
    void save();
    bool saveDefaultLogic();
    void releaseFile();
    void addCompleter(const QString keywordListFilePath);

    inline QString getDocName(){return mDocName;}
    inline QString getFilePath(){return mFilePath;}
    inline ITextEditorCodec* getCodec(){return mCodec;}
    inline bool isTextModified(){return mTextModified;}
    inline void setTextModified(bool modified);
    inline void setCodec(ITextEditorCodec* codec){mCodec = codec;}
    inline bool isOfsFile(){return mIsOfsFile;};
    inline OFS::OfsPtr getOfsPtr(){return mOfsPtr;};
    inline OFS::OFSHANDLE getOfsFileHandle(){return mOfsFileHandle;};
    inline QFile* getFile(){return &mFile;};

protected:
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void focusInEvent(QFocusEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);
    void mousePressEvent(QMouseEvent *event);
    QString textUnderCursor() const;
    int  calculateIndentation(const QString& str);

protected slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);
    void insertCompletion(const QString &completion);
    void documentWasModified();

protected:
    ITextEditorCodec*   mCodec;
    bool                mIsOfsFile;
    QString             mDocName;
    QString             mFilePath;
    QFile               mFile;
    OFS::OfsPtr         mOfsPtr;
    OFS::OFSHANDLE      mOfsFileHandle;
    QWidget*            mLineNumberArea;
    QCompleter*         mCompleter;
    bool                mTextModified;
};

//-----------------------------------------------------------------------------------------

class GTEExport LineNumberArea : public QWidget
{
public:
    LineNumberArea(GenericTextEditorDocument *document) : QWidget(document) 
    {
        genericTextEditorDocument = document;
    }

    QSize sizeHint() const {return QSize(genericTextEditorDocument->lineNumberAreaWidth(), 0);}

protected:
    void paintEvent(QPaintEvent *event){genericTextEditorDocument->lineNumberAreaPaintEvent(event);}

private:
    GenericTextEditorDocument *genericTextEditorDocument;
};

//-----------------------------------------------------------------------------------------

#endif

//-----------------------------------------------------------------------------------------
