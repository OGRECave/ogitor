/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2011 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
//
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
////////////////////////////////////////////////////////////////////////////////*/

#include "OgitorsPrerequisites.h"
#include "OgreStreamSerialiser.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#include "OgreTerrain.h"
#include "OgreTerrainMaterialGenerator.h"
#include "OgreTerrainGroup.h"
#include "OgreTerrainQuadTreeNode.h"
#else
#include "Terrain/OgreTerrain.h"
#include "Terrain/OgreTerrainMaterialGenerator.h"
#include "Terrain/OgreTerrainGroup.h"
#include "Terrain/OgreTerrainQuadTreeNode.h"
#endif

#include "BaseEditor.h"
#include "OgitorsRoot.h"
#include "OgitorsSystem.h"
#include "TerrainEditor.h"
#include "TerrainPageEditor.h"
#include "TerrainGroupEditor.h"
#include "OgitorsUndoManager.h"
#include "TerrainGroupUndo.h"

using namespace Ogitors;

int TerrainUndoCounter = 0;

const Ogre::uint32 TERRAINHEIGHTUNDO_CHUNK_ID = Ogre::StreamSerialiser::makeIdentifier("UHGH");
const Ogre::uint16 TERRAINHEIGHTUNDO_CHUNK_VERSION = 1;
const Ogre::uint32 TERRAINBLENDUNDO_CHUNK_ID = Ogre::StreamSerialiser::makeIdentifier("UBLD");
const Ogre::uint16 TERRAINBLENDUNDO_CHUNK_VERSION = 1;
const Ogre::uint32 TERRAINCOLOURUNDO_CHUNK_ID = Ogre::StreamSerialiser::makeIdentifier("UCOL");
const Ogre::uint16 TERRAINCOLOURUNDO_CHUNK_VERSION = 1;
const Ogre::uint32 TERRAINGRASSUNDO_CHUNK_ID = Ogre::StreamSerialiser::makeIdentifier("UGRS");
const Ogre::uint16 TERRAINGRASSUNDO_CHUNK_VERSION = 1;


//-----------------------------------------------------------------------------------------
TerrainLayerUndo::TerrainLayerUndo(unsigned int objectID, int layerID, LayerUndoType type, const Ogre::String& diffuse, const Ogre::String& normal, Ogre::Real worldSize)
{
    mObjectID = objectID;
    mLayerID = layerID;
    mType = type;
    mDiffuse = diffuse;
    mNormal = normal;
    mWorldSize = worldSize;
    if(type == LU_DELETE)
        mDescription = "Delete Layer";
    else if(type == LU_CREATE)
        mDescription = "Create Layer";
    else if(type == LU_MODIFY)
        mDescription = "Modify Layer";
}
//-----------------------------------------------------------------------------------------
bool TerrainLayerUndo::apply()
{
    CTerrainPageEditor *pageED = static_cast<CTerrainPageEditor *>(OgitorsRoot::getSingletonPtr()->FindObject(mObjectID));
    if(pageED)
    {
        if(mType == LU_CREATE)
            pageED->_deleteLayer(mLayerID);
        else if(mType == LU_DELETE)
            pageED->_createLayer(mLayerID, mDiffuse, mNormal, mWorldSize);
        else if(mType == LU_MODIFY)
            pageED->_changeLayer(mLayerID, mDiffuse, mNormal, mWorldSize);
        return true;
    }
    
    return false;
}
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
TerrainHeightUndo::TerrainHeightUndo(unsigned int objectID, Ogre::Rect area, float *data) :
mObjectID(objectID), mAreaOfEffect(area)
{
    unsigned int size = area.width() * area.height();
    mFileName = OgitorsRoot::getSingletonPtr()->GetProjectOptions()->ProjectDir + "/Temp/TerrainUndo" + Ogre::StringConverter::toString(++TerrainUndoCounter) + ".dat";
    mFileName = OgitorsUtils::QualifyPath(mFileName);

    std::fstream fstr(mFileName.c_str(), std::ios::out|std::ios::binary);
    Ogre::DataStreamPtr stream = Ogre::DataStreamPtr(OGRE_NEW Ogre::FileStreamDataStream(&fstr, false));
    Ogre::StreamSerialiser ss(stream);
	ss.writeChunkBegin(TERRAINHEIGHTUNDO_CHUNK_ID, TERRAINHEIGHTUNDO_CHUNK_VERSION);
    ss.write((const Ogre::Real*)data, size);
    ss.writeChunkEnd(TERRAINHEIGHTUNDO_CHUNK_ID);
    stream.setNull();
}
//-----------------------------------------------------------------------------------------
TerrainHeightUndo::~TerrainHeightUndo()
{
    if(mFileName != "")
        OgitorsSystem::getSingletonPtr()->DeleteFile(mFileName);
}
//-----------------------------------------------------------------------------------------
bool TerrainHeightUndo::apply()
{
    unsigned int size = mAreaOfEffect.width() * mAreaOfEffect.height();

    float *data = OGRE_ALLOC_T(float, size, Ogre::MEMCATEGORY_RESOURCE); 

    std::fstream fstr(mFileName.c_str(), std::ios::in|std::ios::binary);
    Ogre::DataStreamPtr stream = Ogre::DataStreamPtr(OGRE_NEW Ogre::FileStreamDataStream(&fstr, false));
    Ogre::StreamSerialiser ss(stream);
    ss.readChunkBegin(TERRAINHEIGHTUNDO_CHUNK_ID, TERRAINHEIGHTUNDO_CHUNK_VERSION);
    ss.read((Ogre::Real*)data, size);
    stream.setNull();

    CTerrainPageEditor *pageED = static_cast<CTerrainPageEditor *>(OgitorsRoot::getSingletonPtr()->FindObject(mObjectID));
    if(pageED)
    {
        pageED->_swapHeights(mAreaOfEffect, data);

        OgitorsUndoManager::getSingletonPtr()->AddUndo(OGRE_NEW TerrainHeightUndo(mObjectID, mAreaOfEffect, data));
    }

    OGRE_FREE(data, Ogre::MEMCATEGORY_RESOURCE);

    return true;
}
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
TerrainColourUndo::TerrainColourUndo(unsigned int objectID, Ogre::Rect area, Ogre::ColourValue *data) :
mObjectID(objectID), mAreaOfEffect(area)
{
    unsigned int size = area.width() * area.height() * sizeof(Ogre::ColourValue);
    mFileName = OgitorsRoot::getSingletonPtr()->GetProjectOptions()->ProjectDir + "/Temp/TerrainUndo" + Ogre::StringConverter::toString(++TerrainUndoCounter) + ".dat";
    mFileName = OgitorsUtils::QualifyPath(mFileName);

    std::fstream fstr(mFileName.c_str(), std::ios::out|std::ios::binary);
    Ogre::DataStreamPtr stream = Ogre::DataStreamPtr(OGRE_NEW Ogre::FileStreamDataStream(&fstr, false));
    Ogre::StreamSerialiser ss(stream);
	ss.writeChunkBegin(TERRAINCOLOURUNDO_CHUNK_ID, TERRAINCOLOURUNDO_CHUNK_VERSION);
    ss.write((Ogre::uint8*)data, size);
    ss.writeChunkEnd(TERRAINCOLOURUNDO_CHUNK_ID);
    stream.setNull();
}
//-----------------------------------------------------------------------------------------
TerrainColourUndo::~TerrainColourUndo()
{
    if(mFileName != "")
        OgitorsSystem::getSingletonPtr()->DeleteFile(mFileName);
}
//-----------------------------------------------------------------------------------------
bool TerrainColourUndo::apply()
{
    unsigned int size = mAreaOfEffect.width() * mAreaOfEffect.height() * sizeof(Ogre::ColourValue);

    Ogre::ColourValue *data = (Ogre::ColourValue*)OGRE_MALLOC(size, Ogre::MEMCATEGORY_RESOURCE); 

    std::fstream fstr(mFileName.c_str(), std::ios::in|std::ios::binary);
    Ogre::DataStreamPtr stream = Ogre::DataStreamPtr(OGRE_NEW Ogre::FileStreamDataStream(&fstr, false));
    Ogre::StreamSerialiser ss(stream);
    ss.readChunkBegin(TERRAINCOLOURUNDO_CHUNK_ID, TERRAINCOLOURUNDO_CHUNK_VERSION);
    ss.read((Ogre::uint8*)data, size);
    stream.setNull();

    CTerrainPageEditor *pageED = static_cast<CTerrainPageEditor *>(OgitorsRoot::getSingletonPtr()->FindObject(mObjectID));
    if(pageED)
    {
        pageED->_swapColours(mAreaOfEffect, data);

        OgitorsUndoManager::getSingletonPtr()->AddUndo(OGRE_NEW TerrainColourUndo(mObjectID, mAreaOfEffect, data));
    }

    OGRE_FREE(data, Ogre::MEMCATEGORY_RESOURCE);

    return true;
}
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
TerrainBlendUndo::TerrainBlendUndo(unsigned int objectID, Ogre::Rect area, int blendStart, int blendCount, float *data) :
mObjectID(objectID), mAreaOfEffect(area), mBlendStart(blendStart), mBlendCount(blendCount)
{
    unsigned int size = area.width() * area.height() * blendCount;
    mFileName = OgitorsRoot::getSingletonPtr()->GetProjectOptions()->ProjectDir + "/Temp/TerrainUndo" + Ogre::StringConverter::toString(++TerrainUndoCounter) + ".dat";
    mFileName = OgitorsUtils::QualifyPath(mFileName);

    std::fstream fstr(mFileName.c_str(), std::ios::out|std::ios::binary);
    Ogre::DataStreamPtr stream = Ogre::DataStreamPtr(OGRE_NEW Ogre::FileStreamDataStream(&fstr, false));
    Ogre::StreamSerialiser ss(stream);
	ss.writeChunkBegin(TERRAINBLENDUNDO_CHUNK_ID, TERRAINBLENDUNDO_CHUNK_VERSION);
    ss.write((const Ogre::Real*)data, size);
    ss.writeChunkEnd(TERRAINBLENDUNDO_CHUNK_ID);
    stream.setNull();
}
//-----------------------------------------------------------------------------------------
TerrainBlendUndo::~TerrainBlendUndo()
{
    if(mFileName != "")
        OgitorsSystem::getSingletonPtr()->DeleteFile(mFileName);
}
//-----------------------------------------------------------------------------------------
bool TerrainBlendUndo::apply()
{
    unsigned int size = mAreaOfEffect.width() * mAreaOfEffect.height() * mBlendCount;

    float *data = OGRE_ALLOC_T(float, size, Ogre::MEMCATEGORY_RESOURCE); 

    std::fstream fstr(mFileName.c_str(), std::ios::in|std::ios::binary);
    Ogre::DataStreamPtr stream = Ogre::DataStreamPtr(OGRE_NEW Ogre::FileStreamDataStream(&fstr, false));
    Ogre::StreamSerialiser ss(stream);
    ss.readChunkBegin(TERRAINBLENDUNDO_CHUNK_ID, TERRAINBLENDUNDO_CHUNK_VERSION);
    ss.read((Ogre::Real*)data, size);
    stream.setNull();

    CTerrainPageEditor *pageED = static_cast<CTerrainPageEditor *>(OgitorsRoot::getSingletonPtr()->FindObject(mObjectID));
    if(pageED)
    {
        pageED->_swapBlends(mBlendStart, mBlendCount, mAreaOfEffect, data);
        
        OgitorsUndoManager::getSingletonPtr()->AddUndo(OGRE_NEW TerrainBlendUndo(mObjectID, mAreaOfEffect, mBlendStart, mBlendCount, data));
    }

    OGRE_FREE(data, Ogre::MEMCATEGORY_RESOURCE);

    return true;
}
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
TerrainGrassUndo::TerrainGrassUndo(unsigned int objectID, Ogre::Rect area, float *data) :
mObjectID(objectID), mAreaOfEffect(area)
{
    unsigned int size = area.width() * area.height() * 4;
    mFileName = OgitorsRoot::getSingletonPtr()->GetProjectOptions()->ProjectDir + "/Temp/TerrainUndo" + Ogre::StringConverter::toString(++TerrainUndoCounter) + ".dat";
    mFileName = OgitorsUtils::QualifyPath(mFileName);

    std::fstream fstr(mFileName.c_str(), std::ios::out|std::ios::binary);
    Ogre::DataStreamPtr stream = Ogre::DataStreamPtr(OGRE_NEW Ogre::FileStreamDataStream(&fstr, false));
    Ogre::StreamSerialiser ss(stream);
	ss.writeChunkBegin(TERRAINGRASSUNDO_CHUNK_ID, TERRAINGRASSUNDO_CHUNK_VERSION);
    ss.write((const Ogre::Real*)data, size);
    ss.writeChunkEnd(TERRAINGRASSUNDO_CHUNK_ID);
    stream.setNull();
}
//-----------------------------------------------------------------------------------------
TerrainGrassUndo::~TerrainGrassUndo()
{
    if(mFileName != "")
        OgitorsSystem::getSingletonPtr()->DeleteFile(mFileName);
}
//-----------------------------------------------------------------------------------------
bool TerrainGrassUndo::apply()
{
    unsigned int size = mAreaOfEffect.width() * mAreaOfEffect.height() * 4;

    float *data = OGRE_ALLOC_T(float, size, Ogre::MEMCATEGORY_RESOURCE); 

    std::fstream fstr(mFileName.c_str(), std::ios::in|std::ios::binary);
    Ogre::DataStreamPtr stream = Ogre::DataStreamPtr(OGRE_NEW Ogre::FileStreamDataStream(&fstr, false));
    Ogre::StreamSerialiser ss(stream);
    ss.readChunkBegin(TERRAINGRASSUNDO_CHUNK_ID, TERRAINGRASSUNDO_CHUNK_VERSION);
    ss.read((Ogre::Real*)data, size);
    stream.setNull();

    CTerrainPageEditor *pageED = static_cast<CTerrainPageEditor *>(OgitorsRoot::getSingletonPtr()->FindObject(mObjectID));
    if(pageED)
    {
        pageED->_swapGrass(mAreaOfEffect, data);

        OgitorsUndoManager::getSingletonPtr()->AddUndo(OGRE_NEW TerrainGrassUndo(mObjectID, mAreaOfEffect, data));
    }

    OGRE_FREE(data, Ogre::MEMCATEGORY_RESOURCE);

    return true;
}
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
