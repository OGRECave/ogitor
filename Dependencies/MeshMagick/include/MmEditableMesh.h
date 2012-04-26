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
#include <OgreMesh.h>
#include "MeshMagickPrerequisites.h"

namespace meshmagick 
{

    class _MeshMagickExport EditableMesh :
        public Ogre::Mesh
    {
    public:
        EditableMesh(Ogre::ResourceManager* creator, const Ogre::String& name, Ogre::ResourceHandle handle,
            const Ogre::String& group, bool isManual = false, Ogre::ManualResourceLoader* loader = 0);
        ~EditableMesh();

        Ogre::SubMesh* removeSubmesh(const Ogre::String& submeshName);
        void addSubmesh(const Ogre::String& submeshName, Ogre::SubMesh* submesh);
        void renameSubmesh(const Ogre::String& before, const Ogre::String& after);
    };

}
