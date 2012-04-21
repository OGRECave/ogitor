/*
This file is part of MeshMagick - An Ogre mesh file manipulation tool.
Copyright (C) 2007-2010 Steve Streeting

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

#include "MmOptimiseTool.h"

#include "MmOgreEnvironment.h"
#include "MmStatefulMeshSerializer.h"
#include "MmStatefulSkeletonSerializer.h"

#ifdef __APPLE__
#	include <Ogre/OgreStringConverter.h>
#else
#	include <OgreStringConverter.h>
#endif

#include <algorithm>
#include <functional>

using namespace Ogre;

namespace meshmagick
{
	//------------------------------------------------------------------------
	OptimiseTool::OptimiseTool()
	{
	}
	//------------------------------------------------------------------------
    Ogre::String OptimiseTool::getName() const
    {
        return "optimise";
    }
	//------------------------------------------------------------------------
	void OptimiseTool::doInvoke(const OptionList& toolOptions,
		const Ogre::StringVector& inFileNames, const Ogre::StringVector& outFileNamesArg)
	{
		// Name count has to match, else we have no way to figure out how to apply output
		// names to input files.
		if (!(outFileNamesArg.empty() || inFileNames.size() == outFileNamesArg.size()))
		{
			fail("number of output files must match number of input files.");
		}

		mPosTolerance = mNormTolerance = mUVTolerance = 1e-06f;
		mKeepIdentityTracks = OptionsUtil::isOptionSet(toolOptions, "keep-identity-tracks");
		for (OptionList::const_iterator it = toolOptions.begin(); it != toolOptions.end(); ++it)
		{
			if (it->first == "tolerance")
			{
				mPosTolerance = mNormTolerance = mUVTolerance = static_cast<float>(any_cast<Real>(it->second));
			}
			else if (it->first == "pos_tolerance")
			{
				mPosTolerance = static_cast<float>(any_cast<Real>(it->second));
			}
			else if (it->first == "norm_tolerance")
			{
				mNormTolerance = static_cast<float>(any_cast<Real>(it->second));
			}
			else if (it->first == "uv_tolerance")
			{
				mUVTolerance = static_cast<float>(any_cast<Real>(it->second));
			}
		}


		StringVector outFileNames = outFileNamesArg.empty() ? inFileNames : outFileNamesArg;

		// Process the meshes
		for (size_t i = 0, end = inFileNames.size(); i < end; ++i)
		{
			if (StringUtil::endsWith(inFileNames[i], ".mesh", true))
			{
				processMeshFile(inFileNames[i], outFileNames[i]);
			}
			else if (StringUtil::endsWith(inFileNames[i], ".skeleton", true))
			{
				processSkeletonFile(inFileNames[i], outFileNames[i]);
			}
			else
			{
				warn("unrecognised name ending for file " + inFileNames[i]);
				warn("file skipped.");
			}
		}
	}
	//---------------------------------------------------------------------
	void OptimiseTool::processMeshFile(Ogre::String file, Ogre::String outFile)
	{
		StatefulMeshSerializer* meshSerializer =
			OgreEnvironment::getSingleton().getMeshSerializer();

		print("Loading mesh " + file + "...");
		MeshPtr mesh;
		try
		{
			mesh = meshSerializer->loadMesh(file);
		}
		catch(std::exception& e)
		{
			warn(e.what());
			warn("Unable to open mesh file " + file);
			warn("file skipped.");
			return;
		}
		print("Optimising mesh...");
		processMesh(mesh);
		meshSerializer->saveMesh(outFile, true);
		print("Mesh saved as " + outFile + ".");

		if (mFollowSkeletonLink && mesh->hasSkeleton())
		{
			// In this case keep file name.
			processSkeletonFile(mesh->getSkeletonName(), mesh->getSkeletonName());
		}

	}
	//---------------------------------------------------------------------
	void OptimiseTool::processSkeletonFile(Ogre::String file, Ogre::String outFile)
	{
		StatefulSkeletonSerializer* skeletonSerializer =
			OgreEnvironment::getSingleton().getSkeletonSerializer();

		print("Loading skeleton " + file + "...");
		SkeletonPtr skeleton;
		try
		{
			skeleton = skeletonSerializer->loadSkeleton(file);
		}
		catch(std::exception& e)
		{
			warn(e.what());
			warn("Unable to open skeleton file " + file);
			warn("file skipped.");
			return;
		}
		print("Optimising skeleton...");
		processSkeleton(skeleton);
		skeletonSerializer->saveSkeleton(outFile, true);
		print("Skeleton saved as " + outFile + ".");

	}
	//---------------------------------------------------------------------
	void OptimiseTool::processMesh(Ogre::MeshPtr mesh)
	{
		processMesh(mesh.get());
	}
	//---------------------------------------------------------------------
	void OptimiseTool::processMesh(Ogre::Mesh* mesh)
	{
		bool rebuildEdgeList = false;
		// Shared geometry
		if (mesh->sharedVertexData)
		{
			print("Optimising mesh shared vertex data...");
			setTargetVertexData(mesh->sharedVertexData);

			for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
			{
				SubMesh* sm = mesh->getSubMesh(i);
				if (sm->useSharedVertices)
				{
					addIndexData(sm->indexData, sm->operationType);
				}
			}

			if (optimiseGeometry())
			{
				if (mesh->getSkeletonName() != StringUtil::BLANK)
				{
					print("    fixing bone assignments...");
					Mesh::BoneAssignmentIterator currentIt = mesh->getBoneAssignmentIterator();
					Mesh::VertexBoneAssignmentList newList =
						getAdjustedBoneAssignments(currentIt);
					mesh->clearBoneAssignments();
					for (Mesh::VertexBoneAssignmentList::iterator bi = newList.begin();
						bi != newList.end(); ++bi)
					{
						mesh->addBoneAssignment(bi->second);
					}

				}

				for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
				{
					SubMesh* sm = mesh->getSubMesh(i);
					if (mesh->getSkeletonName() != StringUtil::BLANK)
					{
						print("    fixing bone assignments...");
						Mesh::BoneAssignmentIterator currentIt = sm->getBoneAssignmentIterator();
						Mesh::VertexBoneAssignmentList newList =
							getAdjustedBoneAssignments(currentIt);
						sm->clearBoneAssignments();
						for (Mesh::VertexBoneAssignmentList::iterator bi = newList.begin();
							bi != newList.end(); ++bi)
						{
							sm->addBoneAssignment(bi->second);
						}

					}
					if (sm->useSharedVertices)
					{
						fixLOD(sm->mLodFaceList);
					}
				}
				rebuildEdgeList = true;

			}
		}

		// Dedicated geometry
		for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
		{
			SubMesh* sm = mesh->getSubMesh(i);
			if (!sm->useSharedVertices)
			{
				print("Optimising submesh " +
					StringConverter::toString(i) + " dedicated vertex data ");
				setTargetVertexData(sm->vertexData);
				addIndexData(sm->indexData, sm->operationType);
				if (optimiseGeometry())
				{
					if (mesh->getSkeletonName() != StringUtil::BLANK)
					{
						print("    fixing bone assignments...");
						Mesh::BoneAssignmentIterator currentIt = sm->getBoneAssignmentIterator();
						Mesh::VertexBoneAssignmentList newList =
							getAdjustedBoneAssignments(currentIt);
						sm->clearBoneAssignments();
						for (Mesh::VertexBoneAssignmentList::iterator bi = newList.begin();
							bi != newList.end(); ++bi)
						{
							sm->addBoneAssignment(bi->second);
						}

					}

					fixLOD(sm->mLodFaceList);
					rebuildEdgeList = true;
				}
			}
		}

		if (rebuildEdgeList && mesh->isEdgeListBuilt())
		{
			// force rebuild of edge list
			mesh->freeEdgeList();
			mesh->buildEdgeList();
		}


	}
	//---------------------------------------------------------------------
	void OptimiseTool::fixLOD(Ogre::vector<Ogre::IndexData*>::type lodFaces)
	{
        for (Ogre::vector<Ogre::IndexData*>::type::iterator l = lodFaces.begin();
            l != lodFaces.end(); ++l)
        {
            IndexData* idata = *l;
            print("    fixing LOD...");
            remapIndexes(idata);
        }

	}
	//---------------------------------------------------------------------
	Mesh::VertexBoneAssignmentList OptimiseTool::getAdjustedBoneAssignments(
		Mesh::BoneAssignmentIterator& it)
	{
		Mesh::VertexBoneAssignmentList newList;
		while (it.hasMoreElements())
		{
			VertexBoneAssignment ass = it.getNext();
			IndexInfo& ii = mIndexRemap[ass.vertexIndex];

			// If this is the originating vertex index  we want to add the (adjusted)
			// bone assignments. If it's another vertex that was collapsed onto another
			// then we want to skip the bone assignment since it will just be a duplication.
			if (ii.isOriginal)
			{
				ass.vertexIndex = static_cast<unsigned int>(ii.targetIndex);
				assert (ass.vertexIndex < mUniqueVertexMap.size());
				newList.insert(Mesh::VertexBoneAssignmentList::value_type(
					ass.vertexIndex, ass));

			}

		}

		return newList;

	}
	//---------------------------------------------------------------------
	void OptimiseTool::processSkeleton(Ogre::SkeletonPtr skeleton)
	{
		processSkeleton(skeleton.get());
	}
	//---------------------------------------------------------------------
	void OptimiseTool::processSkeleton(Ogre::Skeleton* skeleton)
	{
		skeleton->optimiseAllAnimations(mKeepIdentityTracks);
	}
	//---------------------------------------------------------------------
	void OptimiseTool::setTargetVertexData(Ogre::VertexData* vd)
	{
		mTargetVertexData = vd;
		mUniqueVertexMap.clear();
		mUniqueVertexList.clear();
		mIndexDataList.clear();
		mIndexRemap.clear();
	}
	//---------------------------------------------------------------------
	void OptimiseTool::addIndexData(Ogre::IndexData* id, RenderOperation::OperationType ot)
	{
		mIndexDataList.push_back(IndexDataWithOpType(id, ot));
	}
	//---------------------------------------------------------------------
	bool OptimiseTool::optimiseGeometry()
	{
		bool verticesChanged = false;
		if (calculateDuplicateVertices())
		{
			size_t numDupes = mTargetVertexData->vertexCount -
				mUniqueVertexMap.size();
			print("    " + StringConverter::toString(mTargetVertexData->vertexCount) +
				" source vertices.");
			print("    " + StringConverter::toString(numDupes) +
				" duplicate vertices to be removed.");
			print("    " + StringConverter::toString(mUniqueVertexMap.size()) +
				" vertices will remain.");
			print("    rebuilding vertex buffers...");
			rebuildVertexBuffers();
			print("    re-indexing faces...");
			remapIndexDataList();
			print("    done.");
			verticesChanged = true;
		}

		removeDegenerateFaces();

		return verticesChanged;
	}
	//---------------------------------------------------------------------
	bool OptimiseTool::calculateDuplicateVertices()
	{
		bool duplicates = false;

		// Can't remove duplicates on unindexed geometry, needs to use duplicates
		if (mIndexDataList.empty())
			return false;

		// Lock all the buffers first
		typedef std::vector<char*> BufferLocks;
		BufferLocks bufferLocks;
		const VertexBufferBinding::VertexBufferBindingMap& bindings =
			mTargetVertexData->vertexBufferBinding->getBindings();
		VertexBufferBinding::VertexBufferBindingMap::const_iterator bindi;
		bufferLocks.resize(mTargetVertexData->vertexBufferBinding->getLastBoundIndex()+1);
		for (bindi = bindings.begin(); bindi != bindings.end(); ++bindi)
		{
			char* lock = static_cast<char*>(bindi->second->lock(HardwareBuffer::HBL_READ_ONLY));
			bufferLocks[bindi->first] = lock;
		}

		for (uint32 v = 0; v < mTargetVertexData->vertexCount; ++v)
		{
			UniqueVertex uniqueVertex;
			const VertexDeclaration::VertexElementList& elemList =
				mTargetVertexData->vertexDeclaration->getElements();
			VertexDeclaration::VertexElementList::const_iterator elemi;
			unsigned short uvSets = 0;
			for (elemi = elemList.begin(); elemi != elemList.end(); ++elemi)
			{
				// all float pointers for the moment
				float *pFloat;
				elemi->baseVertexPointerToElement(
					bufferLocks[elemi->getSource()], &pFloat);

				switch(elemi->getSemantic())
				{
				case VES_POSITION:
					uniqueVertex.position.x = *pFloat++;
					uniqueVertex.position.y = *pFloat++;
					uniqueVertex.position.z = *pFloat++;
					break;
				case VES_NORMAL:
					uniqueVertex.normal.x = *pFloat++;
					uniqueVertex.normal.y = *pFloat++;
					uniqueVertex.normal.z = *pFloat++;
					break;
				case VES_TANGENT:
					uniqueVertex.tangent.x = *pFloat++;
					uniqueVertex.tangent.y = *pFloat++;
					uniqueVertex.tangent.z = *pFloat++;
					// support w-component on tangent if present
					if (VertexElement::getTypeCount(elemi->getType()) == 4)
					{
						uniqueVertex.tangent.w = *pFloat++;
					}
					break;
				case VES_BINORMAL:
					uniqueVertex.binormal.x = *pFloat++;
					uniqueVertex.binormal.y = *pFloat++;
					uniqueVertex.binormal.z = *pFloat++;
					break;
				case VES_TEXTURE_COORDINATES:
					// supports up to 4 dimensions
					for (unsigned short dim = 0;
						dim < VertexElement::getTypeCount(elemi->getType()); ++dim)
					{
						uniqueVertex.uv[elemi->getIndex()][dim] = *pFloat++;
					}
					++uvSets;
					break;
				case VES_BLEND_INDICES:
				case VES_BLEND_WEIGHTS:
				case VES_DIFFUSE:
				case VES_SPECULAR:
					// No action needed for these semantics.
					break;
				};
			}

			if (v == 0)
			{
				// set up comparator
				UniqueVertexLess lessObj;
				lessObj.pos_tolerance = mPosTolerance;
				lessObj.norm_tolerance = mNormTolerance;
				lessObj.uv_tolerance = mUVTolerance;
				lessObj.uvSets = uvSets;
				mUniqueVertexMap = UniqueVertexMap(lessObj);
			}

			// try to locate equivalent vertex in the list already
			uint32 indexUsed;
			UniqueVertexMap::iterator ui = mUniqueVertexMap.find(uniqueVertex);
			bool isOrig = false;
			if (ui != mUniqueVertexMap.end())
			{
				// re-use vertex, remap
				indexUsed = ui->second.newIndex;
				duplicates = true;
			}
			else
			{
				// new vertex
				isOrig = true;
				indexUsed = static_cast<uint32>(mUniqueVertexMap.size());
				// store the originating and new vertex index in the unique map
				VertexInfo newInfo(v, indexUsed);
				// lookup
				mUniqueVertexMap[uniqueVertex] = newInfo;
				// ordered
				mUniqueVertexList.push_back(newInfo);

			}
			// Insert remap entry (may map to itself)
			mIndexRemap.push_back(IndexInfo(indexUsed, isOrig));


			// increment buffer lock pointers
			for (bindi = bindings.begin(); bindi != bindings.end(); ++bindi)
			{
				bufferLocks[bindi->first] += bindi->second->getVertexSize();
			}

		}


		// unlock the buffers now
		for (bindi = bindings.begin(); bindi != bindings.end(); ++bindi)
		{
			bindi->second->unlock();
		}

		// Were there duplicates?
		return duplicates;

	}
	//---------------------------------------------------------------------
	void OptimiseTool::rebuildVertexBuffers()
	{
		// We need to build new vertex buffers of the new, reduced size
		VertexBufferBinding* newBind =
			HardwareBufferManager::getSingleton().createVertexBufferBinding();

		// Lock source buffers
		typedef std::vector<char*> BufferLocks;
		BufferLocks srcbufferLocks;
		BufferLocks destbufferLocks;
		const VertexBufferBinding::VertexBufferBindingMap& srcBindings =
			mTargetVertexData->vertexBufferBinding->getBindings();
		VertexBufferBinding::VertexBufferBindingMap::const_iterator bindi;
		srcbufferLocks.resize(mTargetVertexData->vertexBufferBinding->getLastBoundIndex()+1);
		destbufferLocks.resize(mTargetVertexData->vertexBufferBinding->getLastBoundIndex()+1);
		for (bindi = srcBindings.begin(); bindi != srcBindings.end(); ++bindi)
		{
			char* lock = static_cast<char*>(bindi->second->lock(HardwareBuffer::HBL_READ_ONLY));
			srcbufferLocks[bindi->first] = lock;

			// Add a new vertex buffer and binding
			HardwareVertexBufferSharedPtr newBuf =
				HardwareBufferManager::getSingleton().createVertexBuffer(
					bindi->second->getVertexSize(),
					mUniqueVertexList.size(),
					bindi->second->getUsage(),
					bindi->second->hasShadowBuffer());
			newBind->setBinding(bindi->first, newBuf);
			lock = static_cast<char*>(newBuf->lock(HardwareBuffer::HBL_DISCARD));
			destbufferLocks[bindi->first] = lock;
		}
		const VertexBufferBinding::VertexBufferBindingMap& destBindings =
			newBind->getBindings();


		// Iterate over the new vertices
		for (UniqueVertexList::iterator ui = mUniqueVertexList.begin();
			ui != mUniqueVertexList.end(); ++ui)
		{
			uint32 origVertexIndex = ui->oldIndex;
			// copy vertex from each buffer in turn
			VertexBufferBinding::VertexBufferBindingMap::const_iterator srci =
				srcBindings.begin();
			VertexBufferBinding::VertexBufferBindingMap::const_iterator desti =
				destBindings.begin();
			for (; srci != srcBindings.end(); ++srci, ++desti)
			{
				// determine source pointer
				char* pSrc = srcbufferLocks[srci->first] +
					(srci->second->getVertexSize() * origVertexIndex);
				char* pDest = destbufferLocks[desti->first];

				// Copy vertex from source index
				memcpy(pDest, pSrc, desti->second->getVertexSize());

				// increment destination lock pointer
				destbufferLocks[desti->first] += desti->second->getVertexSize();
			}
		}

		// unlock the buffers now
		for (bindi = srcBindings.begin(); bindi != srcBindings.end(); ++bindi)
		{
			bindi->second->unlock();
		}
		for (bindi = destBindings.begin(); bindi != destBindings.end(); ++bindi)
		{
			bindi->second->unlock();
		}

		// now switch over the bindings, and thus the buffers
		VertexBufferBinding* oldBind = mTargetVertexData->vertexBufferBinding;
		mTargetVertexData->vertexBufferBinding = newBind;
		HardwareBufferManager::getSingleton().destroyVertexBufferBinding(oldBind);

		// Update vertex count in data
		mTargetVertexData->vertexCount = mUniqueVertexList.size();


	}
	//---------------------------------------------------------------------
	void OptimiseTool::remapIndexDataList()
	{
		for (IndexDataList::iterator i = mIndexDataList.begin(); i != mIndexDataList.end(); ++i)
		{
			IndexData* idata = i->indexData;
			remapIndexes(idata);

		}

	}
	//---------------------------------------------------------------------
	void OptimiseTool::remapIndexes(IndexData* idata)
	{
		// Time to repoint indexes at the new shared vertices
		uint16* p16 = 0;
		uint32* p32 = 0;

		// Lock for read & write
		if (idata->indexBuffer->getType() == HardwareIndexBuffer::IT_32BIT)
		{
			p32 = static_cast<uint32*>(idata->indexBuffer->lock(HardwareBuffer::HBL_NORMAL));
		}
		else
		{
			p16 = static_cast<uint16*>(idata->indexBuffer->lock(HardwareBuffer::HBL_NORMAL));
		}

		for (size_t j = 0; j < idata->indexCount; ++j)
		{
			uint32 oldIndex = p32? *p32 : *p16;
			uint32 newIndex = static_cast<uint32>(mIndexRemap[oldIndex].targetIndex);
			assert(newIndex < mUniqueVertexMap.size());
			if (newIndex != oldIndex)
			{
				if (p32)
					*p32 = newIndex;
				else
					*p16 = static_cast<uint16>(newIndex);
			}
			if (p32)
				++p32;
			else
				++p16;
		}

		idata->indexBuffer->unlock();

	}
	//---------------------------------------------------------------------
	void OptimiseTool::removeDegenerateFaces()
	{
		for (IndexDataList::iterator i = mIndexDataList.begin(); i != mIndexDataList.end(); ++i)
		{
			// Only remove degenerate faces from triangle lists, strips & fans need them
			if (i->operationType == RenderOperation::OT_TRIANGLE_LIST)
			{
				IndexData* idata = i->indexData;
				removeDegenerateFaces(idata);
			}

		}

	}
	//---------------------------------------------------------------------
	void OptimiseTool::removeDegenerateFaces(Ogre::IndexData* idata)
	{
		// Remove any faces that do not include 3 unique positions

		// Only for triangle lists
		uint16* p16 = 0;
		uint32* p32 = 0;
		uint16* pnewbuf16 = 0;
		uint32* pnewbuf32 = 0;
		uint16* pdest16 = 0;
		uint32* pdest32 = 0;

		// Lock for read only, we'll build another list
		if (idata->indexBuffer->getType() == HardwareIndexBuffer::IT_32BIT)
		{
			p32 = static_cast<uint32*>(idata->indexBuffer->lock(HardwareBuffer::HBL_READ_ONLY));
			pnewbuf32 = pdest32 = OGRE_ALLOC_T(uint32, idata->indexCount, MEMCATEGORY_GENERAL);
		}
		else
		{
			p16 = static_cast<uint16*>(idata->indexBuffer->lock(HardwareBuffer::HBL_READ_ONLY));
			pnewbuf16 = pdest16 = OGRE_ALLOC_T(uint16, idata->indexCount, MEMCATEGORY_GENERAL);
		}


		const VertexElement* posElem = 
			mTargetVertexData->vertexDeclaration->findElementBySemantic(VES_POSITION);
		HardwareVertexBufferSharedPtr posBuf = mTargetVertexData->vertexBufferBinding->getBuffer(posElem->getSource());
		unsigned char *pVertBase = static_cast<unsigned char*>(posBuf->lock(HardwareBuffer::HBL_READ_ONLY));
		size_t vsize = posBuf->getVertexSize();

		size_t newIndexCount = 0;
		for (size_t j = 0; j < idata->indexCount; j += 3)
		{
			uint32 i0 = p32? *p32++ : *p16++;
			uint32 i1 = p32? *p32++ : *p16++;
			uint32 i2 = p32? *p32++ : *p16++;

			unsigned char *pVert;
			float* pPosVert;
			Vector3 v0, v1, v2;
			pVert = pVertBase + (i0 * vsize);
			posElem->baseVertexPointerToElement(pVert, &pPosVert);
			v0 = Vector3(pPosVert[0], pPosVert[1], pPosVert[2]);
			pVert = pVertBase + (i1 * vsize);
			posElem->baseVertexPointerToElement(pVert, &pPosVert);
			v1 = Vector3(pPosVert[0], pPosVert[1], pPosVert[2]);
			pVert = pVertBase + (i2 * vsize);
			posElem->baseVertexPointerToElement(pVert, &pPosVert);
			v2 = Vector3(pPosVert[0], pPosVert[1], pPosVert[2]);

			// No double-indexing
			bool validTri = i0 != i1 && i1 != i2 && i0 != i2;
			// no equal positions
			validTri = validTri &&
				!v0.positionEquals(v1, mPosTolerance) && 
				!v1.positionEquals(v2, mPosTolerance) && 
				!v0.positionEquals(v2, mPosTolerance);
			if (validTri)
			{
				// Make sure triangle has some area
				Vector3 vec1 = v1 - v0;
				Vector3 vec2 = v2 - v0;
				// triangle area is 1/2 magnitude of the cross-product of 2 sides
				// if zero, not a valid triangle
				validTri = !Math::RealEqual((Real)0.0f, (Real)(0.5f * vec1.crossProduct(vec2).length()), 1e-04f);
			}

			if (validTri)
			{
				if (pdest32)
				{
					*pdest32++ = i0;
					*pdest32++ = i1;
					*pdest32++ = i2;
				}
				else
				{
					*pdest16++ = static_cast<uint16>(i0);
					*pdest16++ = static_cast<uint16>(i1);
					*pdest16++ = static_cast<uint16>(i2);
				}
				newIndexCount += 3;
			}
		}

		idata->indexBuffer->unlock();
		posBuf->unlock();

		if (newIndexCount != idata->indexCount)
		{
			print("    " + StringConverter::toString(idata->indexCount - newIndexCount) +
				" degenerate faces removed.");

			// Did we remove all the faces? (really bad data only, but I've seen it happen)
			if (newIndexCount > 0)
			{
				// we eliminated one or more faces
				HardwareIndexBufferSharedPtr newIBuf = HardwareBufferManager::getSingleton().createIndexBuffer(
					idata->indexBuffer->getType(), newIndexCount, 
					idata->indexBuffer->getUsage());
				if (pdest32)
				{
					newIBuf->writeData(0, sizeof(uint32) * newIndexCount, pnewbuf32, true);
				}
				else
				{
					newIBuf->writeData(0, sizeof(uint16) * newIndexCount, pnewbuf16, true);
				}
				idata->indexBuffer = newIBuf;
			}
			else
			{
				idata->indexBuffer.setNull();
			}
			idata->indexCount = newIndexCount;

		}

		OGRE_FREE(pnewbuf16, MEMCATEGORY_GENERAL);
		OGRE_FREE(pnewbuf32, MEMCATEGORY_GENERAL);

	}
	//---------------------------------------------------------------------
	bool OptimiseTool::UniqueVertexLess::equals(
		const Vector3& a, const Vector3& b, Real tolerance) const
	{
		// note during this comparison we treat directions as positions
		// becuase we're interested in numerical equality, not semantics
		// and some of these might be null and thus not be a valid direction
		return a.positionEquals(b, tolerance);
	}
	//---------------------------------------------------------------------
	bool OptimiseTool::UniqueVertexLess::equals(
		const Vector4& a, const Vector4& b, Real tolerance) const
	{
		// no built-in position equals
		for (int i = 0; i < 4; ++i)
		{
			if (Math::RealEqual(a[i], b[i], tolerance))
				return true;
		}
		return false;
	}
	//---------------------------------------------------------------------
	bool OptimiseTool::UniqueVertexLess::less(
		const Vector3& a, const Vector3& b, Real tolerance) const
	{
		// don't use built-in operator, we need sorting
		for (int i = 0; i < 3; ++i)
		{
			if (!Math::RealEqual(a[i], b[i], tolerance))
				return a[i] < b[i];
		}
		// should never get here if equals() has been checked first
		return a.x < b.x;
	}
	//---------------------------------------------------------------------
	bool OptimiseTool::UniqueVertexLess::less(
		const Vector4& a, const Vector4& b, Real tolerance) const
	{
		// don't use built-in operator, we need sorting
		for (int i = 0; i < 4; ++i)
		{
			if (!Math::RealEqual(a[i], b[i], tolerance))
				return a[i] < b[i];
		}
		// should never get here if equals() has been checked first
		return a.x < b.x;
	}
	//---------------------------------------------------------------------
	bool OptimiseTool::UniqueVertexLess::operator ()(
		const OptimiseTool::UniqueVertex &a,
		const OptimiseTool::UniqueVertex &b) const
	{
		if (!equals(a.position, b.position, pos_tolerance))
		{
			return less(a.position, b.position, pos_tolerance);
		}
		else if (!equals(a.normal, b.normal, norm_tolerance))
		{
			return less(a.normal, b.normal, norm_tolerance);
		}
		else if (!equals(a.tangent, b.tangent, norm_tolerance))
		{
			return less(a.tangent, b.tangent, norm_tolerance);
		}
		else if (!equals(a.binormal, b.binormal, norm_tolerance))
		{
			return less(a.binormal, b.binormal, norm_tolerance);
		}
		else
		{
			// position, normal, tangent and binormal are all the same, try UVs
			for (unsigned short i = 0; i < uvSets; ++i)
			{
				if (!equals(a.uv[i], b.uv[i], uv_tolerance))
				{
					return less(a.uv[i], b.uv[i], uv_tolerance);
				}
			}

			// if we get here, must be equal (with tolerance)
			return false;
		}

	}
}
