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
///
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
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#include "OgreTerrain.h"
#include "OgreTerrainMaterialGenerator.h"
#include "OgreTerrainGroup.h"
#else
#include "Terrain/OgreTerrain.h"
#include "Terrain/OgreTerrainMaterialGenerator.h"
#include "Terrain/OgreTerrainGroup.h"
#endif

#include "BaseEditor.h"
#include "OgitorsRoot.h"
#include "OgitorsSystem.h"
#include "TerrainEditor.h"
#include "TerrainPageEditor.h"
#include "TerrainGroupEditor.h"
#include "PagingEditor.h"
#include "tinyxml.h"
#include "OgreStreamSerialiser.h"
#include "OgitorsUndoManager.h"
#include "OFSDataStream.h"

#include "PagedGeometry.h"
#include "GrassLoader.h"

#define MAX_LAYERS_ALLOWED 6

using namespace Forests;
using namespace Ogitors;

//-----------------------------------------------------------------------------------------
void CTerrainPageEditor::_saveGrass(Ogre::String pathPrefix)
{
    Ogre::TerrainGroup *terGroup = static_cast<Ogre::TerrainGroup*>(mParentEditor->get()->getHandle());
    Ogre::String filename = pathPrefix + terGroup->generateFilename(mPageX->get(), mPageY->get());

    Ogre::String denmapname = filename.substr(0, filename.size() - 4) + "_density.png";

    if(pathPrefix == "/Temp/tmp")
    {
        mTempDensityFileName = pathPrefix + Ogre::StringConverter::toString(mObjectID->get()) + "_density.png";
        denmapname = mTempDensityFileName;
    }

    OgitorsUtils::SaveImageOfs(mPGDensityMap, denmapname);

    mPGModified = false;
}
//-----------------------------------------------------------------------------------------
int CTerrainPageEditor::_getGrassLayerID(Ogre::String& texture, bool dontcreate)
{
    for(int i = 0;i < 4;i++)
    {
        if(mPGActive[i]->get() && mPGMaterial[i]->get() == texture)
            return i;
    }

    if(!dontcreate)
        return _createNewGrassLayer(texture);
    else
        return -1;
}
//-----------------------------------------------------------------------------------------
int CTerrainPageEditor::_getEmptyGrassLayer()
{
    bool isFull;

    unsigned int mDensityMapArea = mPGDensityMap.getWidth() * mPGDensityMap.getHeight();

    for(int i = 0;i < 4;i++)
    {
        if(mPGActive[i]->get())
        {
            float *ptr = getGrassPointer(i);
            isFull = false;
            for(unsigned int j = 0;j < mDensityMapArea;j++)
            {
                if(ptr[j] > 0.0f)
                {
                    isFull = true;
                    break;
                }
            }

            if(!isFull)
                return i;
        }
        else
            return i;
    }

    return -1;
}
//-----------------------------------------------------------------------------------------
int CTerrainPageEditor::_createNewGrassLayer(Ogre::String &texture)
{
    int layerID = _getEmptyGrassLayer();

    if(layerID == -1)
        return -1;

    OgitorsUndoManager::getSingletonPtr()->BeginCollection("Change Grass Layer");
    mPGMaterial[layerID]->set(texture);
    mPGActive[layerID]->set(true);
    OgitorsUndoManager::getSingletonPtr()->EndCollection(true);

    return layerID;
}
//-----------------------------------------------------------------------------------------
void CTerrainPageEditor::_loadGrassLayers()
{
    Ogre::TerrainGroup *terGroup = static_cast<Ogre::TerrainGroup*>(mParentEditor->get()->getHandle());
    CTerrainGroupEditor *parentEditor = static_cast<CTerrainGroupEditor*>(mParentEditor->get());

    Ogre::String denmapname;

    if(mTempDensityModified->get())
    {
        if(mTempDensityFileName.empty())
        {
            mTempDensityFileName = "/Temp/tmp" + Ogre::StringConverter::toString(mObjectID->get()) + "_density.png";
        }

        denmapname = mTempDensityFileName;
    }
    else
    {
        Ogre::String terrainDir = mOgitorsRoot->GetProjectOptions()->TerrainDirectory + "/terrain/";
        denmapname = terrainDir + terGroup->generateFilename(mPageX->get(), mPageY->get());
        denmapname = denmapname.substr(0, denmapname.size() - 4) + "_density.png";
    }

    OFS::OFSHANDLE *denmapHandle = new OFS::OFSHANDLE();

    mOgitorsRoot->GetProjectFile()->openFile(*denmapHandle, denmapname.c_str());

    Ogre::DataStreamPtr stream = Ogre::DataStreamPtr(OGRE_NEW OfsDataStream(mOgitorsRoot->GetProjectFile(), denmapHandle));

    try
    {
        mPGDensityMap.load(stream);
    }
    catch(...)
    {
        int densize = parentEditor->getGrassDensityMapSize();
        Ogre::uchar *data = OGRE_ALLOC_T(Ogre::uchar, densize * densize * 4, Ogre::MEMCATEGORY_GENERAL);
        memset(data, 0, densize * densize * 4);

        mPGDensityMap.loadDynamicImage(data, densize, densize, 1, Ogre::PF_A8R8G8B8, true);
        OgitorsUtils::SaveImageOfs(mPGDensityMap, denmapname);
    }

    stream.setNull();

    Ogre::AxisAlignedBox bBox = mHandle->getWorldAABB();
    TBounds bounds(bBox.getMinimum().x, bBox.getMinimum().z, bBox.getMaximum().x, bBox.getMaximum().z);

#if (OGRE_VERSION < ((1 << 16) | (9 << 8) | 0))
    Ogre::TexturePtr denptr = Ogre::TextureManager::getSingletonPtr()->createOrRetrieve(mName->get() + "_densitymap", PROJECT_TEMP_RESOURCE_GROUP).first;
#else
    Ogre::TexturePtr denptr = Ogre::TextureManager::getSingletonPtr()->createOrRetrieve(mName->get() + "_densitymap", PROJECT_TEMP_RESOURCE_GROUP).first.staticCast<Ogre::Texture>();
#endif
    denptr->loadImage(mPGDensityMap);

    for(int i = 0;i < 4;i++)
    {
        if(mPGActive[i]->get())
        {
            mPGLayers[i] = parentEditor->getGrassLoaderHandle()->addLayer(mPGMaterial[i]->get());

            //Configure the grass layer properties (size, density, animation properties, fade settings, etc.)
            mPGLayers[i]->setLightingEnabled(true);
            mPGLayers[i]->setMinimumSize(mPGMinSize[i]->get().x, mPGMinSize[i]->get().y);
            mPGLayers[i]->setMaximumSize(mPGMaxSize[i]->get().x, mPGMaxSize[i]->get().y);
            mPGLayers[i]->setAnimationEnabled(mPGAnimate[i]->get());
            mPGLayers[i]->setSwayDistribution(mPGSwayDistribution[i]->get());
            mPGLayers[i]->setSwayLength(mPGSwayLength[i]->get());
            mPGLayers[i]->setSwaySpeed(mPGSwaySpeed[i]->get());
            mPGLayers[i]->setDensity(mPGDensity[i]->get());

            mPGLayers[i]->setDensityMap(denptr, static_cast<Forests::MapChannel>(CHANNEL_RED + i));
            //layer->setColorMap(mHandle->getCompositeMap());
            mPGLayers[i]->setFadeTechnique(static_cast<Forests::FadeTechnique>(mPGFadeTech[i]->get()));
            mPGLayers[i]->setRenderTechnique(static_cast<Forests::GrassTechnique>(mPGGrassTech[i]->get()));
            mPGLayers[i]->setMapBounds(bounds);
        }
    }

    parentEditor->getPGHandle()->reloadGeometryPages(bounds);
}
//-----------------------------------------------------------------------------------------
void CTerrainPageEditor::_unloadGrassLayers()
{
    CTerrainGroupEditor *parentEditor = static_cast<CTerrainGroupEditor*>(mParentEditor->get());

    for(int i = 0; i < 4;i++)
    {
        if(mPGLayers[i])
            parentEditor->getGrassLoaderHandle()->deleteLayer(mPGLayers[i]);

        mPGLayers[i] = 0;
        OGRE_FREE(mPGLayerData[i], Ogre::MEMCATEGORY_GEOMETRY);
        mPGLayerData[i] = 0;
    }

    mPGDensityMap.freeMemory();

    Ogre::TextureManager::getSingletonPtr()->remove(mName->get() + "_densitymap");

    Ogre::AxisAlignedBox bBox = mHandle->getWorldAABB();
    TBounds bounds(bBox.getMinimum().x, bBox.getMinimum().z, bBox.getMaximum().x, bBox.getMaximum().z);

    parentEditor->getPGHandle()->reloadGeometryPages(bounds);
}
//-----------------------------------------------------------------------------------------
float *CTerrainPageEditor::getGrassPointer(unsigned int layerID)
{
    if(layerID > 3 || !(mPGActive[layerID]->get()))
        return 0;

    if(!mPGLayerData[layerID])
    {
        CTerrainGroupEditor *parentEditor = static_cast<CTerrainGroupEditor*>(mParentEditor->get());
        int mapsize = parentEditor->getGrassDensityMapSize();

        mPGLayerData[layerID] = OGRE_ALLOC_T(float, mapsize * mapsize, Ogre::MEMCATEGORY_GEOMETRY);
        Ogre::uchar *data = mPGDensityMap.getData();

        unsigned char rgbaShift[4];
        Ogre::PixelUtil::getBitShifts(mPGDensityMap.getFormat(), rgbaShift);
        int pos = rgbaShift[layerID] / 8;

        for(int i = 0;i < mapsize * mapsize;i++)
        {
            mPGLayerData[layerID][i] = (float)data[pos];
            pos += 4;
        }
    }

    return mPGLayerData[layerID];
}
//-----------------------------------------------------------------------------------------
void CTerrainPageEditor::updateGrassLayer(unsigned int layerID)
{
    if(layerID > 3)
        return;

    mPGActive[layerID]->set(true);

    Forests::DensityMap *dmap = mPGLayers[layerID]->getDensityMap();
    if(dmap)
    {
        Ogre::PixelBox pbox = dmap->getPixelBox();
        Ogre::uchar *data = static_cast<Ogre::uchar*>(dmap->getPixelBox().data);
        Ogre::uchar *data2 = mPGDensityMap.getData();

        int wsize = dmap->getPixelBox().getWidth();

        unsigned char rgbaShift[4];
        Ogre::PixelUtil::getBitShifts(mPGDensityMap.getFormat(), rgbaShift);
        int pos = rgbaShift[layerID] / 8;

        Ogre::uint val;

        for(int j = mPGDirtyRect.top;j < mPGDirtyRect.bottom;j++)
        {
            int tmploc = j * wsize;
            for(int i = mPGDirtyRect.left;i < mPGDirtyRect.right;i++)
            {
                val = static_cast<Ogre::uint>(mPGLayerData[layerID][tmploc + i]);
                data[tmploc + i] = val;
                data2[((tmploc + i) << 2) + pos] = val;
            }
        }
    }

    Ogre::TerrainGroup *terGroup = static_cast<Ogre::TerrainGroup*>(mParentEditor->get()->getHandle());
    CTerrainGroupEditor *parentEditor = static_cast<CTerrainGroupEditor*>(mParentEditor->get());

    float posL = (float)mPGDirtyRect.left / (float)mPGDensityMap.getWidth() * mHandle->getWorldSize();
    float posT = (float)mPGDirtyRect.top / (float)mPGDensityMap.getWidth() * mHandle->getWorldSize();
    float posR = (float)mPGDirtyRect.right / (float)mPGDensityMap.getWidth() * mHandle->getWorldSize();
    float posB = (float)mPGDirtyRect.bottom / (float)mPGDensityMap.getWidth() * mHandle->getWorldSize();


    Ogre::Vector3 pos = mPosition->get();

    float cornerX = pos.x - (mHandle->getWorldSize() / 2.0f);
    float cornerZ = pos.z - (mHandle->getWorldSize() / 2.0f);

    Forests::TBounds bounds(cornerX + posL, cornerZ + posT, cornerX + posR, cornerZ + posB);
    parentEditor->getPGHandle()->reloadGeometryPages(bounds);
    mPGDirtyRect.setNull();

    mPGModified = true;
}
//-----------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------
bool CTerrainPageEditor::_setPGActive(OgitorsPropertyBase* property, const bool& value)
{
    if(mLoaded->get())
    {
        CTerrainGroupEditor *parentEditor = static_cast<CTerrainGroupEditor*>(mParentEditor->get());
        int idx = property->getTag();

        if(mPGMaterial[idx]->get().empty())
        {
            mSystem->DisplayMessageDialog(OTR("You must first specify a plant material!!"), DLGTYPE_OK);
            return false;
        }

        if(value)
        {
            Ogre::AxisAlignedBox bBox = mHandle->getWorldAABB();
            TBounds bounds(bBox.getMinimum().x, bBox.getMinimum().z, bBox.getMaximum().x, bBox.getMaximum().z);

#if (OGRE_VERSION < ((1 << 16) | (9 << 8) | 0))
            Ogre::TexturePtr denptr = Ogre::TextureManager::getSingletonPtr()->getByName(mName->get() + "_densitymap", PROJECT_TEMP_RESOURCE_GROUP);
#else
            Ogre::TexturePtr denptr = Ogre::TextureManager::getSingletonPtr()->getByName(mName->get() + "_densitymap", PROJECT_TEMP_RESOURCE_GROUP).staticCast<Ogre::Texture>();
#endif

            mPGLayers[idx] = parentEditor->getGrassLoaderHandle()->addLayer(mPGMaterial[idx]->get());

            //Configure the grass layer properties (size, density, animation properties, fade settings, etc.)
            mPGLayers[idx]->setLightingEnabled(true);
            mPGLayers[idx]->setMinimumSize(mPGMinSize[idx]->get().x, mPGMinSize[idx]->get().y);
            mPGLayers[idx]->setMaximumSize(mPGMaxSize[idx]->get().x, mPGMaxSize[idx]->get().y);
            mPGLayers[idx]->setAnimationEnabled(mPGAnimate[idx]->get());
            mPGLayers[idx]->setSwayDistribution(mPGSwayDistribution[idx]->get());
            mPGLayers[idx]->setSwayLength(mPGSwayLength[idx]->get());
            mPGLayers[idx]->setSwaySpeed(mPGSwaySpeed[idx]->get());
            mPGLayers[idx]->setDensity(mPGDensity[idx]->get());

            mPGLayers[idx]->setDensityMap(denptr, static_cast<Forests::MapChannel>(CHANNEL_RED + idx));
            //layer->setColorMap(mHandle->getCompositeMap());
            mPGLayers[idx]->setFadeTechnique(static_cast<Forests::FadeTechnique>(mPGFadeTech[idx]->get()));
            mPGLayers[idx]->setRenderTechnique(static_cast<Forests::GrassTechnique>(mPGGrassTech[idx]->get()));
            mPGLayers[idx]->setMapBounds(bounds);

            
            int mapsize = parentEditor->getGrassDensityMapSize();
            Ogre::uchar *data_src = mPGDensityMap.getData();

            unsigned char rgbaShift[4];
            Ogre::PixelUtil::getBitShifts(mPGDensityMap.getFormat(), rgbaShift);
            int pos = rgbaShift[idx] / 8;

            Forests::DensityMap *dmap = mPGLayers[idx]->getDensityMap();
            if(dmap)
            {
                Ogre::PixelBox pbox = dmap->getPixelBox();
                Ogre::uchar *data_dest = static_cast<Ogre::uchar*>(dmap->getPixelBox().data);

                for(int i = 0;i < mapsize * mapsize;i++)
                {
                    data_dest[i] = data_src[pos];
                    pos += 4;
                }
            }
        }
        else
        {
            if(mPGLayers[idx])
            {
                parentEditor->getGrassLoaderHandle()->deleteLayer(mPGLayers[idx]);
                mPGLayers[idx] = 0;
                OGRE_FREE(mPGLayerData[idx], Ogre::MEMCATEGORY_GEOMETRY);
                mPGLayerData[idx] = 0;
            }
        }

        _refreshGrassGeometry();

        return true;
    }

    return false;
}
//-----------------------------------------------------------------------------------------
void CTerrainPageEditor::_refreshGrassGeometry(Ogre::Rect *rect)
{
    if(mHandle && mHandle->isLoaded())
    {
        CTerrainGroupEditor *parentEditor = static_cast<CTerrainGroupEditor*>(mParentEditor->get());

        if(rect)
        {
            TBounds bounds(rect->left, rect->top, rect->right, rect->bottom);
            parentEditor->getPGHandle()->reloadGeometryPages(bounds);
        }
        else
        {
            Ogre::AxisAlignedBox bBox = mHandle->getWorldAABB();
            TBounds bounds(bBox.getMinimum().x, bBox.getMinimum().z, bBox.getMaximum().x, bBox.getMaximum().z);
            parentEditor->getPGHandle()->reloadGeometryPages(bounds);
        }
    }
}
//-----------------------------------------------------------------------------------------
bool CTerrainPageEditor::_setPGMaterial(OgitorsPropertyBase* property, const Ogre::String& value)
{
    if(mPGLayers[property->getTag()])
    {
        if(value.empty())
            mPGActive[property->getTag()]->set(false);
        else
            mPGLayers[property->getTag()]->setMaterialName(value);

        _refreshGrassGeometry();
    }

    return true;
}
//-----------------------------------------------------------------------------------------
bool CTerrainPageEditor::_setPGMinSize(OgitorsPropertyBase* property, const Ogre::Vector2& value)
{
    if(mPGLayers[property->getTag()])
    {
        mPGLayers[property->getTag()]->setMinimumSize(value.x, value.y);
        _refreshGrassGeometry();
    }
    return true;
}
//-----------------------------------------------------------------------------------------
bool CTerrainPageEditor::_setPGMaxSize(OgitorsPropertyBase* property, const Ogre::Vector2& value)
{
    if(mPGLayers[property->getTag()])
    {
        mPGLayers[property->getTag()]->setMaximumSize(value.x, value.y);
        _refreshGrassGeometry();
    }
    return true;
}
//-----------------------------------------------------------------------------------------
bool CTerrainPageEditor::_setPGAnimate(OgitorsPropertyBase* property, const bool& value)
{
    if(mPGLayers[property->getTag()])
    {
        mPGLayers[property->getTag()]->setAnimationEnabled(value);
        _refreshGrassGeometry();
    }
    return true;
}
//-----------------------------------------------------------------------------------------
bool CTerrainPageEditor::_setPGSwayDistribution(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(mPGLayers[property->getTag()])
    {
        mPGLayers[property->getTag()]->setSwayDistribution(value);
        _refreshGrassGeometry();
    }
    return true;
}
//-----------------------------------------------------------------------------------------
bool CTerrainPageEditor::_setPGSwayLength(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(mPGLayers[property->getTag()])
    {
        mPGLayers[property->getTag()]->setSwayLength(value);
        _refreshGrassGeometry();
    }
    return true;
}
//-----------------------------------------------------------------------------------------
bool CTerrainPageEditor::_setPGSwaySpeed(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(mPGLayers[property->getTag()])
    {
        mPGLayers[property->getTag()]->setSwaySpeed(value);
        _refreshGrassGeometry();
    }
    return true;
}
//-----------------------------------------------------------------------------------------
bool CTerrainPageEditor::_setPGDensity(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(mPGLayers[property->getTag()])
    {
        mPGLayers[property->getTag()]->setDensity(value);
        _refreshGrassGeometry();
    }
    return true;
}
//-----------------------------------------------------------------------------------------
bool CTerrainPageEditor::_setPGFadeTech(OgitorsPropertyBase* property, const int& value)
{
    if(mPGLayers[property->getTag()])
    {
        mPGLayers[property->getTag()]->setFadeTechnique(static_cast<Forests::FadeTechnique>(value));
        _refreshGrassGeometry();
    }
    return true;
}
//-----------------------------------------------------------------------------------------
bool CTerrainPageEditor::_setPGGrassTech(OgitorsPropertyBase* property, const int& value)
{
    if(mPGLayers[property->getTag()])
    {
        mPGLayers[property->getTag()]->setRenderTechnique(static_cast<Forests::GrassTechnique>(value));
        _refreshGrassGeometry();
    }
    return true;
}
//-----------------------------------------------------------------------------------------
