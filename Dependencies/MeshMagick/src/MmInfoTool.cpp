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

#include "MmInfoTool.h"

#include "MmMeshUtils.h"
#include "MmOgreEnvironment.h"
#include "MmStatefulMeshSerializer.h"
#include "MmStatefulSkeletonSerializer.h"
#include "MmToolUtils.h"

#include <OgreAnimation.h>
#include <OgreBone.h>
#include <OgreHardwareVertexBuffer.h>
#include <OgreStringConverter.h>

#include <algorithm>
#include <functional>
#include <map>

using namespace Ogre;

namespace meshmagick
{
    struct FindSubMeshNameByIndex
        : std::binary_function<Mesh::SubMeshNameMap::value_type, unsigned short, bool>
    {
        bool operator()(const Mesh::SubMeshNameMap::value_type& entry, unsigned short index) const
        {
            return entry.second == index;
        }
    };
    //------------------------------------------------------------------------

    InfoTool::InfoTool()
    {
    }
    //------------------------------------------------------------------------

    Ogre::String InfoTool::getName() const
    {
        return "info";
    }
    //------------------------------------------------------------------------

	MeshInfo InfoTool::getInfo(MeshPtr mesh, bool followSkeleton)
	{
		mFollowSkeletonLink = followSkeleton;

		MeshInfo info;
		info.name = mesh->getName();
		info.version = "";
		info.endian = "";
		processMesh(info, mesh);
		return info;
	}
    //------------------------------------------------------------------------

	SkeletonInfo InfoTool::getInfo(SkeletonPtr skeleton)
	{
		SkeletonInfo info;
		info.name = skeleton->getName();

		processSkeleton(info, skeleton);

		return info;
	}
    //------------------------------------------------------------------------

    void InfoTool::doInvoke(const OptionList& toolOptions,
        const Ogre::StringVector& inFileNames, const Ogre::StringVector& outFileNames)
    {
        // info tool doesn't write anything. Warn, if outfiles given.
        if (!outFileNames.empty())
        {
            warn("info tool doesn't write anything. Output files are ignored.");
        }

        for (size_t i = 0, end = inFileNames.size(); i < end; ++i)
        {
            if (StringUtil::endsWith(inFileNames[i], ".mesh", true))
            {
				MeshInfo meshInfo = processMesh(inFileNames[i]);
				printMeshInfo(toolOptions, meshInfo);
            }
            else if (StringUtil::endsWith(inFileNames[i], ".skeleton", true))
            {
				SkeletonInfo skeletonInfo = processSkeleton(inFileNames[i]);
				printSkeletonInfo(toolOptions, skeletonInfo);
            }
            else
            {
                warn("unrecognised name ending for file " + inFileNames[i]);
                warn("file skipped.");
            }
        }
    }
    //------------------------------------------------------------------------

	MeshInfo InfoTool::processMesh(const Ogre::String& meshFileName) const
	{
        StatefulMeshSerializer* meshSerializer =
            OgreEnvironment::getSingleton().getMeshSerializer();

        MeshPtr mesh = meshSerializer->loadMesh(meshFileName);

		MeshInfo info;
		info.name = meshFileName;
		info.version = meshSerializer->getMeshFileVersion();
		info.endian = getEndianModeAsString(meshSerializer->getEndianMode());

		processMesh(info, mesh);
		return info;
	}
    //------------------------------------------------------------------------
	void InfoTool::processMesh(MeshInfo& info, MeshPtr mesh) const
	{
		processMesh(info, mesh.get());
	}
	//---------------------------------------------------------------------
	void InfoTool::processMesh(MeshInfo& info, Mesh* mesh) const
    {
        info.storedBoundingBox = mesh->getBounds();
		info.actualBoundingBox = MeshUtils::getMeshAabb(mesh);

        // Build metadata for bone assignments
		if (mesh->hasSkeleton())
		{
			info.hasSkeleton = true;
			info.skeletonName = mesh->getSkeletonName();

			// Cause mesh to sort out the number of bone assignments per vertex and
			// the bone map to individual submeshes
			mesh->_updateCompiledBoneAssignments();
		}

        if (mesh->sharedVertexData != NULL)
        {
			info.hasSharedVertices = true;
			info.sharedVertices.numVertices = mesh->sharedVertexData->vertexCount;
			processBoneAssignmentData(info.sharedVertices, mesh->sharedVertexData,
				mesh->sharedBlendIndexToBoneIndexMap);
            processVertexDeclaration(info.sharedVertices,
				mesh->sharedVertexData->vertexDeclaration);
			info.maxNumBoneAssignments =
				std::max(info.maxNumBoneAssignments, info.sharedVertices.numBoneAssignments);
			info.maxNumBonesReferenced =
				std::max(info.maxNumBonesReferenced, info.sharedVertices.numBonesReferenced);
			info.numVertices += info.sharedVertices.numVertices;
        }
        else
        {
			info.hasSharedVertices = false;
        }

        const Mesh::SubMeshNameMap& subMeshNames = mesh->getSubMeshNameMap();
        for (int i = 0; i < mesh->getNumSubMeshes(); ++i)
        {
			SubMeshInfo subMeshInfo;
            // Has the submesh got a name?
            Mesh::SubMeshNameMap::const_iterator it = std::find_if(subMeshNames.begin(),
                subMeshNames.end(), std::bind2nd(FindSubMeshNameByIndex(), i));

            subMeshInfo.name = it == subMeshNames.end() ? String() : it->first;
            processSubMesh(subMeshInfo, mesh->getSubMesh(i));
			info.submeshes.push_back(subMeshInfo);

			info.maxNumBoneAssignments =
				std::max(info.maxNumBoneAssignments, subMeshInfo.vertices.numBoneAssignments);
			info.maxNumBonesReferenced =
				std::max(info.maxNumBonesReferenced, subMeshInfo.vertices.numBonesReferenced);

			if (subMeshInfo.elementType == "triangles")
			{
				info.numTrianlges += subMeshInfo.numElements;
			}
			else if (subMeshInfo.elementType == "lines")
			{
				info.numLines += subMeshInfo.numElements;
			}
			else if (subMeshInfo.elementType == "points")
			{
				info.numPoints += subMeshInfo.numElements;
			}
			info.numElements += subMeshInfo.numElements;
			info.numVertices += subMeshInfo.vertices.numVertices;
        }

        // Animation detection

        // Morph animations ?
        if (mesh->getNumAnimations() > 0)
        {
            // Yes, list them
            for (unsigned short i = 0, end = mesh->getNumAnimations(); i < end; ++i)
            {
                Animation* ani = mesh->getAnimation(i);
				info.morphAnimations.push_back(std::make_pair(ani->getName(), ani->getLength()));
            }
        }

        // Poses?
        PoseList poses = mesh->getPoseList();
		for (size_t i = 0; i < poses.size(); ++i)
		{
			info.poseNames.push_back(poses[i]->getName());
		}

        // Is there a skeleton linked and are we supposed to follow it?
        if (mFollowSkeletonLink && mesh->hasSkeleton())
        {
			try
			{
				info.skeleton = processSkeleton(mesh->getSkeletonName());
				info.skeletonValid = true;
			}
			catch (std::exception&)
			{
				warn("Error processing skeleton. skipped.");
				info.skeletonValid = false;
			}
        }
		else
		{
			info.skeletonValid = false;
		}
    }
    //------------------------------------------------------------------------

    void InfoTool::processSubMesh(SubMeshInfo& info, Ogre::SubMesh* submesh) const
    {
		info.materialName = submesh->getMaterialName();
		info.usesSharedVertices = submesh->useSharedVertices;
        if (!info.usesSharedVertices)
        {
			info.vertices.numVertices = submesh->vertexData->vertexCount;
			processBoneAssignmentData(info.vertices, submesh->vertexData, submesh->blendIndexToBoneIndexMap);
            processVertexDeclaration(info.vertices, submesh->vertexData->vertexDeclaration);
        }

        // indices
        if (submesh->indexData != NULL)
        {
            HardwareIndexBufferSharedPtr indexBuffer = submesh->indexData->indexBuffer;
            if (indexBuffer->getType() == HardwareIndexBuffer::IT_16BIT)
            {
				info.indexBitWidth = 16;
            }
            else
            {
				info.indexBitWidth = 32;
            }

			size_t numIndices = indexBuffer->getNumIndexes();
			switch(submesh->operationType)
			{
			case RenderOperation::OT_LINE_LIST:
				info.operationType = "OT_LINE_LIST";
				info.numElements = numIndices / 2;
				info.elementType = "lines";
				break;
			case RenderOperation::OT_LINE_STRIP:
				info.operationType = "OT_LINE_STRIP";
				info.numElements = numIndices / 2;
				info.elementType = "lines";
				break;
			case RenderOperation::OT_POINT_LIST:
				info.operationType = "OT_POINT_LIST";
				info.numElements = numIndices;
				info.elementType = "points";
				break;
			case RenderOperation::OT_TRIANGLE_FAN:
				info.operationType = "OT_TRIANGLE_FAN";
				info.numElements = numIndices - 2;
				info.elementType = "triangles";
				break;
			case RenderOperation::OT_TRIANGLE_LIST:
				info.operationType = "OT_TRIANGLE_LIST";
				info.numElements = numIndices / 3;
				info.elementType = "triangles";
				break;
			case RenderOperation::OT_TRIANGLE_STRIP:
				info.operationType = "OT_TRIANGLE_STRIP";
				info.numElements = numIndices - 2;
				info.elementType = "triangles";
				break;
			}
        }
    }
    //------------------------------------------------------------------------

    SkeletonInfo InfoTool::processSkeleton(const String& skeletonFileName) const
	{
		SkeletonInfo info;

        StatefulSkeletonSerializer* skeletonSerializer =
            OgreEnvironment::getSingleton().getSkeletonSerializer();

        SkeletonPtr skeleton;
        try
        {
            skeleton = skeletonSerializer->loadSkeleton(skeletonFileName);
        }
        catch(std::exception& e)
        {
            warn(e.what());
            warn("Unable to open skeleton file " + skeletonFileName);
            warn("file skipped.");
			throw;
        }
		info.name = skeletonFileName;

		processSkeleton(info, skeleton);

		return info;
	}
    //------------------------------------------------------------------------
	void InfoTool::processSkeleton(SkeletonInfo& info, Ogre::SkeletonPtr skeleton) const
	{
		processSkeleton(info, skeleton.get());
	}
	//---------------------------------------------------------------------
	void InfoTool::processSkeleton(SkeletonInfo& info, Ogre::Skeleton* skeleton) const
    {
        for (unsigned short i = 0, end = skeleton->getNumBones(); i < end; ++i)
        {
            Bone* bone = skeleton->getBone(i);
			info.boneNames.push_back(bone->getName());
        }

        for (unsigned short i = 0, end = skeleton->getNumAnimations(); i < end; ++i)
        {
            Animation* ani = skeleton->getAnimation(i);
			info.animations.push_back(std::make_pair(ani->getName(), ani->getLength()));
        }
    }
    //------------------------------------------------------------------------

    String InfoTool::getEndianModeAsString(MeshSerializer::Endian endian) const
    {
        if (endian == MeshSerializer::ENDIAN_BIG)
        {
            return "Big Endian";
        }
        else if (endian == MeshSerializer::ENDIAN_LITTLE)
        {
            return "Little Endian";
        }
        else if (endian == MeshSerializer::ENDIAN_NATIVE)
        {
            return "Native Endian";
        }
        else
        {
            return "Unknown Endian";
        }
    }
    //------------------------------------------------------------------------

	void InfoTool::processBoneAssignmentData(VertexInfo& info, const Ogre::VertexData* vd,
		const Ogre::Mesh::IndexMap& blendIndexToBoneIndexMap) const
	{
		// Report number of bones per vertex
		const Ogre::VertexElement* elem =
			vd->vertexDeclaration->findElementBySemantic(VES_BLEND_WEIGHTS);
		if (elem)
		{
			info.numBoneAssignments = VertexElement::getTypeCount(elem->getType());
			info.numBonesReferenced = blendIndexToBoneIndexMap.size();
		}
	}
    //------------------------------------------------------------------------

	/// @todo externalise this function, when reorganise-tool is integrated,
    /// because both use the same format
    void InfoTool::processVertexDeclaration(VertexInfo& info, const VertexDeclaration* vd) const
    {
        // First: source-ID, second: offset
        typedef std::pair<unsigned short, size_t> ElementPosition;
        typedef std::pair<VertexElementSemantic, VertexElementType> Element;
        typedef std::map<ElementPosition, Element> ElementMap;

        // This map holds the results from iterating all elements in the declaration.
        // Having them stored in the map makes it easy to create the layout string.
        ElementMap elements;

        // Iterate over declaration elements and put them into the map.
        // We do this, because we don't know in what order the elements are stored, but
        // in order to create the layout string we need them in order of their source and offset.
        const VertexDeclaration::VertexElementList& elementList = vd->getElements();
        for (VertexDeclaration::VertexElementList::const_iterator it = elementList.begin(),
            end = elementList.end(); it != end; ++it)
        {
            elements[std::make_pair((*it).getSource(), (*it).getOffset())] =
                std::make_pair((*it).getSemantic(), (*it).getType());
        }

        // Create the layout string
        String layout;
        unsigned short source = 0;
        for (ElementMap::const_iterator it = elements.begin(), end = elements.end();
            it != end; ++it)
        {
            // If source changed, we append a hyphen to indicate a new buffer.
            if (it->first.first != source)
            {
                layout += '-';
                source = it->first.first;
            }

            // Append char indicating the element semantic
            switch (it->second.first)
            {
            case VES_POSITION:
                layout += "p";
                break;
            case VES_BLEND_WEIGHTS:
                layout += "w";
                break;
            case VES_BLEND_INDICES:
                layout += "i";
                break;
            case VES_NORMAL:
                layout += "n";
                break;
            case VES_DIFFUSE:
                layout += "d";
                break;
            case VES_SPECULAR:
                layout += "s";
                break;
            case VES_TEXTURE_COORDINATES:
                layout += "u";
                break;
            case VES_BINORMAL:
                layout += "b";
                break;
            case VES_TANGENT:
                layout += "t";
                break;
            }
            // Append substring indicating the element type
            switch (it->second.second)
            {
            case VET_FLOAT1:
                layout += "(f1)";
                break;
            case VET_FLOAT2:
                layout += "(f2)";
                break;
            case VET_FLOAT3:
                layout += "(f3)";
                break;
            case VET_FLOAT4:
                layout += "(f4)";
                break;
            case VET_SHORT1:
                layout += "(s1)";
                break;
            case VET_SHORT2:
                layout += "(s2)";
                break;
            case VET_SHORT3:
                layout += "(s3)";
                break;
            case VET_SHORT4:
                layout += "(s4)";
                break;
            case VET_UBYTE4:
                layout += "(u4)";
                break;
            case VET_COLOUR_ARGB:
                layout += "(dx)";
                break;
            case VET_COLOUR_ABGR:
                layout += "(gl)";
                break;
			case VET_COLOUR:
				// Doesn't appear at runtime, so don't handle
				break;
            }
        }

		info.layout = layout;
    }
    //------------------------------------------------------------------------

	void InfoTool::printMeshInfo(const OptionList& toolOptions, const MeshInfo& info) const
	{
		const String list = OptionsUtil::getStringOption(toolOptions, "list");
		if (list == StringUtil::BLANK)
		{
			reportMeshInfo(info);
		}
		else
		{
			const String delimOpt = OptionsUtil::getStringOption(toolOptions, "delim");
			char delim = delimOpt.empty() ? '\t' : delimOpt[0];
			StringVector listItems = StringUtil::split(list, "/");
			listMeshInfo(listItems, delim, info);
		}
	}
    //------------------------------------------------------------------------

	void InfoTool::printSkeletonInfo(const OptionList& toolOptions, const SkeletonInfo& info) const
	{
		const String list = OptionsUtil::getStringOption(toolOptions, "list");
		if (list == StringUtil::BLANK)
		{
			reportSkeletonInfo(info);
		}
		else
		{
			const String delimOpt = OptionsUtil::getStringOption(toolOptions, "delim");
			char delim = delimOpt.empty() ? '\t' : delimOpt[0];
			StringVector listItems = StringUtil::split(list, "/");
			listSkeletonInfo(listItems, delim, info);
		}
	}
    //------------------------------------------------------------------------

	void InfoTool::reportMeshInfo(const MeshInfo& meshInfo) const
	{
		// total amount counters
		size_t numVertices = 0;
		size_t numTriangles = 0;
		size_t numLines = 0;
		size_t numPoints = 0;

		// formatting helpers
		const String& indent = "    ";

		// header info
		print("Mesh file name: " + meshInfo.name);
		print("Mesh file version: " + meshInfo.version);
		print("Endian mode: " + meshInfo.endian);
		print("");

		// bounding box(es)
		if (meshInfo.actualBoundingBox == meshInfo.storedBoundingBox)
		{
			print("Bounding box: "
				+ ToolUtils::getPrettyAabbString(meshInfo.actualBoundingBox));
		}
		else
		{
			print("Stored bounding box: "
				+ ToolUtils::getPrettyAabbString(meshInfo.storedBoundingBox));
			print("Actual bounding box: "
				+ ToolUtils::getPrettyAabbString(meshInfo.actualBoundingBox));
		}
		print("");

		// shared vertices
		if (meshInfo.hasSharedVertices)
		{
			print("Shared vertices:");
			print(indent + StringConverter::toString(meshInfo.sharedVertices.numVertices)
				+ " vertices");
			print(indent
				+ StringConverter::toString(meshInfo.sharedVertices.numBonesReferenced)
				+ " bones referenced.");
			print(indent
				+ StringConverter::toString(meshInfo.sharedVertices.numBoneAssignments)
				+ " bone assignments per vertex.");
			print(indent + "Buffer layout: " + meshInfo.sharedVertices.layout);

			numVertices += meshInfo.sharedVertices.numVertices;
		}
		else
		{
			print("No shared vertices.");
		}
		print("");

		// submesh info
		const size_t numSubmeshes = meshInfo.submeshes.size();
		print(StringConverter::toString(numSubmeshes)
			+ (numSubmeshes > 1 ? " submeshes." : " submesh."));
		for (size_t i = 0; i < numSubmeshes; ++i)
		{
			const SubMeshInfo& info = meshInfo.submeshes[i];

			print("submesh " + StringConverter::toString(i) + "(" + info.name + ")");
			print(indent + "material " + info.materialName);
			if (info.usesSharedVertices)
			{
				print(indent + "submesh uses shared vertices.");
			}
			else
			{
				print(indent + StringConverter::toString(info.vertices.numVertices)
					+ " vertices");
				print(indent + StringConverter::toString(info.vertices.numBonesReferenced)
					+ " bones referenced.");
				print(indent + StringConverter::toString(info.vertices.numBoneAssignments)
					+ " bone assignments per vertex.");
				print(indent + "Buffer layout: " + info.vertices.layout);

				numVertices += info.vertices.numVertices;
			}

			print(indent + "OperationType: " + info.operationType);
			print(indent + StringConverter::toString(info.numElements)
				+ " " + info.elementType);
			print(indent + StringConverter::toString(info.indexBitWidth) + " bit index width");

			// Discriminate element type for total element counts
			if (info.elementType == "triangles")
			{
				numTriangles += info.numElements;
			}
			else if (info.elementType == "lines")
			{
				numLines += info.numElements;
			}
			else if (info.elementType == "points")
			{
				numPoints += info.numElements;
			}
			print("");
		}

		// Print total counts
		print(StringConverter::toString(numVertices) + " vertices in total.");
		if (numTriangles > 0)
		{
			print(StringConverter::toString(numTriangles) + " triangles in total.");
		}
		if (numLines > 0)
		{
			print(StringConverter::toString(numLines) + " lines in total.");
		}
		if (numPoints > 0)
		{
			print(StringConverter::toString(numPoints) + " points in total.");
		}
		print("");

		// Other mesh properties

		if (meshInfo.hasEdgeList)
		{
			print("Edge list stored in file.");
		}
		else
		{
			print("No edge list stored in file.");
		}

		if (meshInfo.numLodLevels > 0)
		{
			print(StringConverter::toString(meshInfo.numLodLevels)
				+ " LOD levels stored in file.");
		}
		else
		{
			print("No LOD info stored in file.");
		}
		print("");

		if (meshInfo.morphAnimations.empty())
		{
			print("No morph animations");
		}
		else
		{
			print(StringConverter::toString(meshInfo.morphAnimations.size())
				+ " morph animations");
			for (size_t i = 0; i < meshInfo.morphAnimations.size(); ++i)
			{
				std::pair<Ogre::String, Ogre::Real> ani = meshInfo.morphAnimations[i];
				print(indent + "name: " + ani.first
					+ " / length: " + StringConverter::toString(ani.second));
			}
			print("");
		}

		if (meshInfo.poseNames.empty())
		{
			print("No poses.");
		}
		else
		{
			print(StringConverter::toString(meshInfo.poseNames.size())
				+ " poses.");
			for (size_t i = 0; i < meshInfo.poseNames.size(); ++i)
			{
				print(indent + meshInfo.poseNames[i]);
			}
		}

		if (meshInfo.hasSkeleton)
		{
			print("Skeleton: " + meshInfo.skeletonName);
		}
		else
		{
			print("No skeleton.");
		}
		print("");
		print("");

		if (meshInfo.skeletonValid)
		{
			reportSkeletonInfo(meshInfo.skeleton);
		}
	}
    //------------------------------------------------------------------------

	void InfoTool::reportSkeletonInfo(const SkeletonInfo& info) const
	{
		const String& indent = "    ";
		print("Skeleton file name: " + info.name);
		print("");

		print(StringConverter::toString(info.boneNames.size()) + " bones");
		for (size_t i = 0; i < info.animations.size(); ++i)
		{
			print(indent + info.boneNames[i]);
		}
		print("");

		print(StringConverter::toString(info.animations.size()) + " animations");
		for (size_t i = 0; i < info.animations.size(); ++i)
		{
			print(indent + "name: " + info.animations[i].first + " / length: "
				+ StringConverter::toString(info.animations[i].second));
		}
	}
    //------------------------------------------------------------------------

	void InfoTool::listMeshInfo(const Ogre::StringVector& listFields, char delim,
		const MeshInfo& info) const
	{
		// First determine whether there are submesh level infos asked about.
		StringVector submeshLevelFields;
		submeshLevelFields.push_back("submesh_index");
		submeshLevelFields.push_back("submesh_name");
		submeshLevelFields.push_back("submesh_material");
		submeshLevelFields.push_back("submesh_use_shared_vertices");
		submeshLevelFields.push_back("submesh_vertex_count");
		submeshLevelFields.push_back("submesh_bone_assignment_count");
		submeshLevelFields.push_back("submesh_bone_references_count");
		submeshLevelFields.push_back("submesh_vertex_layout");
		submeshLevelFields.push_back("submesh_operation_type");
		submeshLevelFields.push_back("submesh_element_count");
		submeshLevelFields.push_back("submesh_index_width");
		bool submeshLevel = std::find_first_of(listFields.begin(), listFields.end(),
			submeshLevelFields.begin(), submeshLevelFields.end()) != listFields.end();
		if (submeshLevel)
		{
			for (size_t i = 0; i < info.submeshes.size(); ++i)
			{
				printMeshInfoList(listFields, delim, info, i);
			}
		}
		else
		{
			printMeshInfoList(listFields, delim, info, -1);
		}
	}
    //------------------------------------------------------------------------

	void InfoTool::printMeshInfoList(const Ogre::StringVector& listFields, char delim,
		const MeshInfo& info, size_t submeshIndex) const
	{
		String out;

		for (size_t i = 0; i < listFields.size(); ++i)
		{
			const String field = listFields[i];
			if (field == "name")
			{
				out += info.name;
			}
			else if (field == "version")
			{
				out += info.version;
			}
			else if (field == "endian")
			{
				out += info.endian;
			}
			else if (field == "stored_bounding_box")
			{
				out += ToolUtils::getPrettyAabbString(info.storedBoundingBox);
			}
			else if (field == "actual_bounding_box")
			{
				out += ToolUtils::getPrettyAabbString(info.actualBoundingBox);
			}
			else if (field == "stored_mesh_extent")
			{
				out += ToolUtils::getPrettyVectorString(info.storedBoundingBox.getSize());
			}
			else if (field == "actual_mesh_extent")
			{
				out += ToolUtils::getPrettyVectorString(info.actualBoundingBox.getSize());
			}
			else if (field == "edge_list")
			{
				out += info.hasEdgeList ? "yes" : "no";
			}
			else if (field == "lod_level_count")
			{
				out += StringConverter::toString(info.numLodLevels);
			}
			else if (field == "shared_vertices")
			{
				out += info.hasSharedVertices ? "yes" : "no";
			}
			else if (field == "shared_vertex_count" && info.hasSharedVertices)
			{
				out += StringConverter::toString(info.sharedVertices.numVertices);
			}
			else if (field == "shared_bone_assignment_count" && info.hasSharedVertices)
			{
				out += StringConverter::toString(info.sharedVertices.numBoneAssignments);
			}
			else if (field == "shared_bone_references_count" && info.hasSharedVertices)
			{
				out += StringConverter::toString(info.sharedVertices.numBonesReferenced);
			}
			else if (field == "shared_vertex_layout" && info.hasSharedVertices)
			{
				out += info.sharedVertices.layout;
			}
			else if (field == "submesh_count")
			{
				out += StringConverter::toString(info.submeshes.size());
			}
			else if (field == "submesh_index")
			{
				out += StringConverter::toString(submeshIndex);
			}
			else if (field == "submesh_name")
			{
				out += info.submeshes[submeshIndex].name;
			}
			else if (field == "submesh_material")
			{
				out += info.submeshes[submeshIndex].materialName;
			}
			else if (field == "submesh_use_shared_vertices")
			{
				out += info.submeshes[submeshIndex].usesSharedVertices ? "yes" : "no";
			}
			else if (field == "submesh_vertex_count")
			{
				out += StringConverter::toString(
					info.submeshes[submeshIndex].vertices.numVertices);
			}
			else if (field == "submesh_bone_assignment_count")
			{
				out += StringConverter::toString(
					info.submeshes[submeshIndex].vertices.numBoneAssignments);
			}
			else if (field == "submesh_bone_references_count")
			{
				out += StringConverter::toString(
					info.submeshes[submeshIndex].vertices.numBonesReferenced);
			}
			else if (field == "submesh_vertex_layout")
			{
				out += info.submeshes[submeshIndex].vertices.layout;
			}
			else if (field == "submesh_operation_type")
			{
				out += info.submeshes[submeshIndex].operationType;
			}
			else if (field == "submesh_element_count")
			{
				out += StringConverter::toString(info.submeshes[submeshIndex].numElements);
			}
			else if (field == "submesh_triangle_count")
			{
				if (info.submeshes[submeshIndex].elementType == "triangles")
				{
					out += StringConverter::toString(info.submeshes[submeshIndex].numElements);
				}
				else
				{
					out += "0";
				}
			}
			else if (field == "submesh_line_count")
			{
				if (info.submeshes[submeshIndex].elementType == "lines")
				{
					out += StringConverter::toString(info.submeshes[submeshIndex].numElements);
				}
				else
				{
					out += "0";
				}
			}
			else if (field == "submesh_point_count")
			{
				if (info.submeshes[submeshIndex].elementType == "points")
				{
					out += StringConverter::toString(info.submeshes[submeshIndex].numElements);
				}
				else
				{
					out += "0";
				}
			}
			else if (field == "submesh_index_width")
			{
				out += StringConverter::toString(info.submeshes[submeshIndex].indexBitWidth);
			}
			else if (field == "morph_animation_count")
			{
				out += StringConverter::toString(info.morphAnimations.size());
			}
			else if (field == "pose_count")
			{
				out += StringConverter::toString(info.poseNames.size());
			}
			else if (field == "max_bone_assignments")
			{
				out += StringConverter::toString(info.maxNumBoneAssignments);
			}
			else if (field == "max_bone_references")
			{
				out += StringConverter::toString(info.maxNumBonesReferenced);
			}
			else if (field == "total_vertex_count")
			{
				out += StringConverter::toString(info.numVertices);
			}
			else if (field == "total_element_count")
			{
				out += StringConverter::toString(info.numElements);
			}
			else if (field == "total_triangle_count")
			{
				out += StringConverter::toString(info.numTrianlges);
			}
			else if (field == "total_line_count")
			{
				out += StringConverter::toString(info.numLines);
			}
			else if (field == "total_point_count")
			{
				out += StringConverter::toString(info.numPoints);
			}
			else if (field == "skeleton")
			{
				out += info.hasSkeleton ? "yes" : "no";
			}
			else if (field == "skeleton_name" && info.hasSkeleton)
			{
				out += info.skeletonName;
			}
			else if (field == "skeleton_bone_count" && info.skeletonValid)
			{
				out += StringConverter::toString(info.skeleton.boneNames.size());
			}
			else if (field == "skeleton_animation_count" && info.skeletonValid)
			{
				out += StringConverter::toString(info.skeleton.animations.size());
			}
			else
			{
				continue;
			}

			if (i < listFields.size() - 1) out += delim;
		}

		print(out);
	}
    //------------------------------------------------------------------------

	void InfoTool::listSkeletonInfo(const Ogre::StringVector& listFields, char delim,
		const SkeletonInfo& info) const
	{
		String out;

		for (size_t i = 0; i < listFields.size(); ++i)
		{
			const String field = listFields[i];
			if (field == "skeleton_name")
			{
				out += info.name;
			}
			else if (field == "skeleton_bone_count")
			{
				out += StringConverter::toString(info.boneNames.size());
			}
			else if (field == "skeleton_animation_count")
			{
				out += StringConverter::toString(info.animations.size());
			}
			else
			{
				continue;
			}

			if (i < listFields.size() - 1) out += delim;
		}

		print(out);
	}
    //------------------------------------------------------------------------
}
