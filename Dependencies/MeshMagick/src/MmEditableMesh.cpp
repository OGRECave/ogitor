/*
This file is part of MeshMagick - An Ogre mesh file manipulation tool.
Copyright (C) 2007-2010 Sascha Kolewa, Daniel Wickert

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

#include "MmEditableMesh.h"

namespace meshmagick 
{

EditableMesh::EditableMesh(Ogre::ResourceManager* creator, const Ogre::String& name, Ogre::ResourceHandle handle,
            const Ogre::String& group, bool isManual, Ogre::ManualResourceLoader* loader)
: Ogre::Mesh(creator, name, handle, group, isManual, loader)
{

}

EditableMesh::~EditableMesh()
{
}

void EditableMesh::renameSubmesh(const Ogre::String& before, const Ogre::String& after)
{
    SubMeshNameMap::iterator it = mSubMeshNameMap.find(before);
    if (it != mSubMeshNameMap.end()) 
    {
        Ogre::ushort submeshId = it->second;
        mSubMeshNameMap.erase(it);
        mSubMeshNameMap[after] = submeshId;
    }
}

}