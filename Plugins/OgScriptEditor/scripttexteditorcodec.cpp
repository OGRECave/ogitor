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

#include "scripttexteditorcodec.hxx"
#include "generictexteditor.hxx"
#include "scripthighlighter.hxx"

#include <QtGui/QColorDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QApplication>
#include <QtCore/QObject>
#include "OgitorsRoot.h"
#include "OgitorsScriptConsole.h"
#include "OgitorsScriptInterpreter.h"

//-----------------------------------------------------------------------------------------

ScriptTextEditorCodecToolBar* ScriptTextEditorCodec::mToolBar = 0;

//-----------------------------------------------------------------------------------------
ScriptTextEditorCodecToolBar::ScriptTextEditorCodecToolBar(const QString& name) : QToolBar(name)
{
    mActRefresh = new QAction("Refresh Script", this);
    mActRefresh->setStatusTip("Refresh Script");
    mActRefresh->setIcon(QIcon( ":/icons/refresh.svg"));
    mActRefresh->setEnabled(true);

    mActRun = new QAction(tr("Run Script"), this);
    mActRun->setStatusTip(tr("Run Script"));
    mActRun->setIcon(QIcon( ":/icons/player_play.svg" ));
    mActRun->setEnabled(true);

    connect(mActRefresh, SIGNAL(triggered()), this, SLOT(onRefresh()));
    connect(mActRun, SIGNAL(triggered()), this, SLOT(onRun()));

    setObjectName("renderwindowtoolbar");
    setIconSize(QSize(20,20));
    setToolButtonStyle(Qt::ToolButtonIconOnly);
    addAction(mActRefresh);
    addAction(mActRun);
}
//-----------------------------------------------------------------------------------------
ScriptTextEditorCodecToolBar::~ScriptTextEditorCodecToolBar()
{
}
//-----------------------------------------------------------------------------------------
void ScriptTextEditorCodecToolBar::onRefresh()
{
    GenericTextEditorDocument* document = GenericTextEditor::getSingletonPtr()->getActiveDocument();
    if(document != 0)
    {
        static_cast<ScriptTextEditorCodec*>(document->getCodec())->onRefresh();
    }
}
//-----------------------------------------------------------------------------------------
void ScriptTextEditorCodecToolBar::onRun()
{
    GenericTextEditorDocument* document = GenericTextEditor::getSingletonPtr()->getActiveDocument();
    if(document != 0)
    {
        static_cast<ScriptTextEditorCodec*>(document->getCodec())->onRun();
    }
}
//-----------------------------------------------------------------------------------------
ScriptTextEditorCodec::ScriptTextEditorCodec(GenericTextEditorDocument* genTexEdDoc, QString docName, QString documentIcon) : 
ITextEditorCodec(genTexEdDoc, docName, documentIcon)
{
    int pos = docName.lastIndexOf(std::string("." + Ogitors::OgitorsRoot::getSingletonPtr()->GetScriptInterpreter()->getScriptExtension()).c_str());

    if(pos != -1)
        mScriptName = docName.left(pos);
    else
        mScriptName = docName;
}
//-----------------------------------------------------------------------------------------
QString ScriptTextEditorCodec::onBeforeDisplay(QString text)
{
    return text;
}
//-----------------------------------------------------------------------------------------
void ScriptTextEditorCodec::onAddHighlighter()
{
    new ScriptHighlighter(GenericTextEditor::getSingletonPtr()->modelFromFile(":/syntax_highlighting/script.txt"), mGenTexEdDoc->document());
}
//-----------------------------------------------------------------------------------------
void ScriptTextEditorCodec::onKeyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_F5 && event->modifiers() == Qt::AltModifier)
    {
        Ogre::String source = mGenTexEdDoc->toPlainText().toStdString();
        Ogitors::OgitorsScriptConsole::getSingletonPtr()->testScript(source.c_str());
    }
    else if(event->key() == Qt::Key_F6 && event->modifiers() == Qt::AltModifier)
    {
        mGenTexEdDoc->save();

        Ogre::String script;
        Ogitors::ObjectVector list;
        Ogitors::OgitorsRoot::getSingletonPtr()->GetObjectList(0, list);

        for(unsigned int i = 0;i < list.size();i++)
        {
            script = list[i]->getUpdateScript();

            if(script == mScriptName.toStdString())
            {
                list[i]->setUpdateScript("");
                list[i]->setUpdateScript(mScriptName.toStdString());
            }
        }
    }
}
//-----------------------------------------------------------------------------------------
void ScriptTextEditorCodec::onAddCompleter()
{
    //TODO: handle completion for different languages!
    mGenTexEdDoc->addCompleter(":/syntax_highlighting/script.txt");
}
//-----------------------------------------------------------------------------------------
void ScriptTextEditorCodec::onRefresh()
{
    mGenTexEdDoc->save();

    Ogre::String script;
    Ogitors::ObjectVector list;
    Ogitors::OgitorsRoot::getSingletonPtr()->GetObjectList(0, list);

    for(unsigned int i = 0;i < list.size();i++)
    {
        script = list[i]->getUpdateScript();

        if(script == mScriptName.toStdString())
        {
            list[i]->setUpdateScript("");
            list[i]->setUpdateScript(mScriptName.toStdString());
        }
    }
}
//-----------------------------------------------------------------------------------------
void ScriptTextEditorCodec::onRun()
{
    mGenTexEdDoc->save();

    std::string source = mDocName.toStdString();
    Ogitors::OgitorsScriptConsole::getSingletonPtr()->runScript(source);
}
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
ITextEditorCodec* ScriptTextEditorCodecFactory::create(GenericTextEditorDocument* genTexEdDoc, QString docName)
{
    return new ScriptTextEditorCodec(genTexEdDoc, docName, Ogitors::OgitorsRoot::getSingletonPtr()->GetScriptInterpreter()->getScriptIcon().c_str());
}
//-----------------------------------------------------------------------------------------
