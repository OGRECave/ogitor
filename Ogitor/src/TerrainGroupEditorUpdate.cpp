/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2012 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
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
#include "OgreStreamSerialiser.h"
#include "ViewportEditor.h"
#include "OgitorsUndoManager.h"

#include "PagedGeometry.h"
#include "GrassLoader.h"

using namespace Ogitors;

//-----------------------------------------------------------------------------------------
bool CTerrainGroupEditor::_getEditRect(Ogre::Vector3& editpos, Ogre::Rect& brushrect, Ogre::Rect& maprect, int size)
{
    int mMapBrushSize = (float)mBrushSize;
    float halfSize = (float)mBrushSize / 2.0f;

    float tx = editpos.x - halfSize;
    float ty = editpos.y - halfSize;

    int mapX = tx;
    mapX += (int)(tx * 2.0f) - (mapX * 2);

    int mapY = ty;
    mapY += (int)(ty * 2.0f) - (mapY * 2);

    maprect = Ogre::Rect(mapX, mapY, mMapBrushSize + mapX, mMapBrushSize + mapY);
    brushrect = Ogre::Rect(0,0,mBrushSize,mBrushSize);

    if(maprect.left < 0)
    {
        brushrect.left += (0 - maprect.left);
        maprect.left = 0;
    }
    if(maprect.top < 0)
    {
        brushrect.top += (0 - maprect.top);
        maprect.top = 0;
    }
    if(maprect.right > size)
    {
        brushrect.right -= (maprect.right - size);
        maprect.right = size;
    }
    if(maprect.bottom > size)
    {
        brushrect.bottom -= (maprect.bottom - size);
        maprect.bottom = size;
    }

    if(((maprect.right - maprect.left) < 1) || ((maprect.bottom - maprect.top) < 1))
        return false;

    brushrect.left *= (float)BRUSH_DATA_SIZE / (float)mBrushSize;
    brushrect.right *= (float)BRUSH_DATA_SIZE / (float)mBrushSize;
    brushrect.top *= (float)BRUSH_DATA_SIZE / (float)mBrushSize;
    brushrect.bottom *= (float)BRUSH_DATA_SIZE / (float)mBrushSize;

    return true;
}
//-----------------------------------------------------------------------------------------
void CTerrainGroupEditor::_deform(CTerrainPageEditor *handle, Ogre::Vector3 &editpos, float timePassed)
{
    Ogre::Rect brushrect, maprect;
    editpos.x *= (float)(mMapSize->get() - 1);
    editpos.y *= (float)(mMapSize->get() - 1);

    if(!_getEditRect(editpos, brushrect, maprect, mMapSize->get()))
        return;

    handle->_notifyModification(-1, maprect);

    Ogre::Terrain *terrain = static_cast<Ogre::Terrain*>(handle->getHandle());

    float *mHeightData = terrain->getHeightData();

    if(mEditDirection)
        timePassed *= -1.0f;

    float mRatio = (float)BRUSH_DATA_SIZE / (float)mBrushSize;
    float brushPos;
    int mapPos;
    for(int j = maprect.top;j < maprect.bottom;j++)
    {
        brushPos = (brushrect.top + (int)((j - maprect.top) * mRatio)) * BRUSH_DATA_SIZE;
        brushPos += brushrect.left;
        mapPos = (j * mMapSize->get()) + maprect.left;

        for(int i = maprect.left;i < maprect.right;i++)
        {
            assert(mapPos < (mMapSize->get() * mMapSize->get()) && mapPos >= 0);
            assert((int)brushPos < (BRUSH_DATA_SIZE * BRUSH_DATA_SIZE) && (int)brushPos >= 0);

            float val = mHeightData[mapPos] + (mBrushData[(int)brushPos] * mBrushIntensity * timePassed);

            assert(val < 10000.0f);
            assert(val > -10000.0f);

            mHeightData[mapPos] = val;
            ++mapPos;

            brushPos += mRatio;
        }
    }

    terrain->dirtyRect(maprect);
}
//-----------------------------------------------------------------------------------------
void CTerrainGroupEditor::_calculatesmoothingfactor(CTerrainPageEditor *handle, Ogre::Vector3 &editpos, float& avg, int& sample_count)
{
    Ogre::Rect brushrect, maprect;
    editpos.x *= (float)(mMapSize->get() - 1);
    editpos.y *= (float)(mMapSize->get() - 1);

    avg = 0.0f;
    sample_count = 0;

    if(!_getEditRect(editpos, brushrect, maprect, mMapSize->get()))
        return;

    Ogre::Terrain *terrain = static_cast<Ogre::Terrain*>(handle->getHandle());

    float *mHeightData = terrain->getHeightData();
    int mapPos;

    for(int j = maprect.top;j < maprect.bottom;j++)
    {
        mapPos = (j * mMapSize->get()) + maprect.left;

        for(int i = maprect.left;i < maprect.right;i++)
        {
            avg += mHeightData[mapPos];
            ++mapPos;
        }
    }

    sample_count = (maprect.right - maprect.left) * (maprect.bottom - maprect.top);
}
//-----------------------------------------------------------------------------------------
void CTerrainGroupEditor::_smooth(CTerrainPageEditor *handle, Ogre::Vector3 &editpos, float avg, float timePassed)
{
    Ogre::Rect brushrect, maprect;
    editpos.x *= (float)(mMapSize->get() - 1);
    editpos.y *= (float)(mMapSize->get() - 1);

    if(!_getEditRect(editpos, brushrect, maprect, mMapSize->get()))
        return;

    handle->_notifyModification(-1, maprect);

    Ogre::Terrain *terrain = static_cast<Ogre::Terrain*>(handle->getHandle());

    float *mHeightData = terrain->getHeightData();

    float mRatio = (float)BRUSH_DATA_SIZE / (float)mBrushSize;
    float brushPos;
    int mapPos;

    float mFactor = mBrushIntensity * timePassed * 0.03f;

    if(mEditDirection)
    {
        for(int j = maprect.top;j < maprect.bottom;j++)
        {
            brushPos = (brushrect.top + (int)((j - maprect.top) * mRatio)) * BRUSH_DATA_SIZE;
            brushPos += brushrect.left;
            mapPos = (j * mMapSize->get()) + maprect.left;

            for(int i = maprect.left;i < maprect.right;i++)
            {
                assert(mapPos < (mMapSize->get() * mMapSize->get()) && mapPos >= 0);
                assert((int)brushPos < (BRUSH_DATA_SIZE * BRUSH_DATA_SIZE) && (int)brushPos >= 0);

                float val = avg - mHeightData[mapPos];
                val = val * std::min(mBrushData[(int)brushPos] * mFactor, 1.0f);
                mHeightData[mapPos] -= val;
                ++mapPos;
                brushPos += mRatio;
            }
        }
    }
    else
    {
        for(int j = maprect.top;j < maprect.bottom;j++)
        {
            brushPos = (brushrect.top + (int)((j - maprect.top) * mRatio)) * BRUSH_DATA_SIZE;
            brushPos += brushrect.left;
            mapPos = (j * mMapSize->get()) + maprect.left;

            for(int i = maprect.left;i < maprect.right;i++)
            {
                assert(mapPos < (mMapSize->get() * mMapSize->get()) && mapPos >= 0);
                assert((int)brushPos < (BRUSH_DATA_SIZE * BRUSH_DATA_SIZE) && (int)brushPos >= 0);

                float val = avg - mHeightData[mapPos];
                val = val * std::min(mBrushData[(int)brushPos] * mFactor, 1.0f);
                mHeightData[mapPos] += val;
                ++mapPos;
                brushPos += mRatio;
            }
        }
    }

    terrain->dirtyRect(maprect);
}
//-----------------------------------------------------------------------------------------
void CTerrainGroupEditor::_splat(CTerrainPageEditor *handle, Ogre::Vector3 &editpos, float timePassed)
{
    Ogre::Rect brushrect, maprect;

    Ogre::Terrain *terrain = static_cast<Ogre::Terrain*>(handle->getHandle());

    int mBlendMapSize = terrain->getLayerBlendMapSize();
    editpos.x *= (float)mBlendMapSize;
    editpos.y = (1.0f - editpos.y) * (float)mBlendMapSize;
    if(!_getEditRect(editpos, brushrect, maprect, mBlendMapSize))
        return;

    int mLayer = 0;

    mLayer = handle->_getLayerID(mTextureDiffuse, mTextureNormal, mEditDirection);

    if(mLayer < 1)
    {
        Ogre::String msg = handle->getName() + " already has maximum number of supported layers...";
        mSystem->DisplayMessageDialog(OTR(msg), DLGTYPE_OK);
        return;
    }

    handle->_notifyModification(mLayer, maprect);

    int mLayerMax = terrain->getLayerCount();
    Ogre::TerrainLayerBlendMap *mBlendMaps[128];
    float *mBlendDatas[128];

    Ogre::TerrainLayerBlendMap *mCurrentBlendMap = terrain->getLayerBlendMap(mLayer);
    float *mCurrentBlendData = mCurrentBlendMap->getBlendPointer();

    for(int l = mLayer;l < mLayerMax;l++)
    {
        mBlendMaps[l] = terrain->getLayerBlendMap(l);
        mBlendDatas[l] = mBlendMaps[l]->getBlendPointer();
    }

    float mRatio = (float)BRUSH_DATA_SIZE / (float)mBrushSize;
    float brushPos;
    int mapPos;

    int right = brushrect.right;
    brushrect.right = BRUSH_DATA_SIZE - brushrect.left;
    brushrect.left = BRUSH_DATA_SIZE - right;

    float factor = mBrushIntensity * timePassed * 0.2f;
    if(!mEditDirection)
    {
        int u;
        float sum;

        for(int j = maprect.top;j < maprect.bottom;j++)
        {
            brushPos = (brushrect.top + (int)((j - maprect.top) * mRatio)) * BRUSH_DATA_SIZE;
            brushPos += brushrect.right;

            mapPos = (j * mBlendMapSize) + maprect.left;

            for(int i = maprect.left;i < maprect.right;i++)
            {
                brushPos -= mRatio;

                assert(mapPos < (mBlendMapSize * mBlendMapSize) && mapPos >= 0);
                assert((int)brushPos < (BRUSH_DATA_SIZE * BRUSH_DATA_SIZE) && (int)brushPos >= 0);

                sum = 0.0f;

                for(u = mLayer + 1;u < mLayerMax;u++)
                    sum += mBlendDatas[u][mapPos];

                float val = mCurrentBlendData[mapPos] + (mBrushData[(int)brushPos] * factor);
                sum += val;

                if(sum > 1.0f)
                {
                    float normfactor = 1.0f / (float)sum;
                    mCurrentBlendData[mapPos] = val * normfactor;
                    for(u = mLayer + 1;u < mLayerMax;u++)
                        mBlendDatas[u][mapPos] *= normfactor;
                }
                else
                    mCurrentBlendData[mapPos] = val;

                ++mapPos;
            }
        }
        for(u = mLayer;u < mLayerMax;u++)
        {
            mBlendMaps[u]->dirtyRect(maprect);
            mBlendMaps[u]->update();
        }
    }
    else
    {
        for(int j = maprect.top;j < maprect.bottom;j++)
        {
            brushPos = (brushrect.top + (int)((j - maprect.top) * mRatio)) * BRUSH_DATA_SIZE;
            brushPos += brushrect.right;

            mapPos = (j * mBlendMapSize) + maprect.left;

            for(int i = maprect.left;i < maprect.right;i++)
            {
                brushPos -= mRatio;

                assert(mapPos < (mBlendMapSize * mBlendMapSize) && mapPos >= 0);
                assert((int)brushPos < (BRUSH_DATA_SIZE * BRUSH_DATA_SIZE) && (int)brushPos >= 0);

                float val = mCurrentBlendData[mapPos] - (mBrushData[(int)brushPos] * factor);

                if(val < 0.0f)
                    val = 0.0f;

                mCurrentBlendData[mapPos] = val;
                ++mapPos;
            }
        }
        mCurrentBlendMap->dirtyRect(maprect);
        mCurrentBlendMap->update();
    }
}
//-----------------------------------------------------------------------------------------
void CTerrainGroupEditor::_paint(CTerrainPageEditor *handle, Ogre::Vector3 &editpos, float timePassed)
{
    Ogre::Rect brushrect, maprect;
    int ColourMapSize = mColourMapTextureSize->get();
    editpos.x *= (float)ColourMapSize;
    editpos.y = (1.0f - editpos.y) * (float)ColourMapSize;
    if(!_getEditRect(editpos, brushrect, maprect, ColourMapSize))
        return;

    handle->_notifyModification(0, maprect);

    Ogre::Terrain *terrain = static_cast<Ogre::Terrain*>(handle->getHandle());

    float mRatio = (float)BRUSH_DATA_SIZE / (float)mBrushSize;
    float brushPos;
    int mapPos;

    int buffersize = terrain->getGlobalColourMap()->getBuffer()->getSizeInBytes();
    int spacing = buffersize / (ColourMapSize * ColourMapSize);
    unsigned char *data = (unsigned char *)terrain->getGlobalColourMap()->getBuffer()->lock(0,  buffersize, Ogre::HardwareBuffer::HBL_NORMAL);
    Ogre::PixelFormat pf = terrain->getGlobalColourMap()->getBuffer()->getFormat();
    Ogre::ColourValue colVal;

    int right = brushrect.right;
    brushrect.right = BRUSH_DATA_SIZE - brushrect.left;
    brushrect.left = BRUSH_DATA_SIZE - right;

    float factor = std::min(mBrushIntensity * timePassed * 0.2f, 1.0f);
    float bfactor;
    if(!mEditDirection)
    {
        for(int j = maprect.top;j < maprect.bottom;j++)
        {
            brushPos = (brushrect.top + (int)((j - maprect.top) * mRatio)) * BRUSH_DATA_SIZE;
            brushPos += brushrect.right;

            mapPos = (j * ColourMapSize) + maprect.left;

            for(int i = maprect.left;i < maprect.right;i++)
            {
                brushPos -= mRatio;

                assert(mapPos < (ColourMapSize * ColourMapSize) && mapPos >= 0);
                assert((int)brushPos < (BRUSH_DATA_SIZE * BRUSH_DATA_SIZE) && (int)brushPos >= 0);

                bfactor = mBrushData[(int)brushPos] * factor;
                Ogre::PixelUtil::unpackColour(&colVal, pf, (void*)&data[mapPos * spacing]);
                colVal.r = colVal.r + ((mColour.r - colVal.r) * bfactor);
                colVal.g = colVal.g + ((mColour.g - colVal.g) * bfactor);
                colVal.b = colVal.b + ((mColour.b - colVal.b) * bfactor);
                Ogre::PixelUtil::packColour(colVal, pf, (void*)&data[mapPos * spacing]);

                ++mapPos;
            }
        }
    }
    else
    {
        for(int j = maprect.top;j < maprect.bottom;j++)
        {
            brushPos = (brushrect.top + (int)((j - maprect.top) * mRatio)) * BRUSH_DATA_SIZE;
            brushPos += brushrect.right;

            mapPos = (j * ColourMapSize) + maprect.left;

            for(int i = maprect.left;i < maprect.right;i++)
            {
                brushPos -= mRatio;

                assert(mapPos < (ColourMapSize * ColourMapSize) && mapPos >= 0);
                assert((int)brushPos < (BRUSH_DATA_SIZE * BRUSH_DATA_SIZE) && (int)brushPos >= 0);

                bfactor = mBrushData[(int)brushPos] * factor;
                Ogre::PixelUtil::unpackColour(&colVal, pf, (void*)&data[mapPos * spacing]);
                colVal.r = colVal.r + ((1.0f - colVal.r) * bfactor);
                colVal.g = colVal.g + ((1.0f - colVal.g) * bfactor);
                colVal.b = colVal.b + ((1.0f - colVal.b) * bfactor);
                Ogre::PixelUtil::packColour(colVal, pf, (void*)&data[mapPos * spacing]);

                ++mapPos;
            }
        }
    }
    terrain->getGlobalColourMap()->getBuffer()->unlock();
}
//-----------------------------------------------------------------------------------------
void CTerrainGroupEditor::_splatGrass(CTerrainPageEditor *handle, Ogre::Vector3 &editpos, float timePassed)
{
    Ogre::Rect brushrect, maprect;
    int mDensityMapSize = mPGDensityMapSize->get();
    editpos.x *= (float)mDensityMapSize;
    editpos.y = (1.0f - editpos.y) * (float)mDensityMapSize;
    if(!_getEditRect(editpos, brushrect, maprect, mDensityMapSize))
        return;

    int mLayer = -1;

    mLayer = handle->_getGrassLayerID(mTextureGrass, mEditDirection);

    if(mLayer < 0)
        return;

    handle->_notifyModification(-2, maprect);

    float *mDensityMapData = handle->getGrassPointer(mLayer);

    float mRatio = (float)BRUSH_DATA_SIZE / (float)mBrushSize;
    float brushPos;
    int mapPos;

    int right = brushrect.right;
    brushrect.right = BRUSH_DATA_SIZE - brushrect.left;
    brushrect.left = BRUSH_DATA_SIZE - right;

    float factor = mBrushIntensity * timePassed * 5.0f;

    if(!mEditDirection)
    {
        for(int j = maprect.top;j < maprect.bottom;j++)
        {
            brushPos = (brushrect.top + (int)((j - maprect.top) * mRatio)) * BRUSH_DATA_SIZE;
            brushPos += brushrect.right;

            mapPos = (j * mDensityMapSize) + maprect.left;

            for(int i = maprect.left;i < maprect.right;i++)
            {
                brushPos -= mRatio;

                assert(mapPos < (mDensityMapSize * mDensityMapSize) && mapPos >= 0);
                assert((int)brushPos < (BRUSH_DATA_SIZE * BRUSH_DATA_SIZE) && (int)brushPos >= 0);

                float val = mDensityMapData[mapPos] + (mBrushData[(int)brushPos] * factor);
                val = std::min(val, 255.0f);

                mDensityMapData[mapPos] = val;

                ++mapPos;
            }
        }
    }
    else
    {
        for(int j = maprect.top;j < maprect.bottom;j++)
        {
            brushPos = (brushrect.top + (int)((j - maprect.top) * mRatio)) * BRUSH_DATA_SIZE;
            brushPos += brushrect.right;

            mapPos = (j * mDensityMapSize) + maprect.left;

            for(int i = maprect.left;i < maprect.right;i++)
            {
                brushPos -= mRatio;

                assert(mapPos < (mDensityMapSize * mDensityMapSize) && mapPos >= 0);
                assert((int)brushPos < (BRUSH_DATA_SIZE * BRUSH_DATA_SIZE) && (int)brushPos >= 0);

                float val = mDensityMapData[mapPos] - (mBrushData[(int)brushPos] * factor);
                val = std::max(val, 0.0f);

                mDensityMapData[mapPos] = val;

                ++mapPos;
            }
        }
    }

    handle->dirtyGrassRect(maprect);
    handle->updateGrassLayer(mLayer);
}
//-----------------------------------------------------------------------------------------
bool CTerrainGroupEditor::update(float timePassed)
{
    if(mEditActive && mDecalFrustum->getVisible())
    {
        mEditDirection = CViewportEditor::mViewKeyboard[CViewportEditor::mSpecial.SPK_REVERSE_UPDATE];

        Ogre::Vector3 cursorpos = mDecalNode->getPosition();
        Ogre::Terrain *terrain;
        Ogre::Vector3 editpos;

        Ogre::TerrainGroup::TerrainList terrainList;
        Ogre::Real halfBrushSizeWorldSpace = (Ogre::Real)(mWorldSize->get() * mBrushSize) / (Ogre::Real)(mMapSize->get()) / 2.0f;
        Ogre::Sphere sphere(cursorpos, halfBrushSizeWorldSpace);
        mHandle->sphereIntersects(sphere, &terrainList);

        mModificationRect.merge(Ogre::Rect(cursorpos.x - halfBrushSizeWorldSpace, cursorpos.z - halfBrushSizeWorldSpace, cursorpos.x + halfBrushSizeWorldSpace, cursorpos.z + halfBrushSizeWorldSpace));

        bool groupUpdateNeeded = false;

        float avg_total = 0.0f;
        float sample_count_total = 0;

        if(mEditMode == EM_SMOOTH)
        {
            for (Ogre::TerrainGroup::TerrainList::iterator ti = terrainList.begin(); ti != terrainList.end(); ++ti)
            {
                terrain = *ti;
                terrain->getTerrainPosition(cursorpos, &editpos);

                CTerrainPageEditor *terED = 0;

                for(NameObjectPairList::iterator it = mChildren.begin(); it != mChildren.end();it++)
                {
                    if(it->second->getHandle() == (void*)terrain)
                    {
                        terED = static_cast<CTerrainPageEditor*>(it->second);
                        break;
                    }
                }

                float avg = 0.0f;
                int sample_count = 0;
                _calculatesmoothingfactor(terED, editpos, avg, sample_count);

                avg_total += avg;
                sample_count_total += sample_count;
            }
        }

        for (Ogre::TerrainGroup::TerrainList::iterator ti = terrainList.begin(); ti != terrainList.end(); ++ti)
        {
            terrain = *ti;
            terrain->getTerrainPosition(cursorpos, &editpos);

            CTerrainPageEditor *terED = 0;

            for(NameObjectPairList::iterator it = mChildren.begin(); it != mChildren.end();it++)
            {
                if(it->second->getHandle() == (void*)terrain)
                {
                    terED = static_cast<CTerrainPageEditor*>(it->second);
                    break;
                }
            }

            if(mEditMode == EM_DEFORM)
            {
                _deform(terED, editpos, timePassed);
                groupUpdateNeeded |= true;
            }
            else if(mEditMode == EM_SMOOTH)
            {
                if(sample_count_total)
                {
                    _smooth(terED, editpos, avg_total / (float)sample_count_total, timePassed);
                    groupUpdateNeeded |= true;
                }
            }
            else if(mEditMode == EM_SPLAT)
                _splat(terED, editpos, timePassed);
            else if(mEditMode == EM_PAINT)
                _paint(terED, editpos, timePassed);
            else if(mEditMode == EM_SPLATGRASS)
                _splatGrass(terED, editpos, timePassed);
        }

        if(groupUpdateNeeded)
            mHandle->update();

        mOgitorsRoot->SetSceneModified(true);
    }

    if(mPGHandle)
        mPGHandle->update();

    return false;
}
//-----------------------------------------------------------------------------------------
