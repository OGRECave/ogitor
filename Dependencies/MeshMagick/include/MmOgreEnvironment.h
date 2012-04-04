/*
This file is part of MeshMagick - An Ogre mesh file manipulation tool.
Copyright (C) 2007-2009 Daniel Wickert

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef __MM_OGRE_ENVIRONMENT_H__
#define __MM_OGRE_ENVIRONMENT_H__

#include "MeshMagickPrerequisites.h"

#ifdef __APPLE__
#	include <Ogre/OgreDefaultHardwareBufferManager.h>
#else
#	include <OgreDefaultHardwareBufferManager.h>
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
        Ogre::LogManager* mLogMgr;
        Ogre::Log* mLog;
        Ogre::ResourceGroupManager* mResourceGroupMgr;
        Ogre::Math* mMath;
        Ogre::MeshManager* mMeshMgr;
        Ogre::MaterialManager* mMaterialMgr;
        Ogre::SkeletonManager* mSkeletonMgr;
        StatefulMeshSerializer* mMeshSerializer;
        StatefulSkeletonSerializer* mSkeletonSerializer;
        Ogre::DefaultHardwareBufferManager* mBufferManager;
		bool mStandalone;
    };
}

#endif
