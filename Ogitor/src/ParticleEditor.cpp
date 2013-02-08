/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2013 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
///
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
#include "NodeEditor.h"
#include "ParticleEditor.h"
#include "tinyxml.h"

using namespace Ogitors;

//-------------------------------------------------------------------------------
CParticleEditor::CParticleEditor(CBaseEditorFactory *factory) : CNodeEditor(factory),
mEntityHandle(0), mParticleHandle(0)
{
}
//-------------------------------------------------------------------------------
Ogre::AxisAlignedBox CParticleEditor::getAABB() 
{
    if(mEntityHandle) 
        return mEntityHandle->getBoundingBox();

    if(mParticleHandle)
    {
        Ogre::AxisAlignedBox box = mParticleHandle->getBoundingBox();
        box.scale(mScale->get());
        return box;
    }
    else
        return Ogre::AxisAlignedBox::BOX_NULL;
}
//-----------------------------------------------------------------------------------------
bool CParticleEditor::setLayerImpl(unsigned int newlayer)
{
    if(mParticleHandle)
        mParticleHandle->setVisibilityFlags(1 << newlayer);

    if(mEntityHandle)
        mEntityHandle->setVisibilityFlags(1 << newlayer);

    return true;
}
//-------------------------------------------------------------------------------
bool CParticleEditor::_setParticle(OgitorsPropertyBase* property, const Ogre::String& particle)
{
    unLoad();
    mParticleSystem->init(particle);
    load();
    return true;
}
//-----------------------------------------------------------------------------------------
bool CParticleEditor::_setCastShadows(OgitorsPropertyBase* property, const bool& value)
{
    if(mParticleHandle)
    {
        mParticleHandle->setCastShadows(value);
    }
    return true;
}
//-----------------------------------------------------------------------------------------
void CParticleEditor::createProperties(OgitorsPropertyValueMap &params)
{
    PROPERTY_PTR(mParticleSystem, "particle",Ogre::String, "EmptyParticle", 0, SETTER(Ogre::String, CParticleEditor, _setParticle));
    PROPERTY_PTR(mCastShadows, "castshadows",bool, true, 0, SETTER(bool, CParticleEditor, _setCastShadows));

    mProperties.initValueMap(params);
}    
//-----------------------------------------------------------------------------------------
bool CParticleEditor::load(bool async)
{
    if(mLoaded->get())
        return true;
    
    if(CNodeEditor::load())
    {
        mParticleHandle = mOgitorsRoot->GetSceneManager()->createParticleSystem("particle" + mName->get(), mParticleSystem->get());
        mHandle->attachObject(mParticleHandle);
        mParticleHandle->setCastShadows(mCastShadows->get());
        mParticleHandle->setKeepParticlesInLocalSpace(true);
        mParticleHandle->setVisible(true);
        Ogre::ParticleSystem::setDefaultNonVisibleUpdateTimeout(5);

        mEntityHandle = mOgitorsRoot->GetSceneManager()->createEntity(mName->get(), "scbLight_Omni.mesh");
        mHandle->attachObject(mEntityHandle);
        mEntityHandle->setQueryFlags(QUERYFLAG_MOVABLE);
        mEntityHandle->setCastShadows(false);
        mEntityHandle->setMaterialName("mtPARTICLEBLUE");
        mParticleHandle->setVisibilityFlags(1 << mLayer->get());
        mEntityHandle->setVisibilityFlags(1 << mLayer->get());
    }
    else
        return false;

    return true;
}
//-----------------------------------------------------------------------------------------
bool CParticleEditor::unLoad()
{
    if(!mLoaded->get())
        return true;

    destroyBoundingBox();

    if(mParticleHandle)
    {
        mParticleHandle->detachFromParent();
        mParticleHandle->_getManager()->destroyParticleSystem(mParticleHandle);
        mParticleHandle = 0;
    }

    if(mEntityHandle)
    {
        mEntityHandle->detachFromParent();
        mEntityHandle->_getManager()->destroyEntity(mEntityHandle);
        mEntityHandle = 0;
    }
    
    return CNodeEditor::unLoad();
}
//-----------------------------------------------------------------------------------------
TiXmlElement* CParticleEditor::exportDotScene(TiXmlElement *pParent)
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

    TiXmlElement *pParticleSystem = pNode->InsertEndChild(TiXmlElement("particleSystem"))->ToElement();
    pParticleSystem->SetAttribute("name", mName->get().c_str());
    pParticleSystem->SetAttribute("script", mParticleSystem->get().c_str());

    return pNode;
}
//-----------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------
//--------------CPARTICLEEDITORFACTORY-------------------------------------------
//-------------------------------------------------------------------------------
CParticleEditorFactory::CParticleEditorFactory(OgitorsView *view) : CNodeEditorFactory(view)
{
    mTypeName = "Particle";
    mEditorType = ETYPE_PARTICLE;
    mAddToObjectList = true;
    mRequirePlacement = true;
    mIcon = "particle.svg";
    mCapabilities = CAN_PAGE | CAN_MOVE | CAN_SCALE | CAN_ROTATE | CAN_CLONE | CAN_DELETE  | CAN_FOCUS | CAN_DRAG | CAN_UNDO | CAN_ACCEPTCOPY;
    mUsesGizmos = true;
    mUsesHelper = false;

    OgitorsPropertyDef *definition;
    definition = AddPropertyDefinition("particle","Particle System","The particle name of the object.",PROP_STRING);
    definition->setOptions(OgitorsRoot::GetParticleTemplateNames());
    AddPropertyDefinition("castshadows","Cast Shadows","Does the object cast shadows?",PROP_BOOL);

    OgitorsPropertyDefMap::iterator it = mPropertyDefs.find("layer");
    it->second.setAccess(true, true);
}
//-----------------------------------------------------------------------------------------
CBaseEditorFactory *CParticleEditorFactory::duplicate(OgitorsView *view)
{
    CBaseEditorFactory *ret = OGRE_NEW CParticleEditorFactory(view);
    ret->mTypeID = mTypeID;

    return ret;
}
//-----------------------------------------------------------------------------------------
CBaseEditor *CParticleEditorFactory::CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params)
{
    CParticleEditor *object = OGRE_NEW CParticleEditor(this);

    OgitorsPropertyValueMap::iterator ni;

    if ((ni = params.find("init")) != params.end())
    {
        OgitorsPropertyValue value = EMPTY_PROPERTY_VALUE;
        value.val = Ogre::Any(CreateUniqueID("Particle"));
        params["name"] = value;
        params.erase(ni);
    }

    object->createProperties(params);
    object->mParentEditor->init(*parent);

    mInstanceCount++;
    return object;
}
//-------------------------------------------------------------------------------
