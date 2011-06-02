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

#ifndef SKYX_EDITOR_H
#define SKYX_EDITOR_H

#include <SkyX.h>

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

    class PluginExport CSkyxEditor: public CBaseEditor
    {
        friend class CSkyxEditorFactory;
    public:

        virtual bool            setNameImpl(Ogre::String name) { return false; };
        virtual void            showBoundingBox(bool bShow) {};
        virtual bool            update(float timePassed);
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
        SkyX::SkyX                *mHandle;

        OgitorsProperty<Ogre::Real> *mTimeMultiplier;
        OgitorsProperty<Ogre::Vector3> *mTime;
        OgitorsProperty<Ogre::Vector2> *mEastPosition;
        OgitorsProperty<Ogre::Vector3> *mWaveLength;
        OgitorsProperty<Ogre::Real> *mRayleighMultiplier;
        OgitorsProperty<Ogre::Real> *mMieMultiplier;
        OgitorsProperty<Ogre::Real> *mExposure;
        OgitorsProperty<Ogre::Real> *mInnerRadius;
        OgitorsProperty<Ogre::Real> *mOuterRadius;
        OgitorsProperty<int>        *mSampleCount;
        OgitorsProperty<Ogre::Real> *mHeight;
        OgitorsProperty<Ogre::Real> *mSunIntensity;
        OgitorsProperty<Ogre::Real> *mG;
        OgitorsProperty<Ogre::Real> *mWindSpeed;
        OgitorsProperty<Ogre::Real> *mWindDirection;
        OgitorsProperty<Ogre::Real> *mNoiseScale;

        CSkyxEditor(CBaseEditorFactory *factory);
        virtual ~CSkyxEditor();

        void _restoreState();

        bool _setTimeMultiplier(OgitorsPropertyBase* property, const Ogre::Real& value);

        bool _setOptionsRayleighMultiplier(OgitorsPropertyBase* property, const Ogre::Real& value);
        bool _setOptionsMieMultiplier(OgitorsPropertyBase* property, const Ogre::Real& value);
        bool _setOptionsSampleCount(OgitorsPropertyBase* property, const int& value);
        bool _setOptionsHeight(OgitorsPropertyBase* property, const Ogre::Real& value);
        bool _setOptionsExposure(OgitorsPropertyBase* property, const Ogre::Real& value);
        bool _setOptionsInnerRadius(OgitorsPropertyBase* property, const Ogre::Real& value);
        bool _setOptionsOuterRadius(OgitorsPropertyBase* property, const Ogre::Real& value);
        bool _setOptionsTime(OgitorsPropertyBase* property, const Ogre::Vector3& value);
        bool _setOptionsEastPosition(OgitorsPropertyBase* property, const Ogre::Vector2& value);
        bool _setOptionsSunIntensity(OgitorsPropertyBase* property, const Ogre::Real& value);
        bool _setOptionsWaveLength(OgitorsPropertyBase* property, const Ogre::Vector3& value);
        bool _setOptionsG(OgitorsPropertyBase* property, const Ogre::Real& value);

        bool _setNoiseScale(OgitorsPropertyBase* property, const Ogre::Real& value);
        bool _setWindSpeed(OgitorsPropertyBase* property, const Ogre::Real& value);
        bool _setWindDirection(OgitorsPropertyBase* property, const Ogre::Real& value);

    };

    class PluginExport CSkyxEditorFactory: public CBaseEditorFactory
    {
    public:
        CSkyxEditorFactory(OgitorsView *view = 0);
        virtual ~CSkyxEditorFactory() {};
        /** @copydoc CBaseEditorFactory::duplicate(OgitorsView *view) */
        virtual CBaseEditorFactory* duplicate(OgitorsView *view);
        virtual CBaseEditor *CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params);
        virtual void DestroyObject(CBaseEditor *object);
        virtual bool CanInstantiate() {return (mInstanceCount == 0);};
    };
}

extern "C" bool PluginExport dllStartPlugin(void *identifier, Ogre::String& name);

extern "C" bool PluginExport dllGetPluginName(Ogre::String& name);

extern "C" bool PluginExport dllStopPlugin(void);

#endif