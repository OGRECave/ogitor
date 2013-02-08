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

        SkeletonInfo processSkeleton(const Ogre::String& skeletonFileName) const;
        void processSkeleton(SkeletonInfo& info, Ogre::SkeletonPtr skeleton) const;

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
