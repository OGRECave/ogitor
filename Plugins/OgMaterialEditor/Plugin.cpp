///////////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////////

#include <QtGui/QDockWidget>

#include "Ogitors.h"
#include "Plugin.h"
#include "MaterialEditor.h"
#include "TechniqueEditor.h"
#include "materialtexteditorcodec.hxx"
#include "materialview.hxx"
#include "materialeditorprefseditor.hxx"

using namespace Ogitors;

//----------------------------------------------------------------------------
bool dllStartPlugin(void *identifier, Ogre::String& name)
{
    name = "Material Editor Plugin";

	mMaterialViewWidget = new MaterialViewWidget();
    mMaterialPrefsEditor = new MaterialEditorPrefsEditor();

	Ogitors::DockWidgetData dockWidgetData;
	dockWidgetData.mCaption = "Materials";
    dockWidgetData.mHandle = mMaterialViewWidget;
    dockWidgetData.mIcon = ":/icons/material.svg";
    dockWidgetData.mParent = DOCK_RESOURCES;
	   
    Ogitors::PreferencesEditorData preferencesEditorData;
    preferencesEditorData.mCaption = "Material Editor";
    preferencesEditorData.mIcon = ":/icons/material.svg";
    preferencesEditorData.mSectionName = "materialEditor";
    preferencesEditorData.mHandle = mMaterialPrefsEditor;

    MaterialTextEditorCodec::mToolBar = new MaterialTextEditorCodecToolBar("Material Actions");

    MaterialTextEditorCodecFactory* matCodecFactory = new MaterialTextEditorCodecFactory();
    GenericTextEditor::getSingletonPtr()->registerCodecFactory("material",     matCodecFactory);
    GenericTextEditor::getSingletonPtr()->registerCodecFactory("cg",           matCodecFactory);
    GenericTextEditor::getSingletonPtr()->registerCodecFactory("hlsl",         matCodecFactory);
    GenericTextEditor::getSingletonPtr()->registerCodecFactory("glsl",         matCodecFactory);
    GenericTextEditor::getSingletonPtr()->registerCodecFactory("frag",         matCodecFactory);
    GenericTextEditor::getSingletonPtr()->registerCodecFactory("vert",         matCodecFactory);
    GenericTextEditor::getSingletonPtr()->registerCodecFactory("program",      matCodecFactory);
    GenericTextEditor::getSingletonPtr()->registerCodecFactory("compositor",   matCodecFactory);

    OgitorsRoot::getSingletonPtr()->RegisterEditorFactory(identifier, OGRE_NEW CMaterialEditorFactory());
    OgitorsRoot::getSingletonPtr()->RegisterEditorFactory(identifier, OGRE_NEW CTechniqueEditorFactory());
	OgitorsRoot::getSingletonPtr()->RegisterDockWidget(identifier, dockWidgetData);
    OgitorsRoot::getSingletonPtr()->RegisterPreferenceEditor(identifier, preferencesEditorData);

    return true;
}
//----------------------------------------------------------------------------
bool dllGetPluginName(Ogre::String& name)
{
    name = "Material Editor Plugin";
    return true;
}
//----------------------------------------------------------------------------
bool dllStopPlugin(void)
{
    return true;
}
//----------------------------------------------------------------------------
