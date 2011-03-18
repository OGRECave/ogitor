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

#include "VCloudsManager.h"

#include "SkyX.h"

namespace SkyX
{
	VCloudsManager::VCloudsManager(SkyX *s)
		: mSkyX(s)
		, mVClouds(0)
		, mHeight(Ogre::Vector2(-1, -1))
		, mWindSpeed(800.0f)
		, mAutoupdate(true)
		, mCreated(false)
	{
		mAmbientGradient = ColorGradient();
		mAmbientGradient.addCFrame(ColorGradient::ColorFrame(Ogre::Vector3(1,1,1)*0.9f, 1.0f));
		mAmbientGradient.addCFrame(ColorGradient::ColorFrame(Ogre::Vector3(0.7,0.7,0.65), 0.625f)); 
		mAmbientGradient.addCFrame(ColorGradient::ColorFrame(Ogre::Vector3(0.6,0.55,0.4)*0.5, 0.5625f));
		mAmbientGradient.addCFrame(ColorGradient::ColorFrame(Ogre::Vector3(0.6,0.55,0.4)*0.2, 0.475f));
		mAmbientGradient.addCFrame(ColorGradient::ColorFrame(Ogre::Vector3(0.6,0.45,0.3)*0.1, 0.5f));
		mAmbientGradient.addCFrame(ColorGradient::ColorFrame(Ogre::Vector3(0.2,0.2,0.3)*0.25, 0.35f));
		mAmbientGradient.addCFrame(ColorGradient::ColorFrame(Ogre::Vector3(0.2,0.2,0.3)*0.3, 0));

		mSunGradient = ColorGradient();
		mSunGradient.addCFrame(ColorGradient::ColorFrame(Ogre::Vector3(1,1,1)*0.9f, 1.0f));
		mSunGradient.addCFrame(ColorGradient::ColorFrame(Ogre::Vector3(1,1,1)*0.8, 0.75f));
		mSunGradient.addCFrame(ColorGradient::ColorFrame(Ogre::Vector3(0.8,0.75,0.55)*1.3, 0.5625f));
		mSunGradient.addCFrame(ColorGradient::ColorFrame(Ogre::Vector3(0.6,0.5,0.2)*1.5, 0.5f));
		mSunGradient.addCFrame(ColorGradient::ColorFrame(Ogre::Vector3(0.6,0.5,0.2)*0.2, 0.4725f));
		mSunGradient.addCFrame(ColorGradient::ColorFrame(Ogre::Vector3(0.5,0.5,0.5)*0.05, 0.45f));
		mSunGradient.addCFrame(ColorGradient::ColorFrame(Ogre::Vector3(0.25,0.25,0.25), 0.3725f));
		mSunGradient.addCFrame(ColorGradient::ColorFrame(Ogre::Vector3(0.5,0.5,0.5), 0.0f));
	}

	VCloudsManager::~VCloudsManager()
	{
		remove();
	}

	void VCloudsManager::create()
	{
		if (mCreated)
		{
			return;
		}

		float radius = mSkyX->getCamera()->getFarClipDistance();
		// Use default options if the user haven't set any specific Height parameters
		Ogre::Vector2 defaultheight = Ogre::Vector2(radius*0.03f, radius*0.0525f); 
		Ogre::Vector2 height = (mHeight.x == -1 || mHeight.y == -1) ? defaultheight : mHeight;
		
		mVClouds = new VClouds::VClouds(mSkyX->getSceneManager(), mSkyX->getCamera(), height, radius * 0.8f);
		mVClouds->setWheater(0.5, 1, 4);
		mVClouds->create();

		mCreated = true;

		_updateWindSpeedConfig();
	}

	void VCloudsManager::update(const Ogre::Real& timeSinceLastFrame)
	{
		if (!mCreated)
		{
			return;
		}

		_setLightParameters();

		mVClouds->update(timeSinceLastFrame);
	}

	void VCloudsManager::remove()
	{
		if (!mCreated)
		{
			return;
		}

		delete mVClouds;

		mCreated = false;
	}

	void VCloudsManager::_setLightParameters()
	{
		Ogre::Vector3 SunDir = mSkyX->getAtmosphereManager()->getSunDirection();
		if (SunDir.y > 0.1f)
		{
			SunDir = -SunDir;
		}

		mVClouds->setSunDirection(SunDir);

		float point = (-mSkyX->getAtmosphereManager()->getSunDirection().y + 1.0f) / 2.0f;

		mVClouds->setAmbientColor(mAmbientGradient.getColor(point));
		mVClouds->setSunColor(mSunGradient.getColor(point));
	}

	void VCloudsManager::_updateWindSpeedConfig()
	{
		if (!mCreated)
		{
			return;
		}

		if (mAutoupdate)
		{
			mVClouds->setWindSpeed(mSkyX->getTimeMultiplier() * mWindSpeed);
		}
		else
		{
			mVClouds->setWindSpeed(mWindSpeed);
		}
	}
}