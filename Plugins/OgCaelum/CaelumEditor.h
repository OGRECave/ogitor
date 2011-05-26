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

#ifndef CAELUM_EDITOR_H
#define CAELUM_EDITOR_H

#include <Caelum.h>

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

    class PluginExport CCaelumEditor: public CBaseEditor
    {
        friend class CCaelumEditorFactory;
    public:

        /// OVERRIDES
        virtual bool         setNameImpl(Ogre::String name) { return false; };
        /** @copydoc CBaseEditor::load(bool) */
        virtual bool         load(bool async = true);
        /** @copydoc CBaseEditor::unLoad() */
        virtual bool         unLoad();
        virtual void         showBoundingBox(bool bShow) {};
        /// Gets the Handle to encapsulated object
        inline virtual void *getHandle() {return static_cast<void*>(mHandle);};

        /// Overridden by Editor Objects to Export Object Dependant Properties
        virtual bool         update(float timePassed);

        virtual void         createProperties(Ogitors::OgitorsPropertyValueMap &params);

    protected:
        Caelum::CaelumSystem *mHandle;
        Ogre::SceneManager   *mSceneManager;
        float _mUpdateCounter;

        OgitorsProperty<int>    *mClockYear;
        OgitorsProperty<int>    *mClockMonth;
        OgitorsProperty<int>    *mClockDay;
        OgitorsProperty<int>    *mClockHour;
        OgitorsProperty<int>    *mClockMinute;
        OgitorsProperty<int>    *mClockSecond;
        OgitorsProperty<Ogre::Real>    *mClockSpeed;

        OgitorsProperty<Ogre::Real>    *mLongitude;
        OgitorsProperty<Ogre::Real>    *mLatitude;

        OgitorsProperty<bool>    *mLightingSingleLightSource;
        OgitorsProperty<bool>    *mLightingSingleShadowSource;
        OgitorsProperty<bool>    *mLightingManageAmbientLight;
        OgitorsProperty<Ogre::ColourValue>    *mLightingMinimumAmbientLight;

        OgitorsProperty<bool>    *mFogManage;
        OgitorsProperty<Ogre::Real>    *mFogDensityMultiplier;

        OgitorsProperty<bool>    *mSunEnable;
        OgitorsProperty<Ogre::ColourValue>    *mSunAmbientMultiplier;
        OgitorsProperty<Ogre::ColourValue>    *mSunDiffuseMultiplier;
        OgitorsProperty<Ogre::ColourValue>    *mSunSpecularMultiplier;
        OgitorsProperty<Ogre::ColourValue>    *mSunLinearMultiplier;
        OgitorsProperty<bool>    *mSunAutoDisable;
        OgitorsProperty<bool>    *mSunCastShadow;
        OgitorsProperty<Ogre::Real>    *mSunAttenuationDistance;
        OgitorsProperty<Ogre::Real>    *mSunAttenuationConstantMultiplier;
        OgitorsProperty<Ogre::Real>    *mSunAttenuationLinearMultiplier;
        OgitorsProperty<Ogre::Real>    *mSunAttenuationQuadricMultiplier;

        OgitorsProperty<Ogre::Vector3>    *mSunPosition;
        OgitorsProperty<Ogre::ColourValue>    *mSunColour;
        OgitorsProperty<Ogre::ColourValue>    *mSunLightColour;

        OgitorsProperty<bool>    *mMoonEnable;
        OgitorsProperty<Ogre::ColourValue>    *mMoonAmbientMultiplier;
        OgitorsProperty<Ogre::ColourValue>    *mMoonDiffuseMultiplier;
        OgitorsProperty<Ogre::ColourValue>    *mMoonSpecularMultiplier;
        OgitorsProperty<Ogre::ColourValue>    *mMoonLinearMultiplier;
        OgitorsProperty<bool>    *mMoonAutoDisable;
        OgitorsProperty<bool>    *mMoonCastShadow;
        OgitorsProperty<Ogre::Real>    *mMoonAttenuationDistance;
        OgitorsProperty<Ogre::Real>    *mMoonAttenuationConstantMultiplier;
        OgitorsProperty<Ogre::Real>    *mMoonAttenuationLinearMultiplier;
        OgitorsProperty<Ogre::Real>    *mMoonAttenuationQuadricMultiplier;

        OgitorsProperty<bool>    *mStarsEnable;
        OgitorsProperty<Ogre::Real>    *mStarsMagnitudeScale;
        OgitorsProperty<Ogre::Real>    *mStarsMag0PixelSize;
        OgitorsProperty<Ogre::Real>    *mStarsMinPixelSize;
        OgitorsProperty<Ogre::Real>    *mStarsMaxPixelSize;

        OgitorsProperty<bool>           *mCloudsEnabled[3];
        OgitorsProperty<Ogre::Real>       *mCloudsCoverage[3];
        OgitorsProperty<Ogre::Real>       *mCloudsHeight[3];
        OgitorsProperty<Ogre::Vector2> *mCloudsSpeed[3];

        bool _setClock(OgitorsPropertyBase* property, const int& value);
        bool _setClockSpeed(OgitorsPropertyBase* property, const Ogre::Real& value);

        bool _setLongitude(OgitorsPropertyBase* property, const Ogre::Real& value);
        bool _setLatitude(OgitorsPropertyBase* property, const Ogre::Real& value);

        bool _setLighting(OgitorsPropertyBase* property, const bool& value);
        bool _setLightingMinimumAmbientLight(OgitorsPropertyBase* property, const Ogre::ColourValue& value);

        bool _setFogManage(OgitorsPropertyBase* property, const bool& value);
        bool _setFogDensityMultiplier(OgitorsPropertyBase* property, const Ogre::Real& value);

        bool _setSun(OgitorsPropertyBase* property, const Ogre::ColourValue& value);
        bool _setSunEnable(OgitorsPropertyBase* property, const bool& value);
        bool _setSunAutoDisable(OgitorsPropertyBase* property, const bool& value);
        bool _setSunCastShadow(OgitorsPropertyBase* property, const bool& value);
        bool _setSunAttenuation(OgitorsPropertyBase* property, const Ogre::Real& value);

        bool _setMoon(OgitorsPropertyBase* property, const Ogre::ColourValue& value);
        bool _setMoonEnable(OgitorsPropertyBase* property, const bool& value);
        bool _setMoonAutoDisable(OgitorsPropertyBase* property, const bool& value);
        bool _setMoonCastShadow(OgitorsPropertyBase* property, const bool& value);
        bool _setMoonAttenuation(OgitorsPropertyBase* property, const Ogre::Real& value);

        bool _setStars(OgitorsPropertyBase* property, const Ogre::Real& value);
        bool _setStarsEnable(OgitorsPropertyBase* property, const bool& value);

        bool _setCloudsEnable(OgitorsPropertyBase* property, const bool& value);
        bool _setCloudsCoverage(OgitorsPropertyBase* property, const Ogre::Real& value);
        bool _setCloudsHeight(OgitorsPropertyBase* property, const Ogre::Real& value);
        bool _setCloudsSpeed(OgitorsPropertyBase* property, const Ogre::Vector2& value);

        CCaelumEditor(CBaseEditorFactory *factory);
        virtual ~CCaelumEditor();

        int  _getMonthValue(Ogre::String val);
        bool _createSky(Ogre::SceneManager *mngr, Ogre::Camera *cam);
        void _initClockProperties();
    };

    class PluginExport CCaelumEditorFactory: public CBaseEditorFactory
    {
    public:
        CCaelumEditorFactory(OgitorsView *view = 0);
        virtual ~CCaelumEditorFactory() {};
        /** @copydoc CBaseEditorFactory::duplicate(OgitorsView *view) */
        virtual CBaseEditorFactory* duplicate(OgitorsView *view);
        virtual CBaseEditor *CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params);
        virtual bool CanInstantiate() {return (mInstanceCount == 0);};
    protected:
        static PropertyOptionsVector mMonths;
    };
}

extern "C" bool PluginExport dllStartPlugin(void *identifier, Ogre::String& name);

extern "C" bool PluginExport dllGetPluginName(Ogre::String& name);

extern "C" bool PluginExport dllStopPlugin(void);

#endif