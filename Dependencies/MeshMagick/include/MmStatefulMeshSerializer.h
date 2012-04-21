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

#ifndef __MM_STATEFUL_MESH_SERIALIZER_H__
#define __MM_STATEFUL_MESH_SERIALIZER_H__

#include "MeshMagickPrerequisites.h"

#ifdef __APPLE__
#	include <Ogre/OgreMesh.h>
#	include <Ogre/OgreMeshSerializer.h>
#	include <Ogre/OgreString.h>
#else
#	include <OgreMesh.h>
#	include <OgreMeshSerializer.h>
#	include <OgreString.h>
#endif

namespace meshmagick
{
    class _MeshMagickExport StatefulMeshSerializer : public Ogre::MeshSerializer
    {
    public:
        Ogre::MeshPtr loadMesh(const Ogre::String& name);
        void saveMesh(const Ogre::String& name, bool keepEndianess);
        void clear();
        Ogre::MeshPtr getMesh() const;
        Ogre::String getMeshFileVersion() const;
        Ogre::Serializer::Endian getEndianMode() const;
    private:
        Ogre::MeshPtr mMesh;
        Ogre::String mMeshFileVersion;
        Endian mMeshFileEndian;

        void determineFileFormat(Ogre::DataStreamPtr stream);
    };
}
#endif
