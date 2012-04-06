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
#include "OgitorsUndoManager.h"
#include "TerrainGroupUndo.h"

using namespace Ogitors;

//-----------------------------------------------------------------------------------------
//----TERRAINEDITOR RELATED EVENTS---------------------------------------------------------
//-----------------------------------------------------------------------------------------
void CTerrainGroupEditor::loadResource(Ogre::Resource* resource)
{
    if(resource->getName() == "OgitorDecalTexture")
        setBrushName(mBrushName);
}
//-----------------------------------------------------------------------------------------
bool CTerrainGroupEditor::isBackgroundProcessActive()
{
    return mHandle->isDerivedDataUpdateInProgress();
}
//-----------------------------------------------------------------------------------------
void CTerrainGroupEditor::setMousePosition(Ogre::Ray camRay)
{
    Ogre::Vector3 pos(999999,-999999,999999);

    Ogre::TerrainGroup::RayResult rayresult = mHandle->rayIntersects(camRay);
    if(rayresult.hit)
    {
        pos = rayresult.position;

        float modulus = 0.0f;
        float offset = 0.5f;
        float fix = 0.5f;

        switch(mEditMode)
        {
        case EM_DEFORM:
        case EM_SMOOTH:modulus = mWorldSize->get() / (float)(mMapSize->get() - 1);if(mBrushSize & 1) offset = 0.0f;break;
        case EM_SPLAT:modulus = mWorldSize->get() / (float)mBlendMapTextureSize->get();if(!(mBrushSize & 1)) offset = 0.0f;fix = 0.0f;break;
        case EM_PAINT:modulus = mWorldSize->get() / (float)mColourMapTextureSize->get();if(!(mBrushSize & 1)) offset = 0.0f;fix = 0.0f;break;
        case EM_SPLATGRASS:modulus = mWorldSize->get() / (float)mPGDensityMapSize->get();if(!(mBrushSize & 1)) offset = 0.0f;fix = 0.0f;break;
        }

        if(modulus != 0.0)
        {
            int XMULT, ZMULT;
            
            if(pos.x > 0)
            {
                XMULT = ((pos.x) / modulus) + fix;
                pos.x = ((float)XMULT + offset) * modulus;
            }
            else
            {
                XMULT = ((pos.x) / modulus) - fix;
                pos.x = ((float)XMULT - offset) * modulus;
            }

            if(pos.z > 0)
            {
                ZMULT = ((pos.z) / modulus) + fix;
                pos.z = ((float)ZMULT + offset) * modulus;
            }
            else
            {
                ZMULT = ((pos.z) / modulus) - fix;
                pos.z = ((float)ZMULT - offset) * modulus;
            }

            pos.y = mHandle->getHeightAtWorldPosition(pos.x, pos.y, pos.z);
        }

        mDecalNode->setPosition(pos);
        mDecalFrustum->setVisible(mEditMode != EM_NONE);

        float value = (float)mBrushSize * modulus;

        mDecalFrustum->setNearClipDistance(value);
        mDecalFrustum->setOrthoWindow(value, value);
    }
    else
    {
        mDecalFrustum->setVisible(false);
    }
}
//-----------------------------------------------------------------------------------------
void CTerrainGroupEditor::setEditMode(unsigned int mode)
{
    if(mModificationRect.height() && mModificationRect.width() && (mEditMode == EM_DEFORM || mEditMode == EM_SMOOTH))
    {
        Ogitors::NameObjectPairList::iterator it;

        for(it = mChildren.begin(); it != mChildren.end();it++)
        {
             CTerrainPageEditor *terrain = static_cast<CTerrainPageEditor*>(it->second);
             terrain->_refreshGrassGeometry(&mModificationRect);
        }
    }

    mModificationRect = Ogre::Rect(0,0,0,0);

    mEditMode = (EditMode)mode;

    mDecalFrustum->setVisible(mEditMode != EM_NONE);

    if(mEditMode == EM_NONE)
        mDecalNode->setPosition(Ogre::Vector3(999999,-999999,999999));
}
//-----------------------------------------------------------------------------------------
void CTerrainGroupEditor::setColour(const Ogre::ColourValue& colour)
{
    mColour = colour;
}
//-----------------------------------------------------------------------------------------
void CTerrainGroupEditor::setBrushName(const std::string& brush)
{
    mBrushName = brush;
    if(!mDecalTexture.isNull())
    {
        Ogre::Image img;
        img.load(brush,"Brushes");

        size_t width  = 256,
               height = 256;

        unsigned char *dataptr = OGRE_ALLOC_T(unsigned char, width * height * 4, Ogre::MEMCATEGORY_GEOMETRY);
        Ogre::PixelBox resultbox(width,height,1,Ogre::PF_A8R8G8B8,dataptr);
        Ogre::Image::scale(img.getPixelBox(), resultbox);

        resultbox.setConsecutive();
        int pos = 0;
        Ogre::ColourValue colval;
        for(unsigned int x = 0;x < width * height;x++)
        {
                dataptr[pos] *= 0.8f;
                dataptr[pos + 1] = 0;
                dataptr[pos + 2] = 0;
                pos += 4;
        }

        mDecalTexture->setHeight(height);
        mDecalTexture->setWidth(width);
        
        /* Width, Height and Format must match when copying into mDecalTexture
           or the decal texture won't show on linux and possibly other platforms. */
        Ogre::HardwarePixelBufferSharedPtr ptr = mDecalTexture->getBuffer();
        ptr->blitFromMemory(resultbox);
        OGRE_FREE(dataptr, Ogre::MEMCATEGORY_GEOMETRY);

        img.resize(mBrushSize, mBrushSize);
        img.resize(BRUSH_DATA_SIZE, BRUSH_DATA_SIZE);
        pos = 0;
        Ogre::ColourValue cval;
        for(unsigned int y = 0;y < BRUSH_DATA_SIZE ;y++)
        {
            pos = ((BRUSH_DATA_SIZE - 1) - y) * BRUSH_DATA_SIZE;
            for(unsigned int x = 0;x < BRUSH_DATA_SIZE ;x++)
            {
                cval = img.getColourAt(x,y,0);
                mBrushData[pos] = cval.r;
                pos++;
            }
        }
    }
}
//-----------------------------------------------------------------------------------------
void CTerrainGroupEditor::setBrushSize(unsigned int size)
{
    if(mDecalNode)
    {
        Ogre::Real ratio = 1.0f;
        if(mBrushSize > 0)
        {
            ratio = mDecalFrustum->getNearClipDistance() / (float)mBrushSize;
        }
        mBrushSize = size;
        ratio *= (float)size;
        mDecalFrustum->setNearClipDistance(ratio);
        mDecalFrustum->setOrthoWindow(ratio, ratio);
        
        //setBrushName(mBrushName);
    }
}
//-----------------------------------------------------------------------------------------
void CTerrainGroupEditor::setBrushIntensity(float intensity) 
{
    mBrushIntensity = intensity;
}
//-----------------------------------------------------------------------------------------
void CTerrainGroupEditor::startEdit()
{
    if(!mEditActive)
    {
        mEditActive = true;
        mModificationRect = Ogre::Rect(0,0,0,0);

        if(mEditMode >= EM_DEFORM && mEditMode <= EM_SPLATGRASS)
        {
            Ogre::String desc;
            switch(mEditMode)
            {
            case EM_DEFORM:desc = "Terrain DEFORM";break;
            case EM_SMOOTH:desc = "Terrain SMOOTH";break;
            case EM_SPLAT:desc = "Terrain SPLAT";break;
            case EM_PAINT:desc = "Terrain PAINT";break;
            case EM_SPLATGRASS:desc = "Terrain SPLAT GRASS";break;
            }

            OgitorsUndoManager::getSingletonPtr()->BeginCollection(desc);
        }
    }
}
//-----------------------------------------------------------------------------------------
void CTerrainGroupEditor::stopEdit()
{
    if(mEditActive)
    {
        mEditActive = false;
    
        Ogitors::NameObjectPairList::iterator it;

        if(mModificationRect.height() && mModificationRect.width() && (mEditMode == EM_DEFORM || mEditMode == EM_SMOOTH))
        {
            for(it = mChildren.begin(); it != mChildren.end();it++)
            {
                 CTerrainPageEditor *terrain = static_cast<CTerrainPageEditor*>(it->second);
                 terrain->_refreshGrassGeometry(&mModificationRect);
            }
        }

        for(it = mChildren.begin(); it != mChildren.end();it++)
        {
            CTerrainPageEditor *terrain = static_cast<CTerrainPageEditor*>(it->second);
            terrain->_notifyEndModification();
        }

        mModificationRect = Ogre::Rect(0,0,0,0);

        if(mEditMode >= EM_DEFORM && mEditMode <= EM_SPLATGRASS)
        {
            OgitorsUndoManager::getSingletonPtr()->EndCollection(true);
        }

        if(mHandle)
            mHandle->freeTemporaryResources();
    }
}
//-----------------------------------------------------------------------------------------
void CTerrainGroupEditor::setTexture(const Ogre::String& texture)
{
    mTextureDiffuse = texture;
    mTextureNormal = "";
    
    int pos = mTextureDiffuse.find(";");
    if(pos != -1)
    {
        mTextureNormal = mTextureDiffuse.substr(pos + 1,mTextureDiffuse.length() - pos + 1);
        mTextureDiffuse = mTextureDiffuse.erase(pos,mTextureDiffuse.length() - pos);
    }
}
//-----------------------------------------------------------------------------------------
void CTerrainGroupEditor::setGrassTexture(const Ogre::String& texture)
{
    mTextureGrass = texture;
}
//-----------------------------------------------------------------------------------------
bool CTerrainGroupEditor::hitTest(Ogre::Ray camRay, Ogre::Vector3 *result)
{
    Ogre::TerrainGroup::RayResult rayresult = mHandle->rayIntersects(camRay);
    if(rayresult.hit)
    {
        if(result != 0)
            *result = rayresult.position;
        return true;
    }
    else
        return false;
}
//-----------------------------------------------------------------------------------------
Ogre::StringVector CTerrainGroupEditor::getMaterialNames() 
{
    Ogre::StringVector list;
    Ogre::String matName;

    if(!mHandle) 
        return list;

    Ogre::TerrainGroup::TerrainIterator it = mHandle->getTerrainIterator();
    
    while(it.hasMoreElements())
    {
        Ogre::TerrainGroup::TerrainSlot *terrain = it.getNext();
        if(terrain->instance && terrain->instance->isLoaded())
        {
            matName = terrain->instance->getMaterialName();
            if(matName != "")
                list.push_back(matName);
        }

    }

    return list;
}
//-----------------------------------------------------------------------------------------
void CTerrainGroupEditor::recalculateLighting()
{
    Ogre::TerrainGroup::TerrainIterator it = mHandle->getTerrainIterator();
    
    while(it.hasMoreElements())
    {
        Ogre::TerrainGroup::TerrainSlot *terrain = it.getNext();
        if(terrain->instance)
            terrain->instance->dirtyLightmap();
    }

    mHandle->update();

    mOgitorsRoot->SetSceneModified(true);
}
//-----------------------------------------------------------------------------------------
void CTerrainGroupEditor::_modifyHeights(float scale, float offset)
{
    NameObjectPairList::iterator it = mChildren.begin();

    OgitorsUndoManager::getSingletonPtr()->BeginCollection("Modify All Height Values");

    while(it != mChildren.end())
    {
        CTerrainPageEditor *ed = static_cast<CTerrainPageEditor*>(it->second);
        ed->_modifyHeights(scale, offset);
        it++;
    }

    mOgitorsRoot->SetSceneModified(true);

    OgitorsUndoManager::getSingletonPtr()->EndCollection(true);
}
//-----------------------------------------------------------------------------------------
