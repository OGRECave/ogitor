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
