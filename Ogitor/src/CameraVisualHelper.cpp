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
#include "VisualHelper.h"
#include "OgitorsRoot.h"
#include "CameraEditor.h"
#include "ViewportEditor.h"
#include "CameraVisualHelper.h"
#include "MultiSelEditor.h"

using namespace Ogitors;


// number of indices we will use
const size_t         gCameraEstimateIndexCount     = 0;

// number of vertices we will draw (2 quads and 4 conecting lines)
const size_t         gCameraEstimatedVertexCount   = 2 * 4 + 4 * 2;

// constructor
CCameraVisualHelper::CCameraVisualHelper(CBaseEditor *parent)
: CEntityVisualHelper(parent),
  mColour(0.588f, 0.588f, 0.588f),
  mNearClipDistance(0.1f),
  mFarClipDistance(100.0f),
  mQuadNearOffset(1.0f),
  mQuadFarOffset(5.0f),
  mResourceGroup("General")
{
    Ogre::Camera *cam = static_cast<CCameraEditor*>(mParent)->getCamera();
    if(cam)
    {
        cam->getFrustumExtents(mFrustumLeft,mFrustumRight,mFrustumTop,mFrustumBottom);
        mNearClipDistance = cam->getNearClipDistance();
        mFarClipDistance = cam->getFarClipDistance();
    }

    SetModel("scbCamera.mesh");

    CONNECT_PROPERTY_MEMFN(mParent, "selected", CCameraVisualHelper, OnSelectionChange, mPropConnections[0]);
    CONNECT_PROPERTY_MEMFN(mParent, "fov", CCameraVisualHelper, OnFOVChange, mPropConnections[1]);
    CONNECT_PROPERTY_MEMFN(mParent, "clipdistance", CCameraVisualHelper, OnClipDistanceChange, mPropConnections[2]);

    mSceneManager = parent->getSceneManager();
    // create our manual object and set initial values
    mVisualHelperObject =  mSceneManager->createManualObject("CVH" + parent->getName());
    mVisualHelperObject->estimateIndexCount(gCameraEstimateIndexCount);
    mVisualHelperObject->estimateVertexCount(gCameraEstimatedVertexCount);
    mVisualHelperObject->setDynamic(true);

    // create a material for the visual representation
#if (OGRE_VERSION < ((1 << 16) | (9 << 8) | 0))
    mMaterial = Ogre::MaterialManager::getSingleton().create("CVH" + parent->getName() + "_Material", mResourceGroup);
#else
    mMaterial = Ogre::MaterialManager::getSingleton().create("CVH" + parent->getName() + "_Material", mResourceGroup).staticCast<Ogre::Material>();
#endif
    mMaterial->setReceiveShadows(false);
    mMaterial->getTechnique(0)->getPass(0)->setLightingEnabled(false); 
    mMaterial->getTechnique(0)->getPass(0)->setVertexColourTracking(Ogre::TVC_DIFFUSE);

    // create 3 sections, 2 for both quads and 1 for the connecting lines
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
    _createCamera();
}

// destructor
CCameraVisualHelper::~CCameraVisualHelper()
{
   mVisualHelperObject->detachFromParent();
   mSceneManager->destroyManualObject(mVisualHelperObject);
   Ogre::MaterialManager::getSingletonPtr()->remove(mMaterial->getHandle());
}

void CCameraVisualHelper::Show(bool bShow)
{
    mVisible = bShow;
    if(mHandle)
        mHandle->setVisible(bShow);

    mVisualHelperObject->setVisible(bShow && (OgitorsRoot::getSingletonPtr()->GetSelection()->getAsSingle() == mParent));
}
//--------------------------------------------------------------------------------
void CCameraVisualHelper::_createCamera()
{
   //// Near Quad ////
   Ogre::Real frustumLeftNear    = mFrustumLeft;
   Ogre::Real frustumRightNear   = mFrustumRight;
   Ogre::Real frustumTopNear     = mFrustumTop;
   Ogre::Real frustumBottomNear  = mFrustumBottom;

   mVisualHelperObject->beginUpdate(0);
   mVisualHelperObject->position(frustumLeftNear, frustumBottomNear, -mQuadNearOffset - mNearClipDistance);
   mVisualHelperObject->colour(mColour);
   mVisualHelperObject->position(frustumRightNear, frustumBottomNear, -mQuadNearOffset - mNearClipDistance);
   mVisualHelperObject->colour(mColour);
   mVisualHelperObject->position(frustumRightNear, frustumTopNear, -mQuadNearOffset - mNearClipDistance);
   mVisualHelperObject->colour(mColour);
   mVisualHelperObject->position(frustumLeftNear, frustumTopNear, -mQuadNearOffset - mNearClipDistance);
   mVisualHelperObject->colour(mColour);
   mVisualHelperObject->position(frustumLeftNear, frustumBottomNear, -mQuadNearOffset - mNearClipDistance);
   mVisualHelperObject->colour(mColour);
   mVisualHelperObject->end();

   //// Far Quad ////
   Ogre::Real frustumLeftFar    = mFrustumLeft   * (mFarClipDistance / mNearClipDistance);
   Ogre::Real frustumRightFar   = mFrustumRight  * (mFarClipDistance / mNearClipDistance);
   Ogre::Real frustumTopFar     = mFrustumTop    * (mFarClipDistance / mNearClipDistance);
   Ogre::Real frustumBottomFar  = mFrustumBottom * (mFarClipDistance / mNearClipDistance);

   mVisualHelperObject->beginUpdate(1);
   mVisualHelperObject->position(frustumLeftFar, frustumBottomFar, -mQuadNearOffset - mFarClipDistance);
   mVisualHelperObject->colour(mColour);
   mVisualHelperObject->position(frustumRightFar, frustumBottomFar, -mQuadNearOffset - mFarClipDistance);
   mVisualHelperObject->colour(mColour);
   mVisualHelperObject->position(frustumRightFar, frustumTopFar, -mQuadNearOffset - mFarClipDistance);
   mVisualHelperObject->colour(mColour);
   mVisualHelperObject->position(frustumLeftFar, frustumTopFar, -mQuadNearOffset - mFarClipDistance);
   mVisualHelperObject->colour(mColour);
   mVisualHelperObject->position(frustumLeftFar, frustumBottomFar, -mQuadNearOffset - mFarClipDistance);
   mVisualHelperObject->colour(mColour);
   mVisualHelperObject->end();

   //// connecting lines ////
   mVisualHelperObject->beginUpdate(2);
   mVisualHelperObject->position(frustumLeftNear, frustumBottomNear, -mQuadNearOffset - mNearClipDistance);
   mVisualHelperObject->colour(mColour);
   mVisualHelperObject->position(frustumLeftFar, frustumBottomFar, -mQuadNearOffset - mFarClipDistance);
   mVisualHelperObject->colour(mColour);

   mVisualHelperObject->position(frustumRightNear, frustumBottomNear, -mQuadNearOffset - mNearClipDistance);
   mVisualHelperObject->colour(mColour);
   mVisualHelperObject->position(frustumRightFar, frustumBottomFar, -mQuadNearOffset - mFarClipDistance);
   mVisualHelperObject->colour(mColour);

   mVisualHelperObject->position(frustumLeftNear, frustumTopNear, -mQuadNearOffset - mNearClipDistance);
   mVisualHelperObject->colour(mColour);
   mVisualHelperObject->position(frustumLeftFar, frustumTopFar, -mQuadNearOffset  - mFarClipDistance);
   mVisualHelperObject->colour(mColour);

   mVisualHelperObject->position(frustumRightNear, frustumTopNear, -mQuadNearOffset - mNearClipDistance);
   mVisualHelperObject->colour(mColour);
   mVisualHelperObject->position(frustumRightFar, frustumTopFar, -mQuadNearOffset - mFarClipDistance);
   mVisualHelperObject->colour(mColour);
   mVisualHelperObject->end();
}


void CCameraVisualHelper::OnFOVChange(const OgitorsPropertyBase* property, Ogre::Any value)
{
    static_cast<Ogre::Camera*>(mParent->getHandle())->getFrustumExtents(mFrustumLeft,mFrustumRight,mFrustumTop,mFrustumBottom);
    _createCamera();
}

void CCameraVisualHelper::OnClipDistanceChange(const OgitorsPropertyBase* property, Ogre::Any value)
{
    Ogre::Vector2 dist = Ogre::any_cast<Ogre::Vector2>(value);
    mNearClipDistance = dist.x;
    mFarClipDistance = dist.y;
    _createCamera();
}

void CCameraVisualHelper::OnSelectionChange(const OgitorsPropertyBase* property, Ogre::Any value)
{
    bool newstate = Ogre::any_cast<bool>(value);
    if(OgitorsRoot::getSingletonPtr()->GetViewport()->getCameraEditor() == mParent)
    {
       newstate = false;
    }
    mVisualHelperObject->setVisible(newstate && mVisible);
}