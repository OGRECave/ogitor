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
#include "OgreTerrainGroup.h"
#else
#include "Terrain/OgreTerrain.h"
#include "Terrain/OgreTerrainGroup.h"
#endif

#include "TerrainMaterialGeneratorB.h"
#include "TerrainMaterialGeneratorC.h"

#include "BaseEditor.h"
#include "OgitorsRoot.h"
#include "OgitorsSystem.h"
#include "TerrainEditor.h"
#include "TerrainPageEditor.h"
#include "TerrainGroupEditor.h"
#include "OgreStreamSerialiser.h"
#include "SceneManagerEditor.h"
#include "CameraEditor.h"
#include "ViewportEditor.h"
#include "MultiSelEditor.h"
#include "tinyxml.h"
#include "ofs.h"

#include "PagedGeometry.h"
#include "GrassLoader.h"

using namespace Forests;
using namespace Ogitors;

PropertyOptionsVector CTerrainGroupEditorFactory::mMapSizeOptions;
PropertyOptionsVector CTerrainGroupEditorFactory::mColourMapSizeOptions;
PropertyOptionsVector CTerrainGroupEditorFactory::mMinBatchSizes;
PropertyOptionsVector CTerrainGroupEditorFactory::mMaxBatchSizes;
PropertyOptionsVector CTerrainGroupEditorFactory::mMaterialGeneratorTypes;

Ogre::TerrainGroup *StaticGroupPtr = 0;

//-----------------------------------------------------------------------------------------
Ogre::Real OgitorTerrainGroupHeightFunction(Ogre::Real x, Ogre::Real z, void *userData)
{
    return StaticGroupPtr->getHeightAtWorldPosition(x,0,z);
}
//-----------------------------------------------------------------------------------------
CTerrainGroupEditor::CTerrainGroupEditor(CBaseEditorFactory *factory) : CBaseEditor(factory),
mHandle(0), mBrushData(0), mModificationRect(0,0,0,0)
{
    mUsesGizmos = false;
    mUsesHelper = false;
    mDecalNode = 0;
    mDecalFrustum = 0;
    mEditMode = EM_NONE;
    mEditActive = false;
    mDecalTexture.setNull();
    mBrushSize = 1;
    mName->init("Terrain Group");
    mTextureGrass = "grass";
    mBrushName = "AllWhite.png";

    mMaxLayersAllowed = 6;

    Ogre::ResourceGroupManager *mngr = Ogre::ResourceGroupManager::getSingletonPtr();
    Ogre::String value = mOgitorsRoot->GetProjectFile()->getFileSystemName() + "::/" + mOgitorsRoot->GetProjectOptions()->TerrainDirectory + "/";
    mngr->addResourceLocation(value,"Ofs","TerrainResources");

    mTerrainGlobalOptions = OGRE_NEW Ogre::TerrainGlobalOptions();
}
//-----------------------------------------------------------------------------------------
CTerrainGroupEditor::~CTerrainGroupEditor()
{
    OGRE_DELETE mTerrainGlobalOptions;
    mOgitorsRoot->DestroyResourceGroup("TerrainResources");
    mOgitorsRoot->DestroyResourceGroup("TerrainTextures");
}
//-----------------------------------------------------------------------------------------
PGHeightFunction *CTerrainGroupEditor::getHeightFunction()
{
    return &OgitorTerrainGroupHeightFunction;
}
//-----------------------------------------------------------------------------------------
Ogre::Real CTerrainGroupEditor::getHeightAt(Ogre::Real x, Ogre::Real z)
{
    return mHandle->getHeightAtWorldPosition(x,0,z);
}
//-----------------------------------------------------------------------------------------
Ogre::AxisAlignedBox CTerrainGroupEditor::getAABB()
{
    return Ogre::AxisAlignedBox::BOX_NULL;
}
//-----------------------------------------------------------------------------------------
void CTerrainGroupEditor::prepareBeforePresentProperties()
{
    OgitorsPropertyDef *definition;

    definition = const_cast<OgitorsPropertyDef *>(mProperties.getProperty("lightmap::texturesize")->getDefinition());
    definition->setAccess(true, mChildren.size() == 0);
    definition = const_cast<OgitorsPropertyDef *>(mProperties.getProperty("blendmap::texturesize")->getDefinition());
    definition->setAccess(true, mChildren.size() == 0);
    definition = const_cast<OgitorsPropertyDef *>(mProperties.getProperty("pg::densitymapsize")->getDefinition());
    definition->setAccess(true, mChildren.size() == 0);
    definition = const_cast<OgitorsPropertyDef *>(mProperties.getProperty("materialgeneratortype")->getDefinition());
    definition->setAccess(true, mChildren.size() == 0);
}
//-----------------------------------------------------------------------------------------
bool CTerrainGroupEditor::getObjectContextMenu(UTFStringVector &menuitems)
{
    bool hasPages = false;
    if(mHandle->getTerrainIterator().begin() != mHandle->getTerrainIterator().end())
        hasPages = true;
    
    menuitems.clear();
    menuitems.push_back(OTR("Add Page") + ";:/icons/additional.svg");
    if(hasPages)
        menuitems.push_back(OTR("Scale/Offset Height Values") + ";:/icons/scale.svg");
    
    menuitems.push_back("---");
    menuitems.push_back(OTR("Import Terrain From Heightmap")+ ";:/icons/import.svg");
    menuitems.push_back("---");
    if(hasPages)
    {
        menuitems.push_back(OTR("Export Heightmaps") + ";:/icons/export.svg");
        menuitems.push_back(OTR("Export Compositemaps") + ";:/icons/export.svg");
    }    

    return true;
}
//-------------------------------------------------------------------------------
void CTerrainGroupEditor::onObjectContextMenu(int menuresult)
{
    if(menuresult == 0)
    {
        Ogitors::NameObjectPairList::iterator it;
        Ogre::NameValuePairList params;
        Ogre::String posval;

        int minX = -1, minY = -1, maxX = 1, maxY = 1, PX, PY;
        for(it = mChildren.begin(); it != mChildren.end();it++)
        {
             CTerrainPageEditor *terrain = static_cast<CTerrainPageEditor*>(it->second);
             PX = terrain->getPageX();
             PY = terrain->getPageY();
             minX = std::min(minX, PX - 1);
             minY = std::min(minY, PY - 1);
             maxX = std::max(maxX, PX + 1);
             maxY = std::max(maxY, PY + 1);
        }

        int width = maxX - minX + 1;
        int height = maxY - minY + 1;

        bool *mtx = OGRE_ALLOC_T(bool, width * height, Ogre::MEMCATEGORY_GEOMETRY);
        for(int i = 0; i < width * height;++i)
            mtx[i] = true;

        for(it = mChildren.begin(); it != mChildren.end();it++)
        {
             CTerrainPageEditor *terrain = static_cast<CTerrainPageEditor*>(it->second);
             PX = terrain->getPageX();
             PY = terrain->getPageY();

             mtx[((PY - minY) * width) + (PX - minX)] = false;
        }

        for(int Y = 0;Y < height;++Y)
        {
            for(int X = 0;X < width;++X)
            {
                if(mtx[(Y * width) + X])
                {
                    posval = Ogre::StringConverter::toString(X + minX) + "x" + Ogre::StringConverter::toString(Y + minY);
                    params[posval] = posval;
                }
            }
        }

        OGRE_FREE(mtx, Ogre::MEMCATEGORY_GEOMETRY);

        if(mSystem->DisplayTerrainDialog(params))
        {
            int x = Ogre::StringConverter::parseInt(params["pagex"]);
            int y = Ogre::StringConverter::parseInt(params["pagey"]);

            OgitorsPropertyValueMap creationparams;
            OgitorsPropertyValue pvalue;

            Ogre::String pagename = mPageNamePrefix->get();
            pagename += Ogre::StringConverter::toString(x);
            pagename += "x";
            pagename += Ogre::StringConverter::toString(y);

            creationparams["init"] = EMPTY_PROPERTY_VALUE;
            pvalue.propType = PROP_STRING;
            pvalue.val = Ogre::Any(pagename);
            creationparams["name"] = pvalue;

            Ogre::Vector3 position;
            mHandle->convertTerrainSlotToWorldPosition(x, y, &position);

            pvalue.propType = PROP_VECTOR3;
            pvalue.val = Ogre::Any(position);
            creationparams["position"] = pvalue;
            pvalue.propType = PROP_INT;
            pvalue.val = Ogre::Any(x);
            creationparams["pagex"] = pvalue;
            pvalue.propType = PROP_INT;
            pvalue.val = Ogre::Any(y);
            creationparams["pagey"] = pvalue;
            pvalue.propType = PROP_STRING;
            pvalue.val = Ogre::Any(params["diffuse"]);
            creationparams["layer0::diffusespecular"] = pvalue;
            pvalue.propType = PROP_STRING;
            pvalue.val = Ogre::Any(params["normal"]);
            creationparams["layer0::normalheight"] = pvalue;
            pvalue.propType = PROP_REAL;
            pvalue.val = Ogre::Any((Ogre::Real)10.0f);
            creationparams["layer0::worldsize"] = pvalue;

            CTerrainPageEditor* page = (CTerrainPageEditor*)mOgitorsRoot->CreateEditorObject(this,"Terrain Page Object", creationparams, true, true);
        }
    }
    else if(menuresult == 1)
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
    else if(menuresult == 2)
    {
        importFullTerrainFromHeightMap();
    }
    else if(menuresult == 3)
    {
        exportHeightMaps();
    }
    else if(menuresult == 4)
    {
        exportCompositeMaps();
    }
}
//-----------------------------------------------------------------------------------------
Ogre::Vector3 CTerrainGroupEditor::getPagePosition(int x, int y)
{
    Ogre::Vector3 pos;
    mHandle->convertTerrainSlotToWorldPosition(x, y, &pos);
    return pos;
}
//-----------------------------------------------------------------------------------------
void CTerrainGroupEditor::removePage(CTerrainPageEditor *page)
{
    if(mHandle)
        mHandle->removeTerrain(page->getPageX(), page->getPageY());
}
//-----------------------------------------------------------------------------------------
bool CTerrainGroupEditor::isSelected()
{
    CBaseEditor *selectedobject = mOgitorsRoot->GetSelection()->getAsSingle();
    bool selected = (selectedobject == this);

    Ogitors::NameObjectPairList::iterator it;

    for(it = mChildren.begin(); it != mChildren.end();it++)
    {
        selected |= (selectedobject == it->second);
    }

    return selected;
}
//-----------------------------------------------------------------------------------------
bool CTerrainGroupEditor::setLayerImpl(unsigned int newlayer)
{
    Ogitors::NameObjectPairList::iterator it;

    for(it = mChildren.begin(); it != mChildren.end();it++)
    {
         static_cast<CTerrainPageEditor*>(it->second)->setLayer(newlayer);
    }

    mGrassLoaderHandle->setVisibilityFlags(1 << mLayer->get());

    return true;
}
//-----------------------------------------------------------------------------------------
bool CTerrainGroupEditor::load(bool async)
{
    if(mLoaded->get())
        return true;

    if(!getParent()->load())
        return false;

    Ogre::ResourceGroupManager *mngr = Ogre::ResourceGroupManager::getSingletonPtr();
    if (!mngr->resourceGroupExists("TerrainTextures")) {
        Ogre::String value = mOgitorsRoot->GetProjectFile()->getFileSystemName() + "::/terrainTextures/";
        mngr->addResourceLocation(value,"Ofs","TerrainTextures");
        mngr->initialiseResourceGroup("TerrainTextures");
    }

    OgitorsRoot::getSingletonPtr()->PrepareTerrainResources();

    mDecalFrustum = OGRE_NEW Ogre::Frustum();
    mDecalNode = getSceneManager()->getRootSceneNode()->createChildSceneNode("OgitorTerrainDecalNode");
    mDecalNode->setPosition(99999,-99999,99999);
    mDecalNode->attachObject(mDecalFrustum);
    mDecalFrustum->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
    mDecalNode->setOrientation(Ogre::Quaternion(Ogre::Degree(-90), Ogre::Vector3::UNIT_X));
    mDecalFrustum->setFOVy(Ogre::Degree(45));
    mDecalFrustum->setNearClipDistance(10);
    mDecalFrustum->setOrthoWindow(10, 10);
    mDecalFrustum->setVisible(false);
    mDecalTexture = Ogre::TextureManager::getSingletonPtr()->createManual("OgitorDecalTexture","TerrainResources",Ogre::TEX_TYPE_2D,256,256,1,1,Ogre::PF_A8R8G8B8,Ogre::TU_DYNAMIC_WRITE_ONLY,this);

    mBrushData = OGRE_ALLOC_T(float, BRUSH_DATA_SIZE * BRUSH_DATA_SIZE, Ogre::MEMCATEGORY_GEOMETRY);

    mTerrainGlobalOptions->setMaxPixelError(mMaxPixelError->get());
    mTerrainGlobalOptions->setCompositeMapSize(mCompositeMapTextureSize->get());
    mTerrainGlobalOptions->setCompositeMapDistance(mCompositeMapDistance->get());
    mTerrainGlobalOptions->setLightMapSize(mLightMapTextureSize->get());
    mTerrainGlobalOptions->setLayerBlendMapSize(mBlendMapTextureSize->get());
    mTerrainGlobalOptions->setUseVertexCompressionWhenAvailable(false);
    mTerrainGlobalOptions->setSkirtSize(mSkirtSize->get());
    mTerrainGlobalOptions->setUseRayBoxDistanceCalculation(mUseRayBoxDistanceCalculation->get());

    if(mMaterialGeneratorType->get() == 1)
    {
        Ogre::TerrainMaterialGeneratorPtr matGenPtr(OGRE_NEW Ogre::TerrainMaterialGeneratorC(this, mDecalFrustum));
        mTerrainGlobalOptions->setDefaultMaterialGenerator(matGenPtr);
        mMaxLayersAllowed = 10;
    }
    else
    {
        Ogre::TerrainMaterialGeneratorPtr matGenPtr(OGRE_NEW Ogre::TerrainMaterialGeneratorB(this, mDecalFrustum));
        mTerrainGlobalOptions->setDefaultMaterialGenerator(matGenPtr);
        mMaxLayersAllowed = 6;
    }

    CSceneManagerEditor *mSceneMgr = static_cast<CSceneManagerEditor*>(mOgitorsRoot->GetSceneManagerEditor());

    if(mSceneMgr->getShadowsEnabled())
    {
        if(mMaterialGeneratorType->get() == 1)
        {
            Ogre::TerrainMaterialGeneratorC::SM2Profile* matProfile = static_cast<Ogre::TerrainMaterialGeneratorC::SM2Profile*>(mTerrainGlobalOptions->getDefaultMaterialGenerator()->getActiveProfile());
            matProfile->setReceiveDynamicShadowsEnabled(true);
            matProfile->setReceiveDynamicShadowsLowLod(false);
            matProfile->setReceiveDynamicShadowsDepth(true);
            matProfile->setReceiveDynamicShadowsPSSM(static_cast<Ogre::PSSMShadowCameraSetup*>(mSceneMgr->getPSSMSetup().get()));
        }
        else
        {
            Ogre::TerrainMaterialGeneratorB::SM2Profile* matProfile = static_cast<Ogre::TerrainMaterialGeneratorB::SM2Profile*>(mTerrainGlobalOptions->getDefaultMaterialGenerator()->getActiveProfile());
            matProfile->setReceiveDynamicShadowsEnabled(true);
            matProfile->setReceiveDynamicShadowsLowLod(false);
            matProfile->setReceiveDynamicShadowsDepth(true);
            matProfile->setReceiveDynamicShadowsPSSM(static_cast<Ogre::PSSMShadowCameraSetup*>(mSceneMgr->getPSSMSetup().get()));
        }
    }

    CONNECT_PROPERTY_MEMFN(mSceneMgr, "shadows::enabled", CTerrainGroupEditor, onShadowsChange, mShadowsConnection[0]);
    CONNECT_PROPERTY_MEMFN(mSceneMgr, "shadows::technique", CTerrainGroupEditor, onShadowsTechniqueChange, mShadowsConnection[1]);

    mHandle = OGRE_NEW Ogre::TerrainGroup(mOgitorsRoot->GetSceneManager() ,Ogre::Terrain::ALIGN_X_Z, mMapSize->get(), mWorldSize->get());
    mHandle->setOrigin(Ogre::Vector3::ZERO);
    mHandle->setResourceGroup("TerrainResources");
    mHandle->setFilenameConvention(mPageNamePrefix->get(), "ogt");
    StaticGroupPtr = mHandle;

    mPGHandle = new PagedGeometry(mOgitorsRoot->GetViewport()->getCameraEditor()->getCamera(), mPGPageSize->get());
    mPGHandle->addDetailLevel<GrassPage>(mPGDetailDistance->get());

    //Create a GrassLoader object
    mGrassLoaderHandle = new GrassLoader(mPGHandle);
    mGrassLoaderHandle->setVisibilityFlags(1 << mLayer->get());

    //Assign the "grassLoader" to be used to load geometry for the PagedGrass instance
    mPGHandle->setPageLoader(mGrassLoaderHandle);

    //Supply a height function to GrassLoader so it can calculate grass Y values
    mGrassLoaderHandle->setHeightFunction(OgitorTerrainGroupHeightFunction);

    Ogre::Vector3 vDir;
    Ogre::ColourValue cDiffuse;
    Ogre::SceneManager::MovableObjectIterator mit = mOgitorsRoot->GetSceneManager()->getMovableObjectIterator("Light");
    while(mit.hasMoreElements())
    {
        Ogre::Light *l = static_cast<Ogre::Light*>(mit.getNext());
        if(l->getType() == Ogre::Light::LT_DIRECTIONAL && l->getCastShadows())
        {
            vDir = l->getDerivedDirection();
            cDiffuse = l->getDiffuseColour();
            break;
        }
    }

    mTerrainGlobalOptions->setLightMapDirection(vDir);
    mTerrainGlobalOptions->setCompositeMapAmbient(mOgitorsRoot->GetSceneManager()->getAmbientLight());
    mTerrainGlobalOptions->setCompositeMapDiffuse(cDiffuse);

    registerForUpdates();

    mLoaded->set(true);
    return true;
}
//-----------------------------------------------------------------------------------------
bool CTerrainGroupEditor::unLoad()
{
    if(!mLoaded->get())
        return true;

    unRegisterForUpdates();

    unLoadAllChildren();
    destroyBoundingBox();

    mDecalFrustum->detachFromParent();
    OGRE_DELETE mDecalFrustum;
    mDecalNode->getParentSceneNode()->removeAndDestroyChild(mDecalNode->getName());
    mDecalFrustum = 0;
    mDecalNode = 0;
    mDecalTexture->unload();
    mDecalTexture.setNull();
    if(mBrushData)
    {
        OGRE_FREE(mBrushData, Ogre::MEMCATEGORY_GEOMETRY);
        mBrushData = 0;
    }

   if(mHandle)
   {
       OGRE_DELETE mHandle;
       mHandle = 0;
       StaticGroupPtr = 0;
   }

    if(mGrassLoaderHandle)
        OGRE_DELETE mGrassLoaderHandle;

    if(mPGHandle)
        OGRE_DELETE mPGHandle;

    mGrassLoaderHandle = 0;
    mPGHandle = 0;

    mLoaded->set(false);
    return true;
}
//-----------------------------------------------------------------------------------------
void CTerrainGroupEditor::createProperties(OgitorsPropertyValueMap &params)
{
    PROPERTY_PTR(mWorldSize, "pageworldsize",Ogre::Real, 512.0f,0, SETTER(Ogre::Real, CTerrainGroupEditor, _setWorldSize));
    PROPERTY_PTR(mMapSize, "pagemapsize",int, 129, 0, SETTER(int, CTerrainGroupEditor, _setMapSize));
    PROPERTY_PTR(mPageNamePrefix, "pagenameprefix",Ogre::String, "Page", 0, SETTER(Ogre::String, CTerrainGroupEditor, _setPageNamePrefix));
    PROPERTY_PTR(mMaterialGeneratorType, "materialgeneratortype",int, 0, 0, SETTER(int, CTerrainGroupEditor, _setMaterialGeneratorType));
    PROPERTY_PTR(mLightMapTextureSize, "lightmap::texturesize",int, 1024, 0, SETTER(int, CTerrainGroupEditor, _setLightMapTextureSize));
    PROPERTY_PTR(mBlendMapTextureSize, "blendmap::texturesize",int, 1024, 0, SETTER(int, CTerrainGroupEditor, _setBlendMapTextureSize));
    PROPERTY_PTR(mCompositeMapTextureSize, "tuning::compositemaptexturesize",int, 1024, 0, SETTER(int, CTerrainGroupEditor, _setCompositeMapTextureSize));
    PROPERTY_PTR(mColourMapEnabled, "colourmap::enabled", bool, false, 0, 0);
    PROPERTY_PTR(mColourMapTextureSize, "colourmap::texturesize",int, 128, 0, 0);
    PROPERTY_PTR(mCompositeMapDistance, "tuning::compositemapdistance",int, 2000, 0, SETTER(int, CTerrainGroupEditor, _setCompositeMapDistance));
    PROPERTY_PTR(mMaxPixelError, "tuning::maxpixelerror",int, 3, 0, SETTER(int, CTerrainGroupEditor, _setMaxPixelError));
    PROPERTY_PTR(mMinBatchSize, "tuning::minbatchsize",int, 33, 0, SETTER(int, CTerrainGroupEditor, _setMinBatchSize));
    PROPERTY_PTR(mMaxBatchSize, "tuning::maxbatchsize",int, 65, 0, SETTER(int, CTerrainGroupEditor, _setMaxBatchSize));
    PROPERTY_PTR(mSkirtSize, "tuning::skirtsize",int, 10, 0, SETTER(int, CTerrainGroupEditor, _setSkirtSize));
    PROPERTY_PTR(mUseRayBoxDistanceCalculation, "tuning::userayboxdistancecalculation",bool, false, 0, SETTER(bool, CTerrainGroupEditor, _setUseRayBoxDistanceCalculation));
    PROPERTY_PTR(mPGDensityMapSize, "pg::densitymapsize",int, 1024, 0, 0);
    PROPERTY_PTR(mPGPageSize, "pg::pagesize",int, 40, 0, SETTER(int, CTerrainGroupEditor, _setPGPageSize));
    PROPERTY_PTR(mPGDetailDistance, "pg::detaildistance",int, 240, 0, SETTER(int, CTerrainGroupEditor, _setPGDetailDistance));

    mProperties.initValueMap(params);
}
//-----------------------------------------------------------------------------------------
bool CTerrainGroupEditor::_setMapSize(OgitorsPropertyBase* property, const int& value)
{
    mHandle->setTerrainSize(value);

    return true;
}
//-----------------------------------------------------------------------------------------
bool CTerrainGroupEditor::_setWorldSize(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(value < 0.0f)
        return false;

    mHandle->setTerrainWorldSize(value);

    Ogitors::NameObjectPairList::iterator it;

    for(it = mChildren.begin(); it != mChildren.end();it++)
    {
        CTerrainPageEditor *child = static_cast<CTerrainPageEditor*>(it->second);
        Ogre::Vector3 newpos;
        mHandle->convertTerrainSlotToWorldPosition(child->mPageX->get(), child->mPageY->get(), &newpos);
        child->mPosition->set(newpos);
    }

    if(mOgitorsRoot->GetPagingEditorObject())
    {
        static_cast<OgitorsProperty<Ogre::Real>*>(mOgitorsRoot->GetPagingEditorObject()->getProperties()->getProperty("terrainCellSize"))->set(value);
        static_cast<OgitorsProperty<Ogre::Real>*>(mOgitorsRoot->GetPagingEditorObject()->getProperties()->getProperty("terrainLoadRadius"))->set(value * 0.45f);
        static_cast<OgitorsProperty<Ogre::Real>*>(mOgitorsRoot->GetPagingEditorObject()->getProperties()->getProperty("terrainHoldRadius"))->set(value * 0.90f);
    }

    return true;
}
//-----------------------------------------------------------------------------------------
bool CTerrainGroupEditor::_setMaterialGeneratorType(OgitorsPropertyBase* property, const int& value)
{
    if(value == 1)
    {
        Ogre::TerrainMaterialGeneratorPtr matGenPtr(OGRE_NEW Ogre::TerrainMaterialGeneratorC(this, mDecalFrustum));
        mTerrainGlobalOptions->setDefaultMaterialGenerator(matGenPtr);
        mMaxLayersAllowed = 10;
    }
    else
    {
        Ogre::TerrainMaterialGeneratorPtr matGenPtr(OGRE_NEW Ogre::TerrainMaterialGeneratorB(this, mDecalFrustum));
        mTerrainGlobalOptions->setDefaultMaterialGenerator(matGenPtr);
        mMaxLayersAllowed = 6;
    }

    CSceneManagerEditor *mSceneMgr = static_cast<CSceneManagerEditor*>(mOgitorsRoot->GetSceneManagerEditor());

    if(mSceneMgr->getShadowsEnabled())
    {
        if(value == 1)
        {
            Ogre::TerrainMaterialGeneratorC::SM2Profile* matProfile = static_cast<Ogre::TerrainMaterialGeneratorC::SM2Profile*>(mTerrainGlobalOptions->getDefaultMaterialGenerator()->getActiveProfile());
            matProfile->setReceiveDynamicShadowsEnabled(true);
            matProfile->setReceiveDynamicShadowsLowLod(false);
            matProfile->setReceiveDynamicShadowsDepth(true);
            matProfile->setReceiveDynamicShadowsPSSM(static_cast<Ogre::PSSMShadowCameraSetup*>(mSceneMgr->getPSSMSetup().get()));
        }
        else
        {
            Ogre::TerrainMaterialGeneratorB::SM2Profile* matProfile = static_cast<Ogre::TerrainMaterialGeneratorB::SM2Profile*>(mTerrainGlobalOptions->getDefaultMaterialGenerator()->getActiveProfile());
            matProfile->setReceiveDynamicShadowsEnabled(true);
            matProfile->setReceiveDynamicShadowsLowLod(false);
            matProfile->setReceiveDynamicShadowsDepth(true);
            matProfile->setReceiveDynamicShadowsPSSM(static_cast<Ogre::PSSMShadowCameraSetup*>(mSceneMgr->getPSSMSetup().get()));
        }
    }

    return true;
}
//-----------------------------------------------------------------------------------------
bool CTerrainGroupEditor::_setLightMapTextureSize(OgitorsPropertyBase* property, const int& value)
{
    mTerrainGlobalOptions->setLightMapSize(value);

    return true;
}
//-----------------------------------------------------------------------------------------
bool CTerrainGroupEditor::_setBlendMapTextureSize(OgitorsPropertyBase* property, const int& value)
{
    mTerrainGlobalOptions->setLayerBlendMapSize(value);

    return true;
}
//-----------------------------------------------------------------------------------------
bool CTerrainGroupEditor::_setPageNamePrefix(OgitorsPropertyBase* property, const Ogre::String& value)
{
    Ogre::String newprefix = value;
    Ogre::StringUtil::trim(newprefix);

    if(newprefix.empty())
        return false;

    mHandle->setFilenamePrefix(newprefix);

    char name[1000];
    Ogitors::NameObjectPairList::iterator it;

    for(it = mChildren.begin(); it != mChildren.end();it++)
    {
        CTerrainPageEditor *child = static_cast<CTerrainPageEditor*>(it->second);
        sprintf(name,"%s%03dx%03d", mPageNamePrefix->get().c_str(), child->mPageX->get(), child->mPageY->get());
        child->setName(name);
    }

    return true;
}
//-----------------------------------------------------------------------------------------
bool CTerrainGroupEditor::_setMaxPixelError(OgitorsPropertyBase* property, const int& value)
{
    if(value < 0)
        return false;

    mTerrainGlobalOptions->setMaxPixelError(value);

    return true;
}
//-----------------------------------------------------------------------------------------
bool CTerrainGroupEditor::_setMinBatchSize(OgitorsPropertyBase* property, const int& value)
{
    if(mSystem->DisplayMessageDialog(OTR("Changing Min Batch Size requires Terrain to re-load.\nDo you want to continue?"), DLGTYPE_YESNO) == DLGRET_NO)
        return false;

    return true;
}
//-----------------------------------------------------------------------------------------
bool CTerrainGroupEditor::_setMaxBatchSize(OgitorsPropertyBase* property, const int& value)
{
    if(mSystem->DisplayMessageDialog(OTR("Changing Max Batch Size requires Terrain to re-load.\nDo you want to continue?"), DLGTYPE_YESNO) == DLGRET_NO)
        return false;

    return true;
}
//-----------------------------------------------------------------------------------------
bool CTerrainGroupEditor::_setSkirtSize(OgitorsPropertyBase* property, const int& value)
{
    if((value > 0) && mSystem->DisplayMessageDialog(OTR("Changing Skirt Size requires Terrain to re-load.\nDo you want to continue?"), DLGTYPE_YESNO) == DLGRET_NO)
        return false;

    return true;
}
//-----------------------------------------------------------------------------------------
bool CTerrainGroupEditor::_setUseRayBoxDistanceCalculation(OgitorsPropertyBase* property, const bool& value)
{
    if(mSystem->DisplayMessageDialog(OTR("Changing Distance Calculation method requires Terrain to re-load.\nDo you want to continue?"), DLGTYPE_YESNO) == DLGRET_NO)
        return false;

    return true;
}
//-----------------------------------------------------------------------------------------
bool CTerrainGroupEditor::_setCompositeMapTextureSize(OgitorsPropertyBase* property, const int& value)
{
    if(mSystem->DisplayMessageDialog(OTR("Changing Composite Map Texture Size requires Terrain to re-load.\nDo you want to continue?"), DLGTYPE_YESNO) == DLGRET_NO)
        return false;

    mTerrainGlobalOptions->setCompositeMapSize(value);

    return true;
}
//-----------------------------------------------------------------------------------------
bool CTerrainGroupEditor::_setCompositeMapDistance(OgitorsPropertyBase* property, const int& value)
{
    if(value < 1)
        return false;

    mTerrainGlobalOptions->setCompositeMapDistance(value);

    Ogre::TerrainGroup::TerrainIterator it = mHandle->getTerrainIterator();

    Ogre::Rect drect(0, 0, mMapSize->get(), mMapSize->get());

    while(it.hasMoreElements())
    {
        Ogre::TerrainGroup::TerrainSlot *terrain = it.getNext();
        if(terrain && terrain->instance)
            terrain->instance->dirtyRect(drect);
    }

    mHandle->update();

    return true;
}
//-----------------------------------------------------------------------------------------
bool CTerrainGroupEditor::_setPGPageSize(OgitorsPropertyBase* property, const int& value)
{
    if(value < 10)
        return false;

    mPGHandle->removeDetailLevels();
    mPGHandle->setPageSize(value);
    mPGHandle->addDetailLevel<GrassPage>(mPGDetailDistance->get());
    return true;
}
//-----------------------------------------------------------------------------------------
bool CTerrainGroupEditor::_setPGDetailDistance(OgitorsPropertyBase* property, const int& value)
{
    if(value < 100)
        return false;

    mPGHandle->removeDetailLevels();
    mPGHandle->addDetailLevel<GrassPage>(value);
    return true;
}
//-----------------------------------------------------------------------------------------
void CTerrainGroupEditor::setSelectedImpl(bool bSelected)
{
    CBaseEditor::setSelectedImpl(bSelected);

    mDecalFrustum->setVisible(bSelected && (mEditMode != EM_NONE));
    mDecalNode->setPosition(Ogre::Vector3(999999,-999999,999999));
}
//-----------------------------------------------------------------------------------------
TiXmlElement* CTerrainGroupEditor::exportDotScene(TiXmlElement *pParent)
{
    TiXmlElement *pTerrain = pParent->InsertEndChild(TiXmlElement("terrain"))->ToElement();
    pTerrain->SetAttribute("worldSize", Ogre::StringConverter::toString(mWorldSize->get()).c_str());
    pTerrain->SetAttribute("mapSize", Ogre::StringConverter::toString(mMapSize->get()).c_str());
    pTerrain->SetAttribute("pagenameprefix", mPageNamePrefix->get().c_str());
    pTerrain->SetAttribute("colourmapEnabled", Ogre::StringConverter::toString(mColourMapEnabled->get()).c_str());
    pTerrain->SetAttribute("colourMapTextureSize", Ogre::StringConverter::toString(mColourMapTextureSize->get()).c_str());
    pTerrain->SetAttribute("tuningCompositeMapDistance", Ogre::StringConverter::toString(mCompositeMapDistance->get()).c_str());
    pTerrain->SetAttribute("tuningMaxPixelError", Ogre::StringConverter::toString(mMaxPixelError->get()).c_str());
    pTerrain->SetAttribute("tuningMinBatchSize", Ogre::StringConverter::toString(mMinBatchSize->get()).c_str());
    pTerrain->SetAttribute("tuningMaxBatchSize", Ogre::StringConverter::toString(mMaxBatchSize->get()).c_str());
    pTerrain->SetAttribute("tuningSkirtSize", Ogre::StringConverter::toString(mSkirtSize->get()).c_str());
    pTerrain->SetAttribute("tuningUseRayBoxDistancealculation", Ogre::StringConverter::toString(mUseRayBoxDistanceCalculation->get()).c_str());

    TiXmlElement *pTerrainPages = pTerrain->InsertEndChild(TiXmlElement("terrainPages"))->ToElement();
    NameObjectPairList::const_iterator it = mChildren.begin();
    while(it != mChildren.end())
    {
        CTerrainPageEditor *terrain = static_cast<CTerrainPageEditor*>(it->second);
        terrain->exportDotScene(pTerrainPages);
        it++;
    }

    return pTerrain;
}
//-----------------------------------------------------------------------------------------
void CTerrainGroupEditor::onShadowsChange(const OgitorsPropertyBase* property, Ogre::Any value)
{
    bool newstate = Ogre::any_cast<bool>(value);

    CSceneManagerEditor *mSceneMgr = static_cast<CSceneManagerEditor*>(mOgitorsRoot->GetSceneManagerEditor());

    if(mSceneMgr->getShadowsTechnique() >= Ogre::SHADOWTYPE_TEXTURE_ADDITIVE && mSceneMgr->getShadowsTechnique() <= Ogre::SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED)
    {
        if(mMaterialGeneratorType->get() == 1)
        {
            Ogre::TerrainMaterialGeneratorC::SM2Profile* matProfile = static_cast<Ogre::TerrainMaterialGeneratorC::SM2Profile*>(mTerrainGlobalOptions->getDefaultMaterialGenerator()->getActiveProfile());
            matProfile->setReceiveDynamicShadowsEnabled(newstate);
            matProfile->setReceiveDynamicShadowsLowLod(false);
            matProfile->setReceiveDynamicShadowsDepth(newstate);
            matProfile->setReceiveDynamicShadowsPSSM(static_cast<Ogre::PSSMShadowCameraSetup*>(mSceneMgr->getPSSMSetup().get()));
        }
        else
        {
            Ogre::TerrainMaterialGeneratorB::SM2Profile* matProfile = static_cast<Ogre::TerrainMaterialGeneratorB::SM2Profile*>(mTerrainGlobalOptions->getDefaultMaterialGenerator()->getActiveProfile());
            matProfile->setReceiveDynamicShadowsEnabled(newstate);
            matProfile->setReceiveDynamicShadowsLowLod(false);
            matProfile->setReceiveDynamicShadowsDepth(newstate);
            matProfile->setReceiveDynamicShadowsPSSM(static_cast<Ogre::PSSMShadowCameraSetup*>(mSceneMgr->getPSSMSetup().get()));
        }
    }
}
//-----------------------------------------------------------------------------------------
void CTerrainGroupEditor::onShadowsTechniqueChange(const OgitorsPropertyBase* property, Ogre::Any value)
{

    CSceneManagerEditor *mSceneMgr = static_cast<CSceneManagerEditor*>(mOgitorsRoot->GetSceneManagerEditor());
    bool newstate = mSceneMgr->getShadowsEnabled();

    if(mSceneMgr->getShadowsTechnique() >= Ogre::SHADOWTYPE_TEXTURE_ADDITIVE && mSceneMgr->getShadowsTechnique() <= Ogre::SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED)
    {
        if(mMaterialGeneratorType->get() == 1)
        {
            Ogre::TerrainMaterialGeneratorC::SM2Profile* matProfile = static_cast<Ogre::TerrainMaterialGeneratorC::SM2Profile*>(mTerrainGlobalOptions->getDefaultMaterialGenerator()->getActiveProfile());
            matProfile->setReceiveDynamicShadowsEnabled(newstate);
            matProfile->setReceiveDynamicShadowsLowLod(false);
            matProfile->setReceiveDynamicShadowsDepth(newstate);
            matProfile->setReceiveDynamicShadowsPSSM(static_cast<Ogre::PSSMShadowCameraSetup*>(mSceneMgr->getPSSMSetup().get()));
        }
        else
        {
            Ogre::TerrainMaterialGeneratorB::SM2Profile* matProfile = static_cast<Ogre::TerrainMaterialGeneratorB::SM2Profile*>(mTerrainGlobalOptions->getDefaultMaterialGenerator()->getActiveProfile());
            matProfile->setReceiveDynamicShadowsEnabled(newstate);
            matProfile->setReceiveDynamicShadowsLowLod(false);
            matProfile->setReceiveDynamicShadowsDepth(newstate);
            matProfile->setReceiveDynamicShadowsPSSM(static_cast<Ogre::PSSMShadowCameraSetup*>(mSceneMgr->getPSSMSetup().get()));
        }
    }
}
//-----------------------------------------------------------------------------------------
//------CTERRAINGROUPEDITORFACTORY---------------------------------------------------------
//-----------------------------------------------------------------------------------------
CTerrainGroupEditorFactory::CTerrainGroupEditorFactory(OgitorsView *view) : CBaseEditorFactory(view)
{
    mTypeName = "Terrain Group Object";
    mEditorType = ETYPE_TERRAIN_MANAGER;
    mAddToObjectList = true;
    mIcon = "pagedterrainmanager.svg";
    mCapabilities = CAN_DELETE | CAN_UNDO;

    mMinBatchSizes.clear();
    mMinBatchSizes.push_back(PropertyOption("5", Ogre::Any((int)5)));
    mMinBatchSizes.push_back(PropertyOption("9", Ogre::Any((int)9)));
    mMinBatchSizes.push_back(PropertyOption("17", Ogre::Any((int)17)));
    mMinBatchSizes.push_back(PropertyOption("33", Ogre::Any((int)33)));

    mMaxBatchSizes.clear();
    mMaxBatchSizes.push_back(PropertyOption("33", Ogre::Any((int)33)));
    mMaxBatchSizes.push_back(PropertyOption("65", Ogre::Any((int)65)));
    mMaxBatchSizes.push_back(PropertyOption("129", Ogre::Any((int)129)));

    mMapSizeOptions.clear();
    mMapSizeOptions.push_back(PropertyOption("128x128", Ogre::Any((int)129)));
    mMapSizeOptions.push_back(PropertyOption("256x256", Ogre::Any((int)257)));
    mMapSizeOptions.push_back(PropertyOption("512x512", Ogre::Any((int)513)));
    mMapSizeOptions.push_back(PropertyOption("1024x1024", Ogre::Any((int)1025)));

    mColourMapSizeOptions.clear();
    mColourMapSizeOptions.push_back(PropertyOption("128x128", Ogre::Any((int)128)));
    mColourMapSizeOptions.push_back(PropertyOption("256x256", Ogre::Any((int)256)));
    mColourMapSizeOptions.push_back(PropertyOption("512x512", Ogre::Any((int)512)));
    mColourMapSizeOptions.push_back(PropertyOption("1024x1024", Ogre::Any((int)1024)));
    mColourMapSizeOptions.push_back(PropertyOption("2048x2048", Ogre::Any((int)2048)));

    mMaterialGeneratorTypes.clear();
    mMaterialGeneratorTypes.push_back(PropertyOption("Default", Ogre::Any((int)0)));
    mMaterialGeneratorTypes.push_back(PropertyOption("Ogitor10", Ogre::Any((int)1)));

    AddPropertyDefinition("pageworldsize","World Size", "The size of page in world coordinates.",PROP_REAL);
    OgitorsPropertyDef *definition = AddPropertyDefinition("pagemapsize","Map Size", "The size of page in vertices per side.",PROP_INT);
    definition->setOptions(&mMapSizeOptions);
    AddPropertyDefinition("pagenameprefix","Page Name Prefix", "The Prefix to be added to page names.",PROP_STRING);
    definition = AddPropertyDefinition("materialgeneratortype","Mat. Generator", "The material generator to be used.",PROP_INT);
    definition->setOptions(&mMaterialGeneratorTypes);
    definition = AddPropertyDefinition("lightmap::texturesize","Light Map::Texture Size", "The size of lightmap texture.",PROP_INT);
    definition->setOptions(&mColourMapSizeOptions);
    definition = AddPropertyDefinition("blendmap::texturesize","Blend Map::Texture Size", "The size of blendmap texture.",PROP_INT);
    definition->setOptions(&mColourMapSizeOptions);
    definition = AddPropertyDefinition("tuning::compositemaptexturesize","Tuning::Composite Map Size", "The size of compositemap texture.",PROP_INT);
    definition->setOptions(&mColourMapSizeOptions);
    AddPropertyDefinition("colourmap::enabled","Colour Map::Enabled", "Is the colourmap enabled?",PROP_BOOL);
    definition = AddPropertyDefinition("colourmap::texturesize","Colour Map::Texture Size", "The size of colourmap texture.",PROP_INT);
    definition->setOptions(&mColourMapSizeOptions);

    AddPropertyDefinition("tuning::compositemapdistance","Tuning::Composite Map Dist.", "The distance at which composite map will be used for terrain texturing", PROP_INT);
    AddPropertyDefinition("tuning::maxpixelerror","Tuning::Max Pixel Error", "Maximum Pixel Error for LOD", PROP_INT);
    definition = AddPropertyDefinition("tuning::minbatchsize","Tuning::Min Batch Size", "Minimum Batch Size.",PROP_INT);
    definition->setOptions(&mMinBatchSizes);
    definition = AddPropertyDefinition("tuning::maxbatchsize","Tuning::Max Batch Size", "Maximum Batch Size.",PROP_INT);
    definition->setOptions(&mMaxBatchSizes);
    AddPropertyDefinition("tuning::skirtsize","Tuning::Skirt Size", "Skirt Size.",PROP_INT);
    AddPropertyDefinition("tuning::userayboxdistancecalculation","Tuning::Adv. Dist. Calc.", "Use Ray Box Distance Calculation for LOD.",PROP_BOOL);

    definition = AddPropertyDefinition("pg::densitymapsize","Paged Grass::Density Map Size", "", PROP_INT);
    definition->setOptions(&mColourMapSizeOptions);
    AddPropertyDefinition("pg::pagesize","Paged Grass::Page Size", "", PROP_INT);
    AddPropertyDefinition("pg::detaildistance","Paged Grass::Detail Distance", "", PROP_INT);

    OgitorsPropertyDefMap::iterator it = mPropertyDefs.find("name");
    it->second.setAccess(true, false);
}
//-----------------------------------------------------------------------------------------
CBaseEditorFactory *CTerrainGroupEditorFactory::duplicate(OgitorsView *view)
{
    CBaseEditorFactory *ret = OGRE_NEW CTerrainGroupEditorFactory(view);
    ret->mTypeID = mTypeID;

    return ret;
}
//-----------------------------------------------------------------------------------------
CBaseEditor *CTerrainGroupEditorFactory::CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params)
{

    OgitorsPropertyValueMap::iterator ni;

    if ((ni = params.find("init")) != params.end())
    {
        Ogre::String value = OgitorsRoot::getSingletonPtr()->GetProjectOptions()->TerrainDirectory + "/";
        OgitorsRoot::getSingletonPtr()->GetProjectFile()->createDirectory(value.c_str());

        value = "/terrainTextures/";
        OgitorsRoot::getSingletonPtr()->GetProjectFile()->createDirectory(value.c_str());

        Ogre::String copydir = Ogitors::Globals::MEDIA_PATH + "/terrainTextures/";
        OgitorsUtils::CopyDirOfs(copydir, value + "/");
        params.erase(ni);
    }

    CTerrainGroupEditor *object = OGRE_NEW CTerrainGroupEditor(this);

    object->createProperties(params);
    object->mParentEditor->init(*parent);
    object->load();

    mInstanceCount++;
    return object;
}
//-----------------------------------------------------------------------------------------
bool CTerrainGroupEditorFactory::CanInstantiate()
{
    return (OgitorsRoot::getSingletonPtr()->GetTerrainEditor() == 0);
}
//-----------------------------------------------------------------------------------------
