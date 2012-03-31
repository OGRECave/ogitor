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

#include "VClouds.h"

#include "../SkyX.h"

namespace SkyX { namespace VClouds
{
	VClouds::VClouds(Ogre::SceneManager *sm, Ogre::Camera *c,
			const Ogre::Vector2& Height, const float& Radius,
			const Ogre::Radian& Alpha, const Ogre::Radian& Beta, 
            const int& NumberOfBlocks, const int& Na, const int& Nb, const int& Nc)
		: mSceneManager(sm)
		, mCamera(c)
		, mCreated(false)
		, mHeight(Height)
		, mAlpha(Alpha)
		, mBeta(Beta)
		, mRadius(Radius)
		, mNumberOfBlocks(NumberOfBlocks)
		, mNa(Na) , mNb(Nb) , mNc (Nc)
		, mWindDirection(Ogre::Degree(0))
		, mWindSpeed(80.0f)
		, mWheater(Ogre::Vector2(1.0f, 1.0f))
		, mNumberOfForcedUpdates(-1)
		, mSunDirection(Ogre::Vector3(0,-1,0))
		, mSunColor(Ogre::Vector3(1,1,1))
		, mAmbientColor(Ogre::Vector3(0.63f,0.63f,0.7f))
		, mLightResponse(Ogre::Vector4(0.25f,0.2f,1.0f,0.1f))
		, mAmbientFactors(Ogre::Vector4(0.4f,1.0f,1.0f,1.0f))
		, mGlobalOpacity(1.0f)
		, mCloudFieldScale(1.0f)
		, mNoiseScale(4.2f)
		, mDataManager(0)
		, mGeometryManager(0)
	{
	}

	VClouds::VClouds(Ogre::SceneManager *sm, Ogre::Camera *c,
			const Ogre::Vector2& Height, const float& Radius)
		: mSceneManager(sm)
		, mCamera(c)
		, mCreated(false)
		, mHeight(Height)
		, mAlpha(Ogre::Degree(12))
		, mBeta(Ogre::Degree(40))
		, mRadius(Radius)
		, mNumberOfBlocks(12)
		, mNa(10) , mNb(8) , mNc (6)
		, mWindDirection(Ogre::Degree(0))
		, mWindSpeed(80.0f)
		, mWheater(Ogre::Vector2(1.0f, 1.0f))
		, mNumberOfForcedUpdates(-1)
		, mSunDirection(Ogre::Vector3(0,-1,0))
		, mSunColor(Ogre::Vector3(1,1,1))
		, mAmbientColor(Ogre::Vector3(0.63f,0.63f,0.7f))
		, mLightResponse(Ogre::Vector4(0.25f,0.2f,1.0f,0.1f))
		, mAmbientFactors(Ogre::Vector4(0.4f,1.0f,1.0f,1.0f))
		, mGlobalOpacity(1.0f)
		, mCloudFieldScale(1.0f)
		, mNoiseScale(4.2f)
		, mDataManager(0)
		, mGeometryManager(0)
	{
	}

	VClouds::~VClouds()
	{
		remove();
	}

	void VClouds::create()
	{
		remove();

		// Data manager
		mDataManager = new DataManager(this);
		mDataManager->create(128,128,20);

		// Geometry manager
		mGeometryManager = new GeometryManager(this, mHeight, mRadius, mAlpha, mBeta, mNumberOfBlocks, mNa, mNb, mNc);
		mGeometryManager->create();

		static_cast<Ogre::MaterialPtr>(
			Ogre::MaterialManager::getSingleton().getByName("SkyX_VolClouds"))
			->getTechnique(0)->getPass(0)->getVertexProgramParameters()->setNamedConstant("uRadius", mRadius);

		mCreated = true;

		// Update material parameters
		setSunColor(mSunColor);
		setAmbientColor(mAmbientColor);
		setLightResponse(mLightResponse);
		setAmbientFactors(mAmbientFactors);

		// Set current wheater
		int nforced = (mNumberOfForcedUpdates == -1) ? 2 : mNumberOfForcedUpdates;
		setWheater(mWheater.x, mWheater.y, nforced);
	}

	void VClouds::remove()
	{
		if (!mCreated)
		{
			return;
		}

		delete mDataManager;
		delete mGeometryManager;

		mCreated = false;
	}

	void VClouds::update(const Ogre::Real& timeSinceLastFrame)
	{
		if (!mCreated)
		{
			return;
		}

		mDataManager->update(timeSinceLastFrame);
		mGeometryManager->update(timeSinceLastFrame);

		static_cast<Ogre::MaterialPtr>(
			Ogre::MaterialManager::getSingleton().getByName("SkyX_VolClouds"))
			->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("uInterpolation", mDataManager->_getInterpolation());
		static_cast<Ogre::MaterialPtr>(
			Ogre::MaterialManager::getSingleton().getByName("SkyX_VolClouds"))
			->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("uSunDirection", -mSunDirection);
	}

	void VClouds::setSunColor(const Ogre::Vector3& SunColor)
	{
		mSunColor = SunColor;

		if (!mCreated)
		{
			return;
		}

		static_cast<Ogre::MaterialPtr>(
			Ogre::MaterialManager::getSingleton().getByName("SkyX_VolClouds"))
			->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("uSunColor", mSunColor);
	}

	void VClouds::setAmbientColor(const Ogre::Vector3& AmbientColor)
	{
		mAmbientColor = AmbientColor;

		if (!mCreated)
		{
			return;
		}

		static_cast<Ogre::MaterialPtr>(
			Ogre::MaterialManager::getSingleton().getByName("SkyX_VolClouds"))
			->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("uAmbientColor", mAmbientColor);
	}

	void VClouds::setLightResponse(const Ogre::Vector4& LightResponse)
	{
		mLightResponse = LightResponse;

		if (!mCreated)
		{
			return;
		}

		static_cast<Ogre::MaterialPtr>(
			Ogre::MaterialManager::getSingleton().getByName("SkyX_VolClouds"))
			->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("uLightResponse", mLightResponse);
	}

	void VClouds::setAmbientFactors(const Ogre::Vector4& AmbientFactors)
	{
		mAmbientFactors = AmbientFactors;

		if (!mCreated)
		{
			return;
		}

		static_cast<Ogre::MaterialPtr>(
			Ogre::MaterialManager::getSingleton().getByName("SkyX_VolClouds"))
			->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("uAmbientFactors", mAmbientFactors);
	}

	void VClouds::setWheater(const float& Humidity, const float& AverageCloudsSize, const int& NumberOfForcedUpdates)
	{
		mWheater = Ogre::Vector2(Humidity, AverageCloudsSize);
		mNumberOfForcedUpdates = NumberOfForcedUpdates;

		if (!mCreated)
		{
			return;
		}

		mDataManager->setWheater(mWheater.x, mWheater.y, mNumberOfForcedUpdates);
	}

}}