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

#ifndef __MM_STATEFUL_SKELETON_SERIALIZER_H__
#define __MM_STATEFUL_SKELETON_SERIALIZER_H__

#include "MeshMagickPrerequisites.h"

#ifdef __APPLE__
#	include <Ogre/OgreSkeleton.h>
#	include <Ogre/OgreSkeletonSerializer.h>
#	include <Ogre/OgreString.h>
#else
#	include <OgreSkeleton.h>
#	include <OgreSkeletonSerializer.h>
#	include <OgreString.h>
#endif

#include "MmOptionsParser.h"

namespace meshmagick
{
    class _MeshMagickExport StatefulSkeletonSerializer : public Ogre::SkeletonSerializer
    {
    public:
        Ogre::SkeletonPtr loadSkeleton(const Ogre::String& name);
        void saveSkeleton(const Ogre::String& name, bool keepEndianess);
        void clear();
        Ogre::SkeletonPtr getSkeleton() const;
    private:
        Ogre::SkeletonPtr mSkeleton;
        Ogre::String mSkeletonFileVersion;
        Endian mSkeletonFileEndian;

        void determineFileFormat(Ogre::DataStreamPtr stream);
    };
}
#endif
