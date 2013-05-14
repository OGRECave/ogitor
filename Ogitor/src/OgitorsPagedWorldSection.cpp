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
#include "OgreGrid2DPageStrategy.h"
#include "OgrePagedWorld.h"
#include "OgrePagedWorldSection.h"
#include "OgrePageManager.h"
#include "OgitorsPagedWorldSection.h"
#include "BaseEditor.h"
#include "ViewportEditor.h"
#include "CameraEditor.h"
#include "OgitorsRoot.h"

using namespace Ogitors;

//---------------------------------------------------------------------
OgitorPage::OgitorPage() : 
mX(0), mY(0), mExtents(Ogre::Vector4::ZERO), mLoaded(false)
{
}
//---------------------------------------------------------------------
OgitorPage::OgitorPage(long x, long y) : 
mX(x), mY(y), mExtents(Ogre::Vector4::ZERO), mLoaded(false)
{
}
//---------------------------------------------------------------------
OgitorPage::~OgitorPage()
{
    mObjects.clear();
}
//---------------------------------------------------------------------
void OgitorPage::addObject(CBaseEditor *object)
{
    NameObjectPairList::const_iterator it = mObjects.find(object->getName());
    if(it == mObjects.end())
    {
        mObjects.insert(NameObjectPairList::value_type(object->getName(), object));
        if(mLoaded)
            object->load();
        else
            object->unLoad();
    }
}
//-----------------------------------------------------------------------------------------
void OgitorPage::removeObject(CBaseEditor *object)
{
    NameObjectPairList::iterator it = mObjects.find(object->getName());
    if(it != mObjects.end())
    {
        mObjects.erase(it);
    }
}
//-----------------------------------------------------------------------------------------
void OgitorPage::load(bool forceSynchronous)
{
    if(mLoaded)
        return;

    NameObjectPairList::const_iterator it = mObjects.begin();
    while(it != mObjects.end())
    {
        it->second->load();
        it++;
    }
    mLoaded = true;
}
//---------------------------------------------------------------------
void OgitorPage::unLoad(bool forceSynchronous)
{
    if(!mLoaded)
        return;

    NameObjectPairList::const_iterator it = mObjects.begin();
    while(it != mObjects.end())
    {
        it->second->unLoad();
        it++;
    }
    mLoaded = false;
}
//---------------------------------------------------------------------


//---------------------------------------------------------------------
//---------------------------------------------------------------------
//---------------------------------------------------------------------
OgitorPagedWorldSection::OgitorPagedWorldSection(const Ogre::String& name, Ogre::PagedWorld* parent, Ogre::SceneManager* sm)
: Ogre::PagedWorldSection(name, parent, sm)
{
    // we always use a grid strategy
    setStrategy(parent->getManager()->getStrategy("Grid2D"));
    getGridStrategyData()->setCellRange(0, 0, 0, 0);
    mDefaultPage.load(true);
}
//---------------------------------------------------------------------
OgitorPagedWorldSection::~OgitorPagedWorldSection()
{
    OgitorPageMap::iterator pit = mOgitorPages.begin();
    while(pit != mOgitorPages.end())
    {
        OGRE_DELETE pit->second;
        pit++;
    }
}
//---------------------------------------------------------------------
void OgitorPagedWorldSection::syncSettings()
{
}
//---------------------------------------------------------------------
void OgitorPagedWorldSection::setLoadRadius(Ogre::Real sz)
{
    getGridStrategyData()->setLoadRadius(sz);
}
//---------------------------------------------------------------------
Ogre::Real OgitorPagedWorldSection::getLoadRadius() const
{
    return getGridStrategyData()->getLoadRadius();
}
//---------------------------------------------------------------------
void OgitorPagedWorldSection::setHoldRadius(Ogre::Real sz)
{
    getGridStrategyData()->setHoldRadius(sz);
}
//---------------------------------------------------------------------
Ogre::Real OgitorPagedWorldSection::getHoldRadius()
{
    return getGridStrategyData()->getHoldRadius();
}
//---------------------------------------------------------------------
void OgitorPagedWorldSection::setCellSize(Ogre::Real sz)
{
    getGridStrategyData()->setCellSize(sz);
}
//---------------------------------------------------------------------
Ogre::Real OgitorPagedWorldSection::getCellSize()
{
    return getGridStrategyData()->getCellSize();
}
//---------------------------------------------------------------------
Ogre::Grid2DPageStrategy* OgitorPagedWorldSection::getGridStrategy() const
{
    return static_cast<Ogre::Grid2DPageStrategy*>(this->getStrategy());
}
//---------------------------------------------------------------------
Ogre::Grid2DPageStrategyData* OgitorPagedWorldSection::getGridStrategyData() const
{
    return static_cast<Ogre::Grid2DPageStrategyData*>(mStrategyData);
}
//---------------------------------------------------------------------
void OgitorPagedWorldSection::loadSubtypeData(Ogre::StreamSerialiser& ser)
{
}
//---------------------------------------------------------------------
void OgitorPagedWorldSection::saveSubtypeData(Ogre::StreamSerialiser& ser)
{
}
//---------------------------------------------------------------------
void OgitorPagedWorldSection::_cleanup()
{
    mObjects.clear();

    OgitorPageMap::iterator pit = mOgitorPages.begin();
    while(pit != mOgitorPages.end())
    {
        OGRE_DELETE pit->second;
        pit++;
    }

    mOgitorPages.clear();
}
//---------------------------------------------------------------------
void OgitorPagedWorldSection::addObject(CBaseEditor *object)
{
    ObjectPagePairList::const_iterator it = mObjects.find(object);
    if(it == mObjects.end())
    {
        OgitorPage *page = &mDefaultPage;

        if(object->supports(CAN_PAGE))
        {
            Ogre::Vector2 pos;
            Ogre::Vector3 obj_pos = object->getDerivedPosition();
            
            Ogre::int32 x, y;
            getGridStrategyData()->convertWorldToGridSpace(obj_pos, pos);
            getGridStrategyData()->determineGridLocation(pos, &x, &y);
            Ogre::PageID id = getGridStrategyData()->calculatePageID(x, y);

            OgitorPageMap::iterator pit = mOgitorPages.find(id);
            if(pit != mOgitorPages.end())
            {
                page = pit->second;
            }
            else
            {
                page = OGRE_NEW OgitorPage(x, y);
                Ogre::Vector4 extents(x * getGridStrategyData()->getCellSize() , y * getGridStrategyData()->getCellSize() , (x + 1) * getGridStrategyData()->getCellSize() , (y + 1) * getGridStrategyData()->getCellSize()); 
                page->setExtents(extents);

                if(x < getGridStrategyData()->getCellRangeMinX())
                    getGridStrategyData()->setCellRangeMinX(x);

                if(x > getGridStrategyData()->getCellRangeMaxX())
                    getGridStrategyData()->setCellRangeMaxX(x);

                if(y < getGridStrategyData()->getCellRangeMinY())
                    getGridStrategyData()->setCellRangeMinY(y);

                if(y > getGridStrategyData()->getCellRangeMaxY())
                    getGridStrategyData()->setCellRangeMaxY(y);

                mOgitorPages.insert(OgitorPageMap::value_type(id, page));

                // set state of new page to "loaded" if its in the loaded cells range
                // nothing is actually being loaded here since the objects list of the page is empty
                if(isPageLoaded(x, y))
                    page->load(false);
            }
        }

        page->addObject(object);
        mObjects.insert(ObjectPagePairList::value_type(object, page));
    }
}
//-----------------------------------------------------------------------------------------
void OgitorPagedWorldSection::updateObjectPage(CBaseEditor *object)
{
    ObjectPagePairList::iterator it = mObjects.find(object);
    if(it != mObjects.end())
    {
        OgitorPage *page = &mDefaultPage;

        if(object->supports(CAN_PAGE))
        {
            Ogre::Vector2 pos;
            Ogre::Vector3 obj_pos = object->getDerivedPosition();
            
            Ogre::int32 x, y;
            getGridStrategyData()->convertWorldToGridSpace(obj_pos, pos);
            getGridStrategyData()->determineGridLocation(pos, &x, &y);
            Ogre::PageID id = getGridStrategyData()->calculatePageID(x, y);

            OgitorPageMap::iterator pit = mOgitorPages.find(id);
            if(pit != mOgitorPages.end())
            {
                page = pit->second;
            }
            else
            {
                page = OGRE_NEW OgitorPage(x, y);
                Ogre::Vector4 extents(x * getGridStrategyData()->getCellSize() , y * getGridStrategyData()->getCellSize() , (x + 1) * getGridStrategyData()->getCellSize() , (y + 1) * getGridStrategyData()->getCellSize()); 
                page->setExtents(extents);

                if(x < getGridStrategyData()->getCellRangeMinX())
                    getGridStrategyData()->setCellRangeMinX(x);

                if(x > getGridStrategyData()->getCellRangeMaxX())
                    getGridStrategyData()->setCellRangeMaxX(x);

                if(y < getGridStrategyData()->getCellRangeMinY())
                    getGridStrategyData()->setCellRangeMinY(y);

                if(y > getGridStrategyData()->getCellRangeMaxY())
                    getGridStrategyData()->setCellRangeMaxY(y);

                mOgitorPages.insert(OgitorPageMap::value_type(id, page));

                // set state of new page to "loaded" if its in the loaded cells range
                // nothing is actually being loaded here since the objects list of the page is empty
                if(isPageLoaded(x, y))
                    page->load(false);
            }
        }

        if(page != it->second)
        {
            it->second->removeObject(object);
            page->addObject(object);
            it->second = page;
        }
    }
}
//-----------------------------------------------------------------------------------------
void OgitorPagedWorldSection::removeObject(CBaseEditor *object)
{
    ObjectPagePairList::iterator it = mObjects.find(object);
    if(it != mObjects.end())
    {
        it->second->removeObject(object);
        mObjects.erase(it);
    }
}
//-----------------------------------------------------------------------------------------
bool OgitorPagedWorldSection::isPageLoaded(int px, int py)
{
    Ogre::Grid2DPageStrategyData* stratData = static_cast<Ogre::Grid2DPageStrategyData*>(getStrategyData());

    const Ogre::Vector3& pos = OgitorsRoot::getSingletonPtr()->GetViewport()->getCameraEditor()->getCamera()->getDerivedPosition();
    Ogre::Vector2 gridpos;
    stratData->convertWorldToGridSpace(pos, gridpos);
    Ogre::int32 x, y;
    stratData->determineGridLocation(gridpos, &x, &y);

    Ogre::Real loadRadius = stratData->getLoadRadiusInCells();
    // scan the whole Hold range
    Ogre::Real fxmin = (Ogre::Real)x - loadRadius;
    Ogre::Real fxmax = (Ogre::Real)x + loadRadius;
    Ogre::Real fymin = (Ogre::Real)y - loadRadius;
    Ogre::Real fymax = (Ogre::Real)y + loadRadius;

    Ogre::int32 xmin = stratData->getCellRangeMinX();
    Ogre::int32 xmax = stratData->getCellRangeMaxX();
    Ogre::int32 ymin = stratData->getCellRangeMinY();
    Ogre::int32 ymax = stratData->getCellRangeMaxY();

    // Round UP max, round DOWN min
    xmin = fxmin < xmin ? xmin : (Ogre::int32)floor(fxmin);
    xmax = fxmax > xmax ? xmax : (Ogre::int32)ceil(fxmax);
    ymin = fymin < ymin ? ymin : (Ogre::int32)floor(fymin);
    ymax = fymax > ymax ? ymax : (Ogre::int32)ceil(fymax);

    return (px >= xmin && px <= xmax && py >= ymin && py <= ymax);
}
//-----------------------------------------------------------------------------------------
void OgitorPagedWorldSection::loadPage(Ogre::PageID pageID, bool forceSynchronous)
{
    if (!mParent->getManager()->getPagingOperationsEnabled())
        return;

    Ogre::PagedWorldSection::PageMap::iterator i = mPages.find(pageID);
    if (i == mPages.end())
    {
        OgitorPageMap::iterator it = mOgitorPages.find(pageID);
        if(it != mOgitorPages.end())
        {
            it->second->load(forceSynchronous);
        }
    }

    Ogre::PagedWorldSection::loadPage(pageID, forceSynchronous);
}
//---------------------------------------------------------------------
void OgitorPagedWorldSection::unloadPage(Ogre::PageID pageID, bool forceSynchronous)
{
    if (!mParent->getManager()->getPagingOperationsEnabled())
        return;

    Ogre::PagedWorldSection::unloadPage(pageID, forceSynchronous);

    OgitorPageMap::iterator it = mOgitorPages.find(pageID);
    if(it != mOgitorPages.end())
    {
        it->second->unLoad(forceSynchronous);
    }
}


