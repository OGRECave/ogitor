/*
This file is part of MeshMagick - An Ogre mesh file manipulation tool.
Copyright (C) 2007-2010 Daniel Wickert

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "MmOgreEnvironment.h"

#include <OgreRoot.h>
#include <OgreLogManager.h>
#include <OgreMaterialManager.h>
#include <OgreMeshManager.h>
#include <OgreResourceGroupManager.h>
#include <OgreSkeletonManager.h>

using namespace Ogre;

template<> meshmagick::OgreEnvironment* Singleton<meshmagick::OgreEnvironment>::msSingleton = NULL;

namespace meshmagick
{
    OgreEnvironment::OgreEnvironment()
        : mLogMgr(NULL),
          mResourceGroupMgr(NULL),
          mMath(NULL),
          mMeshMgr(NULL),
#if OGRE_VERSION_MAJOR > 1 || (OGRE_VERSION_MAJOR == 1 && OGRE_VERSION_MINOR >= 7)
		  mLodStrategyMgr(NULL),
#endif
          mMaterialMgr(NULL),
          mSkeletonMgr(NULL),
          mMeshSerializer(NULL),
          mSkeletonSerializer(NULL),
          mBufferManager(NULL),
		  mStandalone(false)
    {
    }

    OgreEnvironment::~OgreEnvironment()
    {
		delete mSkeletonSerializer;
		delete mMeshSerializer;

		if (mStandalone)
		{
			delete mBufferManager;
#if OGRE_VERSION_MAJOR > 1 || (OGRE_VERSION_MAJOR == 1 && OGRE_VERSION_MINOR >= 7)
			delete mLodStrategyMgr;
#endif
			delete mMath;
			mLogMgr->destroyLog(mLog);
			delete mLogMgr;
			delete mRoot;
		}
    }

	void OgreEnvironment::initialize(bool standalone, Ogre::Log* log)
	{
		if (standalone)
		{
			mLogMgr = new LogManager();
			mLog = mLogMgr->createLog("meshmagick.log", true, false, true);
			mRoot = new Root();
			mResourceGroupMgr = ResourceGroupManager::getSingletonPtr();
			mMath = new Math();
			mMeshMgr = MeshManager::getSingletonPtr();
			mMeshMgr->setBoundsPaddingFactor(0.0f);
#if OGRE_VERSION_MAJOR > 1 || (OGRE_VERSION_MAJOR == 1 && OGRE_VERSION_MINOR >= 7)
			mLodStrategyMgr = new LodStrategyManager();
#endif
			mMaterialMgr = MaterialManager::getSingletonPtr();
			mMaterialMgr->initialise();
			mSkeletonMgr = SkeletonManager::getSingletonPtr();
			mBufferManager = new DefaultHardwareBufferManager();
			mStandalone = true;
		}
		else
		{
			mLog = log;
			mStandalone = false;
		}

		mMeshSerializer = new StatefulMeshSerializer();
        mSkeletonSerializer = new StatefulSkeletonSerializer();
	}

	bool OgreEnvironment::isStandalone() const
	{
		return mStandalone;
	}

	Ogre::Log* OgreEnvironment::getLog() const
	{
		return mLog;
	}

    StatefulMeshSerializer* OgreEnvironment::getMeshSerializer() const
    {
        return mMeshSerializer;
    }

    StatefulSkeletonSerializer* OgreEnvironment::getSkeletonSerializer() const
    {
        return mSkeletonSerializer;
    }
}
