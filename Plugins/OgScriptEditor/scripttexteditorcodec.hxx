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

#ifndef SCRIPT_TEXT_EDITOR_CODEC_HXX
#define SCRIPT_TEXT_EDITOR_CODEC_HXX

#include "itexteditorcodec.hxx"
#include "generictexteditor.hxx"

#include <OgreString.h>
#include <OgreScriptCompiler.h>

//----------------------------------------------------------------------------------------
class ScriptTextEditorCodecToolBar : public QToolBar
{
    Q_OBJECT;

public:
    ScriptTextEditorCodecToolBar(const QString& name);
    ~ScriptTextEditorCodecToolBar();

public Q_SLOTS:
    void onRefresh();
    void onRun();

protected:
    QAction *mActRefresh;
    QAction *mActRun;
};
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
class ScriptTextEditorCodec : public ITextEditorCodec
{
public:
    static ScriptTextEditorCodecToolBar* mToolBar;

    ScriptTextEditorCodec(GenericTextEditorDocument* genTexEdDoc, QString docName, QString documentIcon);

    QString   onBeforeDisplay(QString text);
    void      onKeyPressEvent(QKeyEvent *event);
    void      onAddHighlighter();
    void      onAddCompleter();
    void      onRefresh();
    void      onRun();

    QToolBar* getCustomToolBar(){ return mToolBar; };

private:
    QString mScriptName;
};

//----------------------------------------------------------------------------------------

class ScriptTextEditorCodecFactory : public ITextEditorCodecFactory
{
public:
    ITextEditorCodec* create(GenericTextEditorDocument* genTexEdDoc, QString docName);
};

//----------------------------------------------------------------------------------------

#endif

//----------------------------------------------------------------------------------------