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

#pragma once

#include "OgreTerrain.h"
#include "OgreStreamSerialiser.h"

#if (defined( __WIN32__ ) || defined( _WIN32 )) && ! defined( __GNUC__ )
   #ifdef OgreTerrainConverter_EXPORT
     #define OgreTerrainConverterExport __declspec (dllexport)
   #else
     #define OgreTerrainConverterExport __declspec (dllimport)
   #endif
#else
   #define OgreTerrainConverterExport
#endif

class OgreTerrainConverterExport OgreTerrainConverter
{
public:
    OgreTerrainConverter();
    ~OgreTerrainConverter();

    bool Upgrade(Ogre::StreamSerialiser& stream_in, Ogre::StreamSerialiser& stream_out);

protected:
    typedef Ogre::vector<Ogre::uint8*>::type BytePointerList;

	float* mHeightData;
    Ogre::uint8 mAlign;
	Ogre::Real mWorldSize;
	Ogre::uint16 mSize;
	Ogre::uint16 mMaxBatchSize;
	Ogre::uint16 mMinBatchSize;
	Ogre::Vector3 mPos;
    Ogre::TerrainLayerDeclaration mLayerDecl;
    Ogre::Terrain::LayerInstanceList mLayers;
	Ogre::uint16 mLayerBlendMapSize;
	Ogre::uint16 mLightMapSize;
	Ogre::uint16 mCompositeMapSize;
	BytePointerList mCpuBlendMapStorage;
	Ogre::uint16 mGlobalColourMapSize;
	bool mGlobalColourMapEnabled;
	Ogre::uint8* mCpuColourMapStorage;
    Ogre::uint8* mCpuCompositeMapStorage;

    Ogre::uint8 getBlendTextureCount(Ogre::uint8 numLayers);
    
    bool readLayerDeclaration(Ogre::StreamSerialiser& stream, Ogre::TerrainLayerDeclaration& targetdecl);
    
    bool readLayerInstanceList(Ogre::StreamSerialiser& stream, size_t numSamplers, Ogre::Terrain::LayerInstanceList& targetlayers);

    void freeResources();

    bool Export(Ogre::StreamSerialiser& stream);
};


//------------------------------------------------------------------------------
