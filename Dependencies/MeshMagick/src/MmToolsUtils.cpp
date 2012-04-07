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

#include "MmToolUtils.h"

#include <OgreMesh.h>
#include <OgreStringConverter.h>

using namespace Ogre;

namespace meshmagick
{
    String ToolUtils::getPrettyVectorString(const Vector3& v, unsigned short precision,
        unsigned short width, char fill, std::ios::fmtflags flags)
    {
        return "["
            + StringConverter::toString(v.x, precision, width, fill, flags) + ", "
            + StringConverter::toString(v.y, precision, width, fill, flags) + ", "
            + StringConverter::toString(v.z, precision, width, fill, flags)
            + "]";
    }

    String ToolUtils::getPrettyVectorString(const Vector4& v, unsigned short precision,
        unsigned short width, char fill, std::ios::fmtflags flags)
    {
        return "["
            + StringConverter::toString(v.x, precision, width, fill, flags) + ", "
            + StringConverter::toString(v.y, precision, width, fill, flags) + ", "
            + StringConverter::toString(v.z, precision, width, fill, flags) + ", "
            + StringConverter::toString(v.w, precision, width, fill, flags)
            + "]";
    }

    String ToolUtils::getPrettyAabbString(const AxisAlignedBox& aabb, unsigned short precision,
        unsigned short width, char fill, std::ios::fmtflags flags)
    {
        return "[" + getPrettyVectorString(aabb.getMinimum())
            + ", " + getPrettyVectorString(aabb.getMaximum()) + "]";
    }

    String ToolUtils::getPrettyMatrixString(const Matrix4& mm, unsigned short precision,
        unsigned short width, char fill, std::ios::fmtflags flags)
    {
        // Easier to work with in transposed form
        Matrix4 m = mm.transpose();
        String rval;
        for (unsigned short i = 0; i < 4; ++i)
        {
            Vector4 v = Vector4(m[i]);
            rval += getPrettyVectorString(v, precision, width, fill, flags);
            if (i < 3) rval += "\n";
        }
        return rval;
    }

    String ToolUtils::getPrettyMatrixString(const Matrix3& mm, unsigned short precision,
        unsigned short width, char fill, std::ios::fmtflags flags)
    {
        // Easier to work with in transposed form
        Matrix4 m = mm.Transpose();
        String rval;
        for (unsigned short i = 0; i < 3; ++i)
        {
            Vector3 v = Vector3(m[i]);
            rval += getPrettyVectorString(v, precision, width, fill, flags);
            if (i < 2) rval += "\n";
        }
        return rval;
    }

    String ToolUtils::getSkeletonFileName(const MeshPtr mesh, const String& meshFileName)
    {
        String rval;
        String skeletonName = mesh->getSkeletonName();
        // Decompose meshfilename into path and basename.
        String basename, path;
        StringUtil::splitFilename(meshFileName, basename, path);
        if (fileExists(path + skeletonName))
        {
            rval = path + skeletonName;
        }
        else if (fileExists(skeletonName))
        {
            rval = skeletonName;
        }
        else
        {
            rval = StringUtil::BLANK;
        }
        return rval;
    }

    // Code taken from http://www.codepedia.com/1/CppFileExists
    // Code is public domain according to codepedia terms of use.
    bool ToolUtils::fileExists(const Ogre::String& fileName)
    {
        std::fstream fin;
        fin.open(fileName.c_str(),std::ios::in);
        if( fin.is_open() )
        {
            fin.close();
            return true;
        }
        fin.close();
        return false;
    }
}

