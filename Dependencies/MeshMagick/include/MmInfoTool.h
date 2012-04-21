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

#ifndef __MM_INFO_TOOL_H__
#define __MM_INFO_TOOL_H__

#include "MeshMagickPrerequisites.h"

#include <vector>

#include <OgreMeshSerializer.h>
#include <OgreMesh.h>
#include <OgreSubMesh.h>

#include "MmTool.h"
#include "MmOptionsParser.h"

namespace meshmagick
{
	struct VertexInfo
	{
		size_t numVertices;
		size_t numBoneAssignments;
		size_t numBonesReferenced;
		Ogre::String layout;

		VertexInfo() : numVertices(0), numBoneAssignments(0), numBonesReferenced(0) {}
	};

	struct SubMeshInfo
	{
		Ogre::String name;
		Ogre::String materialName;
		bool usesSharedVertices;
		VertexInfo vertices;

		Ogre::String operationType;
		size_t numElements;
		Ogre::String elementType;
		size_t indexBitWidth;

		SubMeshInfo() : name(), materialName(), usesSharedVertices(false),
			vertices(), operationType(), numElements(0), elementType(), indexBitWidth(16) {}
	};

	struct SkeletonInfo
	{
		Ogre::String name;
		std::vector<Ogre::String> boneNames;
		/// first: animation name, second: animation length
		std::vector<std::pair<Ogre::String, Ogre::Real> > animations;

		SkeletonInfo() : name(), boneNames(), animations() {}
	};

	struct MeshInfo
	{
		Ogre::String name;
		Ogre::String version;
		Ogre::String endian;

		Ogre::AxisAlignedBox storedBoundingBox;
		Ogre::AxisAlignedBox actualBoundingBox;

		bool hasEdgeList;
		unsigned short numLodLevels;

		bool hasSharedVertices;
		VertexInfo sharedVertices;
		std::vector<SubMeshInfo> submeshes;

		/// first: animation name, second: animation length
		std::vector<std::pair<Ogre::String, Ogre::Real> > morphAnimations;
		std::vector<Ogre::String> poseNames;

		size_t numVertices;
		size_t numElements;
		size_t numTrianlges;
		size_t numLines;
		size_t numPoints;
		size_t maxNumBoneAssignments;
		size_t maxNumBonesReferenced;

		bool hasSkeleton;
		Ogre::String skeletonName;
		bool skeletonValid;
		SkeletonInfo skeleton;

		MeshInfo() : name(), version(), endian(),
			storedBoundingBox(Ogre::AxisAlignedBox::BOX_NULL),
			actualBoundingBox(Ogre::AxisAlignedBox::BOX_NULL),
			hasEdgeList(false), numLodLevels(0),
			hasSharedVertices(false), sharedVertices(), submeshes(),
			morphAnimations(), poseNames(),
			numVertices(0), numElements(0), numTrianlges(0), numLines(0), numPoints(0),
			hasSkeleton(false), skeletonName(""), skeletonValid(false), skeleton() {}
	};


    class _MeshMagickExport InfoTool : public Tool
    {
    public:
        InfoTool();

		Ogre::String getName() const;

		MeshInfo getInfo(Ogre::MeshPtr mesh, bool followSkeleton = true);
		SkeletonInfo getInfo(Ogre::SkeletonPtr skeleton);

    private:
        MeshInfo processMesh(const Ogre::String& meshFileName) const;
        void processMesh(MeshInfo& info, Ogre::MeshPtr mesh) const;
		void processMesh(MeshInfo& info, Ogre::Mesh* mesh) const;

        SkeletonInfo processSkeleton(const Ogre::String& skeletonFileName) const;
        void processSkeleton(SkeletonInfo& info, Ogre::SkeletonPtr skeleton) const;
		void processSkeleton(SkeletonInfo& info, Ogre::Skeleton* skeleton) const;

        void processSubMesh(SubMeshInfo&, Ogre::SubMesh* subMesh) const;
		void processBoneAssignmentData(VertexInfo&, const Ogre::VertexData* vd,
			const Ogre::Mesh::IndexMap& blendIndexToBoneIndexMap) const;
		void processVertexDeclaration(VertexInfo&, const Ogre::VertexDeclaration* vd) const;

		void printMeshInfo(const OptionList& toolOptions, const MeshInfo& info) const;
		void printSkeletonInfo(const OptionList& toolOptions, const SkeletonInfo& info) const;

		void reportMeshInfo(const MeshInfo& info) const;
		void reportSkeletonInfo(const SkeletonInfo& info) const;

		void listMeshInfo(const Ogre::StringVector& listFields, char delim,
			const MeshInfo& info) const;
		void listSkeletonInfo(const Ogre::StringVector& listFields, char delim,
			const SkeletonInfo& info) const;

		void printMeshInfoList(const Ogre::StringVector& listFields, char delim,
			const MeshInfo& info, size_t submeshIndex) const;

        Ogre::String getEndianModeAsString(Ogre::MeshSerializer::Endian) const;

        void doInvoke(const OptionList& toolOptions,
            const Ogre::StringVector& inFileNames, const Ogre::StringVector& outFileNames);
    };
}
#endif
