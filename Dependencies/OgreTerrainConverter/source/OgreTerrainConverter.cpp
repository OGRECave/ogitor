/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2013 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
////
/// The MIT License
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE. 
///////////////////////////////////////////////////////////////////////////////////*/

#include "OgreTerrainConverter.h"
#include "OgreTerrainQuadTreeNode.h"
#include "OgreHardwarePixelBuffer.h"
#include "OgreRoot.h"

using namespace Ogre;

const uint32 OLDTERRAIN_CHUNK_ID = StreamSerialiser::makeIdentifier("TERR");
const uint16 OLDTERRAIN_CHUNK_VERSION = 1;
const uint32 OLDTERRAINLAYERDECLARATION_CHUNK_ID = StreamSerialiser::makeIdentifier("TDCL");
const uint16 OLDTERRAINLAYERDECLARATION_CHUNK_VERSION = 1;
const uint32 OLDTERRAINLAYERSAMPLER_CHUNK_ID = StreamSerialiser::makeIdentifier("TSAM");
const uint16 OLDTERRAINLAYERSAMPLER_CHUNK_VERSION = 1;
const uint32 OLDTERRAINLAYERSAMPLERELEMENT_CHUNK_ID = StreamSerialiser::makeIdentifier("TSEL");
const uint16 OLDTERRAINLAYERSAMPLERELEMENT_CHUNK_VERSION = 1;
const uint32 OLDTERRAINLAYERINSTANCE_CHUNK_ID = StreamSerialiser::makeIdentifier("TLIN");
const uint16 OLDTERRAINLAYERINSTANCE_CHUNK_VERSION = 1;
const uint32 OLDTERRAINDERIVEDDATA_CHUNK_ID = StreamSerialiser::makeIdentifier("TDDA");
const uint16 OLDTERRAINDERIVEDDATA_CHUNK_VERSION = 1;
// since 129^2 is the greatest power we can address in 16-bit index
const uint16 OLDTERRAIN_MAX_BATCH_SIZE = 129; 
const uint16 OLDWORKQUEUE_DERIVED_DATA_REQUEST = 1;
const size_t OLDLOD_MORPH_CUSTOM_PARAM = 1001;
const uint8 OLDDERIVED_DATA_DELTAS = 1;
const uint8 OLDDERIVED_DATA_NORMALS = 2;
const uint8 OLDDERIVED_DATA_LIGHTMAP = 4;
// This MUST match the bitwise OR of all the types above with no extra bits!
const uint8 OLDDERIVED_DATA_ALL = 7;


OgreTerrainConverter::OgreTerrainConverter()
{
}

OgreTerrainConverter::~OgreTerrainConverter()
{
}

uint8 OgreTerrainConverter::getBlendTextureCount(uint8 numLayers)
{
	return ((numLayers - 1) / 4) + 1;
}

bool OgreTerrainConverter::readLayerDeclaration(StreamSerialiser& stream, TerrainLayerDeclaration& targetdecl)
{
	if (!stream.readChunkBegin(OLDTERRAINLAYERDECLARATION_CHUNK_ID, OLDTERRAINLAYERDECLARATION_CHUNK_VERSION))
		return false;
	//  samplers
	uint8 numSamplers;
	stream.read(&numSamplers);
	targetdecl.samplers.resize(numSamplers);
	for (uint8 s = 0; s < numSamplers; ++s)
	{
		if (!stream.readChunkBegin(OLDTERRAINLAYERSAMPLER_CHUNK_ID, OLDTERRAINLAYERSAMPLER_CHUNK_VERSION))
			return false;

		stream.read(&(targetdecl.samplers[s].alias));
		uint8 pixFmt;
		stream.read(&pixFmt);
		targetdecl.samplers[s].format = (PixelFormat)pixFmt;
		stream.readChunkEnd(OLDTERRAINLAYERSAMPLER_CHUNK_ID);
	}
	//  elements
	uint8 numElems;
	stream.read(&numElems);
	targetdecl.elements.resize(numElems);
	for (uint8 e = 0; e < numElems; ++e)
	{
		if (!stream.readChunkBegin(OLDTERRAINLAYERSAMPLERELEMENT_CHUNK_ID, OLDTERRAINLAYERSAMPLERELEMENT_CHUNK_VERSION))
			return false;

		stream.read(&(targetdecl.elements[e].source));
		uint8 sem;
		stream.read(&sem);
		targetdecl.elements[e].semantic = (TerrainLayerSamplerSemantic)sem;
		stream.read(&(targetdecl.elements[e].elementStart));
		stream.read(&(targetdecl.elements[e].elementCount));
		stream.readChunkEnd(OLDTERRAINLAYERSAMPLERELEMENT_CHUNK_ID);
	}
	stream.readChunkEnd(OLDTERRAINLAYERDECLARATION_CHUNK_ID);

	return true;
}


bool OgreTerrainConverter::readLayerInstanceList(StreamSerialiser& stream, size_t numSamplers, Terrain::LayerInstanceList& targetlayers)
{
	uint8 numLayers;
	stream.read(&numLayers);
	targetlayers.resize(numLayers);
	for (uint8 l = 0; l < numLayers; ++l)
	{
		if (!stream.readChunkBegin(OLDTERRAINLAYERINSTANCE_CHUNK_ID, OLDTERRAINLAYERINSTANCE_CHUNK_VERSION))
			return false;
		stream.read(&targetlayers[l].worldSize);
		targetlayers[l].textureNames.resize(numSamplers);
		for (size_t t = 0; t < numSamplers; ++t)
		{
			stream.read(&(targetlayers[l].textureNames[t]));
		}
		stream.readChunkEnd(OLDTERRAINLAYERINSTANCE_CHUNK_ID);
	}

	return true;
}


bool OgreTerrainConverter::Upgrade(StreamSerialiser& stream_in, StreamSerialiser& stream_out)
{
    mGlobalColourMapEnabled = false;
    mCpuColourMapStorage = 0;
    mHeightData = 0;
    mLightMapSize = 1024;
    mCompositeMapSize = 1024;

    if (!stream_in.readChunkBegin(OLDTERRAIN_CHUNK_ID, OLDTERRAIN_CHUNK_VERSION))
		return false;

	
	stream_in.read(&mAlign);
	stream_in.read(&mSize);
	stream_in.read(&mWorldSize);

	stream_in.read(&mMaxBatchSize);
	stream_in.read(&mMinBatchSize);
	stream_in.read(&mPos);

	size_t numVertices = mSize * mSize;
	mHeightData = OGRE_ALLOC_T(float, numVertices, MEMCATEGORY_GEOMETRY);
	stream_in.read(mHeightData, numVertices);

	// Layer declaration
	if (!readLayerDeclaration(stream_in, mLayerDecl))
		return false;

	// Layers
	if (!readLayerInstanceList(stream_in, mLayerDecl.samplers.size(), mLayers))
		return false;

	// Packed layer blend data
	uint8 numLayers = (uint8)mLayers.size();
	stream_in.read(&mLayerBlendMapSize);
	// load packed CPU data
	int numBlendTex = getBlendTextureCount(numLayers);
	for (int i = 0; i < numBlendTex; ++i)
	{
		PixelFormat fmt = PF_BYTE_RGBA;
		size_t channels = PixelUtil::getNumElemBytes(fmt);
		size_t dataSz = channels * mLayerBlendMapSize * mLayerBlendMapSize;
		uint8* pData = (uint8*)OGRE_MALLOC(dataSz, MEMCATEGORY_RESOURCE);
		stream_in.read(pData, dataSz);
		mCpuBlendMapStorage.push_back(pData);
	}

	// derived data
	while (!stream_in.isEndOfChunk(OLDTERRAIN_CHUNK_ID) && 
		stream_in.peekNextChunkID() == OLDTERRAINDERIVEDDATA_CHUNK_ID)
	{
		stream_in.readChunkBegin(OLDTERRAINDERIVEDDATA_CHUNK_ID, OLDTERRAINDERIVEDDATA_CHUNK_VERSION);
		// name
		String name;
		stream_in.read(&name);
		uint16 sz;
		stream_in.read(&sz);

    	uint8* pData = static_cast<uint8*>(OGRE_MALLOC(sz * sz * 4, MEMCATEGORY_GENERAL));
        
        if (name == "normalmap")
		{
			stream_in.read(pData, sz * sz * 3);
		}
		else if (name == "colourmap")
		{
			mGlobalColourMapEnabled = true;
			mGlobalColourMapSize = sz;
			mCpuColourMapStorage = static_cast<uint8*>(OGRE_MALLOC(sz * sz * 3, MEMCATEGORY_GENERAL));
			stream_in.read(mCpuColourMapStorage, sz * sz * 3);
		}
		else if (name == "lightmap")
		{
			mLightMapSize = sz;
			stream_in.read(pData, sz * sz);
		}
		else if (name == "compositemap")
		{
   			mCompositeMapSize = sz;
			stream_in.read(pData, sz * sz * 4);
		}

        OGRE_FREE(pData, MEMCATEGORY_GENERAL);

        stream_in.readChunkEnd(OLDTERRAINDERIVEDDATA_CHUNK_ID);
	}

	return Export(stream_out);
}


void OgreTerrainConverter::freeResources()
{
	OGRE_FREE(mHeightData, MEMCATEGORY_GEOMETRY);
	mHeightData = 0;

	OGRE_FREE(mCpuColourMapStorage, MEMCATEGORY_GENERAL);
	mCpuColourMapStorage = 0;

    for( unsigned int i = 0; i < mCpuBlendMapStorage.size(); i++)
	{
		OGRE_FREE(mCpuBlendMapStorage[i], MEMCATEGORY_RESOURCE);
	}

    mCpuBlendMapStorage.clear();
}

bool OgreTerrainConverter::Export(Ogre::StreamSerialiser& stream)
{
    Ogre::TerrainGlobalOptions *mTerrainGlobalOptions = Ogre::TerrainGlobalOptions::getSingletonPtr();
    bool mFreeGlobalOptions = false;

    if( mTerrainGlobalOptions == NULL )
    {
        mTerrainGlobalOptions = OGRE_NEW Ogre::TerrainGlobalOptions();
        mFreeGlobalOptions = true;
    }

    mTerrainGlobalOptions->setLayerBlendMapSize(mLayerBlendMapSize);
    mTerrainGlobalOptions->setUseVertexCompressionWhenAvailable(false);
    mTerrainGlobalOptions->setLightMapSize(mLightMapSize);
    mTerrainGlobalOptions->setCompositeMapSize(mCompositeMapSize);

    SceneManager *OSM = Root::getSingletonPtr()->getSceneManagerIterator().begin()->second;
    
    Terrain *terrain = OGRE_NEW Terrain(OSM);

    Terrain::ImportData data;

    data.inputFloat = mHeightData;
    data.layerDeclaration = mLayerDecl;
    data.layerList = mLayers;
    data.maxBatchSize = mMaxBatchSize;
    data.minBatchSize = mMinBatchSize;
    data.pos = mPos;
    data.terrainAlign = (Terrain::Alignment)mAlign;
    data.terrainSize = mSize;
    data.worldSize = mWorldSize;
    
    terrain->prepare(data);
    terrain->load(0);

    for( unsigned int i = 0; i < mCpuBlendMapStorage.size(); i++)
	{
    	PixelBox src(mLayerBlendMapSize, mLayerBlendMapSize, 1, PF_BYTE_RGBA, mCpuBlendMapStorage[i]);
        terrain->getLayerBlendTexture(i)->getBuffer()->blitFromMemory(src);
	}

    if( mGlobalColourMapEnabled)
    {
        terrain->setGlobalColourMapEnabled(true, mGlobalColourMapSize);
        PixelBox src(mGlobalColourMapSize, mGlobalColourMapSize, 1, PF_BYTE_RGB, mCpuColourMapStorage);
        terrain->getGlobalColourMap()->getBuffer()->blitFromMemory(src);
    }

    terrain->save(stream);

    OGRE_DELETE terrain;

    if( mFreeGlobalOptions )
        OGRE_DELETE mTerrainGlobalOptions;

    freeResources();

    return true;
}