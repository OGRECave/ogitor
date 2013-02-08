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
#include "PagingEditor.h"
#include "BaseEditor.h"
#include "OgitorsRoot.h"
#include "OgitorsSystem.h"
#include "SceneManagerEditor.h"
#include "ViewportEditor.h"
#include "CameraEditor.h"
#include "OgitorsPaging.h"
#include "OgitorsPagedWorldSection.h"
#include "tinyxml.h"
#include "OgrePage.h"
#include "OgrePageManager.h"
#include "OgrePagedWorld.h"

using namespace Ogitors;

const Ogre::String CPagingManager::OgitorSectionFactory::FACTORY_NAME("OgitorSection");

namespace Ogitors
{

class DummyPageProvider : public Ogre::PageProvider
{
public:
    bool prepareProceduralPage(Ogre::Page* page, Ogre::PagedWorldSection* section) { return true; }
    bool loadProceduralPage(Ogre::Page* page, Ogre::PagedWorldSection* section) { return true; }
    bool unloadProceduralPage(Ogre::Page* page, Ogre::PagedWorldSection* section) { return true; }
    bool unprepareProceduralPage(Ogre::Page* page, Ogre::PagedWorldSection* section) { return true; }
};

DummyPageProvider mDummyPageProvider;
}

//-------------------------------------------------------------------------------
CPagingManager::CPagingManager(CBaseEditorFactory *factory) : CBaseEditor(factory),
mHandle(0)
{
    mName->init("Paging Manager");
    mSections.clear();
}
//---------------------------------------------------------------------
void CPagingManager::showBoundingBox(bool bShow) 
{
}
//-------------------------------------------------------------------------------
void CPagingManager::_cleanup()
{
    OgitorSectionMap::iterator it = mSections.begin();

    while(it != mSections.end())
    {
        it->second->_cleanup();
        it++;
    }
}
//-------------------------------------------------------------------------------
void CPagingManager::_kickStart()
{
    _cleanup();

    mOgitorsRoot->GetSceneManagerEditor()->_refreshPaging();
}
//-------------------------------------------------------------------------------
bool CPagingManager::_setCellSize(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    OgitorSectionMap::iterator it = mSections.find(SECT_GENERAL);

    if(it != mSections.end())
    {
        it->second->setCellSize(value);
        _kickStart();
    }

    return true;
}
//-------------------------------------------------------------------------------
bool CPagingManager::_setLoadRadius(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    OgitorSectionMap::iterator it = mSections.find(SECT_GENERAL);

    if(it != mSections.end())
    {
        it->second->setLoadRadius(value);
    }

    return true;
}
//-------------------------------------------------------------------------------
bool CPagingManager::_setHoldRadius(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    OgitorSectionMap::iterator it = mSections.find(SECT_GENERAL);

    if(it != mSections.end())
    {
        it->second->setHoldRadius(value);
    }

    return true;
}
//-------------------------------------------------------------------------------
bool CPagingManager::_setTerrainCellSize(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    OgitorSectionMap::iterator it = mSections.find(SECT_TERRAIN);

    if(it != mSections.end())
    {
        it->second->setCellSize(value);
        _kickStart();
    }

    return true;
}
//-------------------------------------------------------------------------------
bool CPagingManager::_setTerrainLoadRadius(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    OgitorSectionMap::iterator it = mSections.find(SECT_TERRAIN);

    if(it != mSections.end())
    {
        it->second->setLoadRadius(value);
    }

    return true;
}
//-------------------------------------------------------------------------------
bool CPagingManager::_setTerrainHoldRadius(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    OgitorSectionMap::iterator it = mSections.find(SECT_TERRAIN);

    if(it != mSections.end())
    {
        it->second->setHoldRadius(value);
    }

    return true;
}
//-------------------------------------------------------------------------------
bool CPagingManager::load(bool async)
{
    if(mLoaded->get())
        return true;

    if(!getParent()->load())
        return false;
    
    mHandle = OGRE_NEW Ogre::PageManager();
    // Since we're not loading any pages from .page files, we need a way just 
    // to say we've loaded them without them actually being loaded
    mHandle->setPageProvider(&mDummyPageProvider);
    mHandle->addCamera(mOgitorsRoot->GetViewport()->getCameraEditor()->getCamera());
    Ogre::PagedWorld* world = mHandle->createWorld("OgitorsWorld");

    mHandle->addWorldSectionFactory(&mSectionFactory);
    mSections[SECT_GENERAL] = static_cast<OgitorPagedWorldSection*>(
        world->createSection(mOgitorsRoot->GetSceneManager(), OgitorSectionFactory::FACTORY_NAME, "OgitorsGeneralSection"));

    mSections[SECT_GENERAL]->setCellSize(mCellSize->get());
    mSections[SECT_GENERAL]->setLoadRadius(mLoadRadius->get());
    mSections[SECT_GENERAL]->setHoldRadius(mHoldRadius->get());

    mSections[SECT_TERRAIN] = static_cast<OgitorPagedWorldSection*>(
        world->createSection(mOgitorsRoot->GetSceneManager(), OgitorSectionFactory::FACTORY_NAME, "OgitorsTerrainSection"));

    mSections[SECT_TERRAIN]->setCellSize(mTerrainCellSize->get());
    mSections[SECT_TERRAIN]->setLoadRadius(mTerrainLoadRadius->get());
    mSections[SECT_TERRAIN]->setHoldRadius(mTerrainHoldRadius->get());

    mLoaded->set(true);
    return true;
}
//-----------------------------------------------------------------------------------------
bool CPagingManager::unLoad()
{
    if(!mLoaded->get())
        return true;

    OgitorSectionMap::iterator it = mSections.begin();

    while(it != mSections.end())
    {
        it->second->_cleanup();
        it++;
    }

    mSections.clear();

    mHandle->destroyWorld("OgitorsWorld");
    OGRE_DELETE mHandle;
    mHandle = 0;

    mLoaded->set(false);
    return true;
}
//-----------------------------------------------------------------------------------------
void CPagingManager::createProperties(OgitorsPropertyValueMap &params)
{
    Ogre::Real tercellsize = 512.0f;

    if(mOgitorsRoot->GetTerrainEditorObject())
        tercellsize = static_cast<OgitorsProperty<Ogre::Real>*>(mOgitorsRoot->GetTerrainEditorObject()->getProperties()->getProperty("pageworldsize"))->get();


    PROPERTY_PTR(mCellSize, "cellSize",Ogre::Real, 256.0f,0, SETTER(Ogre::Real, CPagingManager, _setCellSize));
    PROPERTY_PTR(mLoadRadius, "loadRadius",Ogre::Real, 512.0f,0, SETTER(Ogre::Real, CPagingManager, _setLoadRadius));
    PROPERTY_PTR(mHoldRadius, "holdRadius",Ogre::Real, 768.0f,0, SETTER(Ogre::Real, CPagingManager, _setHoldRadius));
    PROPERTY_PTR(mTerrainCellSize, "terrainCellSize",Ogre::Real, tercellsize,0, SETTER(Ogre::Real, CPagingManager, _setTerrainCellSize));
    PROPERTY_PTR(mTerrainLoadRadius, "terrainLoadRadius",Ogre::Real, 800.0f,0, SETTER(Ogre::Real, CPagingManager, _setTerrainLoadRadius));
    PROPERTY_PTR(mTerrainHoldRadius, "terrainHoldRadius",Ogre::Real, 1000.0f,0, SETTER(Ogre::Real, CPagingManager, _setTerrainHoldRadius));

    mProperties.initValueMap(params);
}
//-----------------------------------------------------------------------------------------
void CPagingManager::addObject(CBaseEditor *object)
{
    if(object == this)
        return;

    OgitorSectionMap::iterator it = mSections.find(object->getDefaultWorldSection());

    if(it != mSections.end())
    {
        it->second->addObject(object);
    }
}
//-----------------------------------------------------------------------------------------
void CPagingManager::removeObject(CBaseEditor *object)
{
    if(object == this)
        return;

    OgitorSectionMap::iterator it = mSections.find(object->getDefaultWorldSection());

    if(it != mSections.end())
    {
        it->second->removeObject(object);
    }
}
//-----------------------------------------------------------------------------------------
void CPagingManager::updateObjectPage(CBaseEditor *object)
{
    if(object == this)
        return;

    OgitorSectionMap::iterator it = mSections.find(object->getDefaultWorldSection());

    if(it != mSections.end())
    {
        it->second->updateObjectPage(object);
    }
}
//-----------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
const Ogre::String& CPagingManager::OgitorSectionFactory::getName() const
{
    return FACTORY_NAME;
}
//---------------------------------------------------------------------
Ogre::PagedWorldSection* CPagingManager::OgitorSectionFactory::createInstance(const Ogre::String& name, Ogre::PagedWorld* parent, Ogre::SceneManager* sm)
{
    return OGRE_NEW OgitorPagedWorldSection(name, parent, sm);
}
//---------------------------------------------------------------------
void CPagingManager::OgitorSectionFactory::destroyInstance(Ogre::PagedWorldSection* s)
{
    OGRE_DELETE s;
}
//---------------------------------------------------------------------

//-------------------------------------------------------------------------------
//--------------CPAGINGEDITORFACTORY-------------------------------------------
//-------------------------------------------------------------------------------
CPagingManagerFactory::CPagingManagerFactory(OgitorsView *view) : CBaseEditorFactory(view)
{
    mTypeName = "Paging Manager";
    mEditorType = ETYPE_PAGINGMANAGER;
    mAddToObjectList = true;
    mIcon = "pagingmanager.svg";
    mCapabilities = CAN_DELETE | CAN_UNDO;
    mUsesGizmos = false;
    mUsesHelper = false;

    OgitorsPropertyDefMap::iterator it = mPropertyDefs.find("name");
    it->second.setAccess(true, false);

    it = mPropertyDefs.find("layer");
    it->second.setAccess(false, false);

    AddPropertyDefinition("cellSize","General Section::Cell Size", "The size of grid cells.",PROP_REAL);
    AddPropertyDefinition("loadRadius","General Section::Hold Radius", "The distance till which pages will be kept in memory.",PROP_REAL);
    AddPropertyDefinition("holdRadius","General Section::Hold Radius", "The distance till which pages will be kept in memory.",PROP_REAL);
    AddPropertyDefinition("terrainCellSize","Terrain Section::Cell Size", "The size of grid cells.",PROP_REAL,true,false);
    AddPropertyDefinition("terrainLoadRadius","Terrain Section::Load Radius", "The distance till which pages will be kept in memory.",PROP_REAL);
    AddPropertyDefinition("terrainHoldRadius","Terrain Section::Hold Radius", "The distance till which pages will be kept in memory.",PROP_REAL);
}
//-----------------------------------------------------------------------------------------
CBaseEditorFactory *CPagingManagerFactory::duplicate(OgitorsView *view)
{
    CBaseEditorFactory *ret = OGRE_NEW CPagingManagerFactory(view);
    ret->mTypeID = mTypeID;

    return ret;
}
//-----------------------------------------------------------------------------------------
CBaseEditor *CPagingManagerFactory::CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params)
{
    CPagingManager *object = OGRE_NEW CPagingManager(this);

    OgitorsPropertyValueMap::iterator ni;

    *parent = OgitorsRoot::getSingletonPtr()->GetRootEditor();
    object->createProperties(params);
    object->mParentEditor->init(*parent);

    object->load();

    if(OgitorsRoot::getSingletonPtr()->IsSceneLoaded())
        object->_kickStart();

    mInstanceCount++;
    return object;
}
//-------------------------------------------------------------------------------
bool CPagingManagerFactory::CanInstantiate()
{
    return (OgitorsRoot::getSingletonPtr()->GetPagingEditor() == 0);
}
//-----------------------------------------------------------------------------------------

