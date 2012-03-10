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

namespace Ogitors
{
    //! Scene manager editor class
    /*!  
    A class that manages scene editing
    */
    class OgitorExport CSceneManagerEditor: public CBaseEditor
    {
        friend class CSceneManagerEditorFactory;
    public:

        /** @copydoc CBaseEditor::load(bool) */
        virtual bool load(bool async = true);
        /** @copydoc CBaseEditor::unLoad() */
        virtual bool unLoad();
        /** @copydoc CBaseEditor::createProperties(OgitorsPropertyValueMap &) */
        virtual void createProperties(OgitorsPropertyValueMap &params);
        /** @copydoc CBaseEditor::isNodeType() */    
        virtual bool isNodeType() {return true;};
        /** @copydoc CBaseEditor::showBoundingBox(bool) */
        virtual void showBoundingBox(bool bShow) {};
        /** @copydoc CBaseEditor::getObjectContextMenu(UTFStringVector &) */
        virtual bool getObjectContextMenu(UTFStringVector &menuitems);
        /** @copydoc CBaseEditor::onObjectContextMenu(int) */
        virtual void onObjectContextMenu(int menuresult);
        /** @copydoc CBaseEditor::getHandle() */
        inline virtual void         *getHandle() {return static_cast<void*>(mHandle);};
        /** @copydoc CBaseEditor::getAABB() */
        virtual Ogre::AxisAlignedBox getAABB() {return mHandle->getRootSceneNode()->_getWorldAABB();};
        /** @copydoc CBaseEditor::getNode() */
        virtual Ogre::SceneNode     *getNode() {return mHandle->getRootSceneNode();};
        /** @copydoc CBaseEditor::getSceneManager() */
        virtual Ogre::SceneManager  *getSceneManager() {return mHandle;};
        /** @copydoc CBaseEditor::setNameImpl(Ogre::String) */
        virtual bool                 setNameImpl(Ogre::String name);
        /** @copydoc CBaseEditor::update(float) */
        virtual bool                 update(float timePassed);
        /**
        * Exports the parameters to a dotscene file
        * @param output the stream to output
        * @param indent the indentation at the beginning
        */    
        virtual TiXmlElement* exportDotScene(TiXmlElement *pParent);

        virtual Ogre::MaterialPtr    buildDepthShadowMaterial(const Ogre::String& textureName);

        virtual Ogre::MaterialPtr    buildDepthShadowMaterial(Ogre::MaterialPtr cpyMat);

        virtual void                 setupPSSM();

        Ogre::ShadowCameraSetupPtr   getPSSMSetup() { return mPSSMSetup; }
        bool                         getShadowsEnabled() { return mShadowsEnabled->get(); }
        Ogre::ShadowTechnique        getShadowsTechnique() { return (Ogre::ShadowTechnique)(mShadowsTechnique->get()); }

        inline Ogre::SphereSceneQuery *getSphereQuery() { return mSphereSceneQuery; };
        inline Ogre::RaySceneQuery    *getRayQuery() { return mRaySceneQuery; };

    protected:
        Ogre::SceneManager         *mHandle;                        /** Scene manager handle */
        Ogre::ShadowCameraSetupPtr  mPSSMSetup;
        Ogre::SphereSceneQuery     *mSphereSceneQuery;              /** Sphere Scene Query */
        Ogre::RaySceneQuery        *mRaySceneQuery;                 /** Ray Scene Query */

        
        OgitorsProperty<Ogre::String>        *mSceneManagerType;    /** Scene manager type property handle */
        OgitorsProperty<Ogre::String>        *mConfigFile;          /** Config file property handle */
        OgitorsProperty<Ogre::Vector3>       *mPosition;            /** Position property handle */
        OgitorsProperty<Ogre::Vector3>       *mScale;               /** Scale property handle */
        OgitorsProperty<Ogre::Quaternion>    *mOrientation;         /** Orientation property handle */
        OgitorsProperty<Ogre::ColourValue>   *mAmbient;             /** Ambient color property handle */
        OgitorsProperty<Ogre::String>        *mSkyBoxMaterial;      /** Skybox material name property handle */
        OgitorsProperty<Ogre::Real>          *mSkyBoxDistance;      /** Skybox distance property handle */
        OgitorsProperty<bool>                *mSkyBoxActive;        /** Skybox active flag property handle */
        OgitorsProperty<Ogre::String>        *mSkyDomeMaterial;      /** Skydome material name property handle */
        OgitorsProperty<bool>                *mSkyDomeActive;        /** Skydome active flag property handle */
        OgitorsProperty<int>                 *mFogMode;             /** Fog mode property handle */
        OgitorsProperty<Ogre::ColourValue>   *mFogColour;           /** Fog colour property handle */
        OgitorsProperty<Ogre::Real>          *mFogStart;            /** Fog start value property handle */
        OgitorsProperty<Ogre::Real>          *mFogEnd;              /** Fog end value property handle */
        OgitorsProperty<Ogre::Real>          *mFogDensity;          /** Fog density property handle */
        OgitorsProperty<Ogre::Real>          *mRenderingDistance;   /** Maximum Rendering Distance for all entities */

        OgitorsProperty<bool>                *mShadowsEnabled;
        OgitorsProperty<int>                 *mShadowsTechnique;
        OgitorsProperty<int>                 *mShadowsRenderingDistance;
        OgitorsProperty<int>                 *mShadowsResolutionNear;
        OgitorsProperty<int>                 *mShadowsResolutionMiddle;
        OgitorsProperty<int>                 *mShadowsResolutionFar;

        /**
        * Constructor
        * @param factory Handle to terrain editor factory
        */
        CSceneManagerEditor(CBaseEditorFactory *factory);
        /**
        * Destructor
        */
        virtual            ~CSceneManagerEditor() {};

        void               configureShadows(); 

        /**
        * Property setter for scene manager type (internal)
        * @param property Handle to property responsible for scene manager type
        * @param value new scene manager type
        * @return true if property handle is valid 
        */
        bool _setSceneManagerType(OgitorsPropertyBase* property, const Ogre::String& value) { return false; }
        /**
        * Property setter for scene manager configuration filename (internal)
        * @param property Handle to property responsible for scene manager configuration filename
        * @param value new scene manager configuration filename
        * @return true if property handle is valid 
        */
        bool _setConfigFile(OgitorsPropertyBase* property, const Ogre::String& value);
        /**
        * Property setter for scene' ambient color (internal)
        * @param property Handle to property responsible for scene' ambient color
        * @param value new scene' ambient color
        * @return true if property handle is valid 
        */
        bool _setAmbient(OgitorsPropertyBase* property, const Ogre::ColourValue& value);
        /**
        * Property setter for scene' skybox material name (internal)
        * @param property Handle to property responsible for scene' skybox material name
        * @param value new scene' skybox material name
        * @return true if property handle is valid 
        */
        bool _setSkyboxMaterial(OgitorsPropertyBase* property, const Ogre::String& value);
        /**
        * Property setter for scene' skybox distance (internal)
        * @param property Handle to property responsible for scene' skybox distance
        * @param value new scene' skybox distance
        * @return true if property handle is valid 
        */
        bool _setSkyboxDistance(OgitorsPropertyBase* property, const Ogre::Real& value);
        /**
        * Property setter for scene' skybox active flag (internal)
        * @param property Handle to property responsible for scene' skybox active flag
        * @param value new scene' skybox active flag
        * @return true if property handle is valid 
        */
        bool _setSkyboxActive(OgitorsPropertyBase* property, const bool& value);
        /**
        * Property setter for scene' skydome material name (internal)
        * @param property Handle to property responsible for scene' skydome material name
        * @param value new scene' skydome material name
        * @return true if property handle is valid 
        */
        bool _setSkydomeMaterial(OgitorsPropertyBase* property, const Ogre::String& value);
        /**
        * Property setter for scene' skydome active flag (internal)
        * @param property Handle to property responsible for scene' skydome active flag
        * @param value new scene' skydome active flag
        * @return true if property handle is valid 
        */
        bool _setSkydomeActive(OgitorsPropertyBase* property, const bool& value);
        /**
        * Property setter for scene' fog mode (internal)
        * @param property Handle to property responsible for scene' fog mode
        * @param value new scene' fog mode
        * @return true if property handle is valid 
        */
        bool _setFogMode(OgitorsPropertyBase* property, const int& value);
        /**
        * Property setter for scene' fog start location (internal)
        * @param property Handle to property responsible for scene' fog start location
        * @param value new scene' fog start location
        * @return true if property handle is valid 
        */
        bool _setFogStart(OgitorsPropertyBase* property, const Ogre::Real& value);
        /**
        * Property setter for scene' fog end location (internal)
        * @param property Handle to property responsible for scene' fog end location
        * @param value new scene' fog start location
        * @return true if property handle is valid 
        */
        bool _setFogEnd(OgitorsPropertyBase* property, const Ogre::Real& value);
        /**
        * Property setter for scene' fog colour (internal)
        * @param property Handle to property responsible for scene' fog colour
        * @param value new scene' fog colour
        * @return true if property handle is valid 
        */
        bool _setFogColour(OgitorsPropertyBase* property, const Ogre::ColourValue& value);
        /**
        * Property setter for scene' fog density (internal)
        * @param property Handle to property responsible for scene' fog density
        * @param value new scene' fog density
        * @return true if property handle is valid 
        */
        bool _setFogDensity(OgitorsPropertyBase* property, const Ogre::Real& value);
        /**
        * Property setter for scene displacement (position) (internal)
        * @param property Handle to property responsible for scene displacement
        * @param value new scene displacement
        * @return true if property handle is valid 
        */
        bool _setPosition(OgitorsPropertyBase* property, const Ogre::Vector3& position) { return false; }
        /**
        * Property setter for scene displacement (orientation) (internal)
        * @param property Handle to property responsible for scene displacement
        * @param value new scene displacement
        * @return true if property handle is valid 
        */
        bool _setOrientation(OgitorsPropertyBase* property, const Ogre::Quaternion& orientation) { return false; }
        /**
        * Property setter for scene displacement (scale) (internal)
        * @param property Handle to property responsible for scene displacement
        * @param value new scene displacement
        * @return true if property handle is valid 
        */
        bool _setScale(OgitorsPropertyBase* property, const Ogre::Vector3& scale) { return false; }
        /**
        * Property setter for maximum rendering distance for meshes
        * @param property Handle to property responsible for max rendering distance
        * @param value new max rendering distance
        * @return true if property handle is valid 
        */
        bool _setRenderingDistance(OgitorsPropertyBase* property, const Ogre::Real& distance);
        /**
        * Property setter for shadows enable/disable
        * @param property Handle to property for enabling/disabling shadows
        * @param value new shadows state
        * @return true if property handle is valid 
        */
        bool _setShadowsEnabled(OgitorsPropertyBase* property, const bool& value);
        /**
        * Property setter for shadows technique
        * @param property Handle to property for shadows technique
        * @param value new shadows technique
        * @return true if property handle is valid 
        */
        bool _setShadowsTechnique(OgitorsPropertyBase* property, const int& value);
        /**
        * Property setter for shadows rendering distance
        * @param property Handle to property for shadows rendering distance
        * @param value new shadows rendering distance
        * @return true if property handle is valid 
        */
        bool _setShadowsRenderingDistance(OgitorsPropertyBase* property, const int& value);
        /**
        * Property setter for near shadow map resolution
        * @param property Handle to property for near shadow map resolution
        * @param value new near shadow map resolution
        * @return true if property handle is valid 
        */
        bool _setShadowsResolutionNear(OgitorsPropertyBase* property, const int& value);
        /**
        * Property setter for middle shadow map resolution
        * @param property Handle to property for middle shadow map resolution
        * @param value new middle shadow map resolution
        * @return true if property handle is valid 
        */
        bool _setShadowsResolutionMiddle(OgitorsPropertyBase* property, const int& value);
        /**
        * Property setter for far shadow map resolution
        * @param property Handle to property for far shadow map resolution
        * @param value new far shadow map resolution
        * @return true if property handle is valid 
        */
        bool _setShadowsResolutionFar(OgitorsPropertyBase* property, const int& value);
    };

    //! Scene manager editor factory class
    /*!  
    A class that is responsible for instantiating scene manager editor class(es)
    */
    class OgitorExport CSceneManagerEditorFactory: public CBaseEditorFactory
    {
    public:
        /** @copydoc CBaseEditorFactory::CBaseEditorFactory() */
        CSceneManagerEditorFactory(OgitorsView *view = 0);
        /** @copydoc CBaseEditorFactory::~CBaseEditorFactory() */
        virtual ~CSceneManagerEditorFactory() {};
        /** @copydoc CBaseEditorFactory::duplicate(OgitorsView *view) */
        virtual CBaseEditorFactory* duplicate(OgitorsView *view);
        /** @copydoc CBaseEditorFactory::CreateObject(CBaseEditor **, OgitorsPropertyValueMap &) */
        virtual CBaseEditor *CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params);
        /**
        * Fetches a list of properties containing fog mode(s)
        * @return a list of properties containing fog mode(s)
        */
        static PropertyOptionsVector *getFogModes() { return &mFogModes; }
        static PropertyOptionsVector *getTextureMapSizes() { return &mTextureMapSizes; }
        static PropertyOptionsVector *getShadowTechniqueNames() { return &mShadowTechniqueNames; }

    protected:
        static PropertyOptionsVector mFogModes;             /** Fog mode(s) property list */
        static PropertyOptionsVector mTextureMapSizes;      /** Texture Map Sizes property list */
        static PropertyOptionsVector mShadowTechniqueNames; /** Shadow Techniques property list */
    };
}



