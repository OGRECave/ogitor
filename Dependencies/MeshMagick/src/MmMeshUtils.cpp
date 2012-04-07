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

#include "MmMeshUtils.h"

#include <OgreSubMesh.h>

using namespace Ogre;

namespace meshmagick
{
    AxisAlignedBox MeshUtils::getMeshAabb(MeshPtr mesh, const Matrix4& transform)
    {
        AxisAlignedBox aabb;
        if (mesh->sharedVertexData != 0)
        {
            aabb.merge(getVertexDataAabb(mesh->sharedVertexData, transform));
        }
        for (unsigned int i = 0; i < mesh->getNumSubMeshes(); ++i)
        {
            SubMesh* sm = mesh->getSubMesh(i);
            if (sm->vertexData != 0)
            {
                aabb.merge(getVertexDataAabb(sm->vertexData, transform));
            }
        }

        return aabb;
    }

    AxisAlignedBox MeshUtils::getVertexDataAabb(VertexData* vd, const Matrix4& transform)
    {
        AxisAlignedBox aabb;

        const VertexElement* ve = vd->vertexDeclaration->findElementBySemantic(VES_POSITION);
        HardwareVertexBufferSharedPtr vb = vd->vertexBufferBinding->getBuffer(ve->getSource());

        unsigned char* data = static_cast<unsigned char*>(
            vb->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

        for (size_t i = 0; i < vd->vertexCount; ++i)
        {
            float* v;
            ve->baseVertexPointerToElement(data, &v);
            aabb.merge(transform * Vector3(v[0], v[1], v[2]));

            data += vb->getVertexSize();
        }
        vb->unlock();

        return aabb;
    }
}
