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
    class PagedWorld;
    class PagedWorldSection;
    class PagedWorldSectionFactory;
    class PageManager;
}

namespace Ogitors
{
    class OgitorPagedWorldSection;

    class OgitorExport CPagingManager : public CBaseEditor, public IPagingEditor
    {
        friend class CPagingManagerFactory;

    public:
        /**
        * Fetches paging editor handle
        * @return paging editor handle
        */
        virtual IPagingEditor* getPagingEditor() {return dynamic_cast<IPagingEditor*>(this);};
        /** @copydoc CBaseEditor::showBoundingBox(bool bShow) */  
        virtual void showBoundingBox(bool bShow);
        /** @copydoc CBaseEditor::createProperties(OgitorsPropertyValueMap &) */
        virtual void createProperties(OgitorsPropertyValueMap &params);
        /** @copydoc CBaseEditor::load(bool) */
        virtual bool load(bool async = true);
        /** @copydoc CBaseEditor::unLoad() */
        virtual bool unLoad();

        /**
        * Adds an editor Object
        * @param object the editor object to add
        */
        virtual void addObject(CBaseEditor *object);
        /**
        * Removes an editor Object
        * @param object the editor object to remove
        */
        virtual void removeObject(CBaseEditor *object);
        /**
        * Updates an Object's page according to its position
        * @param object the editor object to update it's page
        */
        virtual void updateObjectPage(CBaseEditor *object);
    protected:
        typedef std::map<OgitorWorldSectionId, OgitorPagedWorldSection*> OgitorSectionMap;
        
        Ogre::PageManager       *mHandle;
        OgitorSectionMap        mSections;

        OgitorsProperty<Ogre::Real> *mCellSize;
        OgitorsProperty<Ogre::Real> *mLoadRadius;
        OgitorsProperty<Ogre::Real> *mHoldRadius;

        OgitorsProperty<Ogre::Real> *mTerrainCellSize;
        OgitorsProperty<Ogre::Real> *mTerrainLoadRadius;
        OgitorsProperty<Ogre::Real> *mTerrainHoldRadius;

        /**
        * Constructor
        * @param factory Handle to terrain editor factory
        */
        CPagingManager(CBaseEditorFactory *factory);
        /**
        * Destructor
        */
        virtual     ~CPagingManager() {};

        class OgitorExport OgitorSectionFactory : public Ogre::PagedWorldSectionFactory
        {
        public:
            static const Ogre::String FACTORY_NAME;
            const Ogre::String& getName() const;
            Ogre::PagedWorldSection* createInstance(const Ogre::String& name, Ogre::PagedWorld* parent, Ogre::SceneManager* sm);
            void destroyInstance(Ogre::PagedWorldSection*);

        };

        OgitorSectionFactory mSectionFactory;

        /**
        * Resets all paging data
        */
        void _cleanup();
        /**
        * Re-creates the paging system from scratch
        */
        void _kickStart();
        /**
        * Property setter for cell size (internal)
        * @param property Handle to property responsible for cell size
        * @param value new cell size
        * @return true if allowed 
        */
        bool _setCellSize(OgitorsPropertyBase* property, const Ogre::Real& value);
        /**
        * Property setter for load radius (internal)
        * @param property Handle to property responsible for load radius
        * @param value new load radius
        * @return true if allowed 
        */
        bool _setLoadRadius(OgitorsPropertyBase* property, const Ogre::Real& value);
        /**
        * Property setter for hold radius (internal)
        * @param property Handle to property responsible for hold radius
        * @param value new hold radius
        * @return true if allowed 
        */
        bool _setHoldRadius(OgitorsPropertyBase* property, const Ogre::Real& value);
        /**
        * Property setter for cell size (internal)
        * @param property Handle to property responsible for cell size
        * @param value new cell size
        * @return true if allowed 
        */
        bool _setTerrainCellSize(OgitorsPropertyBase* property, const Ogre::Real& value);
        /**
        * Property setter for load radius (internal)
        * @param property Handle to property responsible for load radius
        * @param value new load radius
        * @return true if allowed 
        */
        bool _setTerrainLoadRadius(OgitorsPropertyBase* property, const Ogre::Real& value);
        /**
        * Property setter for hold radius (internal)
        * @param property Handle to property responsible for hold radius
        * @param value new hold radius
        * @return true if allowed 
        */
        bool _setTerrainHoldRadius(OgitorsPropertyBase* property, const Ogre::Real& value);
    };

    //! Paging editor factory class
    /*!  
        A class that is responsible for instantiating paging editor class(es)
    */
    class OgitorExport CPagingManagerFactory: public CBaseEditorFactory
    {
    public:
        /** @copydoc CBaseEditorFactory::CBaseEditorFactory() */
        CPagingManagerFactory(OgitorsView *view = 0);
        /** @copydoc CBaseEditorFactory::~CBaseEditorFactory() */
        virtual ~CPagingManagerFactory() {};
        /** @copydoc CBaseEditorFactory::duplicate(OgitorsView *view) */
        virtual CBaseEditorFactory* duplicate(OgitorsView *view);
        /** @copydoc CBaseEditorFactory::CreateObject(CBaseEditor **, OgitorsPropertyValueMap &) */
        virtual CBaseEditor *CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params);
        /** @copydoc CBaseEditorFactory::CanInstantiate() */
        virtual bool CanInstantiate();
    };
}
