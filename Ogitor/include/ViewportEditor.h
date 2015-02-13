/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2015 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
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

#pragma once

// Includes
#include "ViewGrid.h"

namespace Ogitors
{

    //! Viewport editor class
    /*!  
    A class that takes care of scene viewport changes
    */
    class OgitorExport CViewportEditor: public CBaseEditor
    {
        friend class CViewportEditorFactory;
    public:
        /**
        * Fetches viewport camera position
        * @return viewport camera position
        */
        inline Ogre::Vector3    getCamPosition() { return mCamPosition->get(); }
        /**
        * Fetches viewport camera orientation
        * @return viewport camera orientation
        */
        inline Ogre::Quaternion getCamOrientation() { return mCamOrientation->get(); }
        /**
        * Fetches viewport camera polygon mode
        * @return viewport camera polygon mode
        */
        inline int              getCamPolyMode() { return mCamPolyMode->get(); }
        /**
        * Fetches viewport camera clip distance
        * @return viewport camera clip distance
        */
        inline Ogre::Vector2    getCamClipDistance() { return mCamClipDistance->get(); }
        /**
        * Fetches viewport index
        * @return viewport index
        */
        inline int              getViewportIndex() { return mViewportIndex->get(); }
        
        /** @copydoc CBaseEditor::load(bool) */
        virtual bool  load(bool async = true);
        /** @copydoc CBaseEditor::unLoad() */
        virtual bool  unLoad();
        virtual void  prepareBeforePresentProperties();
        /** @copydoc CBaseEditor::createProperties(OgitorsPropertyValueMap &) */
        virtual void  createProperties(OgitorsPropertyValueMap &params);
        /** @copydoc CBaseEditor::getPropertyContextMenu(Ogre::String, UTFStringVector &) */
        virtual bool  getPropertyContextMenu(Ogre::String propertyName, UTFStringVector &menuitems);
        /** @copydoc CBaseEditor::onPropertyContextMenu(Ogre::String, int) */
        virtual void  onPropertyContextMenu(Ogre::String propertyName, int menuresult);
        /** @copydoc CBaseEditor::setNameImpl(Ogre::String) */
        virtual bool  setNameImpl(Ogre::String name);
        /** @copydoc CBaseEditor::showBoundingBox(bool) */
        virtual void  showBoundingBox(bool bShow) {};
        /** @copydoc CBaseEditor::getHandle() */
        inline virtual void *getHandle() {return static_cast<void*>(mHandle);};
        /**
        * Fetches viewport rectangle
        * @return viewport rectangle
        */
        int                  getRect(Ogre::Vector4 &rect);
        /**
        * Sets active camera editor handle
        * @param camed active camera editor handle
        */
        virtual void         setCameraEditor(CCameraEditor *camed);
        /**
        * Fetches active camera editor handle
        * @return active camera editor handle
        */
        inline CCameraEditor *getCameraEditor() {return mActiveCamera;};
        /**
        * Fetches view camera editor handle
        * @return view camera editor handle
        */
        inline CCameraEditor *getViewCameraEditor() {return mViewCamera;};
        /**
        * Re-renders active window
        */
        void                 renderWindowResized();
        /**
        * Exports the parameters to a DotScene file
        * @param output the stream to output
        * @param indent the indentation at the beginning
        */    
        virtual TiXmlElement* exportDotScene(TiXmlElement *pParent);

        virtual void onObjectDestroyed(CBaseEditor *object);
		virtual bool onBeforeDestroy();
		virtual void onDuringDestroy();

    protected:
        Ogre::Viewport                     *mHandle;                /** Viewport handle */       

        OgitorsProperty<int>               *mViewportIndex;         /** Viewport index property handle */
        OgitorsProperty<Ogre::Vector4>     *mPlacement;             /** Placement coordinates property handle */
        OgitorsProperty<bool>              *mOverlays;              /** Overlay(s) flag property handle */
        OgitorsProperty<bool>              *mSkies;                 /** Sky(ies) flag property handle */
        OgitorsProperty<bool>              *mShadows;               /** Shadow(s) property handle */
        OgitorsProperty<int>               *mCamViewMode;           /** Camera view mode property handle */
        OgitorsProperty<Ogre::Vector3>     *mCamPosition;           /** Camera position property handle */
        OgitorsProperty<Ogre::Quaternion>  *mCamOrientation;        /** Camera orientation property handle */
        OgitorsProperty<Ogre::Vector2>     *mCamClipDistance;       /** Camera clip distance property handle */
        OgitorsProperty<int>               *mCamPolyMode;           /** Camera clip mode property handle */
        OgitorsProperty<Ogre::Real>        *mCamFOV;                /** Camera FOV property handle */
        OgitorsProperty<Ogre::ColourValue> *mColour;                /** Colour property handle */

        OgitorsScopedConnection             mCameraConnections[6];

        CCameraEditor						*mActiveCamera;         /** Active camera handle */
        CCameraEditor						*mViewCamera;           /** View camera handle */

        /**
        * Constructor
        * @param factory Handle to terrain editor factory
        */
        CViewportEditor(CBaseEditorFactory *factory);
        /**
        * Destructor
        */
        virtual                ~CViewportEditor();
        /**
        * Creates new viewport
        */
        virtual void           _createViewport();
        /**
        * Property setter for compositor index (internal)
        * @param property Handle to property responsible for compositor index
        * @param value new compositor index
        * @return true if property handle is valid 
        */
        bool _setIndex(OgitorsPropertyBase* property, const int& value);
        /**
        * Property setter for compositor placement dimensions (internal)
        * @param property Handle to property responsible for compositor placement dimensions
        * @param value new compositor placement dimensions
        * @return true if property handle is valid 
        */
        bool _setPlacement(OgitorsPropertyBase* property, const Ogre::Vector4& value);
        /**
        * Property setter for overlays to be displayed (internal)
        * @param property Handle to property responsible for overlays display
        * @param value new overlays display flag value
        * @return true if property handle is valid 
        */
        bool _setOverlays(OgitorsPropertyBase* property, const bool& value);
        /**
        * Property setter for skies to be displayed (internal)
        * @param property Handle to property responsible for skies display
        * @param value new skies display flag value
        * @return true if property handle is valid 
        */
        bool _setSkies(OgitorsPropertyBase* property, const bool& value);
        /**
        * Property setter for shadows to be displayed (internal)
        * @param property Handle to property responsible for shadows display
        * @param value new shadows display flag value
        * @return true if property handle is valid 
        */
        bool _setShadows(OgitorsPropertyBase* property, const bool& value);
        /*
        * Property setter for active camera view mode (internal)
        * @param property Handle to property responsible for active camera position
        * @param value new active camera view mode
        * @return true if property handle is valid 
        */
        bool _setCamViewMode(OgitorsPropertyBase* property, const int& value);
        /*
        * Property setter for active camera position (internal)
        * @param property Handle to property responsible for active camera position
        * @param value new active camera position
        * @return true if property handle is valid 
        */
        bool _setCamPosition(OgitorsPropertyBase* property, const Ogre::Vector3& value);
        /**
        * Property setter for active camera orientation (internal)
        * @param property Handle to property responsible for active camera orientation
        * @param value new active camera orientation
        * @return true if property handle is valid 
        */
        bool _setCamOrientation(OgitorsPropertyBase* property, const Ogre::Quaternion& value);
        /**
        * Property setter for active camera clipping distance (internal)
        * @param property Handle to property responsible for active camera clipping distance
        * @param value new active camera clipping distance
        * @return true if property handle is valid 
        */
        bool _setCamClipDistance(OgitorsPropertyBase* property, const Ogre::Vector2& value);
        /**
        * Property setter for active camera polygon display mode (internal)
        * @param property Handle to property responsible for active camera polygon display mode
        * @param value new active camera polygon display mode
        * @return true if property handle is valid 
        */
        bool _setCamPolyMode(OgitorsPropertyBase* property, const int& value);
        /**
        * Property setter for active camera FOV (internal)
        * @param property Handle to property responsible for active camera FOV
        * @param value new active camera FOV
        * @return true if property handle is valid 
        */
        bool _setCamFOV(OgitorsPropertyBase* property, const Ogre::Real& value);
        /**
        * Property setter for scene background color (internal)
        * @param property Handle to property responsible for scene background color
        * @param value new scene background color
        * @return true if property handle is valid 
        */
        bool _setColour(OgitorsPropertyBase* property, const Ogre::ColourValue& value);
        

        /// CAMERA TRACKING EVENTS

        /**
        * Delegate function that is called when viewport camera view mode is changed
        * @param property view mode property handle
        * @param value new viewport camera view mode
        */
        void OnViewCameraViewModeChange(const OgitorsPropertyBase* property, Ogre::Any value)
        {
            int val = Ogre::any_cast<int>(value);
            mCamViewMode->initAndSignal(val);
        }
        /**
        * Delegate function that is called when viewport camera position is changed
        * @param property position property handle
        * @param value new viewport camera position
        */
        void OnViewCameraPositionChange(const OgitorsPropertyBase* property, Ogre::Any value)
        {
            Ogre::Vector3 val = Ogre::any_cast<Ogre::Vector3>(value);
            mCamPosition->initAndSignal(val);
        }
        /**
        * Delegate function that is called when viewport camera orientation is changed
        * @param property orientation property handle
        * @param value new viewport camera orientation
        */
        void OnViewCameraOrientationChange(const OgitorsPropertyBase* property, Ogre::Any value)
        {
            Ogre::Quaternion val = Ogre::any_cast<Ogre::Quaternion>(value);
            mCamOrientation->initAndSignal(val);
        }
        /**
        * Delegate function that is called when viewport camera' FOV is changed
        * @param property FOV property handle
        * @param value new viewport camera FOV
        */
        void OnViewCameraFOVChange(const OgitorsPropertyBase* property, Ogre::Any value)
        {
            Ogre::Real val = Ogre::any_cast<Ogre::Real>(value);
            mCamFOV->initAndSignal(val);
        }
        /**
        * Delegate function that is called when viewport camera clipping distance is changed
        * @param property clipping distance property handle
        * @param value new viewport camera clipping distance
        */
        void OnViewCameraClipDistanceChange(const OgitorsPropertyBase* property, Ogre::Any value)
        {
            Ogre::Vector2 val = Ogre::any_cast<Ogre::Vector2>(value);
            mCamClipDistance->initAndSignal(val);
        }
        /**
        * Delegate function that is called when viewport camera polygon rendering mode is changed
        * @param property polygon rendering mode property handle
        * @param value new viewport camera polygon rendering mode
        */
        void OnViewCameraPolygonModeChange(const OgitorsPropertyBase* property, Ogre::Any value)
        {
            int val = Ogre::any_cast<int>(value);
            mCamPolyMode->initAndSignal(val);

            if(val != Ogre::PM_SOLID)
                mHandle->setVisibilityMask(0x7FFFFFFF);
            else
                mHandle->setVisibilityMask(0xFFFFFFFF);
        }

    protected:
        Ogre::Vector2        mLastMouse;                    /** Last mouse position */
        Ogre::Vector2        mLastClickPoint;               /** Last 2D coordinate mouse was left-clicked at */
        Ogre::Plane          mLastUsedPlane;                /** Last Plane used during transform */
        int                  mLastButtons;                  /** Last button state */
        Ogre::Vector3        mLastPosition;                 /** Last position of viewport editor */
        Ogre::Vector3        mLastDerivedPosition;          /** Last DERIVED position of viewport editor */
        Ogre::Vector3        mLastScale;                    /** Last scale of viewport editor */
        Ogre::Quaternion     mLastOrient;                   /** Last orientation of viewport editor */
        Ogre::Quaternion     mLastDerivedOrient;            /** Last DERIVED orientation of viewport editor */
        Ogre::Vector3        mLast3DDelta;                  /** Last displacement of viewport editor */
        Ogre::Vector3        mNewCamPosition;               /** New camera position */
        ViewportGrid        *mViewGrid;                     /** View grid (assistant) handle */
        bool                 mGridVisible;                  /** Grid visibility flag */
        bool                 mVolumeSelecting;              /** Volume selection flag */
        bool                 mFirstTimeTranslation;         /** The very first translation flag */
        bool                 mMouseMovedSignal;             /** Did the mouse move since last check? */

        static NameObjectPairList  mHighLighted;            /** Highlighted object(s) list */
        static int                 mEditorToolEx;           /** Additional editor tool information */
        static int                 mEditorAxis;             /** Current editor axis */
        static float               mCameraSpeed;            /** Active camera speed */
        static bool                mIsEditing;              /** Active editing flag */
        static bool                mIsSettingPos;           /** Change to movement tool flag */
        static OgitorsUndoManager *mUndoManager;            /** Undo manager handle */
        static float               mSnapMultiplier;         /** Binding (snap) multiplier amount */

    public:
        static bool               *mViewKeyboard;           /** Viewport uses keyboard? flag */
        static OgitorsSpecialKeys  mSpecial;                /** Additional keys binding(s) */

        /**
        * Sets snap multiplier
        * @param value snap multiplier
        */
        static void  SetSnapMultiplier(float value) { mSnapMultiplier = value; }
        /**
        * Resets common values such as Undo manager, camera speed etc (see above)
        */
        static void  ResetCommonValues();
        /**
        * Assigns keyboard to be able to modify active viewport camera values
        * @param keyboard keyboard keys
        * @param keys extra shortcut key(s)
        */
        static void SetKeyboard(bool *keyboard, OgitorsSpecialKeys &keys) {mViewKeyboard = keyboard;mSpecial = keys;};
        /**
        * Fetches extra shortcut key(s)
        * @return extra shortcut key(s)
        */
        static OgitorsSpecialKeys GetKeyboard() {return mSpecial;};
        /**
        * Multiplies camera speed by specified amount
        * @param value amount to multiply camera speed by
        */
        void  MultiplyCameraSpeed(float value) 
        {
            mCameraSpeed *= value; 
            if(mCameraSpeed < 0.1f) 
                mCameraSpeed = 0.1f;
        };
        /**
        * Sets camera speed
        * @param value new camera speed
        */
        void  SetCameraSpeed(float value) 
        {
            mCameraSpeed = value;
            if(mCameraSpeed < 0.1f) 
                mCameraSpeed = 0.1f;
        };
        /**
        * Fetches camera speed
        * @return camera speed
        */
        float GetCameraSpeed() {return mCameraSpeed;};
        /**
        * Sets editor tool (splatting, painting, smoothing etc)
        * @param new editor tool
        */
        virtual void  SetEditorTool(unsigned int tool);
        /**
        * Fetches editor tool (splatting, painting, smoothing etc)
        * @return editor tool 
        */
        static unsigned int GetEditorTool();
        /**
        * Sets selected editor displacement/orientation/scale axis (X, Y, Z)
        * @param axis new editor axis
        */
        virtual void  SetEditorAxis(unsigned int axis) {mEditorAxis = axis;};
        /**
        * Fetches editor axis
        * @return editor axis
        */
        static unsigned int GetEditorAxis() {return mEditorAxis;};
        /**
        * Sets last mouse left-clicking point
        * @param mousepos last mouse left-clicking point
        */
        virtual void SetLastMouse(Ogre::Vector2 mousepos) {mLastMouse = mousepos;};
        /**
        * Gets mouse ray projected from active viewport camera
        * @return true if there is an active viewport, otherwise false
        */
        bool GetMouseRay( Ogre::Vector2 point, Ogre::Ray &mRay );
        /**
        * Saves current editor state(s)
        * @param mouseRay additional parameters to aid in saving required information
        */
        virtual void SaveEditorStates(Ogre::Ray &mouseRay);
        /**
        * Performs selection operation
        * @param mouseRay picking ray
        */
        virtual void DoSelect(Ogre::Ray &mouseRay);
        /**
        * Clones selected entity(ies)
        * @param instance flag, signifying if to instantiate, not clone selected object(s)
        */
        virtual void CloneMove(bool instance);
        /**
        * Updates camera position automatically over time 
        * @param time time delta period after which to update camera position
        */
        virtual void UpdateAutoCameraPosition(float time);
        /**
        * Hightlights object near mouse ray (in-exact picking)
        * @param mouseRay mouse ray cast from active viewport camera
        */
        virtual void HighlightObjectAtPosition(Ogre::Ray &mouseRay);
        /**
        * Fetches view grid (visual aid)
        * @return view grid 
        */
        ViewportGrid* GetViewGrid() {return mViewGrid;};
        /**
        * Fetches grid visibility status
        * @return grid visibility status
        */
        bool IsGridVisible() {return mGridVisible;};
        /**
        * Shows grid in active viewport
        */
        virtual void ShowGrid(bool bShow) { mGridVisible = bShow; mViewGrid->setEnabled(mGridVisible); }
        /**
        * Focuses active viewport camera on selected object
        */
        virtual void FocusSelectedObject();
        /**
        * Deletes selected object
        * @param silent deletes without confirmation if true, otherwise pops up a dialog
        * @param withundo deletes without ability to restore if true, otherwise caches it into undo queue
        */
        virtual void DeleteSelectedObject(bool silent = false, bool withundo = true);
        /**
        * Processes accumulated key action(s)
        * @param timesince previous processing time
        */
        virtual void ProcessKeyActions(unsigned int timesince);
        /**
        * Loads editor object(s) such as grid, gizmos etc
        */
        virtual void LoadEditorObjects();

        /** Keyboard events callback.
        @remarks This will handle Key Down Event.
        @param evt Data regarding the key event.
        */
        virtual void OnKeyDown (unsigned int key);

        /** Keyboard events callback.
        @remarks This will handle Key Up Event.
        @param evt Data regarding the key event.
        */
        virtual void OnKeyUp (unsigned int key);

        /** Mouse events callback.
        @remarks This will handle Mouse Move Event.
        @param evt Data regarding the mouse event.
        */
        virtual void OnMouseMove (Ogre::Vector2 point, unsigned int buttons, bool imitate = false);

        /** Mouse events callback.
        @remarks This will handle Mouse Leave Event.
        @param evt Data regarding the mouse event.
        */
        virtual void OnMouseLeave (Ogre::Vector2 point, unsigned int buttons);

        /** Mouse events callback.
        @remarks This will handle Left Mouse Down Event.
        @param evt Data regarding the mouse event.
        */
        virtual void OnMouseLeftDown (Ogre::Vector2 point, unsigned int buttons);

        /** Mouse events callback.
        @remarks This will handle Left Mouse Up Event.
        @param evt Data regarding the mouse event.
        */
        virtual void OnMouseLeftUp (Ogre::Vector2 point, unsigned int buttons);

        /** Mouse events callback.
        @remarks This will handle Right Mouse Down Event.
        @param evt Data regarding the mouse event.
        */
        virtual void OnMouseRightDown (Ogre::Vector2 point, unsigned int buttons);

        /** Mouse events callback.
        @remarks This will handle Middle Mouse Down Event.
        @param evt Data regarding the mouse event.
        */
        virtual void OnMouseMiddleDown (Ogre::Vector2 point, unsigned int buttons);

        /** Mouse events callback.
        @remarks This will handle Middle Mouse Up Event.
        @param evt Data regarding the mouse event.
        */
        virtual void OnMouseMiddleUp (Ogre::Vector2 point, unsigned int buttons);

        /** Mouse events callback.
        @remarks This will handle Right Mouse Up Event.
        @param evt Data regarding the mouse event.
        */
        virtual void OnMouseRightUp (Ogre::Vector2 point, unsigned int buttons);

        /** Mouse events callback.
        @remarks This will handle Mouse Wheel Event.
        @param evt Data regarding the mouse event.
        */
        virtual void OnMouseWheel (Ogre::Vector2 point, float delta, unsigned int buttons);
        /**
        * Performs volume selection operation
        */
        virtual void DoVolumeSelect();
        /**
        * Fetches object under the mouse cursor
        * @param mouseRay mouse ray from camera through mouse cursor
        * @param pickwidgets picks gizmos if true, otherwise ignores them
        * @param pickterrain picks terrain patches if true, otherwise ignores it/them
        * @return return editor object under mouse cursor
        */
        virtual CBaseEditor* GetObjectUnderMouse(Ogre::Ray &mouseRay, bool pickwidgets = false, bool pickterrain = false);
        /**
        * Prepares to undo the translation operation
        */
        virtual void PrepareTranslationUndo();
        /**
        * Fetches terrain hit position under mouse cursor
        * @param mouseRay mouse ray from camera through mouse cursor
        * @param position position at which to test for a hit
        * @param excludeobject object name to exclude from hit test
        * @return true if terrain is hit, otherwise false
        */
        bool GetHitPosition(Ogre::Ray &mouseRay, Ogre::Vector3& position, const Ogre::String& excludeobject = "");
        /**
        * Fetches terrain hit position under mouse cursor
        * @param mouseRay mouse ray from camera through mouse cursor
        * @param position position at which to test for a hit
        * @param excludeobjects object names to exclude from hit test
        * @return true if terrain is hit, otherwise false
        */
        bool GetHitPosition(Ogre::Ray &mouseRay, Ogre::Vector3& position, const Ogre::StringVector& excludeobjects);
    };
    //! Viewport editor factory class
    /*!  
    A class that is responsible for instantiating viewport editor class(es)
    */
    class OgitorExport CViewportEditorFactory: public CBaseEditorFactory
    {
    public:
        unsigned int mLastZOrder;

        /** @copydoc CBaseEditorFactory::CBaseEditorFactory() */
        CViewportEditorFactory(OgitorsView *view = 0);
        /** @copydoc CBaseEditorFactory::~CBaseEditorFactory() */
        virtual ~CViewportEditorFactory() {};
        /** @copydoc CBaseEditorFactory::duplicate(OgitorsView *view) */
        virtual CBaseEditorFactory* duplicate(OgitorsView *view);
        /** @copydoc CBaseEditorFactory::CreateObject(CBaseEditor **, OgitorsPropertyValueMap &) */
        virtual CBaseEditor *CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params);
        /** @copydoc CBaseEditorFactory::DestroyObject(CBaseEditor *) */
        virtual void DestroyObject(CBaseEditor *object);       
    };
}
