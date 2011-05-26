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

#include "MoonManager.h"

#include "SkyX.h"

namespace SkyX
{
	MoonManager::MoonManager(SkyX *s)
		: mSkyX(s)
		, mMoonBillboard(0)
		, mMoonSceneNode(0)
		, mCreated(false)
		, mMoonSize(0.225f)
	{
	}

	MoonManager::~MoonManager()
	{
		remove();
	}

	void MoonManager::create()
	{
		if (mCreated)
		{
			return;
		}

		mMoonSceneNode = mSkyX->getSceneManager()->getRootSceneNode()->createChildSceneNode();

		mMoonBillboard = mSkyX->getSceneManager()->createBillboardSet("SkyXMoonBillboardSet", 1);
        mMoonBillboard->setMaterialName(mSkyX->getGPUManager()->getMoonMaterialName());
		mMoonBillboard->setBillboardType(Ogre::BBT_ORIENTED_COMMON);
		mMoonBillboard->setRenderQueueGroup(Ogre::RENDER_QUEUE_SKIES_EARLY+1);
		mMoonBillboard->setCastShadows(false);

		mMoonBillboard->createBillboard(Ogre::Vector3(0,0,0));

		mMoonSceneNode->attachObject(mMoonBillboard);

		mMoonSceneNode->setPosition(mSkyX->getCamera()->getDerivedPosition());

		mCreated = true;

		_updateMoonBounds();
		update();
	}

	void MoonManager::update()
	{
		if (!mCreated)
		{
			return;
		}

		float radius = mSkyX->getCamera()->getFarClipDistance()*0.95f,
              size = radius*mMoonSize;

		mMoonBillboard->setCommonDirection((mSkyX->getAtmosphereManager()->getSunDirection()).normalisedCopy().perpendicular());

		Ogre::Vector3 moonRelativePos = mSkyX->getAtmosphereManager()->getSunDirection()*
			Ogre::Math::Cos(Ogre::Math::ASin((size/2)/radius))*radius;

		mMoonSceneNode->setPosition(mSkyX->getCamera()->getDerivedPosition() + moonRelativePos);

		if (moonRelativePos.y < -size/2)
		{
			mMoonSceneNode->setVisible(false);
		}
		else
		{
			mMoonSceneNode->setVisible(true);

			static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("SkyX_Moon"))->getTechnique(0)->getPass(0)
				->getVertexProgramParameters()->setNamedConstant("uSkydomeCenter", mSkyX->getCamera()->getDerivedPosition());
		}
	}

	void MoonManager::remove()
	{
		if (!mCreated)
		{
			return;
		}

		mMoonSceneNode->detachAllObjects();
		mMoonSceneNode->getParentSceneNode()->removeAndDestroyChild(mMoonSceneNode->getName());
		mMoonSceneNode = 0;

		mSkyX->getSceneManager()->destroyBillboardSet(mMoonBillboard);
		mMoonBillboard = 0;

		mCreated = false;
	}

	void MoonManager::setMoonSize(const Ogre::Real& MoonSize)
	{
		mMoonSize = MoonSize;

		if (!mCreated)
		{
			return;
		}

		_updateMoonBounds();
	}

	void MoonManager::_updateMoonBounds()
	{
		float radius = mSkyX->getCamera()->getFarClipDistance()*0.95f,
              size = radius*mMoonSize;

		mMoonBillboard->setDefaultDimensions(size, size);
		mMoonBillboard->setBounds(Ogre::AxisAlignedBox(-size/2, -size/2, -size/2,
														size/2,  size/2,  size/2),
								  1);
		mMoonSceneNode->_updateBounds();
	}
}