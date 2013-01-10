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
#include "PGInstanceEditor.h"
#include "PGInstanceManager.h"
#include "OBBoxRenderable.h"

using namespace Ogitors;

//-----------------------------------------------------------------------------------------
CPGInstanceEditor::CPGInstanceEditor(CBaseEditorFactory *factory) : CBaseEditor(factory)
{
    mIndex = -1;
    mHandle = 0;
    mHelper = 0;
}
//-----------------------------------------------------------------------------------------
CPGInstanceEditor::~CPGInstanceEditor()
{
    static_cast<CPGInstanceManager*>(mParentEditor->get())->removeInstance(mIndex);
}
//-----------------------------------------------------------------------------------------
Ogre::AxisAlignedBox CPGInstanceEditor::getAABB()
{
    return mParentEditor->get()->getAABB();
}
//-----------------------------------------------------------------------------------------
Ogre::Vector3 CPGInstanceEditor::getDerivedPosition()
{
    return mPosition->get();
}
//-----------------------------------------------------------------------------------------
Ogre::Quaternion CPGInstanceEditor::getDerivedOrientation()
{
    return mOrientation->get();
}
//-----------------------------------------------------------------------------------------
Ogre::Vector3 CPGInstanceEditor::getDerivedScale()
{
    return mScale->get();
}
//-----------------------------------------------------------------------------------------
void CPGInstanceEditor::showBoundingBox(bool bShow) 
{
    if(!static_cast<CPGInstanceManager*>(mParentEditor->get())->getEntityHandle())
        return;

    if(bShow)
    {
        if(!mHandle)
        {
            mHandle = mOgitorsRoot->GetSceneManager()->getRootSceneNode()->createChildSceneNode(mName->get(), mPosition->get(), mOrientation->get());
            mHandle->setScale(mScale->get());
        }
        
        if(!mBoxParentNode)
            createBoundingBox();

        if(mBBoxNode)
        {
            int matpos = 0;
            
            mOBBoxRenderable->setMaterial(mOBBMaterials[matpos]);
            mBBoxNode->setVisible(bShow);
        }
    }
    else
    {
        destroyBoundingBox();

        if(mHandle)
        {
            mHandle->getParentSceneNode()->removeAndDestroyChild(mHandle->getName());
            mHandle = 0;
        }

    }
}
//-----------------------------------------------------------------------------------------
bool CPGInstanceEditor::load(bool async)
{
    if(mLoaded->get())
        return true;

    if(mIndex == -1)
    {
        mIndex = static_cast<CPGInstanceManager*>(mParentEditor->get())->addInstance(mPosition->get(), mUniformScale->get(), mYaw->get());
    }

    mLoaded->set(true);
    return true;
}
//-----------------------------------------------------------------------------------------
bool CPGInstanceEditor::unLoad()
{
    if(!mLoaded->get())
        return true;

    mHandle = 0;

    mLoaded->set(false);
    return true;
}
//-----------------------------------------------------------------------------------------
void CPGInstanceEditor::createProperties(OgitorsPropertyValueMap &params)
{    
    PROPERTY_PTR(mPosition       , "position"   ,Ogre::Vector3   ,Ogre::Vector3::ZERO       ,0,SETTER(Ogre::Vector3, CPGInstanceEditor, _setPosition));
    PROPERTY_PTR(mOrientation    , "orientation",Ogre::Quaternion,Ogre::Quaternion::IDENTITY,0,SETTER(Ogre::Quaternion, CPGInstanceEditor, _setOrientation));
    PROPERTY_PTR(mScale          , "scale"      ,Ogre::Vector3   ,Ogre::Vector3(1,1,1)      ,0,SETTER(Ogre::Vector3, CPGInstanceEditor, _setScale));
    PROPERTY_PTR(mUniformScale   , "uniformscale",Ogre::Real     ,1.0f                      ,0,SETTER(Ogre::Real, CPGInstanceEditor, _setUniformScale));
    PROPERTY_PTR(mYaw            , "yaw"        ,Ogre::Real      ,0.0f                      ,0,SETTER(Ogre::Real, CPGInstanceEditor, _setYaw));

    OgitorsPropertyValueMap::iterator ni;

    if((ni = params.find("index")) != params.end())
    {
        mIndex = Ogre::any_cast<int>(ni->second.val);
    }

    mProperties.initValueMap(params);
}
//-----------------------------------------------------------------------------------------
bool CPGInstanceEditor::_setPosition(OgitorsPropertyBase* property, const Ogre::Vector3& position)
{
    if(mHandle)
    {
        mHandle->setPosition(position);
    }

    static_cast<CPGInstanceManager*>(mParentEditor->get())->modifyInstancePosition(mIndex, position);

    return true;
}
//-----------------------------------------------------------------------------------------
bool CPGInstanceEditor::_setOrientation(OgitorsPropertyBase* property, const Ogre::Quaternion& orientation)
{
    Ogre::Radian rad = orientation.getYaw();

    if(mHandle)
    {
        Ogre::Quaternion q1;    

        q1.FromAngleAxis(rad, Ogre::Vector3::UNIT_Y);
        mHandle->setOrientation(q1);
    }

    mYaw->set(rad.valueDegrees());

    return true;
}
//-----------------------------------------------------------------------------------------
bool CPGInstanceEditor::_setScale(OgitorsPropertyBase* property, const Ogre::Vector3& scale)
{
    float scalemax = scale.length() / Ogre::Vector3::UNIT_SCALE.length();

    if(scalemax <= 0.0f)
        return false;

    if(scalemax > 2.0f)
        return false;

    if(mHandle)
    {
        mHandle->setScale(Ogre::Vector3(scalemax, scalemax, scalemax));
        mUniformScale->set(scalemax);
    }    
    return true;
}
//-----------------------------------------------------------------------------------------
bool CPGInstanceEditor::_setUniformScale(OgitorsPropertyBase* property, const Ogre::Real& scale)
{
    if(scale <= 0.0f)
        return false;

    if(mHandle)
    {
        mHandle->setScale(Ogre::Vector3(scale, scale, scale));
    }    
    
    mScale->init(Ogre::Vector3(scale, scale, scale));
    
    static_cast<CPGInstanceManager*>(mParentEditor->get())->modifyInstanceScale(mIndex, scale);

    return true;
}
//-----------------------------------------------------------------------------------------
bool CPGInstanceEditor::_setYaw(OgitorsPropertyBase* property, const Ogre::Real& yaw)
{
    Ogre::Quaternion q1;

    if(mHandle)
    {
        q1.FromAngleAxis(Ogre::Degree(yaw), Ogre::Vector3::UNIT_Y);
        mHandle->setOrientation(q1);
    }    

    mOrientation->init(q1);
    
    static_cast<CPGInstanceManager*>(mParentEditor->get())->modifyInstanceYaw(mIndex, yaw);

    return true;
}
//-----------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------
//------CMATERIALEDITORFACTORY-----------------------------------------------------------------
//-----------------------------------------------------------------------------------------
CPGInstanceEditorFactory::CPGInstanceEditorFactory(OgitorsView *view) : CBaseEditorFactory(view)
{
    mTypeName = "PGInstance";
    mEditorType = ETYPE_MOVABLE;
    mAddToObjectList = false;
    mRequirePlacement = true;
    mIcon = "pginstance.svg";
    mCapabilities = CAN_FOCUS | CAN_DELETE | CAN_UNDO | CAN_MOVE | CAN_SCALE | CAN_ROTATEY;
    mUsesGizmos = true;
    mUsesHelper = false;

    AddPropertyDefinition("position","Position","The position of the object.",PROP_VECTOR3);
    AddPropertyDefinition("orientation","Orientation","The orientation of the object.",PROP_QUATERNION,false, false, false);
    AddPropertyDefinition("scale","Scale","The scale of the object.", PROP_VECTOR3, false, false, false);
    AddPropertyDefinition("uniformscale","Scale","The uniform scale of the object.", PROP_REAL);
    AddPropertyDefinition("yaw","Yaw","The yaw of the object.", PROP_REAL);

    OgitorsPropertyDefMap::iterator it = mPropertyDefs.find("name");
    it->second.setAccess(true, false);

    it = mPropertyDefs.find("layer");
    it->second.setAccess(false, false);
}
//-----------------------------------------------------------------------------------------
CBaseEditorFactory *CPGInstanceEditorFactory::duplicate(OgitorsView *view)
{
    CBaseEditorFactory *ret = new CPGInstanceEditorFactory(view);
    ret->mTypeID = mTypeID;

    return ret;
}
//-----------------------------------------------------------------------------------------
CBaseEditor *CPGInstanceEditorFactory::CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params)
{
    OgitorsPropertyValueMap::iterator ni;

    if((ni = params.find("init")) != params.end())
    {
        OgitorsPropertyValue value = EMPTY_PROPERTY_VALUE;
        
        value.val = Ogre::Any(CreateUniqueID("Instance"));
        params["name"] = value;
        params.erase(ni);
        
        if((ni = params.find("index")) == params.end())
            return 0;
    }

    CPGInstanceEditor *object = new CPGInstanceEditor(this);

    object->createProperties(params);
    object->mParentEditor->init(*parent);
    object->load();

    mInstanceCount++;
    return object;
}
//-----------------------------------------------------------------------------------------
