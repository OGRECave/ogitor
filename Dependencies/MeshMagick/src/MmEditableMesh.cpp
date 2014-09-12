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