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
#include "EntityEditor.h"
#include "SceneManagerEditor.h"
#include "OBBoxRenderable.h"
#include "tinyxml.h"

using namespace Ogitors;

//-----------------------------------------------------------------------------------------
CEntityEditor::CEntityEditor(CBaseEditorFactory *factory) : CNodeEditor(factory)
{
    mEntityHandle = 0;
    mUsesGizmos = true;
    mUsesHelper = false;
    mUsingPlaceHolderMesh = false;
}
//-----------------------------------------------------------------------------------------
void CEntityEditor::onDropMaterial(Ogre::Ray ray, Ogre::Vector3 position, const Ogre::String& materialname)
{
    if(mEntityHandle && !mUsingPlaceHolderMesh)
    {
        int submeshid = OgitorsUtils::PickSubMesh(ray, mEntityHandle);
        if(submeshid > -1)
        {

            CSceneManagerEditor *mSceneMgr = mOgitorsRoot->GetSceneManagerEditor();

            if(mSceneMgr->getShadowsEnabled())
            {
                Ogre::MaterialPtr matEnt = Ogre::MaterialManager::getSingletonPtr()->getByName(materialname); 
                mEntityHandle->getSubEntity(submeshid)->setMaterial(mSceneMgr->buildDepthShadowMaterial(matEnt));
            }
            else
                mEntityHandle->getSubEntity(submeshid)->setMaterialName(materialname);

            Ogre::String propName = "subentity" + Ogre::StringConverter::toString(submeshid) + "::material";
            mProperties.setValue(propName, materialname);
        }
    }

}
//-----------------------------------------------------------------------------------------
bool CEntityEditor::setLayerImpl(unsigned int newlayer)
{
    if(mEntityHandle)
        mEntityHandle->setVisibilityFlags(1 << newlayer);

    return true;
}
//-----------------------------------------------------------------------------------------
bool CEntityEditor::_setMeshFile(OgitorsPropertyBase* property, const Ogre::String& value)
{
    if(value.empty())
        return false;
    
    bool wasLoaded = mLoaded->get();
    bool showBB = mOBBoxRenderable && mOBBoxRenderable->getVisible(); 

    unLoad();

    int count = mSubEntityCount->get();
    for(int ix = 0;ix < count;ix++)
    {
        Ogre::String name = "subentity" + Ogre::StringConverter::toString(ix);
        mProperties.removeProperty(name + "::material");
        mProperties.removeProperty(name + "::visible");
    }

    mSubEntityCount->set(-1);
    
    if(wasLoaded)
        load();

    if(showBB)
        showBoundingBox(true);

    return true;
}
//-----------------------------------------------------------------------------------------
bool CEntityEditor::_setCastShadows(OgitorsPropertyBase* property, const bool& value)
{
    if(mEntityHandle)
    {
        mEntityHandle->setCastShadows(value);
    } 
    return true;
}
//-----------------------------------------------------------------------------------------
bool CEntityEditor::_setSubMaterial(OgitorsPropertyBase* property, const Ogre::String& value)
{
    if(mEntityHandle && !mUsingPlaceHolderMesh)
    {
        CSceneManagerEditor *mSceneMgr = mOgitorsRoot->GetSceneManagerEditor();

        if(mSceneMgr->getShadowsEnabled())
        {
            Ogre::MaterialPtr matEnt = Ogre::MaterialManager::getSingletonPtr()->getByName(value);
            mEntityHandle->getSubEntity(property->getTag())->setMaterial(mSceneMgr->buildDepthShadowMaterial(matEnt));
        }
        else
            mEntityHandle->getSubEntity(property->getTag())->setMaterialName(value);
    } 
    return true;
}
//-----------------------------------------------------------------------------------------
bool CEntityEditor::_setSubVisible(OgitorsPropertyBase* property, const bool& value)
{
    if(mEntityHandle && !mUsingPlaceHolderMesh)
    {
        mEntityHandle->getSubEntity(property->getTag())->setVisible(value);
    } 
    return true;
}
//-----------------------------------------------------------------------------------------
bool CEntityEditor::_setRenderingDistance(OgitorsPropertyBase* property, const Ogre::Real& distance)
{
    if(mEntityHandle)
    {
        mEntityHandle->setRenderingDistance(distance);
    } 
    return true;
}
//-----------------------------------------------------------------------------------------
void CEntityEditor::createProperties(OgitorsPropertyValueMap &params)
{
    PROPERTY_PTR(mMeshFile    , "meshfile"    , Ogre::String,""   ,0, SETTER(Ogre::String, CEntityEditor, _setMeshFile));
    PROPERTY_PTR(mCastShadows , "castshadows" , bool        ,false,0, SETTER(bool, CEntityEditor, _setCastShadows));
    PROPERTY_PTR(mSubEntityCount, "subentitycount", int     , -1  ,0, 0);
    PROPERTY_PTR(mRenderingDistance, "renderingdistance",Ogre::Real     ,5000,0, SETTER(Ogre::Real, CEntityEditor, _setRenderingDistance));
    
    int count = 0;
    OgitorsPropertyValueMap::const_iterator it = params.find("subentitycount");
    if(it != params.end())
        count = Ogre::any_cast<int>(it->second.val);

    OgitorsPropertyDef *definition;
    for(int ix = 0;ix < count;ix++)
    {
        Ogre::String sCount1 = "SubEntities::SubEntity" + Ogre::StringConverter::toString(ix);
        Ogre::String sCount2 = "subentity" + Ogre::StringConverter::toString(ix);
        definition = mFactory->AddPropertyDefinition(sCount2 + "::material", sCount1 + "::Material", "Sub Entity's Material Name", PROP_STRING, true, true);
        definition->setOptions(OgitorsRoot::GetMaterialNames());
        mFactory->AddPropertyDefinition(sCount2 + "::visible", sCount1 + "::Visible", "Sub Entity's Visibility", PROP_BOOL, true, true);
        PROPERTY(sCount2 + "::material", Ogre::String, "", ix, SETTER(Ogre::String, CEntityEditor, _setSubMaterial)); 
        PROPERTY(sCount2 + "::visible", bool, true, ix, SETTER(bool, CEntityEditor, _setSubVisible)); 
    }

    mProperties.initValueMap(params);

    Ogre::String addstr = mMeshFile->get();
    int ret = addstr.find(".mesh");
    if(ret != -1)
    {
       addstr.erase(ret, 5);
       mMeshFile->init(addstr);
    }
}
//-----------------------------------------------------------------------------------------
bool CEntityEditor::load(bool async)
{
    if(mLoaded->get())
        return true;
    
    if(CNodeEditor::load())
    {
        CSceneManagerEditor *mSceneMgr = mOgitorsRoot->GetSceneManagerEditor();

        try
        {
            mEntityHandle = mOgitorsRoot->GetSceneManager()->createEntity(mName->get(), mMeshFile->get() + ".mesh");
            mUsingPlaceHolderMesh = false;
        }
        catch(...)
        {
            mUsingPlaceHolderMesh = true;
            mEntityHandle = mOgitorsRoot->GetSceneManager()->createEntity(mName->get(), "missing_mesh.mesh");
            mEntityHandle->setMaterialName("MAT_GIZMO_X_L");
        }

        mHandle->attachObject(mEntityHandle);
        mEntityHandle->setQueryFlags(QUERYFLAG_MOVABLE);
        mEntityHandle->setCastShadows(mCastShadows->get());
        mEntityHandle->setVisibilityFlags(1 << mLayer->get());
        mEntityHandle->setRenderingDistance(mRenderingDistance->get());
        
        OgitorsPropertyDef *definition;
        if(mSubEntityCount->get() == -1)
        {
            int count = mEntityHandle->getNumSubEntities();
            mSubEntityCount->set(count);
            for(int ix = 0;ix < count;ix++)
            {
                Ogre::String sCount1 = "SubEntities::SubEntity" + Ogre::StringConverter::toString(ix);
                Ogre::String sCount2 = "subentity" + Ogre::StringConverter::toString(ix);
                definition = mFactory->AddPropertyDefinition(sCount2 + "::material", sCount1 + "::Material", "Sub Entity's Material Name", PROP_STRING, true, true);
                definition->setOptions(OgitorsRoot::GetMaterialNames());
                mFactory->AddPropertyDefinition(sCount2 + "::visible", sCount1 + "::Visible", "Sub Entity's Visibility", PROP_BOOL, true, true);
                PROPERTY(sCount2 + "::material", Ogre::String, mEntityHandle->getSubEntity(ix)->getMaterialName(), ix, SETTER(Ogre::String, CEntityEditor, _setSubMaterial)); 
                PROPERTY(sCount2 + "::visible", bool, mEntityHandle->getSubEntity(ix)->isVisible(), ix, SETTER(bool, CEntityEditor, _setSubVisible)); 

                if(mSceneMgr->getShadowsEnabled())
                {
                    Ogre::MaterialPtr matEnt = Ogre::MaterialManager::getSingletonPtr()->getByName(mEntityHandle->getSubEntity(ix)->getMaterialName());
                    mEntityHandle->getSubEntity(ix)->setMaterial(mSceneMgr->buildDepthShadowMaterial(matEnt));
                }
            }
        }
        else if(!mUsingPlaceHolderMesh)
        {
            Ogre::String propname;
            Ogre::String material;
            bool         visible;

            int count = mSubEntityCount->get();
            for(int ix = 0;ix < count;ix++)
            {
                propname = "subentity" + Ogre::StringConverter::toString(ix);
                mProperties.getValue(propname + "::material", material);
                mProperties.getValue(propname + "::visible", visible);
                if(mSceneMgr->getShadowsEnabled())
                {
                    Ogre::MaterialPtr matEnt = Ogre::MaterialManager::getSingletonPtr()->getByName(material);
                    mEntityHandle->getSubEntity(ix)->setMaterial(mSceneMgr->buildDepthShadowMaterial(matEnt));
                }
                else
                    mEntityHandle->getSubEntity(ix)->setMaterialName(material);
                mEntityHandle->getSubEntity(ix)->setVisible(visible);
            }
        }

        CONNECT_PROPERTY_MEMFN(mSceneMgr, "shadows::enabled", CEntityEditor, OnShadowsChange, mShadowsConnection[0]);
        CONNECT_PROPERTY_MEMFN(mSceneMgr, "shadows::technique", CEntityEditor, OnShadowsTechniqueChange, mShadowsConnection[1]);
    }
    else
        return false;

    return true;
}
//-----------------------------------------------------------------------------------------
bool CEntityEditor::unLoad()
{
    if(!mLoaded->get())
        return true;

    if(mEntityHandle)
    {
        mEntityHandle->detachFromParent();
        mEntityHandle->_getManager()->destroyEntity(mEntityHandle);
        mEntityHandle = 0;
    }
    
    return CNodeEditor::unLoad();
}
//-----------------------------------------------------------------------------------------
TiXmlElement* CEntityEditor::exportDotScene(TiXmlElement *pParent)
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
    // entity
    TiXmlElement *pEntity = pNode->InsertEndChild(TiXmlElement("entity"))->ToElement();
    pEntity->SetAttribute("name", mName->get().c_str());
    pEntity->SetAttribute("meshFile", (mMeshFile->get() + ".mesh").c_str());
    pEntity->SetAttribute("castShadows", Ogre::StringConverter::toString(mCastShadows->get()).c_str());

    int count = mSubEntityCount->get();
    for(int ix = 0;ix < count;ix++)
    {
        Ogre::String material;
        Ogre::String propname = "subentity" + Ogre::StringConverter::toString(ix);
        mProperties.getValue(propname + "::material", material);

        TiXmlElement *pSubEntity = pEntity->InsertEndChild(TiXmlElement("subentity"))->ToElement();
        pSubEntity->SetAttribute("index", Ogre::StringConverter::toString(ix).c_str());
        pSubEntity->SetAttribute("materialName", material.c_str());
    }

    return pEntity;
}
//-----------------------------------------------------------------------------------------
void CEntityEditor::OnShadowsChange(const OgitorsPropertyBase* property, Ogre::Any value)
{
    if(mUsingPlaceHolderMesh)
        return;

    bool newstate = Ogre::any_cast<bool>(value);
    
    CSceneManagerEditor *mSceneMgr = mOgitorsRoot->GetSceneManagerEditor();

    int count = mSubEntityCount->get();
    for(int ix = 0;ix < count;ix++)
    {
        if(newstate)
        {
            Ogre::MaterialPtr matEnt = mEntityHandle->getSubEntity(ix)->getMaterial(); 
            mEntityHandle->getSubEntity(ix)->setMaterial(mSceneMgr->buildDepthShadowMaterial(matEnt));
        }
        else
        {
            Ogre::String propName = "subentity" + Ogre::StringConverter::toString(ix) + "::material";
            Ogre::String matName;
            mProperties.getValue(propName, matName);
            mEntityHandle->getSubEntity(ix)->setMaterialName(matName);
        }
    }
}
//-----------------------------------------------------------------------------------------
void CEntityEditor::OnShadowsTechniqueChange(const OgitorsPropertyBase* property, Ogre::Any value)
{
    if(mUsingPlaceHolderMesh)
        return;

    CSceneManagerEditor *mSceneMgr = mOgitorsRoot->GetSceneManagerEditor();

    bool newstate = mSceneMgr->getShadowsEnabled();
    
    int count = mSubEntityCount->get();
    for(int ix = 0;ix < count;ix++)
    {
        if(newstate)
        {
            Ogre::MaterialPtr matEnt = mEntityHandle->getSubEntity(ix)->getMaterial(); 
            mEntityHandle->getSubEntity(ix)->setMaterial(mSceneMgr->buildDepthShadowMaterial(matEnt));
        }
        else
        {
            Ogre::String propName = "subentity" + Ogre::StringConverter::toString(ix) + "::material";
            Ogre::String matName;
            mProperties.getValue(propName, matName);
            mEntityHandle->getSubEntity(ix)->setMaterialName(matName);
        }
    }
}
//-----------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------
//-----------CENTITYEDITORFACTORY----------------------------------------------------------
//-----------------------------------------------------------------------------------------
CEntityEditorFactory::CEntityEditorFactory(OgitorsView *view) : CNodeEditorFactory(view)
{
    mTypeName = "Entity Object";
    mEditorType = ETYPE_ENTITY;
    mAddToObjectList = false;
    mRequirePlacement = true;
    mIcon = "entity.svg";
    mCapabilities = CAN_PAGE | CAN_MOVE | CAN_SCALE | CAN_ROTATE | CAN_CLONE | CAN_DELETE  | CAN_FOCUS | CAN_DRAG | CAN_UNDO | CAN_USEMATERIAL | CAN_ACCEPTCOPY;

    OgitorsPropertyDef *definition = AddPropertyDefinition("meshfile","Mesh File","The mesh filename of the entity.",PROP_STRING, true, true);
    definition->setOptions(OgitorsRoot::GetModelNames());
    AddPropertyDefinition("castshadows","Cast Shadows","Does the entity cast shadows?",PROP_BOOL);
    AddPropertyDefinition("subentitycount","","Number of sub entities this entity has?",PROP_INT, false, false);
    AddPropertyDefinition("renderingdistance","Rendering Distance","The maximum distance to render meshes.",PROP_REAL, false, false);

    OgitorsPropertyDefMap::iterator it = mPropertyDefs.find("layer");
    it->second.setAccess(true, true);
}
//-----------------------------------------------------------------------------------------
CBaseEditorFactory *CEntityEditorFactory::duplicate(OgitorsView *view)
{
    CBaseEditorFactory *ret = OGRE_NEW CEntityEditorFactory(view);
    ret->mTypeID = mTypeID;

    return ret;
}
//-----------------------------------------------------------------------------------------
CBaseEditor *CEntityEditorFactory::CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params)
{
    CEntityEditor *object = OGRE_NEW CEntityEditor(this);

    OgitorsPropertyValueMap::iterator ni;

    if ((ni = params.find("init")) != params.end())
    {
        Ogre::String entName = Ogre::any_cast<Ogre::String>(params["meshfile"].val);
        int pos = entName.find(".");
        if(pos != -1)
            entName.erase(pos, entName.length() - pos);
        
        OgitorsPropertyValue value = EMPTY_PROPERTY_VALUE;
        value.val = Ogre::Any(CreateUniqueID(entName));
        params["name"] = value;
        params.erase(ni);
    }

    object->createProperties(params);
    object->mParentEditor->init(*parent);
    object->mRenderingDistance->connectTo(OgitorsRoot::getSingletonPtr()->GetSceneManagerEditor()->getProperties()->getProperty("renderingdistance"));

    mInstanceCount++;
    return object;
}
//-----------------------------------------------------------------------------------------
