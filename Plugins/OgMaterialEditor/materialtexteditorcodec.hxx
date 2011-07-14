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

#ifndef MATERIAL_TEXT_EDITOR_CODEC_HXX
#define MATERIAL_TEXT_EDITOR_CODEC_HXX

#include "itexteditorcodec.hxx"
#include "generictexteditor.hxx"

#include <QtGui/QAction>

#include <OgreString.h>
#include <OgreScriptCompiler.h>

//----------------------------------------------------------------------------------------

class MaterialTextEditorCodec : public ITextEditorCodec, Ogre::ScriptCompilerListener 
{
public:
    static QAction*  mActRefresh;
    static QToolBar* mToolBar;

    MaterialTextEditorCodec(GenericTextEditorDocument* genTexEdDoc, QString docName, QString documentIcon);

    QString         onBeforeDisplay(QString text);
    void            onAfterDisplay();
    void            onContextMenu(QContextMenuEvent* event);
    void            onKeyPressEvent(QKeyEvent *event);
    void            onRefresh();
    void            onAddHighlighter();
    void            onAddCompleter();
    void            onDisplayRequest();

    void            handleError(Ogre::ScriptCompiler *compiler, Ogre::uint32 code, const Ogre::String &file, int line, const Ogre::String &msg);

    QToolBar*       getCustomToolBar(){ return mToolBar; };

    static  QString getMaterialText(const Ogre::String& input, const Ogre::String& find);

private:
    QStringList     findMaterialNames();
    void            reloadMaterials(QStringList materialList);
    
    bool            mScriptError;
};

//----------------------------------------------------------------------------------------

class MaterialTextEditorCodecFactory : public ITextEditorCodecFactory
{
public:
    ITextEditorCodec* create(GenericTextEditorDocument* genTexEdDoc, QString docName);
};

//----------------------------------------------------------------------------------------

#endif

//----------------------------------------------------------------------------------------