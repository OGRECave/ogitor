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

