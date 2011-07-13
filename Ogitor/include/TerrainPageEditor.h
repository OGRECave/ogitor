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

#pragma once

namespace Ogre
{
    class Terrain;
}

namespace Forests
{
    class GrassLayer;
}


namespace Ogitors
{
    class TerrainLayerUndo;
    class TerrainHeightUndo;
    class TerrainBlendUndo;
    class TerrainColourUndo;
    class TerrainGrassUndo;

    struct CalcBlendData
    {
        float hs;     /// Height Start
        float he;     /// Height End
        float hr;     /// Height Release
        float ss;     /// Slope Start
        float se;     /// Slope End
        float sr;     /// Slope Release
        float skw;    /// Skew Scale
        float skwazm; /// Skew Azimuth
    };

    //! Paged terrain editor class
    /*!  
        A class that handles terrain editing
    */
    class OgitorExport CTerrainPageEditor : public CBaseEditor
    {
        friend class CTerrainPageEditorFactory;
        friend class CTerrainGroupEditor;
        friend class TerrainLayerUndo;
        friend class TerrainHeightUndo;
        friend class TerrainBlendUndo;
        friend class TerrainColourUndo;
        friend class TerrainGrassUndo;
    public:

        /**
        * Fetches editor object' DERIVED position
        * @return editor object' DERIVED position
        */
        virtual Ogre::Vector3 getDerivedPosition() { return mPosition->get(); }
        /**
        * Sets terrain world position
        * @param position terrain world position
        */
        inline void      setPosition(const Ogre::Vector3& position) { mPosition->set(position); }
        /**
        * Fetches terrain world position
        * @return terrain world position
        */
        inline Ogre::Vector3 getPosition() { return mPosition->get(); }
        /**
        * Fetches page X index
        * @return page X index
        */
        inline int       getPageX() { return mPageX->get(); }
        /**
        * Sets page X index
        * @param page X index
        */
        inline void      setPageX(int value) { mPageX->set(value); }
        /**
        * Fetches page Y index
        * @return page Y index
        */
        inline int       getPageY() { return mPageY->get(); }
        /**
        * Sets page Y index
        * @param page Y index
        */
        inline void      setPageY(int value) { mPageY->set(value); }

        virtual bool     setLayerImpl(unsigned int newlayer);        
        /** @copydoc CBaseEditor::setNameImpl(Ogre::String) */
        virtual bool     setNameImpl(Ogre::String name);
        /** @copydoc CBaseEditor::createProperties(OgitorsPropertyValueMap &) */
        virtual void     createProperties(OgitorsPropertyValueMap &params);
        /** @copydoc CBaseEditor::load(bool) */
        virtual bool     load(bool async = true);
        /** @copydoc CBaseEditor::unLoad() */
        virtual bool     unLoad();
        virtual TiXmlElement* exportDotScene(TiXmlElement *pParent);
        /** @copydoc CBaseEditor::getMaterialName() */
        const Ogre::String& getMaterialName();
        /** @copydoc CBaseEditor::update(float) */
        virtual bool update(float timePassed);

        /** @copydoc CBaseEditor::getObjectContextMenu(UTFStringVector &) */
        virtual bool                 getObjectContextMenu(UTFStringVector &menuitems);
        /** @copydoc CBaseEditor::onObjectContextMenu(int) */
        virtual void                 onObjectContextMenu(int menuresult);
        /** @copydoc CBaseEditor::getHandle() */
        inline virtual void         *getHandle() {return static_cast<void*>(mHandle);};
        /** @copydoc CBaseEditor::getAABB() */
        virtual Ogre::AxisAlignedBox getAABB();
        /** @copydoc CBaseEditor::setSelectedImpl(bool) */
        virtual void                 setSelectedImpl(bool bSelected);
        Ogre::Real                   hitTest(Ogre::Ray& ray, Ogre::Vector3& retPos);
          /** @copydoc CBaseEditor::onSave(bool) */
        virtual void                 onSave(bool forced = false);
        /**
        * Called before displaying an object properties in properties view 
        */
        virtual void                 prepareBeforePresentProperties();
        
        virtual bool                 importHeightMap(Ogre::String filename = Ogre::String(""), Ogre::Real fBias = 0.0f, Ogre::Real fScale = 0.0f);
        virtual bool                 calculateBlendMap();
        virtual bool                 importBlendMap(int layerID, Ogre::String filename = Ogre::String(""));
        virtual bool                 importBlendMap(Ogre::String filename = Ogre::String(""));
        virtual bool                 exportHeightMap(Ogre::String path = Ogre::String(""), Ogre::String filename = Ogre::String(""), Ogre::Real fMin = 0.0f, Ogre::Real fMax = 0.0f);
        virtual bool                 exportCompositeMap(Ogre::String path = Ogre::String(""), Ogre::String filename = Ogre::String(""));

        float                       *getGrassPointer(unsigned int layerID);
        void                         updateGrassLayer(unsigned int layerID);
        void                         dirtyGrassRect(Ogre::Rect &rect) { mPGDirtyRect = rect; };
        /**
        * Creates new layer
        * @param texture texture name
        * @param normal normal texture name
        * @param donotuseempty if true layers wont be checked if empty
        * @return new layer ID
        */
        int                          _createNewLayer(Ogre::String &texture, Ogre::String& normal, Ogre::Real worldSize = 100.0f, bool donotuseempty = false);
        /**
        * Changes layer properties
        * @param layerID index of layer
        * @param texture texture name
        * @param normal normal texture name
        * @param worldSize worldsize of layer
        */
        void                         _changeLayer(int layerID, Ogre::String &texture, Ogre::String& normal, Ogre::Real worldSize);
        /**
        * Deletes a terrain layer
        * @param layerID index of the layer
        */
        void                         _deleteLayer(int layerID);

    protected:
        Ogre::Terrain                  *mHandle;                /** Terrain handle */
        Forests::GrassLayer            *mPGLayers[4];
        float                          *mPGLayerData[4];
        Ogre::Image                     mPGDensityMap;
        Ogre::Rect                      mPGDirtyRect;
        bool                            mFirstTimeInit;         /** Is the page being created for the first time? */
        float                          *mExternalDataHandle;    /** External Float array handle to be used during a new page creation */
        bool                            mPGModified;            /** Is the paged geometry modified? */
        Ogre::String                    mTempFileName;          /** Temporary terrain data file name */
        Ogre::String                    mTempDensityFileName;   /** Temporary density map file name */

        Ogre::Rect                      mHeightDirtyRect;
        Ogre::Rect                      mBlendMapDirtyRect;
        Ogre::Rect                      mColourMapDirtyRect;
        Ogre::Rect                      mGrassMapDirtyRect;
        float                          *mHeightSave;
        float                          *mBlendSave;
        Ogre::ColourValue              *mColourSave;
        float                          *mGrassSave;
        int                             mBlendSaveStart;
        int                             mBlendSaveCount;


        OgitorsProperty<bool>          *mColourMapEnabled;      /** Color map enabled flag */
        OgitorsProperty<int>           *mColourMapTextureSize;  /** Color map texture size */
        OgitorsProperty<int>           *mPageX;                 /** Page size (X) */
        OgitorsProperty<int>           *mPageY;                 /** Page size (Y) */
        OgitorsProperty<Ogre::Vector3> *mPosition;              /** Position of terrain in the world */
        OgitorsProperty<int>           *mLayerCount;            /** Layer(s) count */
        OgitorsProperty<bool>          *mTempDensityModified;   /** Is the density map modified and has a temporary save file? */
        OgitorsProperty<bool>          *mTempModified;          /** Is the page modified and has a temporary save file? */
        OgitorsProperty<Ogre::Real>    *mLayerWorldSize[16];
        OgitorsProperty<Ogre::String>  *mLayerDiffuse[16];
        OgitorsProperty<Ogre::String>  *mLayerNormal[16];
        OgitorsProperty<int>           *mMinBatchSize;
        OgitorsProperty<int>           *mMaxBatchSize;

        /// Grass Related Properties
        OgitorsProperty<bool>          *mPGActive[4];
        OgitorsProperty<Ogre::String>  *mPGMaterial[4];
        OgitorsProperty<Ogre::Vector2> *mPGMinSize[4];
        OgitorsProperty<Ogre::Vector2> *mPGMaxSize[4];
        OgitorsProperty<bool>          *mPGAnimate[4];
        OgitorsProperty<Ogre::Real>    *mPGSwayDistribution[4];
        OgitorsProperty<Ogre::Real>    *mPGSwayLength[4];
        OgitorsProperty<Ogre::Real>    *mPGSwaySpeed[4];
        OgitorsProperty<Ogre::Real>    *mPGDensity[4];
        OgitorsProperty<int>           *mPGFadeTech[4];
        OgitorsProperty<int>           *mPGGrassTech[4];

        /**
        * Constructor
        * @param factory Handle to terrain editor factory
        */
        CTerrainPageEditor(CBaseEditorFactory *factory);
        /**
        * Destructor
        */
        virtual ~CTerrainPageEditor();
        /**
        * Property setter for terrain position (internal)
        * @param property Handle to property responsible for terrain world position
        * @param position new terrain world position
        * @return true if terrain handle is valid 
        */
        bool _setPosition(OgitorsPropertyBase* property, const Ogre::Vector3& position);
        /**
        * Property setter for terrain layer size (internal)
        * @param property Handle to property responsible for terrain layer size
        * @param value new terrain layer size
        * @return true if terrain handle is valid 
        */
        bool _setLayerWorldSize(OgitorsPropertyBase* property, const Ogre::Real& value);
        /**
        * Property setter for layer diffuse map name (internal)
        * @param property Handle to property responsible for layer diffuse map name
        * @param value new layer diffuse map name
        * @return true if terrain handle is valid 
        */
        bool _setLayerDiffuseMap(OgitorsPropertyBase* property, const Ogre::String& value);
        /**
        * Property setter for layer normal map name (internal)
        * @param property Handle to property responsible for layer normal map name
        * @param value new layer normal map name
        * @return true if terrain handle is valid 
        */       
        bool _setLayerNormalMap(OgitorsPropertyBase* property, const Ogre::String& value);
        /**
        * Property setter for color flag (internal)
        * @param property Handle to property responsible for color flag
        * @param value new color flag value
        * @return true if terrain handle is valid 
        */
        bool _setColourMapEnabled(OgitorsPropertyBase* property, const bool& value);
        /**
        * Property setter for color map texture size (internal)
        * @param property Handle to property responsible for color map texture size
        * @param value new color map texture size
        * @return true if terrain handle is valid 
        */
        bool _setColourMapTextureSize(OgitorsPropertyBase* property, const int& value);
        /**
        * Property setter for min batch size (internal)
        * @param property Handle to property responsible for min batch size
        * @param value new min batch size
        * @return true if terrain handle is valid 
        */
        bool _setMinBatchSize(OgitorsPropertyBase* property, const int& value);
        /**
        * Property setter for max batch size (internal)
        * @param property Handle to property responsible for max batch size
        * @param value new max batch size
        * @return true if terrain handle is valid 
        */
        bool _setMaxBatchSize(OgitorsPropertyBase* property, const int& value);

        bool _setPGActive(OgitorsPropertyBase* property, const bool& value);
        bool _setPGMaterial(OgitorsPropertyBase* property, const Ogre::String& value);
        bool _setPGMinSize(OgitorsPropertyBase* property, const Ogre::Vector2& value);
        bool _setPGMaxSize(OgitorsPropertyBase* property, const Ogre::Vector2& value);
        bool _setPGAnimate(OgitorsPropertyBase* property, const bool& value);
        bool _setPGSwayDistribution(OgitorsPropertyBase* property, const Ogre::Real& value);
        bool _setPGSwayLength(OgitorsPropertyBase* property, const Ogre::Real& value);
        bool _setPGSwaySpeed(OgitorsPropertyBase* property, const Ogre::Real& value);
        bool _setPGDensity(OgitorsPropertyBase* property, const Ogre::Real& value);
        bool _setPGFadeTech(OgitorsPropertyBase* property, const int& value);
        bool _setPGGrassTech(OgitorsPropertyBase* property, const int& value);


        /**
        * Modifies the heightfield
        * @param scale the scale to multiply current heights with
        * @param offset value to offset current heights with (after scale)
        */
        void _modifyHeights(float scale, float offset);
        /**
        * Fetches empty layer index (internal)
        * @return empty layer index
        */
        int  _getEmptyLayer();
        /**
        * Fetches empty grass layer index (internal)
        * @return empty grass layer index
        */
        int  _getEmptyGrassLayer();
        /**
        * Creates new layer (internal)
        * @param layerID index of layer
        * @param texture texture name
        * @param normal normal texture name
        * @param worldSize worldsize of layer
        */
        void  _createLayer(int layerID, Ogre::String &texture, Ogre::String& normal, Ogre::Real worldSize);
        /**
        * Creates new grass layer (internal)
        * @param texture texture name
        * @return new layer ID
        */
        int  _createNewGrassLayer(Ogre::String &texture);
        /**
        * Fetches new layer (internal)
        * @param texture texture name
        * @param normal normal texture name
        * @return new layer ID
        */
        int  _getLayerID(Ogre::String& texture, Ogre::String& normal, bool dontcreate);
        /**
        * Fetches new grass layer (internal)
        * @param texture grass texture name
        * @return new layer ID
        */
        int  _getGrassLayerID(Ogre::String& texture, bool dontcreate);
        /**
        * Saves Terrain Data File in location ProjectDirectory + prefix + autofilename (internal)
        */
        void _saveTerrain(Ogre::String pathPrefix);
        /**
        * Saves Density Map Data File in location ProjectDirectory + prefix + autofilename (internal)
        */
        void _saveGrass(Ogre::String pathPrefix);

        void _loadGrassLayers();

        void _unloadGrassLayers();

        void _refreshGrassGeometry(Ogre::Rect *rect = 0);
        
        void _checkTerrainSizes();

        void _notifyModification(int layerID, const Ogre::Rect& dirtyRect);

        void _notifyEndModification();

        void _swapHeights(Ogre::Rect rect, float *data);

        void _swapBlends(int layerStart, int layerCount, Ogre::Rect rect, float *data);

        void _swapColours(Ogre::Rect rect, Ogre::ColourValue *data);

        void _swapGrass(Ogre::Rect rect, float *data);
    };

    //! Paged terrain editor factory class
    /*!  
    A class that is responsible for instantiating paged terrain editor class(es)
    */
    class OgitorExport CTerrainPageEditorFactory: public CBaseEditorFactory
    {
    public:
        /** @copydoc CBaseEditorFactory::CBaseEditorFactory() */
        CTerrainPageEditorFactory(OgitorsView *view = 0);
        /** @copydoc CBaseEditorFactory::~CBaseEditorFactory() */
        virtual ~CTerrainPageEditorFactory() {};
        /** @copydoc CBaseEditorFactory::duplicate(OgitorsView *view) */
        virtual CBaseEditorFactory* duplicate(OgitorsView *view);
        /** @copydoc CBaseEditorFactory::CanInstantiate() */
        virtual bool CanInstantiate();
        /** @copydoc CBaseEditorFactory::CreateObject(CBaseEditor **, OgitorsPropertyValueMap &) */
        virtual CBaseEditor *CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params);
        /**
        * Fetches fade technique name(s)
        * @return fade technique name(s)
        */
        static PropertyOptionsVector *GetFadeTechniques() { return &mFadeTechniques; }
        /**
        * Fetches grass technique name(s)
        * @return grass technique name(s)
        */
        static PropertyOptionsVector *GetGrassTechniques() { return &mGrassTechniques; }
    protected:
        static PropertyOptionsVector mFadeTechniques;    /** List of fade techniques */
        static PropertyOptionsVector mGrassTechniques;    /** List of grass techniques */
    };
}

