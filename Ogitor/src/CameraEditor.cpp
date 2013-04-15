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
#include "VisualHelper.h"
#include "CameraVisualHelper.h"
#include "ViewportEditor.h"
#include "CameraEditor.h"
#include "tinyxml.h"
#include "OBBoxRenderable.h"

using namespace Ogitors;

PropertyOptionsVector CCameraEditorFactory::mCameraPolygonModes;
PropertyOptionsVector CCameraEditorFactory::mCameraViewModes;

//-----------------------------------------------------------------------------------------
CCameraEditor::CCameraEditor(CBaseEditorFactory *factory) : CBaseEditor(factory), 
mAutoTrackTargetPtr(0)
{
    mHandle = 0;
    mHelper = 0;
}
//-----------------------------------------------------------------------------------------
CCameraEditor::~CCameraEditor()
{
 
}
//-----------------------------------------------------------------------------------------
bool CCameraEditor::postSceneUpdate(Ogre::SceneManager *SceneMngr, Ogre::Camera *Camera, Ogre::RenderWindow *RenderWindow)
{
    Ogre::String target = mAutoTrackTarget->get();
    
    if(target != "" && target != "None")
        _setAutoTrackTarget(0, target);

    return false;
}
//-----------------------------------------------------------------------------------------
void CCameraEditor::setParentImpl(CBaseEditor *oldparent, CBaseEditor *newparent)
{
    if(!mHandle)
        return;

    if(oldparent) 
        mHandle->detachFromParent();
    
    if(newparent) 
        newparent->getNode()->attachObject(mHandle);
}
//-----------------------------------------------------------------------------------------
bool CCameraEditor::setLayerImpl(unsigned int newlayer)
{
    if(mHelper)
        mHelper->setVisiblityFlags(1 << newlayer);

    return true;
}
//-----------------------------------------------------------------------------------------
void CCameraEditor::setSelectedImpl(bool bSelected)
{
    CBaseEditor::setSelectedImpl(bSelected);
    if(mOgitorsRoot->GetViewport()->getCameraEditor() == this)
    {
       bSelected = false;
    }

    showBoundingBox(bSelected);
}
//-----------------------------------------------------------------------------------------
bool CCameraEditor::setHighlightedImpl(bool highlight )
{
    showBoundingBox(highlight);

    return true;
}
//-----------------------------------------------------------------------------------------
void CCameraEditor::showBoundingBox(bool bShow) 
{
    if(!mBoxParentNode)
        createBoundingBox();

    if(mBBoxNode)
    {
        int matpos = 0;
        if(mHighlighted->get())
        {
            ++matpos;
            if(mSelected->get())
                ++matpos;
        }
        
        mOBBoxRenderable->setMaterial(mOBBMaterials[matpos]);
        mBBoxNode->setVisible((bShow || mSelected->get() || mHighlighted->get()) && (mOgitorsRoot->GetViewport()->getCameraEditor() != this));
    }
}
//-----------------------------------------------------------------------------------------
Ogre::AxisAlignedBox CCameraEditor::getAABB() 
{
    if(mHelper) 
        return mHelper->getAABB(); 
    else 
        return Ogre::AxisAlignedBox::BOX_NULL;
}
//-----------------------------------------------------------------------------------------
Ogre::AxisAlignedBox CCameraEditor::getWorldAABB() 
{
    if(mHelper) 
        return mHelper->getWorldAABB(); 
    else 
        return Ogre::AxisAlignedBox::BOX_NULL;
}
//-----------------------------------------------------------------------------------------
void CCameraEditor::setDerivedPosition(Ogre::Vector3 val)
{
    if(getParent())
    {
        Ogre::Quaternion qParent = getParent()->getDerivedOrientation().Inverse();
        Ogre::Vector3 vParent = getParent()->getDerivedPosition();
        Ogre::Vector3 newPos = (val - vParent);
        val = qParent * newPos;
    }
    mPosition->set(val);
}
//-----------------------------------------------------------------------------------------
void CCameraEditor::setDirection(const Ogre::Vector3 &vec)
{
    if (vec == Ogre::Vector3::ZERO) return;

    Ogre::Vector3 zAdjustVec = -vec;
    zAdjustVec.normalise();

    Ogre::Quaternion orientation;

    Ogre::Vector3 YawFixedAxis = Ogre::Vector3::UNIT_Y;
    Ogre::Vector3 xVec = YawFixedAxis.crossProduct( zAdjustVec );
    xVec.normalise();

    Ogre::Vector3 yVec = zAdjustVec.crossProduct( xVec );
    yVec.normalise();

    orientation.FromAxes( xVec, yVec, zAdjustVec );

    // transform to parent space
    if (mParentEditor->get())
    {
        orientation = mParentEditor->get()->getNode()->_getDerivedOrientation().Inverse() * orientation;
    }

    mOrientation->set(orientation);
}
//-----------------------------------------------------------------------------------------
void CCameraEditor::lookAt(const Ogre::Vector3 &value)
{
    setDirection(value - getDerivedPosition());

    if(mAutoTrackTargetPtr || mViewMode->get())
        return;

    if(mHandle)
    {
        mHandle->lookAt(value);
        mOrientation->set(mHandle->getOrientation());
    }
}
//-----------------------------------------------------------------------------------------
void CCameraEditor::yaw(const Ogre::Radian &value)
{
    Ogre::Quaternion q;

    q.FromAngleAxis(value, Ogre::Vector3::UNIT_Y);
    q.normalise();
    
    mOrientation->set(q * mOrientation->get());
}
//-----------------------------------------------------------------------------------------
void CCameraEditor::pitch(const Ogre::Radian &value)
{
    Ogre::Quaternion q;
    Ogre::Vector3 axis = mOrientation->get() * Ogre::Vector3::UNIT_X;
    q.FromAngleAxis(value, axis);
    q.normalise();
    
    mOrientation->set(q * mOrientation->get());
}
//-----------------------------------------------------------------------------------------
void CCameraEditor::roll(const Ogre::Radian &value)
{
    Ogre::Quaternion q;
    Ogre::Vector3 axis = mOrientation->get() * Ogre::Vector3::UNIT_Z;
    q.FromAngleAxis(value, axis);
    q.normalise();
    
    mOrientation->set(q * mOrientation->get());
}
//-----------------------------------------------------------------------------------------
bool CCameraEditor::_setPosition(OgitorsPropertyBase* property, const Ogre::Vector3& position)
{
    if(mHandle)
    {
        mHandle->setPosition(position);
        if(mAutoTrackTargetPtr)
        {
            mHandle->lookAt(mAutoTrackTargetPtr->getNode()->_getDerivedPosition());
            mOrientation->initAndSignal(mHandle->getOrientation());
        }
    }    
    return true;
}
//-----------------------------------------------------------------------------------------
bool CCameraEditor::_setOrientation(OgitorsPropertyBase* property, const Ogre::Quaternion& orientation)
{
    if(mAutoTrackTargetPtr || mViewMode->get())
        return false;

    if(mHandle)
    {
        mHandle->setOrientation(orientation);
    }
    return true;
}
//-----------------------------------------------------------------------------------------
bool CCameraEditor::_setClipDistance(OgitorsPropertyBase* property, const Ogre::Vector2& value)
{
    Ogre::Vector2 val = value;

    if(val.x < 0.05f)
    {
        val.x = 0.05f;
        mClipDistance->init(val);
    }

    if (val.y == 0.0f)
    {
        if(!Ogre::Root::getSingletonPtr()->getRenderSystem()->getCapabilities()->hasCapability(Ogre::RSC_INFINITE_FAR_PLANE))
        {
            val.y = 50000;
            mClipDistance->init(val);
        }
    }

    if(mHandle)
    {
        mHandle->setNearClipDistance(val.x);
        mHandle->setFarClipDistance(val.y);
    }    
    return true;
}
//-----------------------------------------------------------------------------------------
bool CCameraEditor::_setFOV(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(mHandle)
    {
        mHandle->setFOVy(Ogre::Radian(value));
    }    
    return true;
}
//-----------------------------------------------------------------------------------------
bool CCameraEditor::_setPolygonMode(OgitorsPropertyBase* property, const int& value)
{
    if(mHandle)
    {
        mHandle->setPolygonMode((Ogre::PolygonMode)value);
    }
    return true;
}
//-----------------------------------------------------------------------------------------
bool CCameraEditor::_setViewMode(OgitorsPropertyBase* property, const int& value)
{
    if(value)
    {
        if(mAutoTrackTarget->get() != "None")
            _setAutoTrackTarget(mAutoTrackTarget, "None");

        //Set it to CVM_FREE so we can set to new orientation
        mViewMode->init(0);
        Ogre::Vector3 normal = Ogre::Vector3::UNIT_Z;
        switch(value)
        {
        case CVM_GLOBAL_LEFT: setDerivedOrientation(normal.getRotationTo(Ogre::Vector3::NEGATIVE_UNIT_X));break;
        case CVM_GLOBAL_RIGHT: setDerivedOrientation(normal.getRotationTo(Ogre::Vector3::UNIT_X));break;
        case CVM_GLOBAL_FRONT: setDerivedOrientation(normal.getRotationTo(Ogre::Vector3::NEGATIVE_UNIT_Z));break;
        case CVM_GLOBAL_BACK: setDerivedOrientation(Ogre::Quaternion::IDENTITY);break;
        case CVM_GLOBAL_TOP: setDerivedOrientation(normal.getRotationTo(Ogre::Vector3::UNIT_Y));break;
        case CVM_GLOBAL_BOTTOM: setDerivedOrientation(normal.getRotationTo(Ogre::Vector3::NEGATIVE_UNIT_Y));break;

        case CVM_LOCAL_LEFT: mOrientation->set(normal.getRotationTo(Ogre::Vector3::NEGATIVE_UNIT_X));break;
        case CVM_LOCAL_RIGHT: mOrientation->set(normal.getRotationTo(Ogre::Vector3::UNIT_X));break;
        case CVM_LOCAL_FRONT: mOrientation->set(normal.getRotationTo(Ogre::Vector3::NEGATIVE_UNIT_Z));break;
        case CVM_LOCAL_BACK: mOrientation->set(Ogre::Quaternion::IDENTITY);break;
        case CVM_LOCAL_TOP: mOrientation->set(normal.getRotationTo(Ogre::Vector3::UNIT_Y));break;
        case CVM_LOCAL_BOTTOM: mOrientation->set(normal.getRotationTo(Ogre::Vector3::NEGATIVE_UNIT_Y));break;
        }
        //Restore the value
        mViewMode->init(value);
    }
    else
    {
        if(mAutoTrackTarget->get() != "None")
            _setAutoTrackTarget(mAutoTrackTarget, mAutoTrackTarget->get());
    }

    return true;
}
//-----------------------------------------------------------------------------------------
bool CCameraEditor::_setAutoAspectRatio(OgitorsPropertyBase* property, const bool& value)
{
    if(mHandle)
    {
        mHandle->setAutoAspectRatio(value);
    }

    return true;
}
//-----------------------------------------------------------------------------------------
bool CCameraEditor::_setAutoTrackTarget(OgitorsPropertyBase* property, const Ogre::String& targetname)
{
    if(mViewMode->get())
        return false;

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
        CONNECT_PROPERTY_MEMFN(mAutoTrackTargetPtr,"destroyed", CCameraEditor, OnTrackTargetDestroyed, mAutoTrackTargetConnection[0]); 
        CONNECT_PROPERTY_MEMFN(mAutoTrackTargetPtr,"name", CCameraEditor, OnTrackTargetNameChange, mAutoTrackTargetConnection[1]); 
        CONNECT_PROPERTY_MEMFN(mAutoTrackTargetPtr,"position", CCameraEditor, OnTrackTargetPositionChange, mAutoTrackTargetConnection[2]); 

        if(mHandle)
        {
            mHandle->setAutoTracking(true, mAutoTrackTargetPtr->getNode());
            mHelper->getNode()->setAutoTracking(true, mAutoTrackTargetPtr->getNode());
            mHandle->lookAt(mAutoTrackTargetPtr->getNode()->_getDerivedPosition());
            mOrientation->set(mHandle->getOrientation());
        }
        return true;
    }
    else
    {
        if(mHandle)
        {
            mHandle->setAutoTracking(false);
            mHelper->getNode()->setAutoTracking(false);
        }

        if(targetname == "None")
            return true;
        
        return false;
    }
}
//-----------------------------------------------------------------------------------------
bool CCameraEditor::load(bool async)
{
    if(mLoaded->get())
        return true;

    if(!getParent()->load())
        return false;

    mHandle = mOgitorsRoot->GetSceneManager()->createCamera(mName->get());
    
    mHandle->setPosition(mPosition->get());
    mHandle->setOrientation(mOrientation->get());
    mHandle->setNearClipDistance(mClipDistance->get().x);
    mHandle->setFarClipDistance(mClipDistance->get().y);
    mHandle->setFOVy(Ogre::Radian(mFOV->get()));
    mHandle->setAutoAspectRatio(mAutoAspectRatio->get());
    mHandle->setQueryFlags(0);

    getParent()->getNode()->attachObject(mHandle);

    CCameraVisualHelper *helper = OGRE_NEW CCameraVisualHelper(this);
    helper->setVisiblityFlags(1 << mLayer->get());
    helper->Show(mOgitorsRoot->GetCameraVisiblity());

    setHelper(helper);

    mLoaded->set(true);

    return true;
}
//-----------------------------------------------------------------------------------------
bool CCameraEditor::unLoad()
{
    if(!mLoaded->get())
        return true;

    unLoadAllChildren();

    destroyBoundingBox();

    if(mHandle)
    {
        mHandle->detachFromParent();
        mHandle->getSceneManager()->destroyCamera(mHandle);
        mHandle = 0;
    }

    if(mHelper)
    {
        mHelper->Destroy();
        OGRE_DELETE mHelper;
        mHelper = 0;
    }
    
    mLoaded->set(false);
    return true;
}
//-----------------------------------------------------------------------------------------
void CCameraEditor::createProperties(OgitorsPropertyValueMap &params)
{
    PROPERTY_PTR(mViewMode, "viewmode",int, CVM_FREE, 0, SETTER(int, CCameraEditor, _setViewMode));
    PROPERTY_PTR(mPosition, "position",Ogre::Vector3,Ogre::Vector3::ZERO,0,SETTER(Ogre::Vector3, CCameraEditor, _setPosition));
    PROPERTY_PTR(mOrientation, "orientation",Ogre::Quaternion,Ogre::Quaternion::IDENTITY,0,SETTER(Ogre::Quaternion, CCameraEditor, _setOrientation));
    PROPERTY_PTR(mAutoAspectRatio, "autoaspectratio", bool, true, 0, SETTER(bool, CCameraEditor, _setAutoAspectRatio));
    PROPERTY_PTR(mClipDistance, "clipdistance",Ogre::Vector2,Ogre::Vector2(0.1f,1000.0f),0,SETTER(Ogre::Vector2, CCameraEditor, _setClipDistance));
    PROPERTY_PTR(mFOV, "fov",Ogre::Real,1.0f,0, SETTER(Ogre::Real, CCameraEditor, _setFOV));
    PROPERTY_PTR(mAutoTrackTarget, "autotracktarget",Ogre::String,"None",0,SETTER(Ogre::String, CCameraEditor, _setAutoTrackTarget));
    PROPERTY_PTR(mPolygonMode, "polygonmode",int,Ogre::PM_SOLID,0,SETTER(int, CCameraEditor, _setPolygonMode));

    mProperties.initValueMap(params);
}
//-----------------------------------------------------------------------------------------
Ogre::Vector3 CCameraEditor::getDerivedPosition()
{
    if(mHandle)
        return mHandle->getDerivedPosition();
    else
        return getParent()->getDerivedPosition() + (getParent()->getDerivedOrientation() * mPosition->get());
}
//-----------------------------------------------------------------------------------------
Ogre::Quaternion CCameraEditor::getDerivedOrientation()
{
    if(mHandle)
        return mHandle->getDerivedOrientation();
    else
        return (getParent()->getDerivedOrientation() * mOrientation->get());
}
//-----------------------------------------------------------------------------------------
/** Auto Track Target Related **/
void CCameraEditor::OnTrackTargetDestroyed(const OgitorsPropertyBase* property, Ogre::Any value)
{
    mAutoTrackTarget->set("None");
}
//-----------------------------------------------------------------------------------------
void CCameraEditor::OnTrackTargetPositionChange(const OgitorsPropertyBase* property, Ogre::Any value)
{
    if(mHandle)
        mOrientation->set(mHandle->getOrientation());
}
//-----------------------------------------------------------------------------------------
void CCameraEditor::OnTrackTargetNameChange(const OgitorsPropertyBase* property, Ogre::Any value)
{
    mAutoTrackTarget->set(Ogre::any_cast<Ogre::String>(value));
}
//-----------------------------------------------------------------------------------------
TiXmlElement* CCameraEditor::exportDotScene(TiXmlElement *pParent)
{
    // camera
    TiXmlElement *pCamera = pParent->InsertEndChild(TiXmlElement("camera"))->ToElement();
    pCamera->SetAttribute("name", mName->get().c_str());
    pCamera->SetAttribute("id", Ogre::StringConverter::toString(mObjectID->get()).c_str());
    pCamera->SetAttribute("fov", Ogre::StringConverter::toString(mFOV->get()).c_str());
    pCamera->SetAttribute("projectionType", "perspective");
    // position
    TiXmlElement *pCamPosition = pCamera->InsertEndChild(TiXmlElement("position"))->ToElement();
    pCamPosition->SetAttribute("x", Ogre::StringConverter::toString(mPosition->get().x).c_str());
    pCamPosition->SetAttribute("y", Ogre::StringConverter::toString(mPosition->get().y).c_str());
    pCamPosition->SetAttribute("z", Ogre::StringConverter::toString(mPosition->get().z).c_str());
    // rotation
    TiXmlElement *pCamRotation = pCamera->InsertEndChild(TiXmlElement("rotation"))->ToElement();
    pCamRotation->SetAttribute("qw", Ogre::StringConverter::toString(mOrientation->get().w).c_str());
    pCamRotation->SetAttribute("qx", Ogre::StringConverter::toString(mOrientation->get().x).c_str());
    pCamRotation->SetAttribute("qy", Ogre::StringConverter::toString(mOrientation->get().y).c_str());
    pCamRotation->SetAttribute("qz", Ogre::StringConverter::toString(mOrientation->get().z).c_str());
    // clipping
    TiXmlElement *pClipping = pCamera->InsertEndChild(TiXmlElement("clipping"))->ToElement();
    pClipping->SetAttribute("near", Ogre::StringConverter::toString(mClipDistance->get().x).c_str());
    pClipping->SetAttribute("far", Ogre::StringConverter::toString(mClipDistance->get().y).c_str());

    return pCamera;
}
//-----------------------------------------------------------------------------------------
bool CCameraEditor::getObjectContextMenu(UTFStringVector &menuitems) 
{
    menuitems.clear();

    if(mOgitorsRoot->GetViewport()->getCameraEditor() != this)
    {
        menuitems.push_back(OTR("Make Active Camera"));
        menuitems.push_back("");
    }
    else
    {
        menuitems.push_back("");
        menuitems.push_back(OTR("Deactivate Camera"));
    }

    menuitems.push_back(OTR("Euclidean Rotation"));

    return true;
}
//-------------------------------------------------------------------------------
void CCameraEditor::onObjectContextMenu(int menuresult) 
{
    if(menuresult == 0)
        mOgitorsRoot->GetViewport()->setCameraEditor(this);
    else if(menuresult == 1)
        mOgitorsRoot->GetViewport()->setCameraEditor(0);
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


//-----------------------------------------------------------------------------------------
//--------CCAMERAEDITORFACTORY-------------------------------------------------------------
//-----------------------------------------------------------------------------------------
CCameraEditorFactory::CCameraEditorFactory(OgitorsView *view) : CBaseEditorFactory(view)
{
    mTypeName = "Camera";
    mEditorType = ETYPE_CAMERA;
    mAddToObjectList = true;
    mRequirePlacement = true;
    mIcon = "camera.svg";
    mCapabilities = CAN_MOVE | CAN_ROTATE | CAN_CLONE | CAN_DELETE | CAN_FOCUS | CAN_DRAG | CAN_UNDO | CAN_ACCEPTCOPY;
    mUsesGizmos = true;
    mUsesHelper = true;

    mCameraPolygonModes.clear();
    mCameraPolygonModes.push_back(PropertyOption("PM_SOLID",Ogre::Any((int)Ogre::PM_SOLID)));
    mCameraPolygonModes.push_back(PropertyOption("PM_POINTS",Ogre::Any((int)Ogre::PM_POINTS)));
    mCameraPolygonModes.push_back(PropertyOption("PM_WIREFRAME",Ogre::Any((int)Ogre::PM_WIREFRAME)));

    mCameraViewModes.clear();
    mCameraViewModes.push_back(PropertyOption("FREE", Ogre::Any((int)CVM_FREE)));
    mCameraViewModes.push_back(PropertyOption("FIX_CURRENT", Ogre::Any((int)CVM_FIX_CURRENT)));
    mCameraViewModes.push_back(PropertyOption("GLOBAL_LEFT", Ogre::Any((int)CVM_GLOBAL_LEFT)));
    mCameraViewModes.push_back(PropertyOption("GLOBAL_RIGHT", Ogre::Any((int)CVM_GLOBAL_RIGHT)));
    mCameraViewModes.push_back(PropertyOption("GLOBAL_FRONT", Ogre::Any((int)CVM_GLOBAL_FRONT)));
    mCameraViewModes.push_back(PropertyOption("GLOBAL_BACK", Ogre::Any((int)CVM_GLOBAL_BACK)));
    mCameraViewModes.push_back(PropertyOption("GLOBAL_TOP", Ogre::Any((int)CVM_GLOBAL_TOP)));
    mCameraViewModes.push_back(PropertyOption("GLOBAL_BOTTOM", Ogre::Any((int)CVM_GLOBAL_BOTTOM)));
    mCameraViewModes.push_back(PropertyOption("LOCAL_LEFT", Ogre::Any((int)CVM_LOCAL_LEFT)));
    mCameraViewModes.push_back(PropertyOption("LOCAL_RIGHT", Ogre::Any((int)CVM_LOCAL_RIGHT)));
    mCameraViewModes.push_back(PropertyOption("LOCAL_FRONT", Ogre::Any((int)CVM_LOCAL_FRONT)));
    mCameraViewModes.push_back(PropertyOption("LOCAL_BACK", Ogre::Any((int)CVM_LOCAL_BACK)));
    mCameraViewModes.push_back(PropertyOption("LOCAL_TOP", Ogre::Any((int)CVM_LOCAL_TOP)));
    mCameraViewModes.push_back(PropertyOption("LOCAL_BOTTOM", Ogre::Any((int)CVM_LOCAL_BOTTOM)));

    OgitorsPropertyDef *definition;

    definition = AddPropertyDefinition("viewmode","View Mode","Custom View Mode restricting the rotation of the camera.", PROP_INT);
    definition->setOptions(&mCameraViewModes);
    AddPropertyDefinition("position","Position","The position of the object.",PROP_VECTOR3);
    AddPropertyDefinition("orientation","Orientation","The orientation of the object.",PROP_QUATERNION, true, false);
    AddPropertyDefinition("autoaspectratio","Auto Aspect Ratio","Does the camera auto sets the aspect ratio?", PROP_BOOL);
    definition = AddPropertyDefinition("clipdistance","Clip Distance","The near and far clipping distances of the object.", PROP_VECTOR2);
    definition->setFieldNames("Near", "Far"); 
    AddPropertyDefinition("fov","FOV","The camera's Field of View.",PROP_REAL);
    definition = AddPropertyDefinition("autotracktarget","Tracking Target","The object's tracking target.",PROP_STRING);
    definition->setOptions(OgitorsRoot::GetAutoTrackTargets());
    
    definition = AddPropertyDefinition("polygonmode","Polygon Mode","The camera's polygon mode.",PROP_INT);
    definition->setOptions(&mCameraPolygonModes);

    OgitorsPropertyDefMap::iterator it = mPropertyDefs.find("updatescript");
    it->second.setAccess(true, true);
}
//-----------------------------------------------------------------------------------------
CBaseEditorFactory *CCameraEditorFactory::duplicate(OgitorsView *view)
{
    CBaseEditorFactory *ret = OGRE_NEW CCameraEditorFactory(view);
    ret->mTypeID = mTypeID;

    return ret;
}
//-----------------------------------------------------------------------------------------
CBaseEditor *CCameraEditorFactory::CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params)
{
    CCameraEditor *object = OGRE_NEW CCameraEditor(this);

    OgitorsPropertyValueMap::iterator ni;

    if ((ni = params.find("init")) != params.end())
    {
        OgitorsPropertyValue value = EMPTY_PROPERTY_VALUE;
        value.val = Ogre::Any(CreateUniqueID("Camera"));
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
