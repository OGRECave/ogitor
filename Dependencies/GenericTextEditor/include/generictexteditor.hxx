/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2010 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
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


//-----------------------------------------------------------------------------------------

class GTEExport GenericTextEditor : public QMdiArea
{
    Q_OBJECT

public:
    GenericTextEditor(QString editorName, QString documentIcon, QWidget *parent = 0);

    void    displayTextFromFile(QString fileName);
    void    displayText(QString docName, QString text);

    inline void    setAllowDoubleDisplay(bool allow) {mAllowDoubleDisplay = allow;}
    inline bool    isAllowDoubleDisplay() {return mAllowDoubleDisplay;}

signals:
    void    currentChanged(int);

protected:
    bool    isPathAlreadyShowing(QString filePath, GenericTextEditorDocument*& document);
    bool    isDocAlreadyShowing(QString docName, GenericTextEditorDocument*& document);
    void    closeEvent(QCloseEvent *event);

private slots:
    void    closeTab(int index);
    void    tabChanged(int index);

protected:
    QString mDocumentIcon;
    bool    mAllowDoubleDisplay;
};

//-----------------------------------------------------------------------------------------

class GTEExport GenericTextEditorDocument : public QPlainTextEdit
{
    Q_OBJECT

public:
    GenericTextEditorDocument(QWidget *parent = 0);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int  lineNumberAreaWidth();

    bool saveFile();
    void displayTextFromFile(QString docName, QString filePath);
    void displayText(QString docName, QString text);
    void releaseFile();
    void addCompleter(const QString keywordListFilePath);

    inline QString getDocName(){return mDocName;}
    inline QString getFilePath() {return mFilePath;}
    inline bool isTextModified(){return mTextModified;}
    inline void setTextModified(bool modified) {mTextModified = modified;}

protected:
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void focusInEvent(QFocusEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);
    void mousePressEvent(QMouseEvent *event);
    QString textUnderCursor() const;
    QStringListModel* modelFromFile(const QString& fileName);
    int  calculateIndentation(const QString& str);

protected slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);
    void insertCompletion(const QString &completion);
    void documentWasModified();

protected:
    QString            mDocName;
    QString            mFilePath;
    QFile              mFile;
    QWidget*           mLineNumberArea;
    QCompleter*        mCompleter;
    bool               mTextModified;
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
