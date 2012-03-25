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

#include "OgitorsPrerequisites.h"
#include "BaseEditor.h"
#include "OgitorsRoot.h"
#include "OgitorsSystem.h"
#include "FolderEditor.h"

using namespace Ogitors;

//-----------------------------------------------------------------------------------------
CFolderEditor::CFolderEditor(CBaseEditorFactory *factory) : CBaseEditor(factory) 
{
    mUsesGizmos = false;
    mUsesHelper = false;
}
//-----------------------------------------------------------------------------------------
CFolderEditor::~CFolderEditor()
{

}
//-----------------------------------------------------------------------------------------
Ogre::SceneNode *CFolderEditor::getNode()
{
    return mOgitorsRoot->GetSceneManager()->getRootSceneNode();
}
//-----------------------------------------------------------------------------------------
Ogre::SceneManager *CFolderEditor::getSceneManager()
{
    return mOgitorsRoot->GetSceneManager();
}
//-----------------------------------------------------------------------------------------
bool CFolderEditor::setNameImpl(Ogre::String name)
{
    mSystem->SetTreeItemText(this, name);
    
    return true;
}
//-----------------------------------------------------------------------------------------
bool CFolderEditor::getObjectContextMenu(UTFStringVector &menuitems) 
{
    return false;
}
//-------------------------------------------------------------------------------
void CFolderEditor::onObjectContextMenu(int menuresult) 
{
}
//-----------------------------------------------------------------------------------------
bool CFolderEditor::load(bool async)
{
    if(mLoaded->get())
        return true;

    if(!getParent()->load())
        return false;
    
    mLoaded->set(true);
    return true;
}
//-----------------------------------------------------------------------------------------
bool CFolderEditor::unLoad()
{
    if(!mLoaded->get())
        return true;

    unLoadAllChildren();
    destroyBoundingBox();

    mLoaded->set(false);

    return true;
}
//-----------------------------------------------------------------------------------------
void CFolderEditor::createProperties(OgitorsPropertyValueMap &params)
{
    mProperties.initValueMap(params);
}
//-----------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------
//------CFolderEDITORFACTORY-----------------------------------------------------------------
//-----------------------------------------------------------------------------------------
CFolderEditorFactory::CFolderEditorFactory(OgitorsView *view) : CBaseEditorFactory(view)
{
    mTypeName = "Folder Object";
    mEditorType = ETYPE_FOLDER;
    mAddToObjectList = true;
    mRequirePlacement = false;
    mIcon = "folder.svg";
    mCapabilities = CAN_DELETE | CAN_DROP | CAN_UNDO;

    //OgitorsPropertyDef *definition;

    //AddPropertyDefinition("position","Position","The position of the object.",PROP_VECTOR3);
    //AddPropertyDefinition("orientation","Orientation","The orientation of the object.",PROP_QUATERNION,true,false);
    //AddPropertyDefinition("scale","Scale","The scale of the object.", PROP_VECTOR3);
    //definition = AddPropertyDefinition("autotracktarget","Tracking Target","The object's tracking target.",PROP_STRING);
    //definition->setOptions(OgitorsRoot::GetAutoTrackTargets());

    OgitorsPropertyDefMap::iterator it = mPropertyDefs.find("layer");
    it->second.setAccess(false, false);
}
//-----------------------------------------------------------------------------------------
CBaseEditorFactory *CFolderEditorFactory::duplicate(OgitorsView *view)
{
    CBaseEditorFactory *ret = OGRE_NEW CFolderEditorFactory(view);
    ret->mTypeID = mTypeID;

    return ret;
}
//-----------------------------------------------------------------------------------------
CBaseEditor *CFolderEditorFactory::CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params)
{
    CFolderEditor *object = OGRE_NEW CFolderEditor(this);

    OgitorsPropertyValueMap::iterator ni;

    if ((ni = params.find("init")) != params.end())
    {
        OgitorsPropertyValue value = EMPTY_PROPERTY_VALUE;
        value.val = Ogre::Any(CreateUniqueID("Folder"));
        params["name"] = value;
        params.erase(ni);
    }
    
    object->createProperties(params);
    object->mParentEditor->init(*parent);

    mInstanceCount++;
    return object;
}
//-----------------------------------------------------------------------------------------
