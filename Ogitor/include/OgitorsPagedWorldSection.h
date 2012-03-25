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

#pragma once

namespace Ogre
{
    class PagedWorldSection;
    class Grid2DPageStrategy;
    class Grid2DPageStrategyData;
}

namespace Ogitors
{
    class OgitorExport OgitorPage: public Ogre::GeneralAllocatedObject
    {
    public:
        OgitorPage();
        OgitorPage(long x, long y);
        ~OgitorPage();

        /**
        * Adds an editor Object
        * @param object the editor object to add
        */
        void addObject(CBaseEditor *object);
        /**
        * Removes an editor Object
        * @param object the editor object to remove
        */
        void removeObject(CBaseEditor *object);
        void load(bool forceSynchronous);
        void unLoad(bool forceSynchronous);

        long getX() { return mX; };
        long getY() { return mY; };
        const Ogre::Vector4& getExtents() { return mExtents; };
        void setExtents(const Ogre::Vector4& value) { mExtents = value; };
        bool getLoaded() { return mLoaded; };

    private:
        NameObjectPairList mObjects;
        long               mX;
        long               mY;
        Ogre::Vector4      mExtents;
        bool               mLoaded;

    };

    typedef std::map<CBaseEditor*, OgitorPage*> ObjectPagePairList;
    typedef std::map<Ogre::PageID, OgitorPage*> OgitorPageMap;

    class OgitorExport OgitorPagedWorldSection : public Ogre::PagedWorldSection
    {
    public:
        /** Constructor.
        @param name The name of the section
        @param parent The parent world
        @param sm The SceneManager to use (can be left as null if to be loaded)
        */
        OgitorPagedWorldSection(const Ogre::String& name, Ogre::PagedWorld* parent, Ogre::SceneManager* sm);
        ~OgitorPagedWorldSection();

        /// Get the loaded state 
        bool isPageLoaded(int px, int py);
        /// Set the loading radius 
        virtual void setLoadRadius(Ogre::Real sz);
        /// Get the loading radius 
        virtual Ogre::Real getLoadRadius() const;
        /// Set the Holding radius 
        virtual void setHoldRadius(Ogre::Real sz);
        /// Get the Holding radius 
        virtual Ogre::Real getHoldRadius();
        /// Set the Cell Size 
        virtual void setCellSize(Ogre::Real sz);
        /// Get the Cell Size 
        virtual Ogre::Real getCellSize();

        /// Convenience method - this section always uses a grid strategy
        virtual Ogre::Grid2DPageStrategy* getGridStrategy() const;
        /// Convenience method - this section always uses a grid strategy
        virtual Ogre::Grid2DPageStrategyData* getGridStrategyData() const;

        /// Overridden from PagedWorldSection
        void loadPage(Ogre::PageID pageID, bool forceSynchronous = false);
        /// Overridden from PagedWorldSection
        void unloadPage(Ogre::PageID pageID, bool forceSynchronous = false);

        /**
        * Adds an editor Object
        * @param object the editor object to add
        */
        void addObject(CBaseEditor *object);
        /**
        * Removes an editor Object
        * @param object the editor object to remove
        */
        void removeObject(CBaseEditor *object);
        /**
        * Updates an Object's page according to its position
        * @param object the editor object to update it's page
        */
        void updateObjectPage(CBaseEditor *object);
        /**
        * Resets all paging data
        */
        void _cleanup();

    protected:
        OgitorPage         mDefaultPage;
        OgitorPageMap      mOgitorPages;
        ObjectPagePairList mObjects;

        /// Overridden from PagedWorldSection
        void loadSubtypeData(Ogre::StreamSerialiser& ser);
        void saveSubtypeData(Ogre::StreamSerialiser& ser);

        virtual void syncSettings();

    };

}
