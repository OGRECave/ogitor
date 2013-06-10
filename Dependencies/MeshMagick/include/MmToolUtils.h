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

#ifndef __MM_TOOL_UTILS_H__
#define __MM_TOOL_UTILS_H__

#include "MeshMagickPrerequisites.h"

#ifdef __APPLE__
#	include <Ogre/OgreAxisAlignedBox.h>
#	include <Ogre/OgreMatrix3.h>
#	include <Ogre/OgreMatrix4.h>
#	include <Ogre/OgreVector3.h>
#	include <Ogre/OgreVector4.h>
#else
#	include <OgreAxisAlignedBox.h>
#	include <OgreMatrix3.h>
#	include <OgreMatrix4.h>
#	include <OgreVector3.h>
#	include <OgreVector4.h>
#endif


namespace meshmagick
{
    /// General purpose utility functions, what doesn't fit elsewhere fits here.
    class _MeshMagickExport ToolUtils
    {
    public:
        static Ogre::String getPrettyVectorString(const Ogre::Vector3&, unsigned short precision=3,
			unsigned short width=0, char fill= ' ',
			std::ios::fmtflags flags=std::ios::fmtflags(std::ios_base::fixed));

        static Ogre::String getPrettyVectorString(const Ogre::Vector4&, unsigned short precision=3,
            unsigned short width=0, char fill= ' ',
			std::ios::fmtflags flags=std::ios::fmtflags(std::ios_base::fixed));

        static Ogre::String getPrettyAabbString(const Ogre::AxisAlignedBox&, unsigned short precision=3,
            unsigned short width=0, char fill= ' ',
			std::ios::fmtflags flags=std::ios::fmtflags(std::ios_base::fixed));

        static Ogre::String getPrettyMatrixString(const Ogre::Matrix4&, unsigned short precision=3,
            unsigned short width=0, char fill= ' ',
			std::ios::fmtflags flags=std::ios::fmtflags(std::ios_base::fixed));

        static Ogre::String getPrettyMatrixString(const Ogre::Matrix3&, unsigned short precision=3,
            unsigned short width=0, char fill= ' ',
			std::ios::fmtflags flags=std::ios::fmtflags(std::ios_base::fixed));

        static bool fileExists(const Ogre::String& fileName);
        
        /// Returns the guessed fully qualified file name of the skeleton file referenced by
        /// given mesh.
        /// It first tries to find the skeleton file in the same directory as the mesh file.
        /// If not found there, it is searched in working dir,
        /// if not found there, Ogre::StringUtil::BLANK is returned.
        static Ogre::String getSkeletonFileName(const Ogre::MeshPtr, const Ogre::String& meshFileName);

    };
}
#endif
