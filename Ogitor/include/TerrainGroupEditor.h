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
    class Terrain;
    class TerrainGroup;
    class TerrainGlobalOptions;
}

namespace Forests
{
    class PagedGeometry;
    class GrassLoader;
}

namespace Ogitors
{
    #define BRUSH_DATA_SIZE 128


    //! Paged Terrain Manager class
    /*!  
    A paged terrain manager class that coordinates and assists in managing paged terrain editor(s)
    */  
    class OgitorExport CTerrainGroupEditor : public CBaseEditor, public ITerrainEditor, public Ogre::ManualResourceLoader
    {
        friend class CTerrainGroupEditorFactory;
    public:
        int getMaxLayersAllowed() { return mMaxLayersAllowed; };

        /// Gets the Handle to encapsulated object
        inline virtual void *getHandle() {return static_cast<void*>(mHandle);};
        virtual bool     setLayerImpl(unsigned int newlayer);
        /** @copydoc CBaseEditor::setSelectedImpl(bool) */
        virtual void     setSelectedImpl(bool bSelected);
        /** @copydoc CBaseEditor::createProperties(OgitorsPropertyValueMap &) */
        virtual void     createProperties(OgitorsPropertyValueMap &params);
        /** @copydoc CBaseEditor::load(bool) */
        virtual bool     load(bool async = true);
        /** @copydoc CBaseEditor::unLoad() */
        virtual bool     unLoad();
        virtual TiXmlElement* exportDotScene(TiXmlElement *pParent);
        /**
        * Removes a page from the group
        * @param page handle to terrain page editor to be removed
        */
        void                         removePage(CTerrainPageEditor *page);
        /**
        * Removes a page from the group
        * @param x,y coordinates of the terrain slot relative to the center slot (signed) 
        */
        void                         removePage(int x, int y);
        /** @copydoc CBaseEditor::update(float) */
        virtual bool                 update(float timePassed);
        /** @copydoc CBaseEditor::getObjectContextMenu(UTFStringVector &) */
        virtual bool                 getObjectContextMenu(UTFStringVector &menuitems);
        /** @copydoc CBaseEditor::onObjectContextMenu(int) */
        virtual void                 onObjectContextMenu(int menuresult);
        /** @copydoc CBaseEditor::getAABB() */
        virtual Ogre::AxisAlignedBox getAABB();
        /**
        * Fetches decal frustum handle
        * @return decal frustum handle
        */
        Ogre::Frustum               *getDecalFrustum() {return mDecalFrustum;};
        /**
        * Tests if terrain manager can edit terrain(s)
        * @return true if can, otherwise false
        */
        virtual bool                 isTerrainType() {return true;};

        inline float                *getBrushData() { return mBrushData; };
        /**
        * Called before displaying an object properties in properties view 
        */
        virtual void                 prepareBeforePresentProperties();

        inline Forests::PagedGeometry *getPGHandle() { return mPGHandle; };
        inline Forests::GrassLoader *getGrassLoaderHandle() { return mGrassLoaderHandle; };
        inline int                   getGrassDensityMapSize() { return mPGDensityMapSize->get(); }
        inline int                   getMapSize() { return mMapSize->get(); }
        inline Ogre::Real            getWorldSize() { return mWorldSize->get(); }
        Ogre::Real                   getHeightAt(Ogre::Real x, Ogre::Real z);
        inline Ogre::String          getPageNamePrefix() { return mPageNamePrefix->get(); }
        Ogre::Vector3                getPagePosition(const int x, const int y);
        CTerrainPageEditor*          getPage(const int x, const int y);
        bool                         addPage(const int x, const int y, const Ogre::String diffuse, const Ogre::String normal);
        /**
        * Fetches terrain editor handle
        * @return terrain editor handle
        */
        virtual ITerrainEditor*      getTerrainEditor() {return dynamic_cast<ITerrainEditor*>(this);};
        /** @copydoc ITerrainEditor::hitTest(Ogre::Ray, Ogre::Vector3 *) */
        virtual bool                 hitTest(Ogre::Ray camRay, Ogre::Vector3 *result = NULL);
        /** @copydoc ITerrainEditor::getHeightFunction() */
        virtual PGHeightFunction    *getHeightFunction();
        /** @copydoc ITerrainEditor::getSize() */
        virtual Ogre::AxisAlignedBox getSize() {return getAABB();};
        /** @copydoc ITerrainEditor::getMaterialNames() */
        virtual Ogre::StringVector   getMaterialNames();
        /** @copydoc ITerrainEditor::setMousePosition(Ogre::Ray) */
        virtual void setMousePosition(Ogre::Ray camRay);
        /** @copydoc ITerrainEditor::setBrushame(const std::string&) */
        virtual void setBrushName(const std::string& brush);
        /** @copydoc ITerrainEditor::getBrushName() */
        virtual const Ogre::String& getBrushName() {return mBrushName;};
        /** @copydoc ITerrainEditor::setBrushSize(unsigned int) */
        virtual void setBrushSize(unsigned int size);
        /** @copydoc ITerrainEditor::getBrushSize() */
        virtual int  getBrushSize() {return mBrushSize; };
        /** @copydoc ITerrainEditor::setBrushOrientation(Ogre::Radian) */
        virtual void setBrushOrientation(Ogre::Radian orient) {};
        /** @copydoc ITerrainEditor::setBrushIntensity(float) */
        virtual void setBrushIntensity(float intensity);
        /** @copydoc ITerrainEditor::setEditMode(unsigned int) */
        virtual void setEditMode(unsigned int mode);
        virtual unsigned int getEditMode() {return mEditMode;};
        /** @copydoc ITerrainEditor::setTexture(const std::string&) */
        virtual void setTexture(const std::string& texture);
        /** @copydoc ITerrainEditor::setGrassTexture(const std::string&) */
        virtual void setGrassTexture(const std::string& texture);
        /** @copydoc ITerrainEditor::setColour(const Ogre::ColourValue&) */
        virtual void setColour(const Ogre::ColourValue& colour);
        /** @copydoc ITerrainEditor::setEditDirection(bool) */
        virtual void setEditDirection(bool inverse) {mEditDirection = inverse;};
        /** @copydoc ITerrainEditor::isSelected() */
        virtual bool isSelected();
        /** @copydoc ITerrainEditor::canDeform() */
        virtual bool canDeform() { return true; }
        /** @copydoc ITerrainEditor::canSplat() */
        virtual bool canSplat() { return true; }
        /** @copydoc ITerrainEditor::canPaint() */
        virtual bool canPaint() { return mColourMapEnabled->get(); }
        /** @copydoc ITerrainEditor::startEdit() */
        virtual void startEdit();
        /** @copydoc ITerrainEditor::stopEdit() */
        virtual void stopEdit();
        /** @copydoc ITerrainEditor::recalculateLighting() */
        virtual void recalculateLighting();
        /** @copydoc ITerrainEditor::isBackgroundProcessActive() */
        virtual bool isBackgroundProcessActive();

    protected:
        Ogre::TerrainGroup      *mHandle;                           /** Handle to Ogre::TerrainGroup object */
        Ogre::TerrainGlobalOptions *mTerrainGlobalOptions;          /** Handle to Ogre::TerrainGlobalOptions object */
        Forests::PagedGeometry  *mPGHandle;                         /** Handle to Forests::PagedGeometry object */
        Forests::GrassLoader    *mGrassLoaderHandle;                /** Handle to Forests::GrassLoader object */
        float                   *mBrushData;                        /** Additional brush data */
        Ogre::SceneNode         *mDecalNode;                        /** Decal node handle */
        Ogre::Frustum           *mDecalFrustum;                     /** Decal frustum handle */
        Ogre::TexturePtr         mDecalTexture;                     /** Decal texture handle */
        OgitorsScopedConnection  mShadowsConnection[2];
        Ogre::Rect               mModificationRect; 
        int                      mMaxLayersAllowed;

        OgitorsProperty<Ogre::Real>   *mWorldSize;                /** World size property handle */
        OgitorsProperty<int>          *mMapSize;                  /** Map size property handle */
        OgitorsProperty<Ogre::String> *mPageNamePrefix;           /** Page Name Prefix property handle */
        OgitorsProperty<int>          *mMaterialGeneratorType;    /** MaterialGeneratorType to be used */
        OgitorsProperty<bool>         *mColourMapEnabled;         /** Color flag property handle */
        OgitorsProperty<bool>         *mUseRayBoxDistanceCalculation;
        OgitorsProperty<int>          *mColourMapTextureSize;     /** Color map texture size property handle */
        OgitorsProperty<int>          *mLightMapTextureSize;      /** Light map texture size property handle */
        OgitorsProperty<int>          *mBlendMapTextureSize;      /** Blend map texture size property handle */
        OgitorsProperty<int>          *mCompositeMapTextureSize;  /** Composite map texture size property handle */
        OgitorsProperty<int>          *mCompositeMapDistance;     
        OgitorsProperty<int>          *mMaxPixelError;     
        OgitorsProperty<int>          *mMinBatchSize;
        OgitorsProperty<int>          *mMaxBatchSize;
        OgitorsProperty<int>          *mSkirtSize;

        OgitorsProperty<int>          *mPGPageSize;
        OgitorsProperty<int>          *mPGDetailDistance;
        OgitorsProperty<int>          *mPGDensityMapSize;
        /**
        * Constructor
        * @param factory Handle to terrain editor factory
        */
        CTerrainGroupEditor(CBaseEditorFactory *factory);
        /**
        * Destructor
        */
        virtual ~CTerrainGroupEditor();

        /**
        * Property setter for map size (internal)
        * @param property Handle to property responsible for map size
        * @param value new map size
        * @return true if terrain handle is valid 
        */
        bool _setMapSize(OgitorsPropertyBase* property, const int& value);
        /**
        * Property setter for world size (internal)
        * @param property Handle to property responsible for world size
        * @param value new world size
        * @return true if terrain handle is valid 
        */
        bool _setWorldSize(OgitorsPropertyBase* property, const Ogre::Real& value);
        /**
        * Property setter for material generator type (internal)
        * @param property Handle to property responsible for material generator type
        * @param value new material generator type
        * @return true if terrain handle is valid 
        */
        bool _setMaterialGeneratorType(OgitorsPropertyBase* property, const int& value);
        /**
        * Property setter for page name prefix (internal)
        * @param property Handle to property responsible for page name prefix
        * @param value new page name prefix
        * @return true if terrain handle is valid 
        */
        bool _setPageNamePrefix(OgitorsPropertyBase* property, const Ogre::String& value);
        /**
        * Property setter for max pixel error (internal)
        * @param property Handle to property responsible for max pixel error
        * @param value new max pixel error
        * @return true if terrain handle is valid 
        */
        bool _setMaxPixelError(OgitorsPropertyBase* property, const int& value);
        /**
        * Property setter for min batch size (internal)
        * @param property Handle to property responsible for min batch size
        * @param value new min batch size
        * @return true if terrain handle is valid 
        */
        bool _setMinBatchSize(OgitorsPropertyBase* property, const int& value);
        /**
        * Property setter for skirt size (internal)
        * @param property Handle to property responsible for skirt size
        * @param value new skirt size
        * @return true if terrain handle is valid 
        */
        bool _setSkirtSize(OgitorsPropertyBase* property, const int& value);
        /**
        * Property setter for UseRayBoxDistanceCalculation (internal)
        * @param property Handle to property responsible for UseRayBoxDistanceCalculation
        * @param value new UseRayBoxDistanceCalculation state
        * @return true if terrain handle is valid 
        */
        bool _setUseRayBoxDistanceCalculation(OgitorsPropertyBase* property, const bool& value);
        /**
        * Property setter for colour map size (internal)
        * @param property Handle to property responsible for colour map size
        * @param value new colour map size
        * @return true if terrain handle is valid 
        */
        bool _setColourMapTextureSize(OgitorsPropertyBase* property, const int& value);
        /**
        * Property setter for light map size (internal)
        * @param property Handle to property responsible for light map size
        * @param value new light map size
        * @return true if terrain handle is valid 
        */
        bool _setLightMapTextureSize(OgitorsPropertyBase* property, const int& value);
        /**
        * Property setter for blend map size (internal)
        * @param property Handle to property responsible for blend map size
        * @param value new blend map size
        * @return true if terrain handle is valid 
        */
        bool _setBlendMapTextureSize(OgitorsPropertyBase* property, const int& value);
        /**
        * Property setter for composite map size (internal)
        * @param property Handle to property responsible for composite map size
        * @param value new composite map size
        * @return true if terrain handle is valid 
        */
        bool _setCompositeMapTextureSize(OgitorsPropertyBase* property, const int& value);
        /**
        * Property setter for max batch size (internal)
        * @param property Handle to property responsible for max batch size
        * @param value new max batch size
        * @return true if terrain handle is valid 
        */
        bool _setMaxBatchSize(OgitorsPropertyBase* property, const int& value);
        /**
        * Property setter for composite map distance (internal)
        * @param property Handle to property responsible for composite map distance
        * @param value new composite map distance
        * @return true if terrain handle is valid 
        */
        bool _setCompositeMapDistance(OgitorsPropertyBase* property, const int& value);
        /**
        * Property setter for composite map distance (internal)
        * @param property Handle to property responsible for composite map distance
        * @param value new composite map distance
        * @return true if terrain handle is valid 
        */
        bool _setPGPageSize(OgitorsPropertyBase* property, const int& value);
        /**
        * Property setter for composite map distance (internal)
        * @param property Handle to property responsible for composite map distance
        * @param value new composite map distance
        * @return true if terrain handle is valid 
        */
        bool _setPGDetailDistance(OgitorsPropertyBase* property, const int& value);
        /**
        * Property getter for map editor rectangle (internal)
        * @param property Handle to property responsible for map size
        * @param value new map size
        * @return true if terrain handle is valid 
        */
        bool _getEditRect(Ogre::Vector3& editpos, Ogre::Rect& brushrect, Ogre::Rect& maprect, int size);
        /**
        * Terrain transformation that applies splatting over specified area with certain strength
        * @param handle handle upon which to perform splatting at
        * @param editpos position at which to perform splatting at
        * @param timePassed strength of deformation
        */
        void _splat(CTerrainPageEditor *handle, Ogre::Vector3 &editpos, float timePassed);
        /**
        * Terrain transformation that applies grass splatting over specified area with certain strength
        * @param handle handle upon which to perform grass splatting at
        * @param editpos position at which to perform grass splatting at
        * @param timePassed strength of deformation
        */
        void _splatGrass(CTerrainPageEditor *handle, Ogre::Vector3 &editpos, float timePassed);
        /**
        * Terrain transformation that deforms terrain over specified area with certain strength
        * @param handle handle upon which to perform deformation at
        * @param editpos position at which to perform deformation at
        * @param timePassed strength of deformation
        */
        void _deform(CTerrainPageEditor *handle, Ogre::Vector3 &editpos, float timePassed);
        void _calculatesmoothingfactor(CTerrainPageEditor *handle, Ogre::Vector3 &editpos, float& avg, int& sample_count);
        /**
        * Terrain transformation that smooths terrain over specified area with certain strength
        * @param handle handle upon which to perform deformation at
        * @param editpos position at which to perform smoothing at
        * @param timePassed strength of smoothing
        */
        void _smooth(CTerrainPageEditor *handle, Ogre::Vector3 &editpos, float avg, float timePassed);
        /**
        * Terrain transformation that paints over the terrain at specified area with certain strength
        * @param editor Paged terrain editor handle upon which to perform painting at
        * @param editpos position at which to perform painting at
        * @param timePassed strength of painting
        */
        void _paint(CTerrainPageEditor *handle, Ogre::Vector3 &editpos, float timePassed);
        /**
        * Loads decal texture resource
        * @param resource decal resource handle
        */
        virtual void loadResource(Ogre::Resource* resource);
        
        void onShadowsChange(const OgitorsPropertyBase* property, Ogre::Any value);
        void onShadowsTechniqueChange(const OgitorsPropertyBase* property, Ogre::Any value);

        /**
        * Modifies the height fields of all pages
        * @param scale the scale to multiply current heights with
        * @param offset value to offset current heights with (after scale)
        */
        void _modifyHeights(float scale, float offset);
        void importFullTerrainFromHeightMap();
        void exportHeightMaps();
        void exportCompositeMaps();
    };

    //! Paged terrain manager factory class
    /*!  
        A class that is responsible for managing paged terrain manager class(es)
    */

    class OgitorExport CTerrainGroupEditorFactory: public CBaseEditorFactory
    {
    public:
        /** @copydoc CBaseEditorFactory::CBaseEditorFactory() */
        CTerrainGroupEditorFactory(OgitorsView *view = 0);
        /** @copydoc CBaseEditorFactory::~CBaseEditorFactory() */
        virtual ~CTerrainGroupEditorFactory() {};
        /** @copydoc CBaseEditorFactory::duplicate(OgitorsView *view) */
        virtual CBaseEditorFactory* duplicate(OgitorsView *view);
        /** @copydoc CBaseEditorFactory::CanInstantiate() */
        virtual bool CanInstantiate();
        /** @copydoc CBaseEditorFactory::CreateObject(CBaseEditor **, OgitorsPropertyValueMap &) */
        virtual CBaseEditor *CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params);
        /**
        * Fetches map size options property(ies)
        * @return map size options property(ies)
        */
        static PropertyOptionsVector *GetMapSizeOptions() { return &mMapSizeOptions; }
        /**
        * Fetches colour map size options property(ies)
        * @return colour map size options property(ies)
        */
        static PropertyOptionsVector *GetColourMapSizeOptions() { return &mColourMapSizeOptions; }

        static PropertyOptionsVector *GetMinBatchSizes() { return &mMinBatchSizes; }

        static PropertyOptionsVector *GetMaxBatchSizes() { return &mMaxBatchSizes; }

        static PropertyOptionsVector *GetMaterialGeneratorTypes() { return &mMaterialGeneratorTypes; }

    private:
        static PropertyOptionsVector mColourMapSizeOptions;     /** Colour map size options property(ies) */
        static PropertyOptionsVector mMapSizeOptions;           /** Map size options property(ies) */
        static PropertyOptionsVector mMinBatchSizes;
        static PropertyOptionsVector mMaxBatchSizes;
        static PropertyOptionsVector mMaterialGeneratorTypes;
    };

}

