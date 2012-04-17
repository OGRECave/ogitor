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

#include "MmStatefulSkeletonSerializer.h"

#include <OgreResourceGroupManager.h>
#include <OgreSkeletonManager.h>

#include <ios>
#include <iostream>
#include <stdexcept>

#include "MmEditableSkeleton.h"

using namespace Ogre;

namespace meshmagick
{
    const unsigned short HEADER_CHUNK_ID = 0x1000;

    SkeletonPtr StatefulSkeletonSerializer::loadSkeleton(const String& name)
    {
        // Resource already created upon mesh loading?
        mSkeleton = SkeletonManager::getSingleton().getByName(name);
        if (mSkeleton.isNull())
        {
            // Nope. We create it here then.
            mSkeleton = SkeletonManager::getSingleton().create(name, 
                ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        }

		mSkeleton = SkeletonPtr(new EditableSkeleton(*mSkeleton.getPointer()));

        std::ifstream ifs;
        ifs.open(name.c_str(), std::ios_base::in | std::ios_base::binary);
        if (!ifs)
        {
            throw std::ios_base::failure(("cannot open file " + name).c_str());
        }

        DataStreamPtr stream(new FileStreamDataStream(name, &ifs, false));

        determineFileFormat(stream);

        importSkeleton(stream, mSkeleton.getPointer());

        ifs.close();

		return mSkeleton;
    }

    void StatefulSkeletonSerializer::saveSkeleton(const String& name, bool keepEndianess)
    {
        if (mSkeleton.isNull())
        {
            throw std::logic_error("No skeleton to save set.");
        }

        Endian endianMode = keepEndianess ? mSkeletonFileEndian : ENDIAN_NATIVE;
        exportSkeleton(mSkeleton.getPointer(), name, SKELETON_VERSION_LATEST, endianMode);
    }

    void StatefulSkeletonSerializer::clear()
    {
        mSkeleton.setNull();
    }

    SkeletonPtr StatefulSkeletonSerializer::getSkeleton() const
    {
        return mSkeleton;
    }

    void StatefulSkeletonSerializer::determineFileFormat(DataStreamPtr stream)
    {
        determineEndianness(stream);

#if OGRE_ENDIAN == OGRE_ENDIAN_BIG
        mSkeletonFileEndian = mFlipEndian ? ENDIAN_LITTLE : ENDIAN_BIG;
#else
        mSkeletonFileEndian = mFlipEndian ? ENDIAN_BIG : ENDIAN_LITTLE;
#endif
    }
}
