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

#include "OgitorsPrerequisites.h"
#include "BaseEditor.h"
#include "OgitorsRoot.h"
#include "NodeEditor.h"
#include "MarkerEditor.h"
#include "tinyxml.h"

using namespace Ogitors;

//-----------------------------------------------------------------------------------------
CMarkerEditor::CMarkerEditor(CBaseEditorFactory *factory) : CNodeEditor(factory),
mMarkerHandle(0)
{
    mHandle = 0;
    mUsesGizmos = true;
    mUsesHelper = false;
    mMaterial.setNull();
}
//-----------------------------------------------------------------------------------------
bool CMarkerEditor::setLayerImpl(unsigned int newlayer)
{
    if(mMarkerHandle)
        mMarkerHandle->setVisibilityFlags(1 << newlayer);

    return true;
}
//-----------------------------------------------------------------------------------------
bool CMarkerEditor::_setColour(OgitorsPropertyBase* property, const Ogre::ColourValue& colour)
{
    if(!mMaterial.isNull())
    {
        mMaterial->setSelfIllumination(colour);
    }
    return true;
}
//-----------------------------------------------------------------------------------------
void CMarkerEditor::createProperties(OgitorsPropertyValueMap &params)
{
    PROPERTY_PTR(mColour, "colour", Ogre::ColourValue, Ogre::ColourValue(0.2f,0.2f,1.0f), 0, SETTER(Ogre::ColourValue, CMarkerEditor, _setColour));
    mProperties.initValueMap(params);
}
//-----------------------------------------------------------------------------------------
bool CMarkerEditor::load(bool async)
{
    if(mLoaded->get())
        return true;
    
    if(CNodeEditor::load())
    {
        mMaterial = Ogre::MaterialManager::getSingletonPtr()->create("mt" + mName->get(),PROJECT_RESOURCE_GROUP);
        mMaterial->setAmbient(0,0,0);
        mMaterial->setDiffuse(0.1f,0.1f,0.1f,1.0f);
        mMaterial->setSpecular(0,0,0,1);
        mMaterial->setSelfIllumination(mColour->get());

        mMarkerHandle = mOgitorsRoot->GetSceneManager()->createEntity(mName->get(),"scbMarker.mesh");
        mHandle->attachObject(mMarkerHandle);

        mMarkerHandle->setVisibilityFlags(1 << mLayer->get());
        mMarkerHandle->setQueryFlags(QUERYFLAG_MOVABLE);
        mMarkerHandle->setCastShadows(false);

        mMarkerHandle->setMaterial(mMaterial);
    }
    else
        return false;

    return true;
}
//-----------------------------------------------------------------------------------------
bool CMarkerEditor::unLoad()
{
    if(!mLoaded->get())
        return true;

    if(mMarkerHandle)
    {
        mMarkerHandle->detachFromParent();
        mMarkerHandle->_getManager()->destroyEntity(mMarkerHandle);
        mMarkerHandle = 0;
    }

    if(!mMaterial.isNull())
    {
        Ogre::MaterialManager::getSingletonPtr()->remove(mMaterial->getName());
    }
    
    return CNodeEditor::unLoad();
}
//-----------------------------------------------------------------------------------------
TiXmlElement* CMarkerEditor::exportDotScene(TiXmlElement *pParent)
{
    TiXmlElement *pNode = pParent->InsertEndChild(TiXmlElement("node"))->ToElement();

    // node properties
    pNode->SetAttribute("name", mName->get().c_str());
    pNode->SetAttribute("id", Ogre::StringConverter::toString(mObjectID->get()).c_str());
    // position
    TiXmlElement *pPosition = pNode->InsertEndChild(TiXmlElement("position"))->ToElement();
    pPosition->SetAttribute("x", Ogre::StringConverter::toString(mPosition->get().x).c_str());
    pPosition->SetAttribute("y", Ogre::StringConverter::toString(mPosition->get().y).c_str());
    pPosition->SetAttribute("z", Ogre::StringConverter::toString(mPosition->get().z).c_str());
    // rotation
    TiXmlElement *pRotation = pNode->InsertEndChild(TiXmlElement("rotation"))->ToElement();
    pRotation->SetAttribute("qw", Ogre::StringConverter::toString(mOrientation->get().w).c_str());
    pRotation->SetAttribute("qx", Ogre::StringConverter::toString(mOrientation->get().x).c_str());
    pRotation->SetAttribute("qy", Ogre::StringConverter::toString(mOrientation->get().y).c_str());
    pRotation->SetAttribute("qz", Ogre::StringConverter::toString(mOrientation->get().z).c_str());
    // scale
    TiXmlElement *pScale = pNode->InsertEndChild(TiXmlElement("scale"))->ToElement();
    pScale->SetAttribute("x", Ogre::StringConverter::toString(mScale->get().x).c_str());
    pScale->SetAttribute("y", Ogre::StringConverter::toString(mScale->get().y).c_str());
    pScale->SetAttribute("z", Ogre::StringConverter::toString(mScale->get().z).c_str());

    return pNode;
}
//-----------------------------------------------------------------------------------------
//------CMARKEREDITORFACTORY---------------------------------------------------------------
//-----------------------------------------------------------------------------------------
CMarkerEditorFactory::CMarkerEditorFactory(OgitorsView *view) : CNodeEditorFactory(view)
{
    mTypeName = "Marker";
    mEditorType = ETYPE_MOVABLE;
    mAddToObjectList = true;
    mRequirePlacement = true;
    mIcon = "marker.svg";
    mCapabilities = CAN_PAGE | CAN_MOVE | CAN_ROTATE | CAN_CLONE | CAN_DELETE  | CAN_FOCUS | CAN_DRAG | CAN_UNDO | CAN_ACCEPTCOPY;

    AddPropertyDefinition("colour", "Colour", "The colour of the object.",PROP_COLOUR);

    OgitorsPropertyDefMap::iterator it = mPropertyDefs.find("layer");
    it->second.setAccess(true, true);
}
//-----------------------------------------------------------------------------------------
CBaseEditorFactory *CMarkerEditorFactory::duplicate(OgitorsView *view)
{
    CBaseEditorFactory *ret = OGRE_NEW CMarkerEditorFactory(view);
    ret->mTypeID = mTypeID;

    return ret;
}
//-----------------------------------------------------------------------------------------
CBaseEditor *CMarkerEditorFactory::CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params)
{
    CMarkerEditor *object = OGRE_NEW CMarkerEditor(this);

    OgitorsPropertyValueMap::iterator ni;

    if ((ni = params.find("init")) != params.end())
    {
        OgitorsPropertyValue value = EMPTY_PROPERTY_VALUE;
        value.val = Ogre::Any(CreateUniqueID("Marker"));
        params["name"] = value;
        params.erase(ni);
    }

    object->createProperties(params);
    object->mParentEditor->init(*parent);

    mInstanceCount++;
    return object;
}
//-----------------------------------------------------------------------------------------
