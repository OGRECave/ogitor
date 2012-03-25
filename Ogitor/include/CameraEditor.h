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

namespace Ogitors
{
    class CameraVisualHelper;

    enum CameraViewMode
    {
        CVM_FREE = 0,
        CVM_FIX_CURRENT = 1,
        CVM_GLOBAL_LEFT = 2,
        CVM_GLOBAL_RIGHT = 3,
        CVM_GLOBAL_FRONT = 4,
        CVM_GLOBAL_BACK = 5,
        CVM_GLOBAL_TOP = 6,
        CVM_GLOBAL_BOTTOM = 7,
        CVM_LOCAL_LEFT = 8,
        CVM_LOCAL_RIGHT = 9,
        CVM_LOCAL_FRONT = 10,
        CVM_LOCAL_BACK = 11,
        CVM_LOCAL_TOP = 12,
        CVM_LOCAL_BOTTOM = 13
    };

    //! Camera editor class
    /*!  
        A camera editor class that assists in editing camera(s) on the scene
    */
    class OgitorExport CCameraEditor: public CBaseEditor
    {
        friend class CCameraEditorFactory;

    public:

        inline void setPosition(const Ogre::Vector3& position) { mPosition->set(position); }
        inline void setOrientation(const Ogre::Quaternion& orientation) { mOrientation->set(orientation); }
        inline void setClipDistance(const Ogre::Vector2& value) { mClipDistance->set(value); }
        inline void setFOV(Ogre::Real value) { mFOV->set(value); }
        inline void setPolygonMode(int value) { mPolygonMode->set(value); }
        inline void setViewMode(int value) { mViewMode->set(value); }

        inline Ogre::Vector3    getPosition() { return mPosition->get(); }
        inline Ogre::Quaternion getOrientation() { return mOrientation->get(); }
        inline CameraViewMode   getViewMode() { return (CameraViewMode)mViewMode->get(); }
        inline Ogre::Vector2    getClipDistance() { return mClipDistance->get(); }
        inline int              getPolygonMode() { return mPolygonMode->get(); }
        inline Ogre::Real       getFOV() { return mFOV->get(); }
        inline Ogre::Vector3    getDirection() { return (getDerivedOrientation() * Ogre::Vector3::NEGATIVE_UNIT_Z); }

        /// This function is called when user right clicks a property to get that property specific popup menu
        /// returns false if no menu present (Disabled Menu Items has a "D" prefix where Enabled Menu Items have "E" prefix
        virtual bool     getObjectContextMenu(UTFStringVector &menuitems);
        /// This function is called when user selects a menuitem from ContextMenu
        virtual void     onObjectContextMenu(int menuresult);
        virtual TiXmlElement* exportDotScene(TiXmlElement *pParent);
        /** @copydoc CBaseEditor::setParentImpl(CBaseEditor *,CBaseEditor *) */
        virtual void     setParentImpl(CBaseEditor *oldparent, CBaseEditor *newparent);
        /** @copydoc CBaseEditor::setLayerImpl(unsigned int) */
        virtual bool     setLayerImpl(unsigned int newlayer);
        /** @copydoc CBaseEditor::load(bool) */
        virtual bool     load(bool async = true);
        /** @copydoc CBaseEditor::unLoad() */
        virtual bool     unLoad();
        /** @copydoc CBaseEditor::setSelectedImpl(bool) */
        virtual void     setSelectedImpl(bool bSelected);
        /** @copydoc CBaseEditor::setHighlightedImpl(bool) */
        virtual bool     setHighlightedImpl(bool bSelected);
        /** @copydoc CBaseEditor::showBoundingBox(bool) */
        virtual void     showBoundingBox(bool bShow);
        /**
        * Fetches camera handle
        * @return camera handle
        */
        inline Ogre::Camera*  getCamera() {return mHandle;};
        /** @copydoc CBaseEditor::postSceneUpdate(Ogre::SceneManager *, Ogre::Camera *, Ogre::RenderWindow *) */
        virtual bool     postSceneUpdate(Ogre::SceneManager *SceneMngr, Ogre::Camera *Camera, Ogre::RenderWindow *RenderWindow);
        /** @copydoc CBaseEditor::getHandle() */
        inline virtual void *getHandle() {return static_cast<void*>(mHandle);};
        /** @copydoc CBaseEditor::createProperties(Ogre::NameValuePairList &) */
        virtual void     createProperties(OgitorsPropertyValueMap &params);

        /**
        * Sets camera yaw value
        * @param value camera yaw value
        */
        void             yaw(const Ogre::Radian &value);
        /**
        * Sets camera pitch value
        * @param value camera pitch value
        */
        void             pitch(const Ogre::Radian &value);
        /**
        * Sets camera roll value
        * @param value camera roll value
        */
        void             roll(const Ogre::Radian &value);
        /**
        * Sets camera direction
        * @param value camera direction vector
        */
        void             setDirection(const Ogre::Vector3 &vec);
        /**
        * Sets camera look-at vector coordinate
        * @param value camera look-at vector coordinate
        */
        void             lookAt(const Ogre::Vector3 &value);

        /** @copydoc CBaseEditor::getAABB() */
        virtual Ogre::AxisAlignedBox getAABB();
        /** @copydoc CBaseEditor::getWorldAABB() */
        virtual Ogre::AxisAlignedBox getWorldAABB();
        /** @copydoc CBaseEditor::getDerivedPosition() */
        virtual Ogre::Vector3        getDerivedPosition();
        /** @copydoc CBaseEditor::setDerivedPosition(Ogre::Vector3) */
        virtual void                 setDerivedPosition(Ogre::Vector3 val);
         /** @copydoc CBaseEditor::getDerivedOrientation() */
        virtual Ogre::Quaternion     getDerivedOrientation();

    protected:
        Ogre::Camera     *mHandle;                                 /** Camera handle */
        OgitorsProperty<Ogre::Vector3>     *mPosition;             /** Camera position (wrapper) */
        OgitorsProperty<Ogre::Quaternion>  *mOrientation;          /** Camera orientation (wrapper) */
        OgitorsProperty<Ogre::Vector2>     *mClipDistance;         /** Camera clipping distance (wrapper) */
        OgitorsProperty<Ogre::String>      *mAutoTrackTarget;      /** Camera tracking target name (wrapper) */
        OgitorsProperty<Ogre::Real>        *mFOV;                  /** Camera field of view (wrapper) */
        OgitorsProperty<int>               *mPolygonMode;          /** Camera polygon mode (wrapper) */
        OgitorsProperty<int>               *mViewMode;             /** Camera view restriction mode */
        OgitorsProperty<bool>              *mAutoAspectRatio;      /** Camera auto aspect ratio mode (wrapper) */
        CBaseEditor                        *mAutoTrackTargetPtr;   /** Camera tracking target (wrapper) */
        OgitorsScopedConnection             mAutoTrackTargetConnection[3];    /** Camera tracking target connection(s) (wrapper) */

        /**
        * Constructor
        */
        CCameraEditor(CBaseEditorFactory *factory);
        /**
        * Destructor
        */
        virtual ~CCameraEditor();

        /**
        * Sets camera origin position
        * @param position camera origin position
        */
        bool _setPosition(OgitorsPropertyBase* property, const Ogre::Vector3& position);
        /**
        * Sets camera orientation
        * @param orientation camera orientation
        */
        bool _setOrientation(OgitorsPropertyBase* property, const Ogre::Quaternion& orientation);
        /**
        * Sets camera clipping distance
        * @param value camera clipping distance
        */
        bool _setClipDistance(OgitorsPropertyBase* property, const Ogre::Vector2& value);
        /**
        * Sets camera tracking target name
        * @param targetname camera tracking target name
        */
        bool _setAutoTrackTarget(OgitorsPropertyBase* property, const Ogre::String& targetname);
        /**
        * Sets camera Field of View (FOV) angle
        * @param value camera FOV angle
        */
        bool _setFOV(OgitorsPropertyBase* property, const Ogre::Real& value);
        /**
        * Sets camera polygon mode
        * @param value camera polygon mode
        */
        bool _setPolygonMode(OgitorsPropertyBase* property, const int& value);
        /**
        * Sets camera view restriction mode
        * @param value camera view restriction mode
        */
        bool _setViewMode(OgitorsPropertyBase* property, const int& value);
        /**
        * Sets camera's auto aspect ratio feature
        * @param value true if auto aspect ratio will be enabled, otherwise false
        */
        bool _setAutoAspectRatio(OgitorsPropertyBase* property, const bool& value);
        /**
        * Delegate function that is called when tracking target was destroyed
        * @param value <unused>
        */
        void OnTrackTargetDestroyed(const OgitorsPropertyBase* property, Ogre::Any value);
        /**
        * Delegate function that is called when tracking target' position has changed
        * @param value tracking target new position
        */
        void OnTrackTargetPositionChange(const OgitorsPropertyBase* property, Ogre::Any value);
        /**
        * Delegate function that is called when tracking target' name has changed
        * @param value tracking target new name
        */
        void OnTrackTargetNameChange(const OgitorsPropertyBase* property, Ogre::Any value);
    };
    
    //! Camera editor factory class
    /*!  
    A class that instantiates camera editor and tracks it thereafter
    */
    class OgitorExport CCameraEditorFactory: public CBaseEditorFactory
    {
    public:
        /**
        * Constructor
        */
        CCameraEditorFactory(OgitorsView *view = 0);
        /**
        * Destructor
        */
        virtual ~CCameraEditorFactory() {};
        /** @copydoc CBaseEditorFactory::duplicate(OgitorsView *view) */
        virtual CBaseEditorFactory* duplicate(OgitorsView *view);
        /** @copydoc CBaseEditorFactor::CreateObject(CBaseEditor **, Ogre::NameValuePairList &) */
        virtual CBaseEditor *CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params);
        /** @copydoc CBaseEditorFactor::GetPlaceHolderName) */
        virtual Ogre::String GetPlaceHolderName() {return "scbCamera.mesh";};

        static PropertyOptionsVector *getCameraPolygonModes() { return &mCameraPolygonModes; }
        static PropertyOptionsVector *getCameraViewModes() { return &mCameraViewModes; }

    protected:
        static PropertyOptionsVector mCameraPolygonModes;
        static PropertyOptionsVector mCameraViewModes;

    };
}
