/*
This file is part of MeshMagick - An Ogre mesh file manipulation tool.
Copyright (C) 2010 Steve Streeting

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

#include "MmTootleTool.h"

#include <tootlelib.h>

#include <OgreMesh.h>
#include <OgreSubMesh.h>

#include "MmOgreEnvironment.h"
#include "MmStatefulMeshSerializer.h"

using namespace Ogre;

namespace meshmagick
{

	// Code adapted from OgreTootle, by Ahmed Ismaiel Zakaria
	struct TootleSettings
	{
		const char *pMeshName ;
		const char *pViewpointName ;
		unsigned int nClustering ;
		unsigned int nCacheSize;
		TootleFaceWinding eWinding;
		const char* mOutFile;
	};
	struct TootleStats
	{
		unsigned int nClusters;
		float        fVCacheIn;
		float        fVCacheOut;
		float        fOverdrawIn;
		float        fOverdrawOut;
		float        fMaxOverdrawIn;
		float        fMaxOverdrawOut;
	};

	String getTootleError(TootleResult tr, const String& info)
	{
		switch(tr)
		{
		case TOOTLE_INVALID_ARGS:
			return info + ": Illegal arguments were passed.";
		case TOOTLE_OUT_OF_MEMORY:
			return info + ": Tootle ran out of memory while trying to complete the call.";
		case TOOTLE_3D_API_ERROR:
			return info + ": Errors occurred while setting up the 3D API.";
		case TOOTLE_INTERNAL_ERROR:
			return info + ": Internal error!";
		case TOOTLE_NOT_INITIALIZED:
			return info + ": Tootle was not initialized before a function call.";
		default:
			return info + ": Unknown error.";

		}
	}

	void FillMeshData(HardwareIndexBufferSharedPtr indexBuffer,
		VertexDeclaration *vertexDeclaration,
		VertexBufferBinding* vertexBufferBinding,
		std::vector<Vector3> & vertices,
		std::vector<unsigned int> &indices)
	{
		// Get the target element
		const VertexElement* srcElem = vertexDeclaration->findElementBySemantic(
			VES_POSITION);

		if (!srcElem || srcElem->getType() != VET_FLOAT3)
		{
			OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS,
				"SubMesh has no position semantic!! ",
				"FillMeshData");
		}
		HardwareVertexBufferSharedPtr srcBuf;
		srcBuf = vertexBufferBinding->getBuffer(srcElem->getSource());
		unsigned char *pSrcBase;
		// lock source for read only
		pSrcBase = static_cast<unsigned char*>(
			srcBuf->lock(HardwareBuffer::HBL_READ_ONLY));
		size_t numvertices=srcBuf->getNumVertices();
		size_t vertexsize=srcBuf->getVertexSize();
		float	*pVPos;	                // vertex position buffer, read only

		for(size_t i=0;i<numvertices;i++)
		{
			srcElem->baseVertexPointerToElement(pSrcBase, &pVPos);
			vertices.push_back(Vector3(pVPos[0],pVPos[1],pVPos[2]));
			pSrcBase+=vertexsize;
		}
		srcBuf->unlock();

		//fill the index buffer
		//tootle only work with 32Bit buffers 
		//,so we'll had to recompress them later before saving

		// retrieve buffer pointers
		uint16	*pVIndices16 = NULL;    // the face indices buffer, read only
		uint32	*pVIndices32 = NULL;    // the face indices buffer, read only
		bool use32bit = false;

		if (indexBuffer->getType() == HardwareIndexBuffer::IT_32BIT)
		{
			pVIndices32 = static_cast<uint32*>(
				indexBuffer->lock(HardwareBuffer::HBL_READ_ONLY));
			use32bit = true;
			for(size_t i=0;i<indexBuffer->getNumIndexes();i++)
			{
				indices.push_back((unsigned int)(*pVIndices32));
				pVIndices32++;
			}

		}
		else
		{
			pVIndices16 = static_cast<uint16*>(
				indexBuffer->lock(HardwareBuffer::HBL_READ_ONLY));
			size_t nm_indices=indexBuffer->getNumIndexes();
			for(size_t i=0;i<nm_indices;i++)
			{
				indices.push_back((unsigned int)(*pVIndices16));
				pVIndices16++;
			}
		}
		indexBuffer->unlock();
		int a=0;

	}


	TootleTool::TootleTool()
		: Tool()
		, mVCacheSize(0)
		, mClockwise(false)
		, mClusters(0)
	{
	}

	TootleTool::~TootleTool()
	{
	}

	Ogre::String TootleTool::getName() const
	{
		return "tootle";
	}

	void TootleTool::doInvoke(
		const OptionList &toolOptions, 
		const Ogre::StringVector &inFileNames, 
		const Ogre::StringVector &outFileNamesArg)
	{
		// Name count has to match, else we have no way to figure out how to apply output
		// names to input files.
		if (!(outFileNamesArg.empty() || inFileNames.size() == outFileNamesArg.size()))
		{
			fail("number of output files must match number of input files.");
		}

		setOptions(toolOptions);

		StringVector outFileNames = outFileNamesArg.empty() ? inFileNames : outFileNamesArg;

		// Process the meshes
		for (size_t i = 0, end = inFileNames.size(); i < end; ++i)
		{
			if (StringUtil::endsWith(inFileNames[i], ".mesh", true))
			{
				processMeshFile(inFileNames[i], outFileNames[i]);
			}
			else
			{
				warn("unrecognised name ending for file " + inFileNames[i]);
				warn("file skipped.");
			}
		}
	}

	void TootleTool::setOptions(const OptionList& options)
	{
		// default
		mVCacheSize = 0;
		mClockwise = false;
		mClusters = 0;
		mViewpointList.clear();

		for (OptionList::const_iterator i = options.begin(); i != options.end(); ++i)
		{
			if (i->first == "vcachesize")
				mVCacheSize = static_cast<unsigned int>(any_cast<int>(i->second));
			else if (i->first == "clockwise")
				mClockwise = true;
			else if (i->first == "clusters")
				mClusters = static_cast<unsigned int>(any_cast<int>(i->second));
			else if (i->first == "viewpoint")
				mViewpointList.push_back(any_cast<Vector3>(i->second));

		}
	}


	void TootleTool::processMeshFile(Ogre::String inFile, Ogre::String outFile)
	{
		StatefulMeshSerializer* meshSerializer =
			OgreEnvironment::getSingleton().getMeshSerializer();

		print("Loading mesh " + inFile + "...");
		MeshPtr mesh;
		try
		{
			mesh = meshSerializer->loadMesh(inFile);
		}
		catch(std::exception& e)
		{
			warn(e.what());
			warn("Unable to open mesh file " + inFile);
			warn("file skipped.");
			return;
		}

		processMesh(mesh);

		meshSerializer->saveMesh(outFile, true);
		print("Mesh saved as " + outFile + ".");

	}

	void TootleTool::processMesh(Ogre::MeshPtr mesh)
	{
		processMesh(mesh.get());
	}

	void TootleTool::processMesh(Ogre::Mesh* mesh)
	{
		print("Processing mesh...");

		std::vector<Vector3> vertices;
		std::vector<unsigned int> indices;

		// Init options
		bool gatherStats = OgreEnvironment::getSingleton().isStandalone() && mVerbosity >= V_HIGH;
		unsigned int cacheSize = mVCacheSize ? mVCacheSize : TOOTLE_DEFAULT_VCACHE_SIZE;
		TootleFaceWinding winding = mClockwise ? TOOTLE_CW : TOOTLE_CCW;
		float* pViewpoints = mViewpointList.empty() ? 0 : mViewpointList.begin()->ptr();
		unsigned int numViewpoints = static_cast<unsigned int>(mViewpointList.size());


		int num_lods = mesh->getNumLodLevels();
		int num_submeshes = mesh->getNumSubMeshes();
		for(int i = 0; i < num_submeshes; i++)
		{
			vertices.clear();
			indices.clear();

			// build buffers containing only the vertex positions and indices, since this is what Tootle requires
			SubMesh * smesh=mesh->getSubMesh(i);

			// Skip empty submeshes
			if (!smesh->indexData->indexCount)
				continue;

			if(smesh->operationType!=RenderOperation::OT_TRIANGLE_LIST)
			{
				continue;
			}
			if(smesh->useSharedVertices)
			{
				FillMeshData(smesh->indexData->indexBuffer,
					mesh->sharedVertexData->vertexDeclaration,
					mesh->sharedVertexData->vertexBufferBinding,
					vertices,indices);
			}
			else
			{
				FillMeshData(smesh->indexData->indexBuffer,
					smesh->vertexData->vertexDeclaration,
					smesh->vertexData->vertexBufferBinding,
					vertices,indices);

			}
			if(indices.size()>0)
			{
				//start tootle work


#if 0
				// Dump to .obj for diagnostics for when Tootle asserts
				if (mVerbosity >= V_HIGH)
				{
					std::ofstream ofstream;
					String filename = String("TootleInput_") + mesh->getName() + "_" + StringConverter::toString(i) + ".obj";
					ofstream.open(filename.c_str());

					for (std::vector<Vector3>::iterator v = vertices.begin(); v != vertices.end(); ++v)
					{
						ofstream << "v " << v->x << " " << v->y << " " << v->z << std::endl;
					}
					for (std::vector<unsigned int>::iterator f = indices.begin(); f != indices.end();)
					{
						ofstream << "f " << (*f++ + 1) << " " << (*f++ + 1) << " " << (*f++ + 1) << std::endl;
					}
					ofstream.close();

				}
#endif


				// *****************************************************************
				//   Optimize the mesh
				// *****************************************************************

				unsigned int nTriangles = (unsigned int) indices.size() / 3;
				unsigned int nVertices = (unsigned int) vertices.size();
				const float* pVB = (float*) &vertices[0];
				unsigned int* pIB = (unsigned int*) &indices[0];
				unsigned int nStride = 3*sizeof(float);   

				TootleStats stats;
				TootleResult result;

				// initialize Tootle
				result = TootleInit();
				if( result != TOOTLE_OK )
					fail(getTootleError(result, "TootleInit"));


				if (gatherStats)
				{
					// measure input VCache efficiency
					result = TootleMeasureCacheEfficiency( pIB, nTriangles, cacheSize, &stats.fVCacheIn );
					if( result != TOOTLE_OK )
						fail(getTootleError(result, "TootleMeasureCacheEfficiency"));


					// measure input overdraw.  Note that we assume counter-clockwise vertex winding. 
					result = TootleMeasureOverdraw( pVB, pIB, nVertices, nTriangles, nStride, 
						pViewpoints, numViewpoints, winding, 
						&stats.fOverdrawIn, &stats.fMaxOverdrawIn );
					if( result != TOOTLE_OK )
						fail(getTootleError(result, "TootleMeasureOverdraw"));
				}

				// allocate an array to hold the cluster ID for each face
				std::vector<unsigned int> faceClusters;
				faceClusters.resize( nTriangles + 1 );

				// cluster the mesh, and sort faces by cluster
				result = TootleClusterMesh( pVB, pIB, nVertices, nTriangles, nStride, mClusters, pIB, &faceClusters[0], NULL );
				if( result != TOOTLE_OK )
					fail(getTootleError(result, "TootleClusterMesh"));

				stats.nClusters = (unsigned int)(faceClusters[ nTriangles ]);

				// perform vertex cache optimization on the clustered mesh
				result = TootleVCacheClusters( pIB, nTriangles, nVertices, cacheSize, &faceClusters[0], pIB, NULL );
				if( result != TOOTLE_OK )
					fail(getTootleError(result, "TootleVCacheClusters"));

				// optimize the draw order

				// Problem: TootleOptimizeOverdraw takes a CONST pointer to the face
				// clusters, yet consistently overwrites the data in it. So copy it.
				unsigned int* pDummyFaceClusters = new unsigned int[nTriangles + 1];
				memcpy(pDummyFaceClusters, &faceClusters[0], sizeof(unsigned int) * (nTriangles + 1));
				result = TootleOptimizeOverdraw( pVB, pIB, nVertices, nTriangles, nStride, 
					pViewpoints, numViewpoints, winding, 
					pDummyFaceClusters, pIB, 0 );
				if( result != TOOTLE_OK )
					fail(getTootleError(result, "TootleOptimizeOverdraw"));
				delete [] pDummyFaceClusters;


				if (gatherStats)
				{
					// measure output VCache efficiency
					result = TootleMeasureCacheEfficiency( pIB, nTriangles, cacheSize, &stats.fVCacheOut );
					if( result != TOOTLE_OK )
						fail(getTootleError(result, "TootleMeasureCacheEfficiency"));

					// measure output overdraw
					result = TootleMeasureOverdraw( pVB, pIB, nVertices, nTriangles, nStride,  
						pViewpoints, numViewpoints, winding, 
						&stats.fOverdrawOut, &stats.fMaxOverdrawOut );
					if( result != TOOTLE_OK )
						fail(getTootleError(result, "TootleMeasureOverdraw"));

					// print tootle statistics (only in verbose mode)
					StringUtil::StrStreamType statsStr;
					statsStr 
						<< "Tootle Stats for submesh " << i << ": " << std::endl
						<< "  Clusters: " << stats.nClusters << std::endl
						<< "  Cache In/Out: " << stats.fVCacheIn << " / " << stats.fVCacheOut << " = " << (stats.fVCacheIn/stats.fVCacheOut) << std::endl
						<< "  Overdraw In/Out: " << stats.fOverdrawIn << " / " << stats.fOverdrawOut << " = " << (stats.fOverdrawIn/stats.fOverdrawOut) << std::endl
						<< "  Max Overdraw In/Out: " << stats.fMaxOverdrawIn << " / " << stats.fMaxOverdrawOut << " = " << (stats.fMaxOverdrawIn/stats.fMaxOverdrawOut);
					print(statsStr.str(), V_HIGH);
				}

				// clean up tootle
				TootleCleanup();



				//copy the index buffer back to where it came from
				if (smesh->indexData->indexBuffer->getType() == HardwareIndexBuffer::IT_32BIT)
				{
					uint32	*pVIndices32 = NULL;    // the face indices buffer
					std::vector<unsigned int>::iterator srci = indices.begin();

					pVIndices32 = static_cast<uint32*>(
						smesh->indexData->indexBuffer->lock(HardwareBuffer::HBL_NORMAL));
					for(size_t i=0;i<smesh->indexData->indexBuffer->getNumIndexes();i++)
					{
						*pVIndices32++ = static_cast<uint32>(*srci++);
					}

				}
				else
				{
					uint16	*pVIndices16 = NULL;    // the face indices buffer
					std::vector<unsigned int>::iterator srci = indices.begin();

					pVIndices16 = static_cast<uint16*>(
						smesh->indexData->indexBuffer->lock(HardwareBuffer::HBL_NORMAL));
					size_t nm_indices=smesh->indexData->indexBuffer->getNumIndexes();
					for(size_t i=0;i<nm_indices;i++)
					{
						*pVIndices16++ = static_cast<uint16>(*srci++);
					}
				}
				smesh->indexData->indexBuffer->unlock();
			}
		}


	}

}
