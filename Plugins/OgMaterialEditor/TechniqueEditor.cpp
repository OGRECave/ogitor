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
#include "TechniqueEditor.h"
#include "OgreMaterialManager.h"

namespace Ogitors
{

//-----------------------------------------------------------------------------------------
CTechniqueEditor::CTechniqueEditor(CBaseEditorFactory *factory) : CBaseEditor(factory), mPropertyLevel(1),
mPropertyID(0)
{
    mHandle = 0;
    mHelper = 0;
    mUsesGizmos = false;
    mUsesHelper = false;
}
//-----------------------------------------------------------------------------------------
CTechniqueEditor::~CTechniqueEditor()
{

}
//-----------------------------------------------------------------------------------------
void CTechniqueEditor::showBoundingBox(bool bShow) 
{
}
//-----------------------------------------------------------------------------------------
bool CTechniqueEditor::getObjectContextMenu(UTFStringVector &menuitems) 
{
    /*menuitems.clear();
    if(getParent() && getParent()->getLocked())
        return false;

    if(mPropertyLevel == 0)
    {
        menuitems.push_back(OTR("Add Technique") + ";:/icons/technique.svg");
        return true;
    }
    else if(mPropertyLevel == 1)
    {
        if(mHandle->getNumTechniques() > 1)
            menuitems.push_back(OTR("Delete Technique") + ";:/icons/trash.svg");
        else
            menuitems.push_back("");
        menuitems.push_back(OTR("Add Pass") + ";:/icons/pass.svg");
        return true;
    }
    else if(mPropertyLevel == 2)
    {
        if(mHandle->getTechnique(mPropertyID >> 8)->getNumPasses() > 1)
        {
            menuitems.push_back(OTR("Delete Pass") + ";:/icons/trash.svg");
            return true;
        }
    }*/

    return false;
}
//-------------------------------------------------------------------------------
void CTechniqueEditor::onObjectContextMenu(int menuresult) 
{
    /*if(mHandle.isNull())
        return;

    Ogre::Pass      *pass;
    Ogre::Technique *tech;

    if(mPropertyLevel == 0)
    {
        if(menuresult == 0)
        {
            tech = mHandle->createTechnique();
        }
    }
    else if(mPropertyLevel == 1)
    {
        switch(menuresult)
        {
        case 0:mHandle->removeTechnique(mPropertyID >> 8);break;
        case 1:pass = mHandle->getTechnique(mPropertyID >> 8)->createPass();break;
        };
    }
    else if(mPropertyLevel == 2)
    {
        if(menuresult == 0)
            mHandle->getTechnique(mPropertyID >> 8)->removePass(mPropertyID & 0xFF);
    }*/
}
//-----------------------------------------------------------------------------------------
bool CTechniqueEditor::load(bool async)
{
    if(mLoaded->get())
        return true;

    Ogre::MaterialPtr tmp = Ogre::MaterialManager::getSingletonPtr()->getByName(mMaterialName);
    mHandle = tmp->getTechnique(mName->get());

    mLoaded->set(true);
    return true;
}
//-----------------------------------------------------------------------------------------
bool CTechniqueEditor::unLoad()
{
    if(!mLoaded->get())
        return true;

    mHandle = 0;

    mLoaded->set(false);
    return true;
}
//-----------------------------------------------------------------------------------------
void CTechniqueEditor::createProperties(OgitorsPropertyValueMap &params)
{    
    mProperties.initValueMap(params);
}
//-----------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------
//------CMATERIALEDITORFACTORY-----------------------------------------------------------------
//-----------------------------------------------------------------------------------------
CTechniqueEditorFactory::CTechniqueEditorFactory(OgitorsView *view) : CBaseEditorFactory(view)
{
    mTypeName = "Technqiue Object";
    mEditorType = ETYPE_GENERALPURPOSE;
    mAddToObjectList = false;
    mRequirePlacement = true;
    mIcon = "technique.svg";
    mCapabilities = CAN_UNDO;
}
//-----------------------------------------------------------------------------------------
CBaseEditorFactory *CTechniqueEditorFactory::duplicate(OgitorsView *view)
{
    CBaseEditorFactory *ret = new CTechniqueEditorFactory(view);
    ret->mTypeID = mTypeID;

    return ret;
}
//-----------------------------------------------------------------------------------------
CBaseEditor *CTechniqueEditorFactory::CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params)
{
    CTechniqueEditor *object = new CTechniqueEditor(this);
    OgitorsPropertyValueMap::iterator ni;

    if((ni = params.find("init")) != params.end())
    {
        OgitorsPropertyValue value = EMPTY_PROPERTY_VALUE;
        value.val = Ogre::Any(CreateUniqueID("Technique"));
        params["name"] = value;
        params.erase(ni);
    }

    object->createProperties(params);
    object->mParentEditor->init(*parent);

    mInstanceCount++;
    return object;
}
//-----------------------------------------------------------------------------------------
}
