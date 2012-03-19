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
#include "VisualHelper.h"
#include "OgitorsRoot.h"
#include "LightEditor.h"
#include "LightVisualHelper.h"
#include "MultiSelEditor.h"


using namespace Ogitors;
      

// step count for line circles (higher == smoother)
const unsigned int   gCircleVertexCount      = 32;

// number of indices we will use
const size_t         gEstimateIndexCount     = 0;

// number of vertices we will draw (4 circles + 8 connecting lines)
const size_t         gEstimatedVertexCount   = gCircleVertexCount * 4 + 16;

// constructor
CLightVisualHelper::CLightVisualHelper(CBaseEditor *parent)
: CEntityVisualHelper(parent),
mLightType(Ogre::Light::LT_POINT),
mColour1(0.6f, 0.8f, 0.9f),
mColour2(0.3f, 0.4f, 0.45f),
mSpotlightInnerAngle(Ogre::Degree(50.0f)),
mSpotlightOuterAngle(Ogre::Degree(60.0f)),
mAttenuationRange(100),
mSpotlightStartCircleOffset(0.65f),
mDirectionalLightCircleRadius(2.0f),
mDirectionalLightLength(6.0f),
mResourceGroup("General")
{
    CLightEditor *light = static_cast<CLightEditor*>(mParent);
    if(light)
    {
        mLightType = (Ogre::Light::LightTypes)light->getLightType();
        mColour1 = light->getDiffuse();
        mColour2 = light->getSpecular();
        mSpotlightInnerAngle = Ogre::Degree(light->getRange().x);
        mSpotlightOuterAngle = Ogre::Degree(light->getRange().y);
        mAttenuationRange = light->getAttenuation().x;
    }

    CONNECT_PROPERTY_MEMFN(mParent, "selected", CLightVisualHelper, OnSelectionChange, mPropConnections[0]);
    CONNECT_PROPERTY_MEMFN(mParent, "lighttype", CLightVisualHelper, OnLightTypeChange, mPropConnections[1]);
    CONNECT_PROPERTY_MEMFN(mParent, "diffuse", CLightVisualHelper, OnDiffuseChange, mPropConnections[2]);
    CONNECT_PROPERTY_MEMFN(mParent, "specular", CLightVisualHelper, OnSpecularChange, mPropConnections[3]);
    CONNECT_PROPERTY_MEMFN(mParent, "lightrange", CLightVisualHelper, OnRangeChange, mPropConnections[4]);
    CONNECT_PROPERTY_MEMFN(mParent, "attenuation", CLightVisualHelper, OnAttenuationChange, mPropConnections[5]);

    mSceneManager = parent->getSceneManager();

    // create our manual object and set initial values
    mVisualHelperObject =  mSceneManager->createManualObject("LVH" + parent->getName());
    mVisualHelperObject->estimateIndexCount(gEstimateIndexCount);
    mVisualHelperObject->estimateVertexCount(gEstimatedVertexCount);
    mVisualHelperObject->setDynamic(true);

    // create a material for the visual representation
    mMaterial = Ogre::MaterialManager::getSingleton().create("LVH" + parent->getName() + "_Material", mResourceGroup);
    mMaterial->setReceiveShadows(false);
    mMaterial->getTechnique(0)->getPass(0)->setLightingEnabled(false); 
    mMaterial->getTechnique(0)->getPass(0)->setVertexColourTracking(Ogre::TVC_DIFFUSE);

    // create 5 sections, four LineStrip (for max. 4 circles) and one Line version
    mVisualHelperObject->begin(mMaterial->getName(), Ogre::RenderOperation::OT_LINE_STRIP);
    mVisualHelperObject->position(0,0,0);  // dummy
    mVisualHelperObject->colour(1,1,1);    // dummy
    mVisualHelperObject->position(0,0,0);  // dummy
    mVisualHelperObject->colour(1,1,1);    // dummy
    mVisualHelperObject->end();

    mVisualHelperObject->begin(mMaterial->getName(), Ogre::RenderOperation::OT_LINE_STRIP);
    mVisualHelperObject->position(0,0,0);  // dummy
    mVisualHelperObject->colour(1,1,1);    // dummy
    mVisualHelperObject->position(0,0,0);  // dummy
    mVisualHelperObject->colour(1,1,1);    // dummy
    mVisualHelperObject->end();

    mVisualHelperObject->begin(mMaterial->getName(), Ogre::RenderOperation::OT_LINE_STRIP);
    mVisualHelperObject->position(0,0,0);  // dummy
    mVisualHelperObject->colour(1,1,1);    // dummy
    mVisualHelperObject->position(0,0,0);  // dummy
    mVisualHelperObject->colour(1,1,1);    // dummy
    mVisualHelperObject->end();

    mVisualHelperObject->begin(mMaterial->getName(), Ogre::RenderOperation::OT_LINE_STRIP);
    mVisualHelperObject->position(0,0,0);  // dummy
    mVisualHelperObject->colour(1,1,1);    // dummy
    mVisualHelperObject->position(0,0,0);  // dummy
    mVisualHelperObject->colour(1,1,1);    // dummy
    mVisualHelperObject->end();

    mVisualHelperObject->begin(mMaterial->getName(), Ogre::RenderOperation::OT_LINE_LIST);
    mVisualHelperObject->position(0,0,0);  // dummy
    mVisualHelperObject->colour(1,1,1);    // dummy
    mVisualHelperObject->position(0,0,0);  // dummy
    mVisualHelperObject->colour(1,1,1);    // dummy
    mVisualHelperObject->end();

    mVisualHelperObject->setVisible(false);
    mNode->attachObject(mVisualHelperObject);

    if(mLightType == Ogre::Light::LT_POINT)
    {
        SetModel("scbLight_Omni.mesh");
        _createPointLight();
    }
    else if(mLightType == Ogre::Light::LT_DIRECTIONAL)
    {
        SetModel("scbLight_Direct.mesh");
        _createDirectionalLight();
    }
    else if(mLightType == Ogre::Light::LT_SPOTLIGHT)
    {
        SetModel("scbLight_Spot.mesh");
        _createSpotLight();
    }
}

// destructor
CLightVisualHelper::~CLightVisualHelper()
{
   mVisualHelperObject->detachFromParent();
   mSceneManager->destroyManualObject(mVisualHelperObject);
   Ogre::MaterialManager::getSingletonPtr()->remove(mMaterial->getHandle());
}

void CLightVisualHelper::_createPointLight()
{
   // empty our sections
   mVisualHelperObject->beginUpdate(0);
   mVisualHelperObject->end();
   mVisualHelperObject->beginUpdate(1);
   mVisualHelperObject->end();
   mVisualHelperObject->beginUpdate(2);
   mVisualHelperObject->end();
   mVisualHelperObject->beginUpdate(3);
   mVisualHelperObject->end();
   mVisualHelperObject->beginUpdate(4);
   mVisualHelperObject->end();
}


void CLightVisualHelper::_createDirectionalLight()
{
   // check for valid angles
   Ogre::Real radius;
   if(mDirectionalLightCircleRadius < 0.0f)
      radius = 0.0f;
   else
      radius = mDirectionalLightCircleRadius;

   // draw circles
   mVisualHelperObject->beginUpdate(0);
   _drawCircle(radius, 0, mColour1);
   mVisualHelperObject->end();

   mVisualHelperObject->beginUpdate(1);
   _drawCircle(radius, mDirectionalLightLength, mColour1);
   mVisualHelperObject->end();

   // empty sections
   mVisualHelperObject->beginUpdate(2);
   mVisualHelperObject->end();
   mVisualHelperObject->beginUpdate(3);
   mVisualHelperObject->end();

   // draw four lines between both circles
   mVisualHelperObject->beginUpdate(4);
   mVisualHelperObject->position(radius,   0,       0);
   mVisualHelperObject->colour(mColour1);
   mVisualHelperObject->position(radius,   0,       mDirectionalLightLength);
   mVisualHelperObject->colour(mColour1);
   mVisualHelperObject->position(0,        radius,  0);
   mVisualHelperObject->colour(mColour1);
   mVisualHelperObject->position(0,        radius,  mDirectionalLightLength);
   mVisualHelperObject->colour(mColour1);
   mVisualHelperObject->position(0,        -radius, 0);
   mVisualHelperObject->colour(mColour1);
   mVisualHelperObject->position(0,        -radius, mDirectionalLightLength);
   mVisualHelperObject->colour(mColour1);
   mVisualHelperObject->position(-radius,  0,       0);
   mVisualHelperObject->colour(mColour1);
   mVisualHelperObject->position(-radius,  0,       mDirectionalLightLength);
   mVisualHelperObject->colour(mColour1);
   mVisualHelperObject->end();
}


void CLightVisualHelper::_createSpotLight()
{
   // check for valid angles
   Ogre::Radian outerAngle, innerAngle;
   if(mSpotlightOuterAngle < Ogre::Radian(0.0f))
   {
      outerAngle = 0.0f;
      innerAngle = 0.0f;
   }
   else
   {
      if(mSpotlightOuterAngle > Ogre::Radian(Ogre::Math::PI))
         outerAngle = Ogre::Radian(Ogre::Math::PI);
      else
         outerAngle = mSpotlightOuterAngle;

      if(mSpotlightInnerAngle > outerAngle)
         innerAngle = outerAngle;
      else
         innerAngle = mSpotlightInnerAngle;
   }

   // draw inner circles
   mVisualHelperObject->beginUpdate(0);
   Ogre::Real firstInnerRadius = Ogre::Math::Tan(innerAngle/2) * mSpotlightStartCircleOffset;
   _drawCircle(firstInnerRadius, mSpotlightStartCircleOffset, mColour2);
   mVisualHelperObject->end();

   mVisualHelperObject->beginUpdate(1);
   Ogre::Real secondInnerRadius = Ogre::Math::Tan(innerAngle/2) * mAttenuationRange;
   _drawCircle(secondInnerRadius, mAttenuationRange, mColour2);
   mVisualHelperObject->end();

   // draw first outer circles
   mVisualHelperObject->beginUpdate(2);
   Ogre::Real firstOuterRadius = Ogre::Math::Tan(outerAngle/2) * mSpotlightStartCircleOffset;
   _drawCircle(firstOuterRadius, mSpotlightStartCircleOffset, mColour1);
   mVisualHelperObject->end();
   
   mVisualHelperObject->beginUpdate(3);
   Ogre::Real secondOuterRadius = Ogre::Math::Tan(outerAngle/2) * mAttenuationRange;
   _drawCircle(secondOuterRadius, mAttenuationRange, mColour1);
   mVisualHelperObject->end();

   mVisualHelperObject->beginUpdate(4);  
   
   // draw 4 lines between inner circles
   mVisualHelperObject->position(firstInnerRadius,    0,             mSpotlightStartCircleOffset);
   mVisualHelperObject->colour(mColour2);
   mVisualHelperObject->position(secondInnerRadius,   0,             mAttenuationRange);
   mVisualHelperObject->colour(mColour2);
   mVisualHelperObject->position(0,              firstInnerRadius,   mSpotlightStartCircleOffset);
   mVisualHelperObject->colour(mColour2);
   mVisualHelperObject->position(0,              secondInnerRadius,  mAttenuationRange);
   mVisualHelperObject->colour(mColour2);
   mVisualHelperObject->position(0,              -firstInnerRadius,  mSpotlightStartCircleOffset);
   mVisualHelperObject->colour(mColour2);
   mVisualHelperObject->position(0,              -secondInnerRadius, mAttenuationRange);
   mVisualHelperObject->colour(mColour2);
   mVisualHelperObject->position(-firstInnerRadius,   0,             mSpotlightStartCircleOffset);
   mVisualHelperObject->colour(mColour2);
   mVisualHelperObject->position(-secondInnerRadius,  0,             mAttenuationRange);
   mVisualHelperObject->colour(mColour2);

   // draw 4 lines between outer circles
   mVisualHelperObject->position(firstOuterRadius,    0,             mSpotlightStartCircleOffset);
   mVisualHelperObject->colour(mColour1);
   mVisualHelperObject->position(secondOuterRadius,   0,             mAttenuationRange);
   mVisualHelperObject->colour(mColour1);
   mVisualHelperObject->position(0,              firstOuterRadius,   mSpotlightStartCircleOffset);
   mVisualHelperObject->colour(mColour1);
   mVisualHelperObject->position(0,              secondOuterRadius,  mAttenuationRange);
   mVisualHelperObject->colour(mColour1);
   mVisualHelperObject->position(0,              -firstOuterRadius,  mSpotlightStartCircleOffset);
   mVisualHelperObject->colour(mColour1);
   mVisualHelperObject->position(0,              -secondOuterRadius, mAttenuationRange);
   mVisualHelperObject->colour(mColour1);
   mVisualHelperObject->position(-firstOuterRadius,   0,             mSpotlightStartCircleOffset);
   mVisualHelperObject->colour(mColour1);
   mVisualHelperObject->position(-secondOuterRadius,  0,             mAttenuationRange);
   mVisualHelperObject->colour(mColour1);
   
   mVisualHelperObject->end();
}


void CLightVisualHelper::_drawCircle(const Ogre::Real radius, const Ogre::Real zOffset, const Ogre::ColourValue& colour)
{
   for (unsigned int i = 0; i <= gCircleVertexCount; i++)
   {
      Ogre::Real x = Ogre::Math::Cos(Ogre::Radian((Ogre::Math::TWO_PI*i)/gCircleVertexCount)) * radius;
      Ogre::Real y = Ogre::Math::Sin(Ogre::Radian((Ogre::Math::TWO_PI*i)/gCircleVertexCount)) * radius;
      mVisualHelperObject->position(x, y, zOffset);
      mVisualHelperObject->colour(colour);
   }
}

void CLightVisualHelper::Show(bool bShow)
{
    mVisible = bShow;
    if(mHandle)
        mHandle->setVisible(bShow);

    mVisualHelperObject->setVisible(bShow && (OgitorsRoot::getSingletonPtr()->GetSelection()->getAsSingle() == mParent));
}

void CLightVisualHelper::OnDiffuseChange(const OgitorsPropertyBase* property, Ogre::Any value)
{
    mColour1 = Ogre::any_cast<Ogre::ColourValue>(value);
    if(mLightType == Ogre::Light::LT_POINT)
        _createPointLight();
    else if(mLightType == Ogre::Light::LT_DIRECTIONAL)
        _createDirectionalLight();
    else if(mLightType == Ogre::Light::LT_SPOTLIGHT)
        _createSpotLight();
}

void CLightVisualHelper::OnSpecularChange(const OgitorsPropertyBase* property, Ogre::Any value)
{
    mColour2 = Ogre::any_cast<Ogre::ColourValue>(value);
    if(mLightType == Ogre::Light::LT_POINT)
        _createPointLight();
    else if(mLightType == Ogre::Light::LT_DIRECTIONAL)
        _createDirectionalLight();
    else if(mLightType == Ogre::Light::LT_SPOTLIGHT)
        _createSpotLight();
}

void CLightVisualHelper::OnLightTypeChange(const OgitorsPropertyBase* property, Ogre::Any value)
{
    mLightType = (Ogre::Light::LightTypes)Ogre::any_cast<int>(value);
    if(mLightType == Ogre::Light::LT_POINT)
    {
        SetModel("scbLight_Omni.mesh");
        _createPointLight();
    }
    else if(mLightType == Ogre::Light::LT_DIRECTIONAL)
    {
        SetModel("scbLight_Direct.mesh");
        _createDirectionalLight();
    }
    else if(mLightType == Ogre::Light::LT_SPOTLIGHT)
    {
        SetModel("scbLight_Spot.mesh");
        _createSpotLight();
    }
}

void CLightVisualHelper::OnRangeChange(const OgitorsPropertyBase* property, Ogre::Any value)
{
    Ogre::Vector3 range = Ogre::any_cast<Ogre::Vector3>(value);
    mSpotlightInnerAngle = Ogre::Degree(range.x);
    mSpotlightOuterAngle = Ogre::Degree(range.y);
    if(mLightType == Ogre::Light::LT_POINT)
        _createPointLight();
    else if(mLightType == Ogre::Light::LT_DIRECTIONAL)
        _createDirectionalLight();
    else if(mLightType == Ogre::Light::LT_SPOTLIGHT)
        _createSpotLight();
}

void CLightVisualHelper::OnAttenuationChange(const OgitorsPropertyBase* property, Ogre::Any value)
{
    Ogre::Vector4 att = Ogre::any_cast<Ogre::Vector4>(value);
    mAttenuationRange = att.x;
    if(mLightType == Ogre::Light::LT_POINT)
        _createPointLight();
    else if(mLightType == Ogre::Light::LT_DIRECTIONAL)
        _createDirectionalLight();
    else if(mLightType == Ogre::Light::LT_SPOTLIGHT)
        _createSpotLight();
}

void CLightVisualHelper::OnSelectionChange(const OgitorsPropertyBase* property, Ogre::Any value)
{
    mVisualHelperObject->setVisible(Ogre::any_cast<bool>(value) && mVisible);
}