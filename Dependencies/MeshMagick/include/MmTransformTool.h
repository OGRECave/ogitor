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
		void processSkeleton(Ogre::Skeleton* skeleton);
		void processMesh(Ogre::Mesh* mesh);

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
