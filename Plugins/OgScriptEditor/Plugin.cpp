///////////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////////

#include <QtGui/QDockWidget>

#include "Ogitors.h"
#include "Plugin.h"
#include "scripttexteditorcodec.hxx"
#include "scriptview.hxx"

using namespace Ogitors;

//----------------------------------------------------------------------------
bool dllStartPlugin(void *identifier, Ogre::String& name)
{
    name = "AngelScript Editor Plugin";
	
    mScriptViewWidget = new ScriptViewWidget();

    ScriptTextEditorCodec::mToolBar = new ScriptTextEditorCodecToolBar("Script Actions");

	Ogitors::DockWidgetData dockWidgetData;
	dockWidgetData.mCaption = "AngleScripts";
    dockWidgetData.mHandle = mScriptViewWidget;
    dockWidgetData.mIcon = ":/icons/script.svg";
    dockWidgetData.mParent = DOCK_RESOURCES;

    ScriptTextEditorCodecFactory* scriptCodecFactory = new ScriptTextEditorCodecFactory();
    GenericTextEditor::getSingletonPtr()->registerCodecFactory("as", scriptCodecFactory);

    OgitorsRoot::getSingletonPtr()->RegisterDockWidget(identifier, dockWidgetData);
    
    return true;
}
//----------------------------------------------------------------------------
bool dllGetPluginName(Ogre::String& name)
{
    name = "AngelScript Editor Plugin";
    return true;
}
//----------------------------------------------------------------------------
bool dllStopPlugin(void)
{
    return true;
}
//----------------------------------------------------------------------------