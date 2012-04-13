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

#ifndef __MM_TRANSFORM_TOOL_H__
#define __MM_TRANSFORM_TOOL_H__

#include "MeshMagickPrerequisites.h"

#ifdef __APPLE__
#	include <Ogre/OgreAnimation.h>
#	include <Ogre/OgreMesh.h>
#	include <Ogre/OgreSkeleton.h>
#	include <Ogre/OgreBone.h>
#else
#	include <OgreAnimation.h>
#	include <OgreMesh.h>
#	include <OgreSkeleton.h>
#	include <OgreBone.h>
#endif

#include "MmOptionsParser.h"
#include "MmTool.h"

namespace meshmagick
{
    class _MeshMagickExport TransformTool : public Tool
    {
    public:
        TransformTool();

		Ogre::String getName() const;

		void transform(Ogre::MeshPtr mesh, Ogre::Matrix4 transformation, bool followSkeleton = true);
		void transform(Ogre::SkeletonPtr skeleton, Ogre::Matrix4 transformation);

    private:
        Ogre::Matrix4 mTransform;
        Ogre::AxisAlignedBox mBoundingBox;
        bool mNormaliseNormals;
        bool mUpdateBoundingBox;
        bool mFlipVertexWinding;
        OptionList mOptions;

        void processSkeletonFile(Ogre::String file, Ogre::String outFile,
            bool calcTransform);
        void processMeshFile(Ogre::String file, Ogre::String outFile);

        void processSkeleton(Ogre::SkeletonPtr skeleton);
        void processMesh(Ogre::MeshPtr mesh);

        void setOptions(const OptionList& options);

        void processVertexData(Ogre::VertexData* vertexData);
        void processPositionElement(Ogre::VertexData* vertexData,
            const Ogre::VertexElement* vertexElem);
        void processDirectionElement(Ogre::VertexData* vertexData,
            const Ogre::VertexElement* vertexElem);

        void processAnimation(Ogre::Animation* ani);
        void processBone(Ogre::Bone* bone);
        void processPose(Ogre::Pose* pose);
        void processVertexMorphKeyFrame(Ogre::VertexMorphKeyFrame* keyframe, size_t vertexCount);

        void processIndexData(Ogre::IndexData* indexData);

        /// Calculate transformation matrix from input arguments and, if given, a mesh.
        /// The mesh is used to retrieve the AABB, which is needed for alignment operation
        /// and for resize.
        /// Alignment and resize operations are ignored, if no mesh is given.
        /// This doesn't matter for alignment operations on skeletons,
        /// since translations don't apply there. But resizing a skeleton seperately from the mesh
        /// is not possible and the result will look weird.
        void calculateTransform(Ogre::MeshPtr mesh = Ogre::MeshPtr());

		void doInvoke(const OptionList& toolOptions,
            const Ogre::StringVector& inFileNames,
            const Ogre::StringVector& outFileNames);
    };
}
#endif
