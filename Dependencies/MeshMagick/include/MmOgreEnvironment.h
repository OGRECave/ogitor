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

#ifndef __MM_OGRE_ENVIRONMENT_H__
#define __MM_OGRE_ENVIRONMENT_H__

#include "MeshMagickPrerequisites.h"

#ifdef __APPLE__
#	include <Ogre/OgreDefaultHardwareBufferManager.h>
#else
#	include <OgreDefaultHardwareBufferManager.h>
#endif

#if OGRE_VERSION_MAJOR > 1 || (OGRE_VERSION_MAJOR == 1 && OGRE_VERSION_MINOR >= 7)
# ifdef __APPLE__
#	include <Ogre/OgreLodStrategyManager.h>
# else
#	include <OgreLodStrategyManager.h>
# endif
#endif

#include "MmStatefulMeshSerializer.h"
#include "MmStatefulSkeletonSerializer.h"

namespace meshmagick
{
    class _MeshMagickExport OgreEnvironment : public Ogre::Singleton<OgreEnvironment>
    {
    public:
        OgreEnvironment();
        ~OgreEnvironment();

		/** Initializes the environment
		 * @param standalone set to <code>true</code> if the environment should be set up
		 * for standalone use (all managers are created in this case), if set to 
		 * <code>false</code>, all managers must have been created in advance, a log must 
		 * be set in the 2nd parameter
		 * @param log a logfile (only used when standalone is <code>false</code>)
		 */
		void initialize(bool standalone = true, Ogre::Log* log = NULL);

        StatefulMeshSerializer* getMeshSerializer() const;
        StatefulSkeletonSerializer* getSkeletonSerializer() const;
		Ogre::Log* getLog() const;
		bool isStandalone() const;

    private:
		Ogre::Root* mRoot;
        Ogre::LogManager* mLogMgr;
        Ogre::Log* mLog;
        Ogre::ResourceGroupManager* mResourceGroupMgr;
        Ogre::Math* mMath;
        Ogre::MeshManager* mMeshMgr;
#if OGRE_VERSION_MAJOR > 1 || (OGRE_VERSION_MAJOR == 1 && OGRE_VERSION_MINOR >= 7)
        Ogre::LodStrategyManager* mLodStrategyMgr;
#endif
        Ogre::MaterialManager* mMaterialMgr;
        Ogre::SkeletonManager* mSkeletonMgr;
        StatefulMeshSerializer* mMeshSerializer;
        StatefulSkeletonSerializer* mSkeletonSerializer;
        Ogre::DefaultHardwareBufferManager* mBufferManager;
		bool mStandalone;
    };
}

#endif
