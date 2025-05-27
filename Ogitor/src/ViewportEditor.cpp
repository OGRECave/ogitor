/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2015 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
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
#include "CameraEditor.h"
#include "ViewportEditor.h"
#include "MultiSelEditor.h"
#include "SceneManagerEditor.h"
#include "OgitorsUndoManager.h"
#include "tinyxml.h"

using namespace Ogitors;

//-------------------------------------------------------------------------------
CViewportEditor::CViewportEditor(CBaseEditorFactory *factory) : CBaseEditor(factory)
{
    mHandle = 0;

    mActiveCamera = 0;
    mViewCamera = 0;

    mGridVisible = true;
    mNewCamPosition = Ogre::Vector3::ZERO;
    mViewGrid = 0;
    mVolumeSelecting = false;
    mHighLighted.clear();
    mLastMouse = Ogre::Vector2(0,0);
    mLastClickPoint = Ogre::Vector2(-5,-5);
    mLastButtons = 0;
    mUndoManager = OgitorsUndoManager::getSingletonPtr();
}
//-------------------------------------------------------------------------------
CViewportEditor::~CViewportEditor()
{
}
//-------------------------------------------------------------------------------
int CViewportEditor::getRect(Ogre::Vector4 &rect)
{
    if(mHandle)
    {
        int iLeft,iTop,iWidth,iHeight;
        mHandle->getActualDimensions(iLeft,iTop,iWidth,iHeight);
        rect.x = iLeft;
        rect.y = iTop;
        rect.z = iWidth;
        rect.w = iHeight;
    }
    else
    {
        Ogre::Real width = mOgitorsRoot->GetRenderWindow()->getWidth();
        Ogre::Real height = mOgitorsRoot->GetRenderWindow()->getHeight();
        rect.x = width * mPlacement->get().x;
        rect.y = height * mPlacement->get().y;
        rect.z = width * mPlacement->get().z;
        rect.w = height * mPlacement->get().w;
    }
    return mViewportIndex->get();
}
//-------------------------------------------------------------------------------
void CViewportEditor::renderWindowResized()
{
    if (mHandle && mActiveCamera) 
    {
        mActiveCamera->getCamera()->setAspectRatio((Ogre::Real)mHandle->getActualWidth() / (Ogre::Real)mHandle->getActualHeight()); 
        mActiveCamera->getCamera()->getParentNode()->yaw(Ogre::Radian(0));
    }
}
//-------------------------------------------------------------------------------
void CViewportEditor::_createViewport()
{
    OgitorsPropertyValueMap params;

    OgitorsPropertyValue value;
    value.propType = PROP_STRING;
    value.val = Ogre::Any("intViewCamera" + Ogre::StringConverter::toString(mViewportIndex->get()));
    params["name"] = value;
    value.propType = PROP_VECTOR2;
    value.val = Ogre::Any(mCamClipDistance->get());
    params["clipdistance"] = value;
    value.propType = PROP_VECTOR3;
    value.val = Ogre::Any(mCamPosition->get());
    params["position"] = value;
    value.propType = PROP_QUATERNION;
    value.val = Ogre::Any(mCamOrientation->get());
    params["orientation"] = value;
    value.propType = PROP_REAL;
    value.val = Ogre::Any(mCamFOV->get());
    params["fov"] = value;
    value.propType = PROP_BOOL;
    value.val = Ogre::Any(true);
    params["autoaspectratio"] = value;

    CBaseEditor *ScnMgr = mOgitorsRoot->GetSceneManagerEditor();
    CBaseEditorFactory *factory = mOgitorsRoot->GetEditorObjectFactory("Camera");
    mViewCamera = static_cast<CCameraEditor*>(factory->CreateObject(&ScnMgr,params));
    mViewCamera->load();

    mHandle = mOgitorsRoot->GetRenderWindow()->addViewport(mViewCamera->getCamera(),mViewportIndex->get(),mPlacement->get().x,mPlacement->get().y,mPlacement->get().z,mPlacement->get().w);

    if(mName->get() == "")
    {    
        mName->init("Viewport" + Ogre::StringConverter::toString(mViewportIndex->get()));
    }

    mViewCamera->getCamera()->setAspectRatio((Ogre::Real)mHandle->getActualWidth() / (Ogre::Real)mHandle->getActualHeight());
    mViewCamera->showHelper(false);

    mActiveCamera = mViewCamera;

    mActiveCamera->getProperties()->removeListener(mOgitorsRoot->GetGlobalPropertyListener());

    CONNECT_PROPERTY_MEMFN(mActiveCamera, "position", CViewportEditor, OnViewCameraPositionChange, mCameraConnections[0]);
    CONNECT_PROPERTY_MEMFN(mActiveCamera, "orientation", CViewportEditor, OnViewCameraOrientationChange, mCameraConnections[1]);
    CONNECT_PROPERTY_MEMFN(mActiveCamera, "fov", CViewportEditor, OnViewCameraFOVChange, mCameraConnections[2]);
    CONNECT_PROPERTY_MEMFN(mActiveCamera, "clipdistance", CViewportEditor, OnViewCameraClipDistanceChange, mCameraConnections[3]);
    CONNECT_PROPERTY_MEMFN(mActiveCamera, "polygonmode", CViewportEditor, OnViewCameraPolygonModeChange, mCameraConnections[4]);
    CONNECT_PROPERTY_MEMFN(mActiveCamera, "viewmode", CViewportEditor, OnViewCameraViewModeChange, mCameraConnections[5]);

    mHandle->setBackgroundColour(mColour->get());
    mHandle->setOverlaysEnabled(mOverlays->get());
    mHandle->setSkiesEnabled(mSkies->get());
    mHandle->setShadowsEnabled(mShadows->get());

    PROJECTOPTIONS *pOpt = mOgitorsRoot->GetProjectOptions();
    if(pOpt->CameraSaveCount > 0)
    {
        mViewCamera->setPosition(pOpt->CameraPositions[pOpt->CameraSaveCount - 1]);
        mViewCamera->setOrientation(pOpt->CameraOrientations[pOpt->CameraSaveCount - 1]);
    }

    LoadEditorObjects();

    CSceneManagerEditor *mSceneMgr = mOgitorsRoot->GetSceneManagerEditor();
    mSceneMgr->setupPSSM();
}
//-------------------------------------------------------------------------------
bool CViewportEditor::setNameImpl(Ogre::String name)
{
    if(mOgitorsRoot->FindObject(name))
        return false;
    else
    {    
        mSystem->SetTreeItemText(this, name);
        return true;
    }
}
//-------------------------------------------------------------------------------
void CViewportEditor::setCameraEditor(CCameraEditor *camed)
{
    if(camed == mActiveCamera)
        return;

    if(mActiveCamera) 
    {
        mActiveCamera->showHelper(true);
        mActiveCamera->getProperties()->addListener(mOgitorsRoot->GetGlobalPropertyListener());
        if(mOgitorsRoot->GetSelection()->contains(mActiveCamera))
        {
            mActiveCamera->showBoundingBox(true);
        }
    }

    if(camed)
        mActiveCamera = camed;
    else
        mActiveCamera = mViewCamera;

    mActiveCamera->getProperties()->removeListener(mOgitorsRoot->GetGlobalPropertyListener());


    CONNECT_PROPERTY_MEMFN(mActiveCamera, "position", CViewportEditor, OnViewCameraPositionChange, mCameraConnections[0]);
    CONNECT_PROPERTY_MEMFN(mActiveCamera, "orientation", CViewportEditor, OnViewCameraOrientationChange, mCameraConnections[1]);
    CONNECT_PROPERTY_MEMFN(mActiveCamera, "fov", CViewportEditor, OnViewCameraFOVChange, mCameraConnections[2]);
    CONNECT_PROPERTY_MEMFN(mActiveCamera, "clipdistance", CViewportEditor, OnViewCameraClipDistanceChange, mCameraConnections[3]);
    CONNECT_PROPERTY_MEMFN(mActiveCamera, "polygonmode", CViewportEditor, OnViewCameraPolygonModeChange, mCameraConnections[4]);
    CONNECT_PROPERTY_MEMFN(mActiveCamera, "viewmode", CViewportEditor, OnViewCameraViewModeChange, mCameraConnections[5]);

    Ogre::Quaternion qval;
    Ogre::Vector2 v2val;
    Ogre::Vector3 v3val;
    Ogre::Real fval;
    int ival;

    mActiveCamera->getProperties()->getValue("position", v3val);
    mCamPosition->initAndSignal(v3val);
    mActiveCamera->getProperties()->getValue("orientation", qval);
    mCamOrientation->initAndSignal(qval);
    mActiveCamera->getProperties()->getValue("fov", fval);
    mCamFOV->initAndSignal(fval);
    mActiveCamera->getProperties()->getValue("polygonmode", ival);
    mCamPolyMode->initAndSignal(ival);
    if(ival != Ogre::PM_SOLID)
        mHandle->setVisibilityMask(0x7FFFFFFF);
    else
        mHandle->setVisibilityMask(0xFFFFFFFF);

    mActiveCamera->getProperties()->getValue("clipdistance", v2val);
    mCamClipDistance->initAndSignal(v2val);
    mActiveCamera->getProperties()->getValue("viewmode", ival);
    mCamViewMode->initAndSignal(ival);

    mActiveCamera->showBoundingBox(false);
    mActiveCamera->showHelper(false);

    mActiveCamera->getCamera()->setAspectRatio((Ogre::Real)mHandle->getActualWidth() / (Ogre::Real)mHandle->getActualHeight());
    mHandle->setCamera(mActiveCamera->getCamera());
}
//-----------------------------------------------------------------------------------------
bool CViewportEditor::getPropertyContextMenu(Ogre::String propertyName, UTFStringVector &menuitems)
{
    menuitems.clear();
    return false;
}
//-------------------------------------------------------------------------------
void CViewportEditor::onPropertyContextMenu(Ogre::String propertyName, int menuresult)
{
}
//-------------------------------------------------------------------------------
bool CViewportEditor::_setIndex(OgitorsPropertyBase* property, const int& value)
{
    return false;
}
//-----------------------------------------------------------------------------------------
bool CViewportEditor::_setPlacement(OgitorsPropertyBase* property, const Ogre::Vector4& value)
{
    if(mHandle)
    {
        mHandle->setDimensions(value.x, value.y, value.z, value.w);
        mHandle->getCamera()->setAspectRatio((Ogre::Real)mHandle->getActualWidth() / (Ogre::Real)mHandle->getActualHeight());
        mOgitorsRoot->ClearScreenBackground(true);
    }

    return true;
}
//-----------------------------------------------------------------------------------------
bool CViewportEditor::_setOverlays(OgitorsPropertyBase* property, const bool& value)
{
    if(mHandle)
    {
        mHandle->setOverlaysEnabled(value);
    }

    return true;
}
//-----------------------------------------------------------------------------------------
bool CViewportEditor::_setSkies(OgitorsPropertyBase* property, const bool& value)
{
    if(mHandle)
    {
        mHandle->setSkiesEnabled(value);
    }

    return true;
}
//-----------------------------------------------------------------------------------------
bool CViewportEditor::_setShadows(OgitorsPropertyBase* property, const bool& value)
{
    if(mHandle)
    {
        mHandle->setShadowsEnabled(value);
    }

    return true;
}
//-----------------------------------------------------------------------------------------
bool CViewportEditor::_setCamViewMode(OgitorsPropertyBase* property, const int& value)
{
    if(mViewCamera)
    {
        mViewCamera->setViewMode(value);
    }

    return true;
}
//-----------------------------------------------------------------------------------------
bool CViewportEditor::_setCamPosition(OgitorsPropertyBase* property, const Ogre::Vector3& value)
{
    if(mViewCamera)
    {
        mViewCamera->setPosition(value);
    }

    return true;
}
//-----------------------------------------------------------------------------------------
bool CViewportEditor::_setCamOrientation(OgitorsPropertyBase* property, const Ogre::Quaternion& value)
{
    if(mViewCamera)
    {
        mViewCamera->setOrientation(value);
    }

    return true;
}
//-----------------------------------------------------------------------------------------
bool CViewportEditor::_setCamClipDistance(OgitorsPropertyBase* property, const Ogre::Vector2& value)
{
    Ogre::Vector2 val = value;

    if(val.x < 0.05f)
    {
        val.x = 0.05f;
        mCamClipDistance->init(val);
    }

    if(mViewCamera)
    {
        mViewCamera->setClipDistance(val);
    }

    return true;
}
//-----------------------------------------------------------------------------------------
bool CViewportEditor::_setCamPolyMode(OgitorsPropertyBase* property, const int& value)
{
    if(mViewCamera)
    {
        mViewCamera->setPolygonMode(value);
        if(value != Ogre::PM_SOLID)
            mHandle->setVisibilityMask(0x7FFFFFFF);
        else
            mHandle->setVisibilityMask(0xFFFFFFFF);
    }

    return true;
}
//-----------------------------------------------------------------------------------------
bool CViewportEditor::_setCamFOV(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(mViewCamera)
    {
        mViewCamera->setFOV(value);
    }

    return true;
}
//-----------------------------------------------------------------------------------------
bool CViewportEditor::_setColour(OgitorsPropertyBase* property, const Ogre::ColourValue& value)
{
    if(mHandle)
    {
        mHandle->setBackgroundColour(value);
    }

    return true;
}
//-----------------------------------------------------------------------------------------
void CViewportEditor::prepareBeforePresentProperties()
{
}
//-----------------------------------------------------------------------------------------
void CViewportEditor::createProperties(OgitorsPropertyValueMap &params)
{
    PROPERTY_PTR(mViewportIndex  , "index"             ,int              ,0                       ,0, SETTER(int, CViewportEditor, _setIndex));
    PROPERTY_PTR(mColour         , "colour"            ,Ogre::ColourValue,Ogre::ColourValue(0,0,0),0, SETTER(Ogre::ColourValue, CViewportEditor, _setColour));
    PROPERTY_PTR(mPlacement      , "placement"         ,Ogre::Vector4    ,Ogre::Vector4(0,0,1,1)  ,0, SETTER(Ogre::Vector4, CViewportEditor, _setPlacement));
    PROPERTY_PTR(mOverlays       , "overlays"          ,bool             ,true                    ,0, SETTER(bool, CViewportEditor, _setOverlays));
    PROPERTY_PTR(mSkies          , "skies"             ,bool             ,true                    ,0, SETTER(bool, CViewportEditor, _setSkies));
    PROPERTY_PTR(mShadows        , "shadows"           ,bool             ,true                    ,0, SETTER(bool, CViewportEditor, _setShadows));
    PROPERTY_PTR(mCamViewMode    , "camera::viewmode"  ,int              ,CVM_FREE                ,0, SETTER(int, CViewportEditor, _setCamViewMode));
    PROPERTY_PTR(mCamPosition    , "camera::position"  ,Ogre::Vector3    ,Ogre::Vector3::ZERO     ,0, SETTER(Ogre::Vector3, CViewportEditor, _setCamPosition));
    PROPERTY_PTR(mCamOrientation , "camera::orientation",Ogre::Quaternion,Ogre::Quaternion::IDENTITY,0, SETTER(Ogre::Quaternion, CViewportEditor, _setCamOrientation));
    PROPERTY_PTR(mCamClipDistance, "camera::clipdistance",Ogre::Vector2  ,Ogre::Vector2(0.1f,9000.0f)   ,0, SETTER(Ogre::Vector2, CViewportEditor, _setCamClipDistance));
    PROPERTY_PTR(mCamPolyMode    , "camera::polymode"  ,int              ,Ogre::PM_SOLID          ,0, SETTER(int, CViewportEditor, _setCamPolyMode));
    PROPERTY_PTR(mCamFOV         , "camera::fov"       ,Ogre::Real       ,1.0f                    ,0, SETTER(Ogre::Real, CViewportEditor, _setCamFOV));

    mProperties.initValueMap(params);
}
//-----------------------------------------------------------------------------------------
bool CViewportEditor::load(bool async)
{
    if(mLoaded->get())
        return true;

    if(getParent()->load())
    {
        _createViewport();
    }
    else
        return false;

    mLoaded->set(true);
    return true;
}
//-----------------------------------------------------------------------------------------
bool CViewportEditor::unLoad()
{
    if(!mLoaded->get())
        return true;

    unLoadAllChildren();
    destroyBoundingBox();

    if(mHandle)
    {
        OGRE_DELETE mViewGrid;
        mViewGrid = 0;

        mViewCamera->destroy();
        mViewCamera = 0;

        Ogre::CompositorManager::getSingletonPtr()->removeCompositorChain(mHandle);

        mOgitorsRoot->GetRenderWindow()->removeViewport(mHandle->getZOrder());
        mHandle = 0;
    }

    mLoaded->set(false);
    return true;
}
//-----------------------------------------------------------------------------------------
TiXmlElement* CViewportEditor::exportDotScene(TiXmlElement *pParent)
{
    TiXmlElement *pViewport = pParent->InsertEndChild(TiXmlElement("viewport"))->ToElement();
    pViewport->SetAttribute("name", mName->get().c_str());
    pViewport->SetAttribute("id", Ogre::StringConverter::toString(mObjectID->get()).c_str());
    pViewport->SetAttribute("index", Ogre::StringConverter::toString(mViewportIndex->get()).c_str());

    TiXmlElement *pColour = pViewport->InsertEndChild(TiXmlElement("colour"))->ToElement();
    pColour->SetAttribute("r", Ogre::StringConverter::toString(mColour->get().r).c_str());
    pColour->SetAttribute("g", Ogre::StringConverter::toString(mColour->get().g).c_str());
    pColour->SetAttribute("b", Ogre::StringConverter::toString(mColour->get().b).c_str());
    pColour->SetAttribute("a", Ogre::StringConverter::toString(mColour->get().a).c_str());

    //TODO: actually, a camera can be moved from viewport to viewport - would it not be
    // more appropriate to give the camera a viewport index?
    TiXmlElement *pCamera = pParent->InsertEndChild(TiXmlElement("camera"))->ToElement();
    pCamera->SetAttribute("name", mActiveCamera->getName().c_str());
    pCamera->SetAttribute("viewMode", Ogre::StringConverter::toString(mCamViewMode->get()).c_str());
    pCamera->SetAttribute("polyMode", Ogre::StringConverter::toString(mCamPolyMode->get()).c_str());
    pCamera->SetAttribute("fov", Ogre::StringConverter::toString(mCamFOV->get()).c_str());

    TiXmlElement *pClipping = pCamera->InsertEndChild(TiXmlElement("clipping"))->ToElement();
    pClipping->SetAttribute("near", Ogre::StringConverter::toString(mCamClipDistance->get().x).c_str());
    pClipping->SetAttribute("far", Ogre::StringConverter::toString(mCamClipDistance->get().y).c_str());

    TiXmlElement *pPosition = pCamera->InsertEndChild(TiXmlElement("position"))->ToElement();
    pPosition->SetAttribute("x", Ogre::StringConverter::toString(mCamPosition->get().x).c_str());
    pPosition->SetAttribute("y", Ogre::StringConverter::toString(mCamPosition->get().y).c_str());
    pPosition->SetAttribute("z", Ogre::StringConverter::toString(mCamPosition->get().z).c_str());

    TiXmlElement *pOrientation = pCamera->InsertEndChild(TiXmlElement("rotation"))->ToElement();
    pOrientation->SetAttribute("qw", Ogre::StringConverter::toString(mCamOrientation->get().w).c_str());
    pOrientation->SetAttribute("qx", Ogre::StringConverter::toString(mCamOrientation->get().x).c_str());
    pOrientation->SetAttribute("qy", Ogre::StringConverter::toString(mCamOrientation->get().y).c_str());
    pOrientation->SetAttribute("qz", Ogre::StringConverter::toString(mCamOrientation->get().z).c_str());

    return pViewport;
}

//-------------------------------------------------------------------------------
//----------------CVIEWPORTEDITORFACTORY-----------------------------------------
//-------------------------------------------------------------------------------
CViewportEditorFactory::CViewportEditorFactory(OgitorsView *view) : CBaseEditorFactory(view)
{
    mTypeName = "Viewport";
    mEditorType = ETYPE_VIEWPORT;
    mIcon = "viewport.svg";
    mCapabilities = CAN_UNDO;
    mLastZOrder = 0;
    mUsesGizmos = false;
    mUsesHelper = false;

    OgitorsPropertyDef *definition;

    AddPropertyDefinition("index","Viewport Index","The index of the Viewport.",PROP_INT, true, false);
    AddPropertyDefinition("compositorcount","","The number of compositors chained.",PROP_INT, false, false);
    definition = AddPropertyDefinition("placement","Placement","The window placement of the Viewport.",PROP_VECTOR4);
    definition->setFieldNames("Left", "Top", "Width", "Height"); 
    AddPropertyDefinition("overlays","Overlays","Draw overlays?",PROP_BOOL);
    AddPropertyDefinition("skies","Skies","Draw skies?",PROP_BOOL);
    AddPropertyDefinition("shadows","Shadows","Draw shadows?",PROP_BOOL);
    definition = AddPropertyDefinition("camera::viewmode","Camera::View Mode","Custom View Mode restricting the rotation of the camera.", PROP_INT);
    definition->setOptions(CCameraEditorFactory::getCameraViewModes());
    AddPropertyDefinition("camera::position","Camera::Position","The camera position of the object.",PROP_VECTOR3,true, true, false);
    AddPropertyDefinition("camera::orientation","Camera::Orientation","The camera orientation of the object.",PROP_QUATERNION, true, false, false);
    definition = AddPropertyDefinition("camera::clipdistance","Camera::Clip Distance","The near and far clipping distances of the viewport camera. Use '0' for infinite far clip distance.", PROP_VECTOR2);
    definition->setFieldNames("Near", "Far"); 

    definition = AddPropertyDefinition("camera::polymode","Camera::Polygon Mode","The polygon mode of the viewport camera.", PROP_INT);
    definition->setOptions(CCameraEditorFactory::getCameraPolygonModes());

    AddPropertyDefinition("camera::fov","Camera::FOV","The FOV the viewport camera.", PROP_REAL);
    AddPropertyDefinition("colour","Colour","The colour of viewport background.", PROP_COLOUR);

    OgitorsPropertyDefMap::iterator it = mPropertyDefs.find("layer");
    it->second.setAccess(false, false);
}
//-----------------------------------------------------------------------------------------
CBaseEditorFactory *CViewportEditorFactory::duplicate(OgitorsView *view)
{
    CBaseEditorFactory *ret = OGRE_NEW CViewportEditorFactory(view);
    ret->mTypeID = mTypeID;

    return ret;
}
//-----------------------------------------------------------------------------------------
CBaseEditor *CViewportEditorFactory::CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params)
{
    CViewportEditor *object = OGRE_NEW CViewportEditor(this);

    OgitorsPropertyValueMap::const_iterator ni;

    int vIndex;

    if((ni = params.find("index")) != params.end())
    {
        vIndex = Ogre::any_cast<int>(ni->second.val);
        if(mLastZOrder < (unsigned int)vIndex)
            mLastZOrder = vIndex;
    }
    else
    {
        vIndex = ++mLastZOrder;
        OgitorsPropertyValue value;
        value.propType = PROP_INT;
        value.val = Ogre::Any(vIndex);
        params["index"] = value;     
    }

    if(vIndex == 1)
        OgitorsRoot::getSingletonPtr()->SetActiveViewport(object);

    *parent = OgitorsRoot::getSingletonPtr()->GetRootEditor();
    object->createProperties(params);
    object->mParentEditor->init(*parent);
    object->load(); 

    object->registerForPostSceneUpdates();

    mInstanceCount++;
    return object;
}
//-------------------------------------------------------------------------------
void CViewportEditorFactory::DestroyObject(CBaseEditor *object)
{
    CViewportEditor *editor = static_cast<CViewportEditor*>(object);

    OgitorsRoot::getSingletonPtr()->ClearScreenBackground(true);

    editor->destroyAllChildren();
    editor->unLoad();

    if(editor->getName() != "") 
        OgitorsRoot::getSingletonPtr()->UnRegisterObjectName(editor->getName(), editor);

    OGRE_DELETE editor;

    mInstanceCount--;

    NameObjectPairList viewports = OgitorsRoot::getSingletonPtr()->GetObjectsByType(ETYPE_VIEWPORT);
    NameObjectPairList::const_iterator vt = viewports.begin();

    mLastZOrder = 0;
    while(vt != viewports.end())
    {
        CViewportEditor *viewport = static_cast<CViewportEditor*>(vt->second);
        if(viewport->mViewportIndex->get() > (int)mLastZOrder)
            mLastZOrder = viewport->mViewportIndex->get();
        vt++;
    }
}
//-----------------------------------------------------------------------------------------
bool Ogitors::CViewportEditor::onBeforeDestroy()
{
    // Main viewport cannot be deleted
    if(getViewportIndex() == 1)
    {
        OgitorsSystem::getSingletonPtr()->DisplayMessageDialog(OTR("Cannot delete the main viewport!"), DLGTYPE_OK);
        return false;
    }

    return true;
}
//-----------------------------------------------------------------------------------------
void Ogitors::CViewportEditor::onDuringDestroy()
{
    // This viewport will be deleted now, so reorder the remaining ones.
    NameObjectPairList viewports = mOgitorsRoot->GetObjectsByType(ETYPE_VIEWPORT);
    NameObjectPairList::const_iterator vt = viewports.begin();
    while(vt != viewports.end())
    {
        if(vt->second == this)
        {
            vt++;
            continue;
        }
        mOgitorsRoot->SetActiveViewport(static_cast<CViewportEditor*>(vt->second));
        break;
    }
}
//-----------------------------------------------------------------------------------------
