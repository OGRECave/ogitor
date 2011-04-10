/*
--------------------------------------------------------------------------------
This source file is part of SkyX.
Visit ---

Copyright (C) 2009 Xavier Verguín González <xavierverguin@hotmail.com>
                                           <xavyiy@gmail.com>

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
--------------------------------------------------------------------------------
*/

#include "GeometryManager.h"

#include "VClouds.h"

namespace SkyX { namespace VClouds
{

	GeometryManager::GeometryManager(VClouds* vc,
		    const Ogre::Vector2& Height, const float& Radius,
		    const Ogre::Radian& Alpha, const Ogre::Radian& Beta, 
            const int& NumberOfBlocks, const int& Na, const int& Nb, const int& Nc)
		: mVClouds(vc)
		, mCreated(false)
		, mHeight(Height)
		, mRadius(Radius)
		, mAlpha(Alpha)
		, mBeta(Beta)
		, mPhi(Ogre::Math::TWO_PI / NumberOfBlocks)
		, mNumberOfBlocks(NumberOfBlocks)
		, mNa(Na) , mNb(Nb) , mNc (Nc)
		, mWorldOffset(Ogre::Vector2(0,0))
		, mLastCameraPosition(Ogre::Vector3(0,0,0))
	{
	}

	GeometryManager::~GeometryManager()
	{
		remove();
	}

	void GeometryManager::create()
	{
		remove();

		mSceneNode = mVClouds->getSceneManager()->getRootSceneNode()->createChildSceneNode();
		_createGeometry();

		mCreated = true;
	}

	void GeometryManager::remove()
	{
		if (!mCreated)
		{
			return;
		}

		mSceneNode->detachAllObjects();
		mSceneNode->getParentSceneNode()->removeAndDestroyChild(mSceneNode->getName());
		mSceneNode = 0;

		for (int k = 0; k < mNumberOfBlocks; k++)
		{
			delete mGeometryBlocks.at(k);
			mGeometryBlocks.at(k) = 0;
		}

		mGeometryBlocks.clear();

		mCreated = false;
	}

	void GeometryManager::update(const Ogre::Real& timeSinceLastFrame)
	{
		if (!mCreated)
		{
			return;
		}

		mSceneNode->setPosition(mVClouds->getCamera()->getDerivedPosition().x, mHeight.x, mVClouds->getCamera()->getDerivedPosition().z);

		_updateGeometry(timeSinceLastFrame);

		mLastCameraPosition = mVClouds->getCamera()->getDerivedPosition();
	}

	void GeometryManager::_createGeometry()
	{
		for (int k = 0; k < mNumberOfBlocks; k++)
		{
			mGeometryBlocks.push_back(new GeometryBlock(mVClouds, mHeight.y, mAlpha, mBeta, mRadius, mPhi, mNa, mNb, mNc, k));
			mGeometryBlocks.at(k)->create();
			// Each geometry block must be in a different scene node, See: GeometryBlock::isInFrustum(Ogre::Camera *c)
			Ogre::SceneNode *sn = mSceneNode->createChildSceneNode();
			sn->attachObject(mGeometryBlocks.at(k)->getEntity());
		}
	}

	void GeometryManager::_updateGeometry(const Ogre::Real& timeSinceLastFrame)
	{
		// Calculate wind offset
		Ogre::Vector2 CameraDirection = Ogre::Vector2(mVClouds->getCamera()->getDerivedDirection().x, mVClouds->getCamera()->getDerivedDirection().z);
		float offset = - CameraDirection.dotProduct(mVClouds->getWindDirectionV2()) * mVClouds->getWindSpeed() * timeSinceLastFrame;
		mWorldOffset += mVClouds->getWindDirectionV2() * mVClouds->getWindSpeed() * timeSinceLastFrame;

		// Calculate camera offset
		Ogre::Vector2 CameraOffset = Ogre::Vector2(mVClouds->getCamera()->getDerivedPosition().x - mLastCameraPosition.x, mVClouds->getCamera()->getDerivedPosition().z - mLastCameraPosition.z);
		offset -= CameraOffset.dotProduct(CameraDirection);
		mWorldOffset += CameraOffset;

		for (int k = 0; k < mNumberOfBlocks; k++)
		{
			mGeometryBlocks.at(k)->update(offset);
			mGeometryBlocks.at(k)->setWorldOffset(mWorldOffset);
		}
	}
}}