///////////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////////

#ifndef HYDRAX_WATER_EDITOR_H
#define HYDRAX_WATER_EDITOR_H

#include "Hydrax.h"
#include "Noise/Perlin/Perlin.h"
#include "Modules/ProjectedGrid/ProjectedGrid.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
   #ifdef PLUGIN_EXPORT
     #define PluginExport __declspec (dllexport)
   #else
     #define PluginExport __declspec (dllimport)
   #endif
#else
   #define PluginExport
#endif

namespace Ogitors
{

    class PluginExport CHydraxEditor: public CBaseEditor
    {
        friend class CHydraxEditorFactory;
    public:

        bool getObjectContextMenu(UTFStringVector &menuitems);
        void onObjectContextMenu(int menuresult);

        virtual bool            setNameImpl(Ogre::String name) { return false; };
        virtual void            showBoundingBox(bool bShow) {};
        virtual bool            postSceneUpdate(Ogre::SceneManager *SceneMngr, Ogre::Camera *Camera, Ogre::RenderWindow *RenderWindow);
        virtual bool            update(float timePassed);
        Ogre::String            dummyGetter() {return "";};
        void                    addDepthTechnique(const Ogre::String& matname);
        void                    removeDepthTechnique(const Ogre::String& matname);
        void                    refresh();

        /** @copydoc CBaseEditor::load(bool) */
        virtual bool            load(bool async = true);
        /** @copydoc CBaseEditor::unLoad() */
        virtual bool            unLoad();
        /// Processes a NameValuePairList and Sets Object's Properties according to it
        virtual void            createProperties(OgitorsPropertyValueMap &params);
        /// Called by Serializer to write custom files during an export
        virtual void            onSave(bool forced = false);
        /// Gets the Handle to encapsulated object
        inline virtual void    *getHandle() {return static_cast<void*>(mHandle);};
        virtual Ogre::SceneManager *getSceneManager();

    protected:
        Hydrax::Hydrax                *mHandle;
        Hydrax::Module::ProjectedGrid *mModule;

        OgitorsProperty<Ogre::String>      *mConfigFile;
        OgitorsProperty<Ogre::Vector3>     *mPosition;
        OgitorsProperty<Ogre::ColourValue> *mOriginalWaterColour;
        OgitorsProperty<bool>              *mCaelumIntegration;

        OgitorsProperty<Ogre::Real>    *mPlanesError;
        OgitorsProperty<int>           *mShaderMode;
        OgitorsProperty<Ogre::Real>    *mFullReflectionDistance;
        OgitorsProperty<Ogre::Real>    *mGlobalTransparency;
        OgitorsProperty<Ogre::Real>    *mNormalDistortion;

        OgitorsProperty<bool> *mComponentSun;
        OgitorsProperty<bool> *mComponentFoam;
        OgitorsProperty<bool> *mComponentDepth;
        OgitorsProperty<bool> *mComponentSmooth;
        OgitorsProperty<bool> *mComponentCaustics;
        OgitorsProperty<bool> *mComponentUnderwater;
        OgitorsProperty<bool> *mComponentUnderwaterReflections;
        OgitorsProperty<bool> *mComponentUnderwaterGodrays;

        OgitorsProperty<Ogre::Vector3>      *mSunPosition;
        OgitorsProperty<Ogre::Real>         *mSunStrength;
        OgitorsProperty<Ogre::Real>         *mSunArea;
        OgitorsProperty<Ogre::ColourValue>  *mSunColour;

        OgitorsProperty<Ogre::Real>         *mFoamStart;
        OgitorsProperty<Ogre::Real>         *mFoamMaxDistance;
        OgitorsProperty<Ogre::Real>         *mFoamScale;
        OgitorsProperty<Ogre::Real>         *mFoamTransparency;

        OgitorsProperty<Ogre::Real>         *mDepthLimit;
        OgitorsProperty<Ogre::Real>         *mSmoothPower;

        OgitorsProperty<Ogre::Real>         *mCausticsScale;
        OgitorsProperty<Ogre::Real>         *mCausticsPower;
        OgitorsProperty<Ogre::Real>         *mCausticsEnd;

        OgitorsProperty<Ogre::Vector3>      *mGodraysExposure;
        OgitorsProperty<Ogre::Real>         *mGodraysIntensity;
        OgitorsProperty<Ogre::Real>         *mGodraysSpeed;
        OgitorsProperty<int>                *mGodraysNumberOfRays;
        OgitorsProperty<Ogre::Real>         *mGodraysSize;
        OgitorsProperty<bool>               *mGodraysIntersections;

        OgitorsProperty<int>                *mRttReflection;
        OgitorsProperty<int>                *mRttRefraction;
        OgitorsProperty<int>                *mRttDepth;
        OgitorsProperty<int>                *mRttDepthReflection;
        OgitorsProperty<int>                *mRttDepthAIP;
        OgitorsProperty<int>                *mRttGpuNormalMap;

        OgitorsProperty<Ogre::String>       *mModuleName;
        OgitorsProperty<Ogre::Real>         *mPGChoppyStrength;
        OgitorsProperty<bool>               *mPGChoppyWaves;
        OgitorsProperty<int>                *mPGComplexity;
        OgitorsProperty<float>              *mPGElevation;
        OgitorsProperty<bool>               *mPGForceRecalculateGeometry;
        OgitorsProperty<bool>               *mPGSmooth;
        OgitorsProperty<float>              *mPGStrength;

        OgitorsProperty<Ogre::String>       *mNoiseModuleName;
        OgitorsProperty<int>                *mNoiseOctaves;
        OgitorsProperty<Ogre::Real>         *mNoiseScale;
        OgitorsProperty<Ogre::Real>         *mNoiseFallOff;
        OgitorsProperty<Ogre::Real>         *mNoiseAnimSpeed;
        OgitorsProperty<Ogre::Real>         *mNoiseTimeMulti;
        OgitorsProperty<Ogre::Real>         *mNoiseGpuStrength;
        OgitorsProperty<Ogre::Vector3>      *mNoiseGpuLod;

        CHydraxEditor(CBaseEditorFactory *factory);
        virtual ~CHydraxEditor();

        void _initAndSignalProperties();
        void _createWaterPlane(Ogre::SceneManager *mSceneMgr, Ogre::Camera *mCamera, Ogre::Viewport *mViewport);

        // SETTERS
        bool _setConfigFile(OgitorsPropertyBase* property, const Ogre::String& value);
        bool _setPosition(OgitorsPropertyBase* property, const Ogre::Vector3& value);
        bool _setAddDepthTechnique(OgitorsPropertyBase* property, const Ogre::String& value);
        bool _setRemoveDepthTechnique(OgitorsPropertyBase* property, const Ogre::String& value);

        bool _setPlanesError(OgitorsPropertyBase* property, const Ogre::Real& value);
        bool _setFullReflectionDistance(OgitorsPropertyBase* property, const Ogre::Real& value);
        bool _setGlobalTransparency(OgitorsPropertyBase* property, const Ogre::Real& value);
        bool _setWaterColour(OgitorsPropertyBase* property, const Ogre::ColourValue& value);
        bool _setNormalDistortion(OgitorsPropertyBase* property, const Ogre::Real& value);

        bool _setShaderMode(OgitorsPropertyBase* property, const int& value);

        void __setComponent( Hydrax::HydraxComponent hc, const bool &value );
        bool _setComponentSun(OgitorsPropertyBase* property, const bool& value);
        bool _setComponentFoam(OgitorsPropertyBase* property, const bool& value);
        bool _setComponentDepth(OgitorsPropertyBase* property, const bool& value);
        bool _setComponentSmooth(OgitorsPropertyBase* property, const bool& value);
        bool _setComponentCaustics(OgitorsPropertyBase* property, const bool& value);
        bool _setComponentUnderwater(OgitorsPropertyBase* property, const bool& value);
        bool _setComponentUnderwaterReflections(OgitorsPropertyBase* property, const bool& value);
        bool _setComponentUnderwaterGodRays(OgitorsPropertyBase* property, const bool& value);

        bool _setSunPosition(OgitorsPropertyBase* property, const Ogre::Vector3& value);
        bool _setSunStrength(OgitorsPropertyBase* property, const Ogre::Real& value);
        bool _setSunArea(OgitorsPropertyBase* property, const Ogre::Real& value);
        bool _setSunColour(OgitorsPropertyBase* property, const Ogre::ColourValue& value);

        bool _setFoamMaxDistance(OgitorsPropertyBase* property, const Ogre::Real& value);
        bool _setFoamScale(OgitorsPropertyBase* property, const Ogre::Real& value);
        bool _setFoamStart(OgitorsPropertyBase* property, const Ogre::Real& value);
        bool _setFoamTransparency(OgitorsPropertyBase* property, const Ogre::Real& value);

        bool _setDepthLimit(OgitorsPropertyBase* property, const Ogre::Real& value);

        bool _setSmoothPower(OgitorsPropertyBase* property, const Ogre::Real& value);

        bool _setCausticsScale(OgitorsPropertyBase* property, const Ogre::Real& value);
        bool _setCausticsPower(OgitorsPropertyBase* property, const Ogre::Real& value);
        bool _setCausticsEnd(OgitorsPropertyBase* property, const Ogre::Real& value);

        bool _setGodraysExposure(OgitorsPropertyBase* property, const Ogre::Vector3& value);
        bool _setGodraysIntensity(OgitorsPropertyBase* property, const Ogre::Real& value);
        bool _setGodraysSpeed(OgitorsPropertyBase* property, const Ogre::Real& value);
        bool _setGodraysNumberOfRays(OgitorsPropertyBase* property, const int& value);
        bool _setGodraysSize(OgitorsPropertyBase* property, const Ogre::Real& value);
        bool _setGodraysIntersections(OgitorsPropertyBase* property, const bool& value);

        bool _setRttReflection(OgitorsPropertyBase* property, const int& value);
        bool _setRttRefraction(OgitorsPropertyBase* property, const int& value);
        bool _setRttDepth(OgitorsPropertyBase* property, const int& value);
        bool _setRttDepthReflection(OgitorsPropertyBase* property, const int& value);
        bool _setRttDepthAIP(OgitorsPropertyBase* property, const int& value);
        bool _setRttGpuNormalMap(OgitorsPropertyBase* property, const int& value);

        bool _setModuleName(OgitorsPropertyBase* property, const Ogre::String& value);
        bool _setPGChoppyStrength(OgitorsPropertyBase* property, const Ogre::Real& value);
        bool _setPGChoppyWaves(OgitorsPropertyBase* property, const bool& value);
        bool _setPGComplexity(OgitorsPropertyBase* property, const int& value);
        bool _setPGElevation(OgitorsPropertyBase* property, const Ogre::Real& value);
        bool _setPGForceRecalculateGeometry(OgitorsPropertyBase* property, const bool& value);
        bool _setPGSmooth(OgitorsPropertyBase* property, const bool& value);
        bool _setPGStrength(OgitorsPropertyBase* property, const Ogre::Real& value);

        bool _setNoiseModuleName(OgitorsPropertyBase* property, const Ogre::String& value);
        bool _setNoiseOctaves(OgitorsPropertyBase* property, const int& value);
        bool _setNoiseScale(OgitorsPropertyBase* property, const Ogre::Real& value);
        bool _setNoiseFallOff(OgitorsPropertyBase* property, const Ogre::Real& value);
        bool _setNoiseAnimSpeed(OgitorsPropertyBase* property, const Ogre::Real& value);
        bool _setNoiseTimeMulti(OgitorsPropertyBase* property, const Ogre::Real& value);
        bool _setNoiseGpuStrength(OgitorsPropertyBase* property, const Ogre::Real& value);
        bool _setNoiseGpuLod(OgitorsPropertyBase* property, const Ogre::Vector3& value);

    };

    class PluginExport CHydraxEditorFactory: public CBaseEditorFactory
    {
    public:
        CHydraxEditorFactory(OgitorsView *view = 0);
        virtual ~CHydraxEditorFactory() {};
        /** @copydoc CBaseEditorFactory::duplicate(OgitorsView *view) */
        virtual CBaseEditorFactory* duplicate(OgitorsView *view);
        virtual CBaseEditor *CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params);
        virtual void DestroyObject(CBaseEditor *object);
        virtual bool CanInstantiate() {return (mInstanceCount == 0);};
    protected:
        static PropertyOptionsVector mShaderModes;
        static PropertyOptionsVector mTextureSizes;
    };
}

extern "C" bool PluginExport dllStartPlugin(void *identifier, Ogre::String& name);

extern "C" bool PluginExport dllGetPluginName(Ogre::String& name);

extern "C" bool PluginExport dllStopPlugin(void);

#endif