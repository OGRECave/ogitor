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
#ifndef MATERIAL_TEXT_EDITOR_HXX
#define MATERIAL_TEXT_EDITOR_HXX

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

#include <OgreScriptCompiler.h>

#include "materialhighlighter.hxx"

class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;
class QTextDocument;
class QTextFormat;
class QPainter;
class QRectF;
class QSizeF;

class MaterialTextEditorDocument;
class LineNumberArea;

//-----------------------------------------------------------------------------------------

class MaterialTextEditor : public QMdiArea
{
    Q_OBJECT

public:
    MaterialTextEditor(QWidget *parent = 0);

    void    displayMaterialFromFile(Ogre::String fileName);
    void    displayMaterial(Ogre::String materialName, Ogre::String resourceGroup, Ogre::String materialFileName);

signals:
    void    currentChanged(int);

protected:
    void    closeEvent(QCloseEvent *event);

private slots:
    void    closeTab(int index);
    void    tabChanged(int index);
};

//-----------------------------------------------------------------------------------------

class MaterialTextEditorDocument : public QPlainTextEdit, Ogre::ScriptCompilerListener 
{
    Q_OBJECT

public:
    MaterialTextEditorDocument(QWidget *parent = 0);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

    Ogre::String saveMaterial();
    void displayMaterialFromFile(Ogre::String fileName);
    void displayMaterial(Ogre::String materialName, Ogre::String resourceGroup, Ogre::String materialFileName);
    inline Ogre::String getMaterialName(){return mMaterialName;}
    inline Ogre::String getMaterialFileName() {return mMaterialFileName;}
    inline bool isTextModified(){return mTextModified;}
    inline void setTextModified(bool modified){mTextModified = modified;}

protected:
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void focusInEvent(QFocusEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);
    void mousePressEvent(QMouseEvent *event);

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);
    void insertCompletion(const QString &completion);
    void documentWasModified();

private:
    QString textUnderCursor() const;
    QStringListModel* modelFromFile(const QString& fileName);
    void handleError(Ogre::ScriptCompiler *compiler, Ogre::uint32 code, const Ogre::String &file, int line, const Ogre::String &msg);
    
private:
    Ogre::String            mMaterialName;
    Ogre::String            mMaterialFileName;
    Ogre::String            mLastMaterialSource;
    Ogre::String            mResourceGroup;
    QWidget*                mLineNumberArea;
    MaterialHighlighter*    mHighlighter;
    QCompleter*             mCompleter;

    bool                    mTextModified;
    bool                    mScriptError;
};

//-----------------------------------------------------------------------------------------

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(MaterialTextEditorDocument *document) : QWidget(document) 
    {
        materialTextEditorDocument = document;
    }

    QSize sizeHint() const {return QSize(materialTextEditorDocument->lineNumberAreaWidth(), 0);}

protected:
    void paintEvent(QPaintEvent *event){materialTextEditorDocument->lineNumberAreaPaintEvent(event);}

private:
    MaterialTextEditorDocument *materialTextEditorDocument;
};

//-----------------------------------------------------------------------------------------

extern MaterialTextEditor *mMaterialTextEditor;

#endif

//-----------------------------------------------------------------------------------------
