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
#include "OgitorsSystem.h"
#include "NodeEditor.h"
#include "VisualHelper.h"
#include "PagingEditor.h"
#include "tinyxml.h"
#include "OBBoxRenderable.h"

using namespace Ogitors;

//-----------------------------------------------------------------------------------------
void saveUserData(OgitorsCustomPropertySet *set, TiXmlElement *pParent)
{
    if(!set || !pParent)
        return;

    OgitorsPropertyVector vec = set->getPropertyVector();

    if(vec.size() < 1)
        return;

    TiXmlElement *pNode = pParent->InsertEndChild(TiXmlElement("userData"))->ToElement();

    for(unsigned int i = 0;i < vec.size();i++)
    {
        OgitorsPropertyBase *property = vec[i];
        const OgitorsPropertyDef *def = property->getDefinition();
        OgitorsPropertyValue value;
        value.propType = property->getType();
        value.val = property->getValue();

        TiXmlElement *pProp = pNode->InsertEndChild(TiXmlElement("property"))->ToElement();
        pProp->SetAttribute("type", Ogre::StringConverter::toString(value.propType).c_str());
        pProp->SetAttribute("name", property->getName().c_str());
        pProp->SetAttribute("data", OgitorsUtils::GetValueString(value).c_str());
    }
}
//-----------------------------------------------------------------------------------------
CNodeEditor::CNodeEditor(CBaseEditorFactory *factory) : CBaseEditor(factory), 
mAutoTrackTargetPtr(0)
{
    mHandle = 0;
    mUsesGizmos = true;
    mUsesHelper = false;

    PROPERTY_PTR(mPosition       , "position"   ,Ogre::Vector3   ,Ogre::Vector3::ZERO       ,0,SETTER(Ogre::Vector3, CNodeEditor, _setPosition));
    PROPERTY_PTR(mOrientation    , "orientation",Ogre::Quaternion,Ogre::Quaternion::IDENTITY,0,SETTER(Ogre::Quaternion, CNodeEditor, _setOrientation));
    PROPERTY_PTR(mScale          , "scale"      ,Ogre::Vector3   ,Ogre::Vector3(1,1,1)      ,0,SETTER(Ogre::Vector3, CNodeEditor, _setScale));
    PROPERTY_PTR(mAutoTrackTarget, "autotracktarget",Ogre::String,"None"                    ,0,SETTER(Ogre::String, CNodeEditor, _setAutoTrackTarget));
}
//-----------------------------------------------------------------------------------------
CNodeEditor::~CNodeEditor()
{

}
//-----------------------------------------------------------------------------------------
const Ogre::AxisAlignedBox NULL_NODE_BOX(Ogre::Vector3(-0.25f, -0.25f, -0.25f), Ogre::Vector3(0.25f, 0.25f, 0.25f));

Ogre::AxisAlignedBox CNodeEditor::getAABB() 
{
    if(mHandle) 
    {
        Ogre::AxisAlignedBox box = mHandle->_getWorldAABB();
        
        if(box == Ogre::AxisAlignedBox::BOX_NULL)
            return NULL_NODE_BOX;
        else
        {
            Ogre::Vector3 scale(1,1,1);
            Ogre::Vector3 pos(0,0,0);
            if(mParentEditor->get())
            {
                scale = mParentEditor->get()->getDerivedScale();
                scale = Ogre::Vector3::UNIT_SCALE / scale;
            }

            pos = getDerivedPosition();

            box = Ogre::AxisAlignedBox(box.getMinimum() - pos, box.getMaximum() - pos);

            box.scale(scale);
            
            return box;
        }
    }
    else 
        return NULL_NODE_BOX;
}
//-----------------------------------------------------------------------------------------
Ogre::AxisAlignedBox CNodeEditor::getWorldAABB()
{
    if(getEditorType() == ETYPE_NODE)
    {
        if(mHandle)
        {
            Ogre::AxisAlignedBox box = mHandle->_getWorldAABB();
            // Check for NULL Bounding Boxes
            if(box == Ogre::AxisAlignedBox::BOX_NULL)
                return Ogre::AxisAlignedBox(Ogre::Vector3(-0.25f, -0.25f, -0.25f) + mPosition->get(), Ogre::Vector3(0.25f, 0.25f, 0.25f) + mPosition->get());
            else
                return box;
        }
        else
            return Ogre::AxisAlignedBox(Ogre::Vector3(-0.25f, -0.25f, -0.25f) + mPosition->get(), Ogre::Vector3(0.25f, 0.25f, 0.25f) + mPosition->get());
    }

    Ogre::AxisAlignedBox box = getAABB();

    Ogre::Vector3 scale = getDerivedScale();
    Ogre::Vector3 pos = getDerivedPosition();

    box.scale(scale);
    box = Ogre::AxisAlignedBox(box.getMinimum() + pos, box.getMaximum() + pos);

    return box;
}
//-----------------------------------------------------------------------------------------
void CNodeEditor::setParentImpl(CBaseEditor *oldparent, CBaseEditor *newparent)
{
    if(!mHandle)
        return;

    if(oldparent) 
        oldparent->getNode()->removeChild(mHandle);
    
    if(newparent) 
        newparent->getNode()->addChild(mHandle);
}
//-----------------------------------------------------------------------------------------
bool CNodeEditor::postSceneUpdate(Ogre::SceneManager *SceneMngr, Ogre::Camera *Camera, Ogre::RenderWindow *RenderWindow)
{
    Ogre::String target = mAutoTrackTarget->get();
    
    if(target != "" && target != "None")
        _setAutoTrackTarget(0, mAutoTrackTarget->get());

    return false;
}
//-------------------------------------------------------------------------------
bool CNodeEditor::getObjectContextMenu(UTFStringVector &menuitems) 
{
    menuitems.clear();
    if(getParent() && getParent()->getLocked())
        return false;

    if(!mLocked->get())
    {
        menuitems.push_back(OTR("Lock") + ";:/icons/lock.svg");
        menuitems.push_back("");
    }
    else
    {
        menuitems.push_back("");
        menuitems.push_back(OTR("UnLock") + ";:/icons/unlock.svg");
    }

    menuitems.push_back(OTR("Euclidean Rotation"));

    return true;
}
//-------------------------------------------------------------------------------
void CNodeEditor::onObjectContextMenu(int menuresult) 
{
    if(menuresult == 0)
        mLocked->set(true);
    else if(menuresult == 1)
        mLocked->set(false);
    else if(menuresult == 2)
    {
        Ogre::NameValuePairList params;
        if(mSystem->DisplayEuclidDialog(params))
        {
            Ogre::Real fYaw = Ogre::StringConverter::parseReal(params["input2"]);
            Ogre::Real fPitch = Ogre::StringConverter::parseReal(params["input1"]);
            Ogre::Real fRoll = Ogre::StringConverter::parseReal(params["input3"]);

            Ogre::Quaternion quatR, quatY, quatP;
            quatY.FromAngleAxis(Ogre::Degree(fYaw), Ogre::Vector3::UNIT_Y);
            quatP.FromAngleAxis(Ogre::Degree(fPitch), Ogre::Vector3::UNIT_X);
            quatR.FromAngleAxis(Ogre::Degree(fRoll), Ogre::Vector3::UNIT_Z);
            Ogre::Quaternion newQuat = quatY * quatP * quatR;

            mOrientation->set(newQuat);
        }
    }
}
//-----------------------------------------------------------------------------------------
bool CNodeEditor::_setPosition(OgitorsPropertyBase* property, const Ogre::Vector3& position)
{
    if(mHandle)
    {
        mHandle->setPosition(position);
        if(mAutoTrackTargetPtr)
        {
            mHandle->lookAt(mAutoTrackTargetPtr->getNode()->_getDerivedPosition(), Ogre::Node::TS_WORLD);
            mOrientation->initAndSignal(mHandle->getOrientation());
        }
    }

    _updatePaging();

    return true;
}
//-----------------------------------------------------------------------------------------
bool CNodeEditor::_setOrientation(OgitorsPropertyBase* property, const Ogre::Quaternion& orientation)
{
    if(mAutoTrackTargetPtr)
        return false;

    if(mHandle)
    {
        mHandle->setOrientation(orientation);
    }    
    return true;
}
//-----------------------------------------------------------------------------------------
bool CNodeEditor::_setScale(OgitorsPropertyBase* property, const Ogre::Vector3& scale)
{
    if(scale.x == 0.0f || scale.y == 0.0f || scale.z == 0.0f)
        return false;

    if(mHandle)
    {
        mHandle->setScale(scale);
    }    
    return true;
}
//-----------------------------------------------------------------------------------------
bool CNodeEditor::_setAutoTrackTarget(OgitorsPropertyBase* property, const Ogre::String& targetname)
{
    if(targetname == mName->get())
        return false;

    if(mAutoTrackTargetPtr)
    {
        mAutoTrackTargetConnection[0].disconnect();
        mAutoTrackTargetConnection[1].disconnect();
        mAutoTrackTargetConnection[2].disconnect();
    }
    mAutoTrackTargetPtr = mOgitorsRoot->FindObject(targetname);
    if(mAutoTrackTargetPtr)
    {
        CONNECT_PROPERTY_MEMFN(mAutoTrackTargetPtr,"destroyed", CNodeEditor, OnTrackTargetDestroyed, mAutoTrackTargetConnection[0]); 
        CONNECT_PROPERTY_MEMFN(mAutoTrackTargetPtr,"name", CNodeEditor, OnTrackTargetNameChange, mAutoTrackTargetConnection[1]); 
        CONNECT_PROPERTY_MEMFN(mAutoTrackTargetPtr,"position", CNodeEditor, OnTrackTargetPositionChange, mAutoTrackTargetConnection[2]); 
        if(mHandle)
        {
            mHandle->setAutoTracking(true, mAutoTrackTargetPtr->getNode());
            if(mHelper)
                mHelper->getNode()->setAutoTracking(true, mAutoTrackTargetPtr->getNode());
            mHandle->lookAt(mAutoTrackTargetPtr->getNode()->_getDerivedPosition(), Ogre::Node::TS_WORLD);
            mOrientation->set(mHandle->getOrientation());
        }
        return true;
    }
    else
    {
        if(mHandle)
            mHandle->setAutoTracking(false);

        if(mHelper)
            mHelper->getNode()->setAutoTracking(false);

        if(targetname == "None")
            return true;
        
        return false;
    }
}
//-----------------------------------------------------------------------------------------
Ogre::Vector3 CNodeEditor::getDerivedPosition()
{
    if(mHandle)
        return mHandle->_getDerivedPosition();
    else
        return getParent()->getDerivedPosition() + (getParent()->getDerivedOrientation() * (getParent()->getDerivedScale() * mPosition->get()));
}
//-----------------------------------------------------------------------------------------
Ogre::Quaternion CNodeEditor::getDerivedOrientation()
{
    if(mHandle)
        return mHandle->_getDerivedOrientation();
    else
        return (getParent()->getDerivedOrientation() * mOrientation->get());
}
//-----------------------------------------------------------------------------------------
Ogre::Vector3 CNodeEditor::getDerivedScale()
{
    if(mHandle)
        return mHandle->_getDerivedScale();
    else
        return (getParent()->getDerivedScale() * mScale->get());
}
//-----------------------------------------------------------------------------------------
bool CNodeEditor::load(bool async)
{
    if(mLoaded->get())
        return true;

    if(!getParent()->load())
        return false;
    
    mHandle = getParent()->getNode()->createChildSceneNode(mName->get(), mPosition->get(), mOrientation->get());
    mHandle->setScale(mScale->get());

    mLoaded->set(true);
    return true;
}
//-----------------------------------------------------------------------------------------
bool CNodeEditor::unLoad()
{
    if(!mLoaded->get())
        return true;

    unLoadAllChildren();
    destroyBoundingBox();

    if(mHandle)
    {
        mHandle->getParentSceneNode()->removeAndDestroyChild(mHandle->getName());
        mHandle = 0;
    }

    mLoaded->set(false);

    return true;
}
//-----------------------------------------------------------------------------------------
void CNodeEditor::createProperties(OgitorsPropertyValueMap &params)
{
    mProperties.initValueMap(params);
}
//-----------------------------------------------------------------------------------------
/** Auto Track Target Related **/

void CNodeEditor::OnTrackTargetDestroyed(const OgitorsPropertyBase* property, Ogre::Any value)
{
    mAutoTrackTarget->set("None");
}
//-----------------------------------------------------------------------------------------
void CNodeEditor::OnTrackTargetPositionChange(const OgitorsPropertyBase* property, Ogre::Any value)
{
    if(mHandle)
    {
        mOrientation->set(mHandle->getOrientation());
    }
}
//-----------------------------------------------------------------------------------------
void CNodeEditor::OnTrackTargetNameChange(const OgitorsPropertyBase* property, Ogre::Any value)
{
    mAutoTrackTarget->set(Ogre::any_cast<Ogre::String>(value));
}
//-----------------------------------------------------------------------------------------
TiXmlElement* CNodeEditor::exportDotScene(TiXmlElement *pParent)
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
    
    // loop over children
    NameObjectPairList::const_iterator it = mChildren.begin();
    while(it != mChildren.end())
    {
        TiXmlElement *result = it->second->exportDotScene(pNode);
        saveUserData(it->second->getCustomProperties(), result);

        it++;
    }

    return pNode;
}
//-----------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------
//------CNODEEDITORFACTORY-----------------------------------------------------------------
//-----------------------------------------------------------------------------------------
CNodeEditorFactory::CNodeEditorFactory(OgitorsView *view) : CBaseEditorFactory(view)
{
    mTypeName = "Node";
    mEditorType = ETYPE_NODE;
    mAddToObjectList = true;
    mRequirePlacement = true;
    mIcon = "node.svg";
    mCapabilities = CAN_PAGE | CAN_MOVE | CAN_ROTATE | CAN_SCALE | CAN_FOCUS | CAN_DELETE | CAN_DRAG | CAN_DROP | CAN_UNDO | CAN_ACCEPTCOPY | CAN_ACCEPTPASTE;

    OgitorsPropertyDef *definition;

    AddPropertyDefinition("position","Position","The position of the object.",PROP_VECTOR3);
    AddPropertyDefinition("orientation","Orientation","The orientation of the object.",PROP_QUATERNION,true,false);
    AddPropertyDefinition("scale","Scale","The scale of the object.", PROP_VECTOR3);
    definition = AddPropertyDefinition("autotracktarget","Tracking Target","The object's tracking target.",PROP_STRING);
    definition->setOptions(OgitorsRoot::GetAutoTrackTargets());

    OgitorsPropertyDefMap::iterator it = mPropertyDefs.find("layer");
    it->second.setAccess(false, false);

    it = mPropertyDefs.find("updatescript");
    it->second.setAccess(true, true);
}
//-----------------------------------------------------------------------------------------
CBaseEditorFactory *CNodeEditorFactory::duplicate(OgitorsView *view)
{
    CBaseEditorFactory *ret = OGRE_NEW CNodeEditorFactory(view);
    ret->mTypeID = mTypeID;

    return ret;
}
//-----------------------------------------------------------------------------------------
CBaseEditor *CNodeEditorFactory::CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params)
{
    CNodeEditor *object = OGRE_NEW CNodeEditor(this);

    OgitorsPropertyValueMap::iterator ni;

    if ((ni = params.find("init")) != params.end())
    {
        OgitorsPropertyValue value = EMPTY_PROPERTY_VALUE;
        value.val = Ogre::Any(CreateUniqueID("Node"));
        params["name"] = value;
        params.erase(ni);
    }
    
    object->createProperties(params);
    object->mParentEditor->init(*parent);
    object->registerForPostSceneUpdates();

    mInstanceCount++;
    return object;
}
//-----------------------------------------------------------------------------------------
