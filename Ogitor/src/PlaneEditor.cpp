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
#include "PlaneEditor.h"
#include "SceneManagerEditor.h"
#include "tinyxml.h"

using namespace Ogitors;

//-------------------------------------------------------------------------------
CPlaneEditor::CPlaneEditor(CBaseEditorFactory *factory) : CNodeEditor(factory)
{
    mPlaneHandle = 0;
}
//-------------------------------------------------------------------------------
void CPlaneEditor::onDropMaterial(Ogre::Ray ray, Ogre::Vector3 position, const Ogre::String& materialname)
{
    mMaterial->set(materialname);
}
//-------------------------------------------------------------------------------
Ogre::Entity *CPlaneEditor::_createPlane()
{
    Ogre::MeshPtr mesh;
    if(mPlaneHandle)
    {
        mPlaneHandle->detachFromParent();
        mPlaneHandle->_getManager()->destroyEntity(mPlaneHandle);
        Ogre::MeshManager::getSingletonPtr()->remove(mName->get());        
    }

    Ogre::Plane plane(mNormal->get(), mDistance->get());
    
    mesh = Ogre::MeshManager::getSingletonPtr()->createPlane(mName->get(),PROJECT_RESOURCE_GROUP,plane,mWidth->get(),mHeight->get(),mxSegments->get(),mySegments->get(),mHasNormals->get(),mNumCoordSets->get(),muTile->get(),mvTile->get(),Ogre::Vector3::UNIT_Z);
    mPlaneHandle = mOgitorsRoot->GetSceneManager()->createEntity(mName->get(), mName->get());
    mPlaneHandle->setQueryFlags(QUERYFLAG_MOVABLE);

    CSceneManagerEditor *mSceneMgr = mOgitorsRoot->GetSceneManagerEditor();
    if(mSceneMgr->getShadowsEnabled())
    {
#if (OGRE_VERSION < ((1 << 16) | (9 << 8) | 0))
        Ogre::MaterialPtr matEnt = Ogre::MaterialManager::getSingletonPtr()->getByName(mMaterial->get());
#else
        Ogre::MaterialPtr matEnt = Ogre::MaterialManager::getSingletonPtr()->getByName(mMaterial->get()).staticCast<Ogre::Material>();
#endif
        mPlaneHandle->setMaterial(mSceneMgr->buildDepthShadowMaterial(matEnt));
    }
    else
        mPlaneHandle->setMaterialName(mMaterial->get());

    mPlaneHandle->setCastShadows(mCastShadows->get());
    mPlaneHandle->setVisibilityFlags(1 << mLayer->get());

    mHandle->attachObject(mPlaneHandle);

    if(mBBoxNode)
        adjustBoundingBox();

    return mPlaneHandle;
}
//-----------------------------------------------------------------------------------------
bool CPlaneEditor::setLayerImpl(unsigned int newlayer)
{
    if(mPlaneHandle)
        mPlaneHandle->setVisibilityFlags(1 << newlayer);

    return true;
}
//-------------------------------------------------------------------------------
bool CPlaneEditor::_setNormal(OgitorsPropertyBase* property, const Ogre::Vector3& value)
{
    if(value == Ogre::Vector3::ZERO)
        mNormal->init(mNormal->getOld());

    if(mPlaneHandle)
        _createPlane();

    return true;
}
//-----------------------------------------------------------------------------------------
bool CPlaneEditor::_setDistance(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(mPlaneHandle)
        _createPlane();

    return true;
}
//-----------------------------------------------------------------------------------------
bool CPlaneEditor::_setWidth(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(mPlaneHandle)
        _createPlane();

    return true;
}
//-----------------------------------------------------------------------------------------
bool CPlaneEditor::_setHeight(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(mPlaneHandle)
        _createPlane();

    return true;
}
//-----------------------------------------------------------------------------------------
bool CPlaneEditor::_setXSegments(OgitorsPropertyBase* property, const int& value)
{
    if(value < 1)
        return false;

    // (XSeg + 1) * (YSeg + 1) must not exceed 65536 index buffer size...
    if((value + 1) * (mySegments->get() + 1) > 65536)
        mxSegments->init((65536 / (mySegments->get() + 1)) - 1);

    if(mPlaneHandle)
        _createPlane();

    return true;
}
//-----------------------------------------------------------------------------------------
bool CPlaneEditor::_setYSegments(OgitorsPropertyBase* property, const int& value)
{
    if(value < 1)
        return false;

    // (XSeg + 1) * (YSeg + 1) must not exceed 65536 index buffer size...
    if((value + 1) * (mxSegments->get() + 1) > 65536)
        mySegments->init((65536 / (mxSegments->get() + 1)) - 1);

    if(mPlaneHandle)
        _createPlane();

    return true;
}
//-----------------------------------------------------------------------------------------
bool CPlaneEditor::_setNumCoordSets(OgitorsPropertyBase* property, const int& value)
{
    if(mPlaneHandle)
        _createPlane();

    return true;
}
//-----------------------------------------------------------------------------------------
bool CPlaneEditor::_setUTile(OgitorsPropertyBase* property, const int& value)
{
    if(mPlaneHandle)
        _createPlane();

    return true;
}
//-----------------------------------------------------------------------------------------
bool CPlaneEditor::_setVTile(OgitorsPropertyBase* property, const int& value)
{
    if(mPlaneHandle)
        _createPlane();

    return true;
}
//-----------------------------------------------------------------------------------------
bool CPlaneEditor::_setHasNormals(OgitorsPropertyBase* property, const bool& value)
{
    if(mPlaneHandle)
        _createPlane();

    return true;
}
//-----------------------------------------------------------------------------------------
bool CPlaneEditor::_setMaterial(OgitorsPropertyBase* property, const Ogre::String& value)
{
    if(mPlaneHandle)
    {
        CSceneManagerEditor *mSceneMgr = mOgitorsRoot->GetSceneManagerEditor();
        if(mSceneMgr->getShadowsEnabled())
        {
#if (OGRE_VERSION < ((1 << 16) | (9 << 8) | 0))
            Ogre::MaterialPtr matEnt = Ogre::MaterialManager::getSingletonPtr()->getByName(value);
#else
            Ogre::MaterialPtr matEnt = Ogre::MaterialManager::getSingletonPtr()->getByName(value).staticCast<Ogre::Material>();
#endif
            mPlaneHandle->setMaterial(mSceneMgr->buildDepthShadowMaterial(matEnt));
        }
        else
            mPlaneHandle->setMaterialName(value);
    }
    return true;
}
//-----------------------------------------------------------------------------------------
bool CPlaneEditor::_setCastShadows(OgitorsPropertyBase* property, const bool& value)
{
    if(mPlaneHandle)
    {
        mPlaneHandle->setCastShadows(value);
    }

    return true;
}
//-----------------------------------------------------------------------------------------
void CPlaneEditor::createProperties(OgitorsPropertyValueMap &params)
{
    PROPERTY_PTR(mNormal      , "normal"      ,Ogre::Vector3,Ogre::Vector3::UNIT_Y,0, SETTER(Ogre::Vector3, CPlaneEditor, _setNormal));
    PROPERTY_PTR(mDistance    , "distance"    ,Ogre::Real  ,0   ,0, SETTER(Ogre::Real, CPlaneEditor, _setDistance));
    PROPERTY_PTR(mWidth       , "width"       ,Ogre::Real  ,10  ,0, SETTER(Ogre::Real, CPlaneEditor, _setWidth));
    PROPERTY_PTR(mHeight      , "height"      ,Ogre::Real  ,10  ,0, SETTER(Ogre::Real, CPlaneEditor, _setHeight));
    PROPERTY_PTR(mxSegments   , "xsegments"   ,int         ,2   ,0, SETTER(int, CPlaneEditor, _setXSegments));
    PROPERTY_PTR(mySegments   , "ysegments"   ,int         ,2   ,0, SETTER(int, CPlaneEditor, _setYSegments));
    PROPERTY_PTR(mNumCoordSets, "numcoordsets",int         ,1   ,0, SETTER(int, CPlaneEditor, _setNumCoordSets));
    PROPERTY_PTR(muTile       , "utile"       ,int         ,1   ,0, SETTER(int, CPlaneEditor, _setUTile));
    PROPERTY_PTR(mvTile       , "vtile"       ,int         ,1   ,0, SETTER(int, CPlaneEditor, _setVTile));
    PROPERTY_PTR(mHasNormals  , "hasnormals"  ,bool        ,true,0, SETTER(bool, CPlaneEditor, _setHasNormals));
    PROPERTY_PTR(mMaterial    , "material"    ,Ogre::String,"DefaultPlaneMaterial"  ,0, SETTER(Ogre::String, CPlaneEditor, _setMaterial));
    PROPERTY_PTR(mCastShadows , "castshadows" ,bool        ,true,0, SETTER(bool, CPlaneEditor, _setCastShadows));

    mProperties.initValueMap(params);
}    
//-----------------------------------------------------------------------------------------
bool CPlaneEditor::load(bool async)
{
    if(mLoaded->get())
        return true;
    
    if(CNodeEditor::load())
    {
        _createPlane();
    }
    else
        return false;

    return true;
}
//-----------------------------------------------------------------------------------------
bool CPlaneEditor::unLoad()
{
    if(!mLoaded->get())
        return true;

    if(mPlaneHandle)
    {
        mPlaneHandle->detachFromParent();
        mPlaneHandle->_getManager()->destroyEntity(mPlaneHandle);
        Ogre::MeshManager::getSingletonPtr()->remove(mName->get());        
        mPlaneHandle = 0;
    }
    
    return CNodeEditor::unLoad();
}
//-----------------------------------------------------------------------------------------
TiXmlElement* CPlaneEditor::exportDotScene(TiXmlElement *pParent)
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
    // plane
    TiXmlElement *pPlane = pNode->InsertEndChild(TiXmlElement("plane"))->ToElement();
    pPlane->SetAttribute("name", mName->get().c_str());
    pPlane->SetAttribute("distance", Ogre::StringConverter::toString(mDistance->get()).c_str());
    pPlane->SetAttribute("width", Ogre::StringConverter::toString(mWidth->get()).c_str());
    pPlane->SetAttribute("height", Ogre::StringConverter::toString(mHeight->get()).c_str());
    pPlane->SetAttribute("xSegments", Ogre::StringConverter::toString(mxSegments->get()).c_str());
    pPlane->SetAttribute("ySegments", Ogre::StringConverter::toString(mySegments->get()).c_str());
    pPlane->SetAttribute("numTexCoordSets", Ogre::StringConverter::toString(mNumCoordSets->get()).c_str());
    pPlane->SetAttribute("uTile", Ogre::StringConverter::toString(muTile->get()).c_str());
    pPlane->SetAttribute("vTile", Ogre::StringConverter::toString(mvTile->get()).c_str());
    pPlane->SetAttribute("material", mMaterial->get().c_str());
    pPlane->SetAttribute("hasNormals", Ogre::StringConverter::toString(mHasNormals->get()).c_str());
    // plane normal
    TiXmlElement *pNormal = pPlane->InsertEndChild(TiXmlElement("normal"))->ToElement();
    pNormal->SetAttribute("x", Ogre::StringConverter::toString(mNormal->get().x).c_str());
    pNormal->SetAttribute("y", Ogre::StringConverter::toString(mNormal->get().y).c_str());
    pNormal->SetAttribute("z", Ogre::StringConverter::toString(mNormal->get().z).c_str());
    // plane upvector
    TiXmlElement *pUpVector = pPlane->InsertEndChild(TiXmlElement("upVector"))->ToElement();
    pUpVector->SetAttribute("x", "0");
    pUpVector->SetAttribute("y", "0");
    pUpVector->SetAttribute("z", "1");

    return pNode;
}
//-----------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------
//----------CPLANEEDITORFACTORY--------------------------------------------------
//-------------------------------------------------------------------------------
CPlaneEditorFactory::CPlaneEditorFactory(OgitorsView *view) : CNodeEditorFactory(view)
{
    mTypeName = "Plane";
    mEditorType = ETYPE_MOVABLE;
    mAddToObjectList = true;
    mRequirePlacement = true;
    mIcon = "plane.svg";
    mCapabilities = CAN_PAGE | CAN_MOVE | CAN_SCALE | CAN_ROTATE | CAN_CLONE | CAN_DELETE  | CAN_FOCUS | CAN_DRAG | CAN_UNDO | CAN_USEMATERIAL | CAN_ACCEPTCOPY;
    mUsesGizmos = true;
    mUsesHelper = false;

    AddPropertyDefinition("normal","Normal","The normal of the object.",PROP_VECTOR3);
    AddPropertyDefinition("distance","Distance","The distance of the object.",PROP_REAL);
    AddPropertyDefinition("width","Width","The width of the object.",PROP_REAL);
    AddPropertyDefinition("height","Height","The height of the object.",PROP_REAL);
    AddPropertyDefinition("xsegments","X Segments","The xSegments of the plane.",PROP_INT);
    AddPropertyDefinition("ysegments","Y Segments","The ySegments of the plane.",PROP_INT);
    AddPropertyDefinition("numcoordsets","Num. Coord. Sets","The number of coord sets of the object.",PROP_INT);
    AddPropertyDefinition("utile","U Tile","The uTiling of the object.",PROP_INT);
    AddPropertyDefinition("vtile","V Tile","The vTiling of the object.",PROP_INT);
    AddPropertyDefinition("hasnormals","Has Normals","Does the object has normals?",PROP_BOOL);

    OgitorsPropertyDef *definition;
    
    definition = AddPropertyDefinition("material","Material","The material of the object.",PROP_STRING);
    definition->setOptions(OgitorsRoot::GetMaterialNames());

    AddPropertyDefinition("castshadows","Cast Shadows","Does the object cast shadows?",PROP_BOOL);

    OgitorsPropertyDefMap::iterator it = mPropertyDefs.find("layer");
    it->second.setAccess(true, true);
}
//-----------------------------------------------------------------------------------------
CBaseEditorFactory *CPlaneEditorFactory::duplicate(OgitorsView *view)
{
    CBaseEditorFactory *ret = OGRE_NEW CPlaneEditorFactory(view);
    ret->mTypeID = mTypeID;

    return ret;
}
//-----------------------------------------------------------------------------------------
CBaseEditor *CPlaneEditorFactory::CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params)
{
    CPlaneEditor *object = OGRE_NEW CPlaneEditor(this);
    OgitorsPropertyValueMap::iterator ni;

    if ((ni = params.find("init")) != params.end())
    {
        OgitorsPropertyValue value = EMPTY_PROPERTY_VALUE;
        value.val = Ogre::Any(CreateUniqueID("Plane"));
        params["name"] = value;
        params.erase(ni);
    }

    object->createProperties(params);
    object->mParentEditor->init(*parent);

    mInstanceCount++;
    return object;
}
//-------------------------------------------------------------------------------
