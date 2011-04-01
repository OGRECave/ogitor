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

#include <generictexteditor.hxx>
#include "materialhighlighter.hxx"

class MaterialTextEditorDocument;

//-----------------------------------------------------------------------------------------

class MaterialTextEditor : public GenericTextEditor
{
    Q_OBJECT

public:
    MaterialTextEditor(QWidget *parent = 0);

    void    displayMaterial(QString materialName, QString resourceGroup, QString materialFileName);

private slots:
    void    tabChanged(int index);
};

//-----------------------------------------------------------------------------------------

class MaterialTextEditorDocument : public GenericTextEditorDocument, Ogre::ScriptCompilerListener 
{
    Q_OBJECT

public:
    MaterialTextEditorDocument(QWidget *parent = 0);

    bool saveFile();
    void displayMaterial(QString materialName, QString resourceGroup, QString materialFilePath);

protected:
    void keyPressEvent(QKeyEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);
    void mousePressEvent(QMouseEvent *event);

private:
    void handleError(Ogre::ScriptCompiler *compiler, Ogre::uint32 code, const QString &file, int line, const QString &msg);
    
private:
    QString            mLastMaterialSource;
    QString            mResourceGroup;
    MaterialHighlighter*    mHighlighter;

    bool                    mTextModified;
    bool                    mScriptError;
};

//-----------------------------------------------------------------------------------------

extern MaterialTextEditor *mMaterialTextEditor;

#endif

//-----------------------------------------------------------------------------------------