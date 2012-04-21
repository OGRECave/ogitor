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
