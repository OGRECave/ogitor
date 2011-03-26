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

#pragma warning(disable:4355)

#include "SkyX.h"

namespace SkyX
{
	SkyX::SkyX(Ogre::SceneManager* sm, Ogre::Camera* c)
		: mSceneManager(sm)
		, mCamera(c)
		, mMeshManager(new MeshManager(this))
		, mAtmosphereManager(new AtmosphereManager(this))
		, mGPUManager(new GPUManager(this))
		, mMoonManager(new MoonManager(this))
		, mCloudsManager(new CloudsManager(this))
		, mVCloudsManager(new VCloudsManager(this))
		, mCreated(false)
		, mLastCameraPosition(Ogre::Vector3(0,0,0))
		, mLastCameraFarClipDistance(0)
		, mLightingMode(LM_LDR)
		, mStarfield(true)
		, mTimeMultiplier(0.1f)
		, mTimeOffset(0.0f)
	{
	}

	SkyX::~SkyX()
	{
		remove();

		delete mMeshManager;
		delete mAtmosphereManager;
		delete mGPUManager;
		delete mMoonManager;
		delete mCloudsManager;
		delete mVCloudsManager;
	}

	void SkyX::create()
	{
		if (mCreated)
		{
			return;
		}

		mMeshManager->create();

		mMeshManager->setMaterialName(mGPUManager->getSkydomeMaterialName());

		mAtmosphereManager->_update(mAtmosphereManager->getOptions(), true);

		mLastCameraPosition = mCamera->getDerivedPosition();
		mLastCameraFarClipDistance = mCamera->getFarClipDistance();

		mMoonManager->create();

		mCreated = true;
	}

	void SkyX::remove()
	{
		if (!mCreated)
		{
			return;
		}

		mCloudsManager->removeAll();
		mMeshManager->remove();
		mMoonManager->remove();
		mVCloudsManager->remove();

		mCreated = false;
	}

	void SkyX::update(const Ogre::Real& timeSinceLastFrame)
	{
		if (!mCreated)
		{
			return;
		}

		if (mTimeMultiplier != 0)
		{
			AtmosphereManager::Options opt = mAtmosphereManager->getOptions();

			float timemultiplied = timeSinceLastFrame * mTimeMultiplier;
			opt.Time.x += timemultiplied;
			mTimeOffset += timemultiplied;
			if (opt.Time.x > 24)
			{
				opt.Time.x -= 24;
			} 
			else if (opt.Time.x < 0)
			{
				opt.Time.x += 24;
			}

			mAtmosphereManager->setOptions(opt);
		}

		if (mLastCameraPosition != mCamera->getDerivedPosition())
		{
			mMeshManager->getSceneNode()->setPosition(mCamera->getDerivedPosition());
			mMoonManager->getMoonSceneNode()->setPosition(mCamera->getDerivedPosition());

			mLastCameraPosition = mCamera->getDerivedPosition();
		}

		if (mLastCameraFarClipDistance != mCamera->getFarClipDistance())
		{
			mMeshManager->_updateGeometry();
			mMoonManager->update();

			mLastCameraFarClipDistance = mCamera->getFarClipDistance();
		}

		mVCloudsManager->update(timeSinceLastFrame);
	}

	void SkyX::setLightingMode(const LightingMode& lm)
	{
		mLightingMode = lm;

		if (!mCreated)
		{
			return;
		}

		// Update skydome material
		mMeshManager->setMaterialName(mGPUManager->getSkydomeMaterialName());
		// Update layered clouds material
		mCloudsManager->registerAll();
		// Update ground passes materials
		mGPUManager->_updateFP();

		// Update parameters
		mAtmosphereManager->_update(mAtmosphereManager->getOptions(), true);
	}

	void SkyX::setStarfieldEnabled(const bool& Enabled)
	{
		mStarfield = Enabled;

		if (!mCreated)
		{
			return;
		}

		// Update skydome material
		mMeshManager->setMaterialName(mGPUManager->getSkydomeMaterialName());

		// Update parameters
		mAtmosphereManager->_update(mAtmosphereManager->getOptions(), true);
	}
}