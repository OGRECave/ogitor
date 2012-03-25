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
#include "PagingEditor.h"
#include "tinyxml.h"
#include "OgreStreamSerialiser.h"
#include "OgreDeflate.h"
#include "OgitorsUndoManager.h"
#include "TerrainGroupUndo.h"
#include "OFSDataStream.h"

#include "PagedGeometry.h"
#include "GrassLoader.h"

using namespace Forests;
using namespace Ogitors;

PropertyOptionsVector CTerrainPageEditorFactory::mFadeTechniques;    /** List of fade techniques */
PropertyOptionsVector CTerrainPageEditorFactory::mGrassTechniques;    /** List of grass techniques */

//-----------------------------------------------------------------------------------------
CTerrainPageEditor::CTerrainPageEditor(CBaseEditorFactory *factory) : CBaseEditor(factory),
mPageX(0), mPageY(0), mFirstTimeInit(false), mExternalDataHandle(0), mPGModified(false)
{
    mHandle = 0;
    mPGLayers[0] = 0;
    mPGLayers[1] = 0;
    mPGLayers[2] = 0;
    mPGLayers[3] = 0;
    mPGLayerData[0] = 0;
    mPGLayerData[1] = 0;
    mPGLayerData[2] = 0;
    mPGLayerData[3] = 0;
    mPGDirtyRect.setNull();

    for(int i = 0;i < 16;i++)
    {
        mLayerWorldSize[i] = 0;
        mLayerDiffuse[i] = 0;
        mLayerNormal[i] = 0;
    }

    mHeightDirtyRect = Ogre::Rect(0,0,0,0);
    mBlendMapDirtyRect = Ogre::Rect(0,0,0,0);
    mColourMapDirtyRect = Ogre::Rect(0,0,0,0);
    mGrassMapDirtyRect = Ogre::Rect(0,0,0,0);
    mHeightSave = 0;
    mBlendSave = 0;
    mColourSave = 0;
    mGrassSave = 0;
    mBlendSaveStart = 1;
    mBlendSaveCount = 0;

    mUsesGizmos = false;
    mUsesHelper = false;

    mTempFileName = "";
    mTempDensityFileName = "";
}
//-----------------------------------------------------------------------------------------
CTerrainPageEditor::~CTerrainPageEditor()
{
    if(getParent())
    {
        static_cast<CTerrainGroupEditor*>(getParent())->removePage(this);
    }
}
//-----------------------------------------------------------------------------------------
void CTerrainPageEditor::_saveTerrain(Ogre::String pathPrefix)
{
    Ogre::TerrainGroup *terGroup = static_cast<Ogre::TerrainGroup*>(mParentEditor->get()->getHandle());
    Ogre::String filename = pathPrefix + terGroup->generateFilename(mPageX->get(), mPageY->get());

    if(pathPrefix == "/Temp/tmp")
    {
        mTempFileName = pathPrefix + Ogre::StringConverter::toString(mObjectID->get()) + ".ogt";
        filename = mTempFileName;
    }

    OFS::OFSHANDLE *fileHandle = new OFS::OFSHANDLE();

    mOgitorsRoot->GetProjectFile()->createFile(*fileHandle, filename.c_str());

    Ogre::DataStreamPtr stream = Ogre::DataStreamPtr(OGRE_NEW OfsDataStream(mOgitorsRoot->GetProjectFile(), fileHandle));
    Ogre::DataStreamPtr compressStream(OGRE_NEW Ogre::DeflateStream(filename, stream));
    Ogre::StreamSerialiser ser(compressStream);
    mHandle->save(ser);
}
//-----------------------------------------------------------------------------------------
void CTerrainPageEditor::onSave(bool forced)
{
    if(mHandle)
    {
        if(mHandle->isModified() || mTempModified->get() || forced)
        {
            mOgitorsRoot->GetProjectFile()->deleteFile(mTempFileName.c_str());
            _saveTerrain("/Terrain/");
        }
    }
    else if(mTempModified->get())
    {
        Ogre::TerrainGroup *terGroup = static_cast<Ogre::TerrainGroup*>(mParentEditor->get()->getHandle());
        Ogre::String filename = terGroup->generateFilename(mPageX->get(), mPageY->get());

        Ogre::String pathFrom = mTempFileName;
        Ogre::String pathTo = "/Terrain/" + filename;

        mOgitorsRoot->GetProjectFile()->moveFile(pathFrom.c_str(), pathTo.c_str());
    }

    if(mLoaded->get())
    {
        if(mPGModified || mTempDensityModified->get() || forced)
        {
            mOgitorsRoot->GetProjectFile()->deleteFile(mTempDensityFileName.c_str());
            _saveGrass("/Terrain/");
        }
    }
    else if(mTempDensityModified->get())
    {
        Ogre::TerrainGroup *terGroup = static_cast<Ogre::TerrainGroup*>(mParentEditor->get()->getHandle());
        Ogre::String filename = terGroup->generateFilename(mPageX->get(), mPageY->get());

        Ogre::String pathFrom = mTempDensityFileName;
        Ogre::String pathTo = "/Terrain/" + filename.substr(0, filename.size() - 4) + "_density.tga";

        mOgitorsRoot->GetProjectFile()->moveFile(pathFrom.c_str(), pathTo.c_str());
    }

    mTempModified->set(false);
    mTempDensityModified->set(false);
}
//-----------------------------------------------------------------------------------------
Ogre::AxisAlignedBox CTerrainPageEditor::getAABB()
{
    if(mHandle)
        return mHandle->getWorldAABB();
    else
        return Ogre::AxisAlignedBox::BOX_NULL;
}
//-----------------------------------------------------------------------------------------
bool CTerrainPageEditor::getObjectContextMenu(UTFStringVector &menuitems)
{
    menuitems.clear();
    menuitems.push_back(OTR("Remove Page"));
    if(mLoaded->get())
    {
        menuitems.push_back(OTR("Re-Light"));
        menuitems.push_back(OTR("Calculate Blendmap"));
        menuitems.push_back(OTR("Scale/Offset Height Values"));
        menuitems.push_back("-");
        menuitems.push_back(OTR("Import Heightmap"));
        menuitems.push_back(OTR("Export Heightmap"));
        menuitems.push_back(OTR("Export Compositemap"));
        menuitems.push_back("-");
        menuitems.push_back(OTR("Import Blendmaps") +  Ogre::UTFString(" (RGB+A)"));

        for(int i = 1; i < mLayerCount->get();i++)
        {
            Ogre::UTFString prefix = "Layer ";
            prefix = prefix + Ogre::StringConverter::toString(i);
            prefix = prefix + ": ";
            menuitems.push_back(prefix  + OTR("Import Blendmap") +  Ogre::UTFString(" (R)"));
        }
    }

    return true;
}
//-------------------------------------------------------------------------------
void CTerrainPageEditor::onObjectContextMenu(int menuresult)
{
    if(menuresult == 0)
    {
        Ogre::UTFString msgStr = OTR("Do you want to remove %s?");
        int pos = msgStr.find("%s");

        if(pos != -1)
        {
            msgStr.erase(pos,2);
            msgStr.insert(pos, mName->get());
        }

        if(mSystem->DisplayMessageDialog(msgStr, DLGTYPE_YESNO) == DLGRET_YES)
        {
            mOgitorsRoot->DestroyEditorObject(this,true,true);
        }
    }
    else if(menuresult == 1)
    {
        if(!mHandle || !mHandle->isLoaded())
            return;

        mHandle->dirtyLightmap();
        mHandle->update(true);
        mHandle->updateCompositeMap();
    }
    else if(menuresult == 2)
    {
        if(!mHandle || !mHandle->isLoaded())
            return;

        calculateBlendMap();
    }
    else if(menuresult == 3)
    {
        Ogre::NameValuePairList params;

        params["title"] = "Scale/Offset values";
        params["input1"] = "Scale";
        params["input2"] = "Offset";

        if(!mSystem->DisplayImportHeightMapDialog(params))
            return;

        Ogre::Real fScale = Ogre::StringConverter::parseReal(params["input1"]);
        Ogre::Real fOffset = Ogre::StringConverter::parseReal(params["input2"]);

        _modifyHeights(fScale, fOffset);
    }
    else if(menuresult == 4)
    {
        importHeightMap();
    }
    else if(menuresult == 5)
    {
        exportHeightMap();
    }
    else if(menuresult == 6)
    {
        exportCompositeMap();
    }
    else if(menuresult == 7)
    {
        importBlendMap();
    }
    else if(menuresult > 7)
    {
        int lyID = menuresult - 7;
        importBlendMap(lyID);
    }
}
//-----------------------------------------------------------------------------------------
void CTerrainPageEditor::setSelectedImpl(bool bSelected)
{
    CBaseEditor::setSelectedImpl(bSelected);

    getSceneManager()->getSceneNode("OgitorTerrainDecalNode")->setPosition(Ogre::Vector3(999999,-999999,999999));
}
//-----------------------------------------------------------------------------------------
Ogre::Real CTerrainPageEditor::hitTest(Ogre::Ray& ray, Ogre::Vector3& retPos)
{
    if(!mHandle)
        return -1;

    std::pair<bool,Ogre::Vector3> result = mHandle->rayIntersects(ray);
    if(result.first)
    {
        retPos = result.second;
        return (result.second - ray.getOrigin()).length();
    }
    else
        return -1;
}
//-----------------------------------------------------------------------------------------
bool CTerrainPageEditor::setLayerImpl(unsigned int newlayer)
{
    if(mHandle)
        mHandle->setVisibilityFlags(1 << newlayer);

    return true;
}//-----------------------------------------------------------------------------------------
bool CTerrainPageEditor::_setPosition(OgitorsPropertyBase* property, const Ogre::Vector3& position)
{
    if(mHandle)
    {
        mHandle->setPosition(position);
    }

    if(mOgitorsRoot->GetPagingEditor())
        mOgitorsRoot->GetPagingEditor()->updateObjectPage(this);

    return true;
}
//-----------------------------------------------------------------------------------------
bool CTerrainPageEditor::_setLayerWorldSize(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(mHandle)
    {
        mHandle->setLayerWorldSize(property->getTag(), value);
        return true;
    }
    return false;
}
//-----------------------------------------------------------------------------------------
bool CTerrainPageEditor::_setLayerDiffuseMap(OgitorsPropertyBase* property, const Ogre::String& value)
{
    if(mHandle)
    {
        mHandle->setLayerTextureName(property->getTag(), 0, value);
        return true;
    }
    return false;
}
//-----------------------------------------------------------------------------------------
bool CTerrainPageEditor::_setLayerNormalMap(OgitorsPropertyBase* property, const Ogre::String& value)
{
    if(mHandle)
    {
        mHandle->setLayerTextureName(property->getTag(), 1, value);
        return true;
    }
    return false;
}
//-----------------------------------------------------------------------------------------
bool CTerrainPageEditor::_setColourMapEnabled(OgitorsPropertyBase* property, const bool& value)
{
    if(mHandle)
    {
        bool resetwhite= false;

        mHandle->setGlobalColourMapEnabled(value, (Ogre::uint16)mColourMapTextureSize->get());

        if(value)
        {
            Ogre::Rect modrect(0,0, mColourMapTextureSize->get(), mColourMapTextureSize->get());

            _notifyModification(0, modrect);
            Ogre::ColourValue *data = OGRE_ALLOC_T(Ogre::ColourValue, mColourMapTextureSize->get() * mColourMapTextureSize->get(), Ogre::MEMCATEGORY_RESOURCE);

            for(int i = 0;i < mColourMapTextureSize->get() * mColourMapTextureSize->get();i++)
                data[i] = Ogre::ColourValue(1,1,1,1);

            this->_swapColours(modrect, data);

            OGRE_FREE(data, Ogre::MEMCATEGORY_RESOURCE);
            _notifyEndModification();
        }
    }

    return true;
}
//-----------------------------------------------------------------------------------------
bool CTerrainPageEditor::_setColourMapTextureSize(OgitorsPropertyBase* property, const int& value)
{
    if(!mColourMapEnabled->get())
        return true;

    if(mHandle)
    {
        Ogre::Image img;
        mHandle->getGlobalColourMap()->convertToImage(img);
        mHandle->getGlobalColourMap()->unload();
        img.resize(value, value, Ogre::Image::FILTER_BILINEAR);
        mHandle->getGlobalColourMap()->loadImage(img);
    }

    return true;
}
//-----------------------------------------------------------------------------------------
bool CTerrainPageEditor::setNameImpl(Ogre::String name)
{
    mSystem->SetTreeItemText(this, name);

    destroyBoundingBox();

    return true;
}
//-----------------------------------------------------------------------------------------
static Ogre::String dummyString = "";

const Ogre::String& CTerrainPageEditor::getMaterialName()
{
    if(mHandle)
        return mHandle->getMaterialName();
    else
        return dummyString;
}
//-----------------------------------------------------------------------------------------
int CTerrainPageEditor::_getLayerID(Ogre::String& texture, Ogre::String& normal, bool dontcreate)
{
    Ogre::TerrainLayerSamplerList list = mHandle->getLayerDeclaration().samplers;
    int id = -1;
    for(unsigned int samplerid = 0;samplerid < list.size();samplerid++)
    {
        if(list[samplerid].alias == "albedo_specular")
        {
            id = samplerid;
            break;
        }
    }
    if(id == -1)
        return -1;

    int layerID = -1;
    unsigned int count = mHandle->getLayerCount();
    for(unsigned int i = 0;i < count;i++)
    {
        if(mHandle->getLayerTextureName(i,id) == texture)
        {
            layerID = i;
            break;
        }
    }
    if(layerID == -1 && !dontcreate)
    {
        layerID = _createNewLayer(texture, normal);
    }
    return layerID;
}
//-----------------------------------------------------------------------------------------
int CTerrainPageEditor::_getEmptyLayer()
{
    bool isFull;
    unsigned int mBlendMapArea = mHandle->getLayerBlendMapSize() * mHandle->getLayerBlendMapSize();
    for(int i = 1;i < mLayerCount->get();i++)
    {
        float *ptr = mHandle->getLayerBlendMap(i)->getBlendPointer();
        isFull = false;
        for(unsigned int j = 0;j < mBlendMapArea;j++)
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
    return -1;
}
//-----------------------------------------------------------------------------------------
int CTerrainPageEditor::_createNewLayer(Ogre::String &texture,  Ogre::String& normal, Ogre::Real worldSize, bool donotuseempty)
{
    int layerID = -1;

    if(!donotuseempty)
    {
        layerID = _getEmptyLayer();

        if(layerID != -1)
        {
            _changeLayer(layerID, texture, normal, worldSize);

            return layerID;
        }
    }

    CTerrainGroupEditor *parentEditor = static_cast<CTerrainGroupEditor*>(mParentEditor->get());

    if(mLayerCount->get() == parentEditor->getMaxLayersAllowed())
        return -1;

    layerID = mLayerCount->get();

    _createLayer(layerID, texture, normal, worldSize);

    return layerID;
}
//-----------------------------------------------------------------------------------------
void CTerrainPageEditor::_createLayer(int layerID, Ogre::String &texture,  Ogre::String& normal, Ogre::Real worldSize)
{
    OgitorsUndoManager::getSingletonPtr()->AddUndo(OGRE_NEW TerrainLayerUndo(mObjectID->get(), layerID, TerrainLayerUndo::LU_CREATE, texture, normal, worldSize));

    OgitorsUndoManager::getSingletonPtr()->BeginCollection("NULL");

    bool unload = !mLoaded->get();
    load(false);

    assert(mHandle != 0);

    Ogre::StringVector vTextures;
    vTextures.push_back(texture);
    vTextures.push_back(normal);
    for(unsigned int ll = 2;ll < mHandle->getLayerDeclaration().samplers.size();ll++)
        vTextures.push_back("");

    mHandle->addLayer(layerID, worldSize, &vTextures);

    Ogre::String sCount2 = "layer" + Ogre::StringConverter::toString(mLayerCount->get());

    PROPERTY_PTR(mLayerWorldSize[mLayerCount->get()], sCount2 + "::worldsize", Ogre::Real, worldSize, mLayerCount->get(), SETTER(Ogre::Real, CTerrainPageEditor, _setLayerWorldSize));
    PROPERTY_PTR(mLayerDiffuse[mLayerCount->get()], sCount2 + "::diffusespecular", Ogre::String, "", mLayerCount->get(), SETTER(Ogre::String, CTerrainPageEditor, _setLayerDiffuseMap));
    PROPERTY_PTR(mLayerNormal[mLayerCount->get()], sCount2 + "::normalheight", Ogre::String, "", mLayerCount->get(), SETTER(Ogre::String, CTerrainPageEditor, _setLayerNormalMap));

    for(int i = mLayerCount->get();i > layerID;i--)
    {
        mLayerWorldSize[i]->initAndSignal(mLayerWorldSize[i - 1]->get());
        mLayerDiffuse[i]->initAndSignal(mLayerDiffuse[i - 1]->get());
        mLayerNormal[i]->initAndSignal(mLayerNormal[i - 1]->get());
    }

    mLayerWorldSize[layerID]->initAndSignal(worldSize);
    mLayerDiffuse[layerID]->initAndSignal(texture);
    mLayerNormal[layerID]->initAndSignal(normal);

    if(unload)
        unLoad();

    mLayerCount->set(mLayerCount->get() + 1);

    OgitorsUndoManager::getSingletonPtr()->EndCollection(false, true);
}
//-----------------------------------------------------------------------------------------
void CTerrainPageEditor::_changeLayer(int layerID, Ogre::String &texture,  Ogre::String& normal, Ogre::Real worldSize)
{
    assert(layerID < mLayerCount->get());

    OgitorsUndoManager::getSingletonPtr()->BeginCollection("NULL");

    bool unload = !mLoaded->get();
    load(false);

    assert(mHandle != 0);

    TerrainLayerUndo *undo = OGRE_NEW TerrainLayerUndo(mObjectID->get(), layerID, TerrainLayerUndo::LU_MODIFY, mHandle->getLayerTextureName(layerID, 0), mHandle->getLayerTextureName(layerID, 1), mHandle->getLayerWorldSize(layerID));

    mHandle->setLayerTextureName(layerID, 0, texture);
    mHandle->setLayerTextureName(layerID, 1, normal);
    mHandle->setLayerWorldSize(layerID, worldSize);

    mLayerWorldSize[layerID]->initAndSignal(worldSize);
    mLayerDiffuse[layerID]->initAndSignal(texture);
    mLayerNormal[layerID]->initAndSignal(normal);

    if(unload)
        unLoad();

    OgitorsUndoManager::getSingletonPtr()->EndCollection(false, true);

    OgitorsUndoManager::getSingletonPtr()->AddUndo(undo);
}
//-----------------------------------------------------------------------------------------
void  CTerrainPageEditor::_deleteLayer(int layerID)
{
    assert(layerID != 0 && layerID < mLayerCount->get());

    OgitorsUndoManager::getSingletonPtr()->BeginCollection("NULL");

    bool unload = !mLoaded->get();
    load(false);

    assert(mHandle != 0);

    mLayerCount->set(mLayerCount->get() - 1);

    TerrainLayerUndo *undo = OGRE_NEW TerrainLayerUndo(mObjectID->get(), layerID, TerrainLayerUndo::LU_DELETE, mHandle->getLayerTextureName(layerID, 0), mHandle->getLayerTextureName(layerID, 1), mHandle->getLayerWorldSize(layerID));

    mHandle->removeLayer(layerID);

    for(int i = layerID + 1;i <= mLayerCount->get();i++)
    {
        mLayerWorldSize[i - 1]->initAndSignal(mLayerWorldSize[i]->get());
        mLayerDiffuse[i - 1]->initAndSignal(mLayerDiffuse[i]->get());
        mLayerNormal[i - 1]->initAndSignal(mLayerNormal[i]->get());
    }

    mProperties.removeProperty(mLayerWorldSize[mLayerCount->get()]);
    mLayerWorldSize[mLayerCount->get()] = 0;
    mProperties.removeProperty(mLayerDiffuse[mLayerCount->get()]);
    mLayerDiffuse[mLayerCount->get()] = 0;
    mProperties.removeProperty(mLayerNormal[mLayerCount->get()]);
    mLayerNormal[mLayerCount->get()] = 0;

    if(unload)
        unLoad();

    OgitorsUndoManager::getSingletonPtr()->EndCollection(false, true);

    OgitorsUndoManager::getSingletonPtr()->AddUndo(undo);
}
//-----------------------------------------------------------------------------------------
bool CTerrainPageEditor::_setMinBatchSize(OgitorsPropertyBase* property, const int& value)
{
    bool loaded = mLoaded->get();

    unLoad();

    if(loaded)
        load();
    return true;
}
//-----------------------------------------------------------------------------------------
bool CTerrainPageEditor::_setMaxBatchSize(OgitorsPropertyBase* property, const int& value)
{
    bool loaded = mLoaded->get();

    unLoad();

    if(loaded)
        load();
    return true;
}
//-----------------------------------------------------------------------------------------
void CTerrainPageEditor::_checkTerrainSizes()
{
    CTerrainGroupEditor *parentEditor = static_cast<CTerrainGroupEditor*>(mParentEditor->get());

    int mapsize = parentEditor->getMapSize();
    Ogre::Real worldsize = parentEditor->getWorldSize();

    if(mHandle->getWorldSize() != worldsize)
    {
        mHandle->setWorldSize(worldsize);
        Ogre::Vector3 newpos;
        static_cast<Ogre::TerrainGroup*>(parentEditor->getHandle())->convertTerrainSlotToWorldPosition(mPageX->get(), mPageY->get(), &newpos);
        mPosition->set(newpos);
    }

    if(mHandle->getSize() != mapsize)
    {
        mHandle->setSize(mapsize);
    }
}
//-----------------------------------------------------------------------------------------
bool CTerrainPageEditor::update(float timePassed)
{
    if(!mHandle)
    {
        unRegisterForUpdates();
        return false;
    }

    if(mHandle->isLoaded())
    {
        _checkTerrainSizes();

        _loadGrassLayers();

        mHandle->setVisibilityFlags(1 << mLayer->get());

        unRegisterForUpdates();
    }

    return false;
}
//-----------------------------------------------------------------------------------------
bool CTerrainPageEditor::load(bool async)
{
    if(mLoaded->get())
        return true;

    if(!getParent()->load())
        return false;

    Ogre::TerrainGroup *terGroup = static_cast<Ogre::TerrainGroup*>(mParentEditor->get()->getHandle());
    CTerrainGroupEditor *parentEditor = static_cast<CTerrainGroupEditor*>(mParentEditor->get());

    if(mFirstTimeInit)
    {
        Ogre::Terrain::ImportData imp;
        imp.pos = mPosition->get();
        imp.inputImage = 0;
        if(mExternalDataHandle != 0)
            imp.inputFloat = mExternalDataHandle;
        else
            imp.inputFloat = 0;

        imp.constantHeight = 0.0f;
        imp.terrainSize = parentEditor->getMapSize();
        imp.worldSize = terGroup->getTerrainWorldSize();
        imp.inputScale = 1.0f;
        imp.minBatchSize = mMinBatchSize->get();
        imp.maxBatchSize = mMaxBatchSize->get();
        // textures
        imp.layerList.resize(mLayerCount->get());
        for(int i = 0;i < mLayerCount->get();i++)
        {
            Ogre::String texturename;

            imp.layerList[i].worldSize = mLayerWorldSize[i]->get();
            texturename = mLayerDiffuse[i]->get();
            imp.layerList[i].textureNames.push_back(texturename);
            texturename = mLayerNormal[i]->get();
            imp.layerList[i].textureNames.push_back(texturename);
        }

        terGroup->defineTerrain(mPageX->get(), mPageY->get(), &imp);
    }
    else
    {
        if(mTempModified->get())
        {
            if(mTempFileName.empty())
            {
                mTempFileName = mOgitorsRoot->GetProjectOptions()->ProjectDir + "/Temp/tmp" + Ogre::StringConverter::toString(mObjectID->get()) + ".ogt";
                mTempFileName = OgitorsUtils::QualifyPath(mTempFileName);
            }

            terGroup->defineTerrain(mPageX->get(), mPageY->get(), mTempFileName);
        }
        else
            terGroup->defineTerrain(mPageX->get(), mPageY->get());
    }

    try
    {
        terGroup->loadTerrain(mPageX->get(), mPageY->get(), mFirstTimeInit || !async);
        mHandle = terGroup->getTerrain(mPageX->get(), mPageY->get());
    }
    catch(...)
    {
    }

    if(!mHandle)
    {
        Ogre::UTFString msg = "Failed to load page: ";
        msg = msg + terGroup->generateFilename(mPageX->get(), mPageY->get());
        mSystem->DisplayMessageDialog(msg, DLGTYPE_OK);

        return false;
    }

    if(mFirstTimeInit)
    {
        mFirstTimeInit = false;

        mTempModified->set(true);
        mTempDensityModified->set(true);

        int densize = parentEditor->getGrassDensityMapSize();
        Ogre::uchar *data = OGRE_ALLOC_T(Ogre::uchar, densize * densize * 4, Ogre::MEMCATEGORY_GENERAL);
        memset(data, 0, densize * densize * 4);

        mPGDensityMap.loadDynamicImage(data, densize, densize, 1, Ogre::PF_A8R8G8B8, true);

        _saveTerrain("/Temp/tmp");
        _saveGrass("/Temp/tmp");
    }

    registerForUpdates();

    mLoaded->set(true);
    return true;
}
//-----------------------------------------------------------------------------------------
TiXmlElement* CTerrainPageEditor::exportDotScene(TiXmlElement *pParent)
{
    Ogre::TerrainGroup *terGroup = static_cast<Ogre::TerrainGroup*>(mParentEditor->get()->getHandle());
    Ogre::String name = terGroup->generateFilename(mPageX->get(), mPageY->get());

    TiXmlElement *pTerrainPage = pParent->InsertEndChild(TiXmlElement("terrainPage"))->ToElement();
    pTerrainPage->SetAttribute("name", name.c_str());
    pTerrainPage->SetAttribute("pageX", Ogre::StringConverter::toString(mPageX->get()).c_str());
    pTerrainPage->SetAttribute("pageY", Ogre::StringConverter::toString(mPageY->get()).c_str());

    CTerrainGroupEditor *parentEditor = static_cast<CTerrainGroupEditor*>(mParentEditor->get());
    pTerrainPage->SetAttribute("pagedGeometryPageSize", Ogre::StringConverter::toString(static_cast<OgitorsProperty<int>*>(parentEditor->getProperties()->getProperty("pg::pagesize"))->get()).c_str());
    pTerrainPage->SetAttribute("pagedGeometryDetailDistance", Ogre::StringConverter::toString(static_cast<OgitorsProperty<int>*>(parentEditor->getProperties()->getProperty("pg::detaildistance"))->get()).c_str());

    // export grass layer info only if we have grass layer(s). 
    // A grass layer can be considered existing if it has material
    int idx = 0;
    while(idx < 4 && (mPGMaterial[idx]->get() == "")) idx++;

    if(idx < 4)
    {
        Ogre::String tempStr;
        Ogre::String denmapname = name.substr(0, name.size() - 4) + "_density.tga";
        TiXmlElement *pGrassPage = pTerrainPage->InsertEndChild(TiXmlElement("grassLayers"))->ToElement();
        TiXmlElement *pGrassLayers;
        TiXmlElement *pGrassLayerProps;

        // density map, visibility flag
        pGrassPage->SetAttribute("densityMap", denmapname.c_str());
        idx = 0;
        while(idx < 4 && !mPGActive[idx]->get()) idx++;
        if(idx < 4){
            pGrassPage->SetAttribute("visibilityFlags", Ogre::StringConverter::toString(mPGLayers[idx]->getParentGrassLoader()->getVisibilityFlags()).c_str());
        }else{
            //0xFFFFFFFF == 4294967295
            pGrassPage->SetAttribute("visibilityFlags", Ogre::StringConverter::toString(UINT_MAX).c_str());
        }
        for(int i = 0; i < 4; i++) /* PGLayers array num == 4 */
        {
            if(mPGMaterial[i]->get() != "")
            {
                /* grass layer header with id */
                pGrassLayers = pGrassPage->InsertEndChild(TiXmlElement("grassLayer"))->ToElement();
                pGrassLayers->SetAttribute("id", Ogre::StringConverter::toString(i).c_str());
                if(mPGActive[i]->get()) tempStr = "true"; else tempStr = "false";
                pGrassLayers->SetAttribute("enabled", tempStr.c_str());
                // material, maxSlope, lighting
                pGrassLayers->SetAttribute("material", mPGMaterial[i]->get().c_str());
                if(!mPGLayers[i]){
                    pGrassLayers->SetAttribute("maxSlope", "1000");
                    tempStr = "false";
                }else{ 
                    pGrassLayers->SetAttribute("maxSlope", Ogre::StringConverter::toString(mPGLayers[i]->getMaxSlope()).c_str());
                    if(mPGLayers[i]->getLightingEnabled()) tempStr = "true"; else tempStr = "false";
                }
                pGrassLayers->SetAttribute("lighting", tempStr.c_str());

                // get the density map channel
                MapChannel mapCh;
                if(!mPGLayers[i]){
                    mapCh = MapChannel(CHANNEL_RED + i);
                }else{
                    mapCh = mPGLayers[i]->getDensityMapChannel();
                }
                switch(mapCh){
                    case CHANNEL_ALPHA: tempStr = "ALPHA"; break;
                    case CHANNEL_BLUE:  tempStr = "BLUE";  break;
                    case CHANNEL_COLOR: tempStr = "COLOR"; break;
                    case CHANNEL_GREEN: tempStr = "GREEN"; break;
                    case CHANNEL_RED:   tempStr = "RED";   break;
                }
                
                // density map channel
                pGrassLayerProps = pGrassLayers->InsertEndChild(TiXmlElement("densityMapProps"))->ToElement();
                pGrassLayerProps->SetAttribute("channel", tempStr.c_str());
                pGrassLayerProps->SetAttribute("density", Ogre::StringConverter::toString(mPGDensity[i]->get()).c_str());

                // map bounds
                TBounds mapBounds;
                if(!mPGLayers[i]){
                    Ogre::AxisAlignedBox bBox = mHandle->getWorldAABB();
                    mapBounds.left  = bBox.getMinimum().x; mapBounds.top    = bBox.getMinimum().z;
                    mapBounds.right = bBox.getMaximum().x; mapBounds.bottom = bBox.getMaximum().z;
                }else{
                    mapBounds = mPGLayers[i]->getMapBounds();
                }
                pGrassLayerProps = pGrassLayers->InsertEndChild(TiXmlElement("mapBounds"))->ToElement();
                pGrassLayerProps->SetAttribute("top", Ogre::StringConverter::toString(mapBounds.top).c_str());
                pGrassLayerProps->SetAttribute("bottom", Ogre::StringConverter::toString(mapBounds.bottom).c_str());
                pGrassLayerProps->SetAttribute("left", Ogre::StringConverter::toString(mapBounds.left).c_str());
                pGrassLayerProps->SetAttribute("right", Ogre::StringConverter::toString(mapBounds.right).c_str());

                // sizes
                pGrassLayerProps = pGrassLayers->InsertEndChild(TiXmlElement("grassSizes"))->ToElement();
                pGrassLayerProps->SetAttribute("minWidth",  Ogre::StringConverter::toString(mPGMinSize[i]->get().x).c_str());
                pGrassLayerProps->SetAttribute("minHeight", Ogre::StringConverter::toString(mPGMinSize[i]->get().y).c_str());
                pGrassLayerProps->SetAttribute("maxWidth",  Ogre::StringConverter::toString(mPGMaxSize[i]->get().x).c_str());
                pGrassLayerProps->SetAttribute("maxHeight", Ogre::StringConverter::toString(mPGMaxSize[i]->get().y).c_str());

                // techniques
                pGrassLayerProps = pGrassLayers->InsertEndChild(TiXmlElement("techniques"))->ToElement();
                // grass render technique
                int rendTech = mPGGrassTech[i]->get();
                switch(rendTech){
                    case GRASSTECH_QUAD:       tempStr = "QUAD";       break;
                    case GRASSTECH_CROSSQUADS: tempStr = "CROSSQUADS"; break;
                    case GRASSTECH_SPRITE:     tempStr = "SPRITE";     break;
                }
                pGrassLayerProps->SetAttribute("renderTechnique", tempStr.c_str());

                if(!mPGLayers[i]){
                    tempStr = "false";
                }else{
                    if(mPGLayers[i]->getBlendValue()) tempStr = "true"; else tempStr = "false";
                }
                pGrassLayerProps->SetAttribute("blend", tempStr.c_str());
                // fade technique
                int fadeTech = mPGFadeTech[i]->get();
                switch(fadeTech){
                    case FADETECH_ALPHA:     tempStr = "ALPHA";     break;
                    case FADETECH_GROW:      tempStr = "GROW";      break;
                    case FADETECH_ALPHAGROW: tempStr = "ALPHAGROW"; break;
                }
                pGrassLayerProps->SetAttribute("fadeTechnique", tempStr.c_str());

                // animation and sway
                pGrassLayerProps = pGrassLayers->InsertEndChild(TiXmlElement("animation"))->ToElement();
                if(mPGAnimate[i]->get()) tempStr = "true"; else tempStr = "false";
                pGrassLayerProps->SetAttribute("animate", tempStr.c_str());
                pGrassLayerProps->SetAttribute("swayLength", Ogre::StringConverter::toString(mPGSwayLength[i]->get()).c_str());
                pGrassLayerProps->SetAttribute("swaySpeed",  Ogre::StringConverter::toString(mPGSwaySpeed[i]->get()).c_str());
                pGrassLayerProps->SetAttribute("swayDistribution", Ogre::StringConverter::toString(mPGSwayDistribution[i]->get()).c_str());
            }
        }
    }

    return pTerrainPage;
}
//-----------------------------------------------------------------------------------------
bool CTerrainPageEditor::unLoad()
{
    if(!mLoaded->get())
        return true;

    _notifyEndModification();

    if(mHandle->isModified() && mOgitorsRoot->GetLoadState() != LS_UNLOADED)
    {
        mTempModified->set(true);
        _saveTerrain("/Temp/tmp");
    }

    if(mPGModified && mOgitorsRoot->GetLoadState() != LS_UNLOADED)
    {
        mTempDensityModified->set(true);
        _saveGrass("/Temp/tmp");
    }

    unLoadAllChildren();
    destroyBoundingBox();

    _unloadGrassLayers();

    Ogre::TerrainGroup *terGroup = static_cast<Ogre::TerrainGroup*>(mParentEditor->get()->getHandle());

    if(mHandle)
    {
        terGroup->unloadTerrain(mPageX->get(), mPageY->get());
        mHandle = 0;
    }

    mLoaded->set(false);
    return true;
}
//-----------------------------------------------------------------------------------------
void CTerrainPageEditor::prepareBeforePresentProperties()
{
}
//-----------------------------------------------------------------------------------------
void CTerrainPageEditor::createProperties(OgitorsPropertyValueMap &params)
{
    PROPERTY_PTR(mPageX, "pagex", int, 0, 0, 0);
    PROPERTY_PTR(mPageY, "pagey", int, 0, 0, 0);
    PROPERTY_PTR(mPosition, "position", Ogre::Vector3, Ogre::Vector3::ZERO, 0, SETTER(Ogre::Vector3, CTerrainPageEditor, _setPosition));
    PROPERTY_PTR(mMinBatchSize, "tuning::minbatchsize",int, 33, 0, SETTER(int, CTerrainPageEditor, _setMinBatchSize));
    PROPERTY_PTR(mMaxBatchSize, "tuning::maxbatchsize",int, 65, 0, SETTER(int, CTerrainPageEditor, _setMaxBatchSize));
    PROPERTY_PTR(mColourMapEnabled, "colourmap::enabled", bool, false, 0, SETTER(bool, CTerrainPageEditor, _setColourMapEnabled));
    PROPERTY_PTR(mColourMapTextureSize, "colourmap::texturesize",int, 128, 0, SETTER(int, CTerrainPageEditor, _setColourMapTextureSize));
    PROPERTY_PTR(mLayerCount, "layercount", int, 1, 0, 0);
    PROPERTY_PTR(mTempModified, "tempmodified", bool, false, 0, 0);
    PROPERTY_PTR(mTempDensityModified, "tempdensitymodified", bool, false, 0, 0);
    PROPERTY_PTR(mLayerWorldSize[0], "layer0::worldsize", Ogre::Real, 100.0f, 0, SETTER(Ogre::Real, CTerrainPageEditor, _setLayerWorldSize));
    PROPERTY_PTR(mLayerDiffuse[0], "layer0::diffusespecular", Ogre::String, "dirt_grayrocky_diffusespecular.dds", 0, SETTER(Ogre::String, CTerrainPageEditor, _setLayerDiffuseMap));
    PROPERTY_PTR(mLayerNormal[0], "layer0::normalheight", Ogre::String, "dirt_grayrocky_normalheight.dds", 0, SETTER(Ogre::String, CTerrainPageEditor, _setLayerNormalMap));

    int count = 0;
    OgitorsPropertyValueMap::const_iterator it = params.find("layercount");
    if(it != params.end())
        count = Ogre::any_cast<int>(it->second.val);

    for(int i = 1;i < count;i++)
    {
        Ogre::String propStr1 = "layer" + Ogre::StringConverter::toString(i);
        PROPERTY_PTR(mLayerWorldSize[i], propStr1 + "::worldsize", Ogre::Real, 0, i, SETTER(Ogre::Real, CTerrainPageEditor, _setLayerWorldSize));
        PROPERTY_PTR(mLayerDiffuse[i], propStr1 + "::diffusespecular", Ogre::String, "", i, SETTER(Ogre::String, CTerrainPageEditor, _setLayerDiffuseMap));
        PROPERTY_PTR(mLayerNormal[i], propStr1 + "::normalheight", Ogre::String, "", i, SETTER(Ogre::String, CTerrainPageEditor, _setLayerNormalMap));
    }

    Ogre::Vector2 v1(1,1);
    int ftech = FADETECH_GROW;
    int gtech = GRASSTECH_CROSSQUADS;

    for(int i = 0;i < 4;i++)
    {
        Ogre::String label = "pg::layer" + Ogre::StringConverter::toString(i);
        PROPERTY_PTR(mPGActive[i]          , label + "::active"          , bool         , false, i, SETTER(bool, CTerrainPageEditor, _setPGActive));
        PROPERTY_PTR(mPGMaterial[i]        , label + "::material"        , Ogre::String , ""   , i, SETTER(Ogre::String, CTerrainPageEditor, _setPGMaterial));
        PROPERTY_PTR(mPGDensity[i]         , label + "::density"         , Ogre::Real   , 3.0f , i, SETTER(Ogre::Real, CTerrainPageEditor, _setPGDensity));
        PROPERTY_PTR(mPGMinSize[i]         , label + "::minsize"         , Ogre::Vector2, v1   , i, SETTER(Ogre::Vector2, CTerrainPageEditor, _setPGMinSize));
        PROPERTY_PTR(mPGMaxSize[i]         , label + "::maxsize"         , Ogre::Vector2, v1   , i, SETTER(Ogre::Vector2, CTerrainPageEditor, _setPGMaxSize));
        PROPERTY_PTR(mPGAnimate[i]         , label + "::animate"         , bool         , true , i, SETTER(bool, CTerrainPageEditor, _setPGAnimate));
        PROPERTY_PTR(mPGSwayDistribution[i], label + "::swaydistribution", Ogre::Real   , 10.0f, i, SETTER(Ogre::Real, CTerrainPageEditor, _setPGSwayDistribution));
        PROPERTY_PTR(mPGSwayLength[i]      , label + "::swaylength"      , Ogre::Real   , 0.2f , i, SETTER(Ogre::Real, CTerrainPageEditor, _setPGSwayLength));
        PROPERTY_PTR(mPGSwaySpeed[i]       , label + "::swayspeed"       , Ogre::Real   , 0.5f , i, SETTER(Ogre::Real, CTerrainPageEditor, _setPGSwaySpeed));
        PROPERTY_PTR(mPGFadeTech[i]        , label + "::fadetech"        , int          , ftech, i, SETTER(int, CTerrainPageEditor, _setPGFadeTech));
        PROPERTY_PTR(mPGGrassTech[i]       , label + "::grasstech"       , int          , gtech, i, SETTER(int, CTerrainPageEditor, _setPGGrassTech));
    }

    mProperties.initValueMap(params);
}
//-----------------------------------------------------------------------------------------
void CTerrainPageEditor::_notifyModification(int layerID, const Ogre::Rect& dirtyRect)
{
    if(!mHandle || !mHandle->isLoaded())
        return;

    if(layerID == -2)
    {
        if(!mGrassSave)
        {
            if(!(mPGActive[0]->get() || mPGActive[1]->get() || mPGActive[2]->get() || mPGActive[3]->get()))
                return;

            CTerrainGroupEditor *parentEditor = static_cast<CTerrainGroupEditor*>(mParentEditor->get());

            size_t mapsize = parentEditor->getGrassDensityMapSize() * parentEditor->getGrassDensityMapSize();
            mGrassSave = OGRE_ALLOC_T(float, mapsize * 4, Ogre::MEMCATEGORY_RESOURCE);

            int pos = 0;

            for(int i = 0;i < 4;i++)
            {
                float *ptr = getGrassPointer(i);
                if(ptr)
                    memcpy(&(mGrassSave[pos * mapsize]), ptr, sizeof(float) * mapsize);
                else
                {
                    int loc = pos * mapsize;
                    for(unsigned int pgx = 0;pgx < mapsize;pgx++)
                         mGrassSave[loc + pgx] = 0.0f;
                }
                ++pos;
            }
        }

        mGrassMapDirtyRect.merge(dirtyRect);
    }
    else if(layerID == -1)
    {
        if(!mHeightSave)
        {
            float *ptr = mHandle->getHeightData();
            size_t mapsize = mHandle->getSize() * mHandle->getSize();
            mHeightSave = OGRE_ALLOC_T(float, mapsize, Ogre::MEMCATEGORY_RESOURCE);
            memcpy(mHeightSave, ptr, sizeof(float) * mapsize);
        }
        mHeightDirtyRect.merge(dirtyRect);
    }
    else if(layerID == 0)
    {
        if(!mColourSave && mColourMapEnabled->get())
        {
            size_t buffersize = mHandle->getGlobalColourMap()->getBuffer()->getSizeInBytes();
            unsigned char *ptr = (unsigned char *)mHandle->getGlobalColourMap()->getBuffer()->lock(0,  buffersize, Ogre::HardwareBuffer::HBL_NORMAL);
            int mapsize = mHandle->getGlobalColourMapSize() * mHandle->getGlobalColourMapSize();
            int spacing = buffersize / mapsize;
            mColourSave = OGRE_ALLOC_T(Ogre::ColourValue, mapsize, Ogre::MEMCATEGORY_RESOURCE);
            Ogre::PixelFormat pf = mHandle->getGlobalColourMap()->getBuffer()->getFormat();
            for(int mc = 0;mc < mapsize;mc++)
            {
                Ogre::PixelUtil::unpackColour(&(mColourSave[mc]), pf, (void*)&(ptr[mc * spacing]));
            }

            mHandle->getGlobalColourMap()->getBuffer()->unlock();
        }
        mColourMapDirtyRect.merge(dirtyRect);
    }
    else
    {
        if(!mBlendSave)
        {
            int numlayers = mLayerCount->get() - layerID;
            if(!numlayers)
                return;

            size_t mapsize = mHandle->getLayerBlendMapSize() * mHandle->getLayerBlendMapSize();
            mBlendSave = OGRE_ALLOC_T(float, mapsize * numlayers, Ogre::MEMCATEGORY_RESOURCE);

            int pos = 0;

            for(int i = layerID;i < mLayerCount->get();i++)
            {
                float *ptr = mHandle->getLayerBlendMap(i)->getBlendPointer();
                memcpy(&(mBlendSave[pos * mapsize]), ptr, sizeof(float) * mapsize);
                ++pos;
            }

            mBlendSaveCount = pos;

            mBlendSaveStart = layerID;
        }

        mBlendMapDirtyRect.merge(dirtyRect);
    }
}
//-----------------------------------------------------------------------------------------
void CTerrainPageEditor::_notifyEndModification()
{
    if(!mHandle || !mHandle->isLoaded())
        return;

    if(mHeightSave)
    {
        unsigned int size = mHeightDirtyRect.width() * mHeightDirtyRect.height();

        if(size)
        {
            float *data = OGRE_ALLOC_T(float, size, Ogre::MEMCATEGORY_RESOURCE);
            int pos = 0;
            int rowSize = mHandle->getSize();

            for(int y = mHeightDirtyRect.top;y < mHeightDirtyRect.bottom;y++)
            {
                for(int x = mHeightDirtyRect.left;x < mHeightDirtyRect.right;x++)
                {
                    data[pos] = mHeightSave[y * rowSize + x];
                    ++pos;
                }
            }

            OgitorsUndoManager::getSingletonPtr()->AddUndo(OGRE_NEW TerrainHeightUndo(mObjectID->get(), mHeightDirtyRect, data));
            OGRE_FREE(data, Ogre::MEMCATEGORY_RESOURCE);
        }
        OGRE_FREE(mHeightSave, Ogre::MEMCATEGORY_RESOURCE);
        mHeightSave = 0;
        mHeightDirtyRect = Ogre::Rect(0,0,0,0);
    }

    if(mBlendSave)
    {
        unsigned int size = mBlendMapDirtyRect.width() * mBlendMapDirtyRect.height() * mBlendSaveCount;

        if(size)
        {
            float *data = OGRE_ALLOC_T(float, size, Ogre::MEMCATEGORY_RESOURCE);
            int pos = 0;
            int rowSize = mHandle->getLayerBlendMapSize();
            int mapSize = rowSize * rowSize;
            float *blendmap;

            for(int c = 0;c < mBlendSaveCount;c++)
            {
                blendmap = &(mBlendSave[c * mapSize]);
                for(int y = mBlendMapDirtyRect.top;y < mBlendMapDirtyRect.bottom;y++)
                {
                    for(int x = mBlendMapDirtyRect.left;x < mBlendMapDirtyRect.right;x++)
                    {
                        data[pos] = blendmap[(y * rowSize) + x];
                        ++pos;
                    }
                }
            }

            OgitorsUndoManager::getSingletonPtr()->AddUndo(OGRE_NEW TerrainBlendUndo(mObjectID->get(), mBlendMapDirtyRect, mBlendSaveStart, mBlendSaveCount, data));
            OGRE_FREE(data, Ogre::MEMCATEGORY_RESOURCE);
        }
        OGRE_FREE(mBlendSave, Ogre::MEMCATEGORY_RESOURCE);
        mBlendSave = 0;
        mBlendSaveStart = 1;
        mBlendSaveCount = 0;
        mBlendMapDirtyRect = Ogre::Rect(0,0,0,0);
    }

    if(mGrassSave)
    {
        unsigned int size = mGrassMapDirtyRect.width() * mGrassMapDirtyRect.height() * 4;

        if(size)
        {
            float *data = OGRE_ALLOC_T(float, size, Ogre::MEMCATEGORY_RESOURCE);
            int pos = 0;

            CTerrainGroupEditor *parentEditor = static_cast<CTerrainGroupEditor*>(mParentEditor->get());

            int rowSize = parentEditor->getGrassDensityMapSize();
            int mapSize = rowSize * rowSize;
            float *grassmap;

            for(int c = 0;c < 4;c++)
            {
                grassmap = &(mGrassSave[c * mapSize]);
                for(int y = mGrassMapDirtyRect.top;y < mGrassMapDirtyRect.bottom;y++)
                {
                    for(int x = mGrassMapDirtyRect.left;x < mGrassMapDirtyRect.right;x++)
                    {
                        data[pos] = grassmap[(y * rowSize) + x];
                        ++pos;
                    }
                }
            }

            OgitorsUndoManager::getSingletonPtr()->AddUndo(OGRE_NEW TerrainGrassUndo(mObjectID->get(), mGrassMapDirtyRect, data));
            OGRE_FREE(data, Ogre::MEMCATEGORY_RESOURCE);
        }
        OGRE_FREE(mGrassSave, Ogre::MEMCATEGORY_RESOURCE);
        mGrassSave = 0;
        mGrassMapDirtyRect = Ogre::Rect(0,0,0,0);
    }

    if(mColourSave && mColourMapEnabled->get())
    {
        unsigned int size = mColourMapDirtyRect.width() * mColourMapDirtyRect.height();

        if(size)
        {
            Ogre::ColourValue *data = OGRE_ALLOC_T(Ogre::ColourValue, size, Ogre::MEMCATEGORY_RESOURCE);

            int pos = 0;
            int rowSize = mHandle->getGlobalColourMapSize();

            for(int y = mColourMapDirtyRect.top;y < mColourMapDirtyRect.bottom;y++)
            {
                for(int x = mColourMapDirtyRect.left;x < mColourMapDirtyRect.right;x++)
                {
                    data[pos] = mColourSave[y * rowSize + x];
                    ++pos;
                }
            }

            OgitorsUndoManager::getSingletonPtr()->AddUndo(OGRE_NEW TerrainColourUndo(mObjectID->get(), mColourMapDirtyRect, data));
            OGRE_FREE(data, Ogre::MEMCATEGORY_RESOURCE);
        }

        OGRE_FREE(mColourSave, Ogre::MEMCATEGORY_RESOURCE);
        mColourSave = 0;
        mColourMapDirtyRect = Ogre::Rect(0,0,0,0);
    }
}
//-----------------------------------------------------------------------------------------
void CTerrainPageEditor::_swapHeights(Ogre::Rect rect, float *data)
{
    bool unload = !mLoaded->get();
    load(false);

    assert(mHandle != 0);

    float *hdata = mHandle->getHeightData();
    int rowSize = mHandle->getSize();

    int pos = 0;
    for(int y = rect.top;y < rect.bottom;y++)
    {
        for(int x = rect.left;x < rect.right;x++)
        {
            assert(data[pos] > -100000.0f);
            assert(data[pos] < 100000.0f);
            float tmpdata = hdata[y * rowSize + x];
            hdata[y * rowSize + x] = data[pos];
            data[pos] = tmpdata;
            ++pos;
        }
    }

    mHandle->dirtyRect(rect);
    mHandle->update();

    Ogre::Rect dirty;

    float ratio = (float)mHandle->getWorldSize() / (float)(mHandle->getSize() - 1);
    float halfSize = mHandle->getWorldSize() / 2.0f;

    dirty.left = mHandle->getPosition().x + (rect.left * ratio) - halfSize;
    dirty.right = mHandle->getPosition().x + (rect.right * ratio) - halfSize;
    dirty.top = mHandle->getPosition().z + ((mHandle->getSize() - rect.top) * ratio) - halfSize;
    dirty.bottom = mHandle->getPosition().z + ((mHandle->getSize() - rect.bottom) * ratio) - halfSize;

    _refreshGrassGeometry(&dirty);

    if(unload)
        unLoad();
}
//-----------------------------------------------------------------------------------------
void CTerrainPageEditor::_swapBlends(int layerStart, int layerCount, Ogre::Rect rect, float *data)
{
    bool unload = !mLoaded->get();
    load(false);

    assert(mHandle != 0);

    int rowSize = mHandle->getLayerBlendMapSize();
    int mapSize = rowSize * rowSize;

    int pos = 0;

    for(int c = 0;c < layerCount;c++)
    {
        Ogre::TerrainLayerBlendMap *blendmap = mHandle->getLayerBlendMap(c + layerStart);
        float *hdata = blendmap->getBlendPointer();

        for(int y = rect.top;y < rect.bottom;y++)
        {
            for(int x = rect.left;x < rect.right;x++)
            {
                float tmpdata = hdata[y * rowSize + x];
                hdata[y * rowSize + x] = data[pos];
                data[pos] = tmpdata;
                ++pos;
            }
        }

        blendmap->dirtyRect(rect);
        blendmap->update();
    }

    if(unload)
        unLoad();
}
//-----------------------------------------------------------------------------------------
void CTerrainPageEditor::_swapColours(Ogre::Rect rect, Ogre::ColourValue *data)
{
    bool unload = !mLoaded->get();
    load(false);

    assert(mHandle != 0);

    int buffersize = mHandle->getGlobalColourMap()->getBuffer()->getSizeInBytes();
    int rowSize = mHandle->getGlobalColourMapSize();
    int spacing = buffersize / (rowSize * rowSize);
    unsigned char *hdata = (unsigned char *)mHandle->getGlobalColourMap()->getBuffer()->lock(0,  buffersize, Ogre::HardwareBuffer::HBL_NORMAL);
    Ogre::PixelFormat pf = mHandle->getGlobalColourMap()->getBuffer()->getFormat();
    Ogre::ColourValue colVal;

    int pos = 0;

    for(int y = rect.top;y < rect.bottom;y++)
    {
        for(int x = rect.left;x < rect.right;x++)
        {
            Ogre::PixelUtil::unpackColour(&colVal, pf, (void*)&(hdata[(y * rowSize + x) * spacing]));
            Ogre::PixelUtil::packColour(data[pos], pf, (void*)&(hdata[(y * rowSize + x) * spacing]));
            data[pos] = colVal;
            ++pos;
        }
    }

    mHandle->getGlobalColourMap()->getBuffer()->unlock();

    if(unload)
        unLoad();
}
//-----------------------------------------------------------------------------------------
void CTerrainPageEditor::_swapGrass(Ogre::Rect rect, float *data)
{
    bool unload = !mLoaded->get();
    load(false);

    assert(mHandle != 0);

    CTerrainGroupEditor *parentEditor = static_cast<CTerrainGroupEditor*>(mParentEditor->get());

    int rowSize = parentEditor->getGrassDensityMapSize();
    int mapSize = rowSize * rowSize;

    int pos = 0;

    for(int c = 0;c < 4;c++)
    {
        float *hdata = getGrassPointer(c);

        if(!hdata)
        {
            pos += mapSize;
            continue;
        }

        for(int y = rect.top;y < rect.bottom;y++)
        {
            for(int x = rect.left;x < rect.right;x++)
            {
                float tmpdata = hdata[y * rowSize + x];
                hdata[y * rowSize + x] = data[pos];
                data[pos] = tmpdata;
                ++pos;
            }
        }

        dirtyGrassRect(rect);
        updateGrassLayer(c);
    }

    if(unload)
        unLoad();
}
//-----------------------------------------------------------------------------------------
void CTerrainPageEditor::_modifyHeights(float scale, float offset)
{
    bool unload = !mLoaded->get();
    load(false);

    OgitorsUndoManager::getSingletonPtr()->BeginCollection("Modify Height Values");

    Ogre::Rect rect(0,0,mHandle->getSize(), mHandle->getSize());
    _notifyModification(-1, rect);
    _notifyEndModification();

    float *data = mHandle->getHeightData();
    int numvertexes = mHandle->getSize() * mHandle->getSize();

    for(int px = 0;px < numvertexes;px++)
    {
        float val = (data[px] * scale) + offset;
        data[px] = val;
    }

    mHandle->dirtyRect(rect);
    mHandle->update();
    
    OgitorsUndoManager::getSingletonPtr()->EndCollection(true);

    mOgitorsRoot->SetSceneModified(true);

    if(unload)
        unLoad();
    else
    {
        Ogre::AxisAlignedBox bBox = mHandle->getWorldAABB();
        Ogre::Rect dirty(bBox.getMinimum().x, bBox.getMinimum().z, bBox.getMaximum().x, bBox.getMaximum().z);

        _refreshGrassGeometry(&dirty);
    }
}
//-----------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------
//------CTERRAINPAGEEDITORFACTORY-----------------------------------------------------------------
//-----------------------------------------------------------------------------------------
CTerrainPageEditorFactory::CTerrainPageEditorFactory(OgitorsView *view) : CBaseEditorFactory(view)
{
    mTypeName = "Terrain Page Object";
    mEditorType = ETYPE_MOVABLE;
    mIcon = "pagedterrain.svg";
    mCapabilities = CAN_PAGE | CAN_DELETE | CAN_UNDO;
    mDefaultWorldSection = SECT_TERRAIN;

    OgitorsPropertyDef *definition;

    mFadeTechniques.clear();
    mFadeTechniques.push_back(PropertyOption("ALPHA",Ogre::Any((int)FADETECH_ALPHA)));
    mFadeTechniques.push_back(PropertyOption("GROW",Ogre::Any((int)FADETECH_GROW)));
    mFadeTechniques.push_back(PropertyOption("ALPHA_GROW",Ogre::Any((int)FADETECH_ALPHAGROW)));

    mGrassTechniques.clear();
    mGrassTechniques.push_back(PropertyOption("QUAD",Ogre::Any((int)GRASSTECH_QUAD)));
    mGrassTechniques.push_back(PropertyOption("CROSSQUADS",Ogre::Any((int)GRASSTECH_CROSSQUADS)));
    mGrassTechniques.push_back(PropertyOption("SPRITE",Ogre::Any((int)GRASSTECH_SPRITE)));

    AddPropertyDefinition("position", "Position","The origin of the terrain page.",PROP_VECTOR3, true, false);
    AddPropertyDefinition("layercount","", "Number of texture layers.",PROP_INT, false, false);
    AddPropertyDefinition("tempmodified","", "Does the terrain have a temporary file?",PROP_BOOL, false, false);
    AddPropertyDefinition("tempdensitymodified","", "Does the terrain Density Map have a temporary file?",PROP_BOOL, false, false);
    AddPropertyDefinition("pagex","Page X", "The X index of the page.",PROP_INT, true, false);
    AddPropertyDefinition("pagey","Page Y", "The Y index of the page.",PROP_INT, true, false);
    AddPropertyDefinition("colourmap::enabled","", "Is the colourmap enabled?",PROP_BOOL, false, false);
    AddPropertyDefinition("colourmap::texturesize","", "The size of colourmap texture.",PROP_INT, false, false);
    AddPropertyDefinition("tuning::minbatchsize","Tuning::Min Batch Size", "Minimum Batch Size.",PROP_INT, false);
    AddPropertyDefinition("tuning::maxbatchsize","Tuning::Max Batch Size", "Maximum Batch Size.",PROP_INT, false);

    int i;
    for(i = 0;i < 32;i++)
    {
        Ogre::String propStr1 = "layer" + Ogre::StringConverter::toString(i);
        Ogre::String propStr2 = "Layers::Layer" + Ogre::StringConverter::toString(i);

        AddPropertyDefinition(propStr1 + "::worldsize", propStr2 + "::WorldSize", "Layer's World Size", PROP_REAL);
        definition = AddPropertyDefinition(propStr1 + "::diffusespecular", propStr2 + "::Diffuse Map", "Layer's Diffuse Texture Map", PROP_STRING);
        definition->setOptions(OgitorsRoot::GetTerrainDiffuseTextureNames());
        definition = AddPropertyDefinition(propStr1 + "::normalheight", propStr2 + "::Normal Map", "Layer's Normal Texture Map", PROP_STRING);
        definition->setOptions(OgitorsRoot::GetTerrainNormalTextureNames());
    }

    for(i = 0;i < 4;i++)
    {
        Ogre::String propStr1 = "pg::layer" + Ogre::StringConverter::toString(i);
        Ogre::String propStr2 = "Paged Geometry::Layer" + Ogre::StringConverter::toString(i);

        AddPropertyDefinition(propStr1 + "::active",propStr2 + "::Active", "",PROP_BOOL);
        definition = AddPropertyDefinition(propStr1 + "::material",propStr2 + "::Material", "",PROP_STRING);
        definition->setOptions(OgitorsRoot::GetTerrainPlantMaterialNames());
        AddPropertyDefinition(propStr1 + "::density",propStr2 + "::Density", "",PROP_REAL);
        AddPropertyDefinition(propStr1 + "::minsize",propStr2 + "::Min. Size", "",PROP_VECTOR2);
        AddPropertyDefinition(propStr1 + "::maxsize",propStr2 + "::Max. Size", "",PROP_VECTOR2);
        AddPropertyDefinition(propStr1 + "::animate",propStr2 + "::Animate", "",PROP_BOOL);
        AddPropertyDefinition(propStr1 + "::swaydistribution",propStr2 + "::Sway Dist.", "",PROP_REAL);
        AddPropertyDefinition(propStr1 + "::swaylength",propStr2 + "::Sway Length", "",PROP_REAL);
        AddPropertyDefinition(propStr1 + "::swayspeed",propStr2 + "::Sway Speed", "",PROP_REAL);
        definition = AddPropertyDefinition(propStr1 + "::fadetech",propStr2 + "::Fade Tech", "",PROP_INT);
        definition->setOptions(&mFadeTechniques);
        definition = AddPropertyDefinition(propStr1 + "::grasstech",propStr2 + "::Grass Tech", "",PROP_INT);
        definition->setOptions(&mGrassTechniques);
    }

    OgitorsPropertyDefMap::iterator it = mPropertyDefs.find("name");
    it->second.setAccess(true, false);

    it = mPropertyDefs.find("layer");
    it->second.setAccess(false, false);
}
//-----------------------------------------------------------------------------------------
CBaseEditorFactory *CTerrainPageEditorFactory::duplicate(OgitorsView *view)
{
    CBaseEditorFactory *ret = OGRE_NEW CTerrainPageEditorFactory(view);
    ret->mTypeID = mTypeID;

    return ret;
}
//-----------------------------------------------------------------------------------------
CBaseEditor *CTerrainPageEditorFactory::CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params)
{
    CTerrainGroupEditor *manager = (CTerrainGroupEditor*)OgitorsRoot::getSingletonPtr()->FindObject("Terrain Group");
    if(!manager)
        return 0;

    *parent = manager;

    CTerrainPageEditor *object = OGRE_NEW CTerrainPageEditor(this);

    OgitorsPropertyValueMap::iterator ni;

    if ((ni = params.find("init")) != params.end())
    {
        params.erase(ni);
        object->mFirstTimeInit = true;
    }

    if ((ni = params.find("externaldatahandle")) != params.end())
    {
        object->mExternalDataHandle = (float*)(Ogre::any_cast<unsigned long>(ni->second.val));
    }

    object->createProperties(params);
    object->mParentEditor->init(*parent);

    object->mColourMapEnabled->connectTo(manager->getProperties()->getProperty("colourmap::enabled"));
    object->mColourMapTextureSize->connectTo(manager->getProperties()->getProperty("colourmap::texturesize"));
    object->mMinBatchSize->connectTo(manager->getProperties()->getProperty("tuning::minbatchsize"));
    object->mMaxBatchSize->connectTo(manager->getProperties()->getProperty("tuning::maxbatchsize"));

    mInstanceCount++;
    return object;
}
//-----------------------------------------------------------------------------------------
bool CTerrainPageEditorFactory::CanInstantiate()
{
    CBaseEditor *manager = OgitorsRoot::getSingletonPtr()->FindObject("Terrain Group");
    return (manager != 0);
}
//-----------------------------------------------------------------------------------------
