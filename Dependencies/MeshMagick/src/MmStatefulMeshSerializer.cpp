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

#include "MmStatefulMeshSerializer.h"

#include <OgreResourceGroupManager.h>
#include <OgreMeshManager.h>

#include <ios>
#include <iostream>
#include <stdexcept>

#include "MmEditableMesh.h"

using namespace Ogre;

namespace meshmagick
{
    const unsigned short HEADER_CHUNK_ID = 0x1000;

    MeshPtr StatefulMeshSerializer::loadMesh(const String& name)
    {
        MeshManager* mm = MeshManager::getSingletonPtr();
        MeshPtr mesh = mm->create(name, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        mMesh = MeshPtr(new EditableMesh(mm, name, mesh->getHandle(),
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME));

        std::ifstream ifs;
        ifs.open(name.c_str(), std::ios_base::in | std::ios_base::binary);
        if (!ifs)
        {
            throw std::ios_base::failure(("cannot open file " + name).c_str());
        }

        DataStreamPtr stream(new FileStreamDataStream(name, &ifs, false));

        determineFileFormat(stream);

        importMesh(stream, mMesh.getPointer());

        ifs.close();

        return mMesh;
    }

    void StatefulMeshSerializer::saveMesh(const Ogre::String& name, bool keepEndianess)
    {
        if (mMesh.isNull())
        {
            throw std::logic_error("No mesh to save set.");
        }

        Endian endianMode = keepEndianess ? mMeshFileEndian : ENDIAN_NATIVE;
        exportMesh(mMesh.getPointer(), name, endianMode);
    }

    void StatefulMeshSerializer::clear()
    {
        mMesh.setNull();
        mMeshFileEndian = ENDIAN_NATIVE;
        mMeshFileVersion = "";
    }

    MeshPtr StatefulMeshSerializer::getMesh() const
    {
        return mMesh;
    }

    void StatefulMeshSerializer::determineFileFormat(DataStreamPtr stream)
    {
        determineEndianness(stream);

        // Read header and determine the version
        unsigned short headerID;
        
        // Read header ID
        readShorts(stream, &headerID, 1);
        
        if (headerID != HEADER_CHUNK_ID)
        {
            OGRE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "File header not found",
                "MeshSerializer::importMesh");
        }
        // Read version
        mMeshFileVersion = readString(stream);
        // Jump back to start
        stream->seek(0);

#if OGRE_ENDIAN == OGRE_ENDIAN_BIG
        mMeshFileEndian = mFlipEndian ? ENDIAN_LITTLE : ENDIAN_BIG;
#else
        mMeshFileEndian = mFlipEndian ? ENDIAN_BIG : ENDIAN_LITTLE;
#endif
    }

    Ogre::String StatefulMeshSerializer::getMeshFileVersion() const
    {
        return mMeshFileVersion;
    }

    Ogre::Serializer::Endian StatefulMeshSerializer::getEndianMode() const
    {
        return mMeshFileEndian;
    }
}
