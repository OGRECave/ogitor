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

namespace Forests
{
    class PagedGeometry;
    class TreeLoader3D;
}

namespace Ogitors
{
    class CPGInstanceEditor;

    struct PGInstanceInfo
    {
        Ogre::Vector3 pos;
        Ogre::Real    scale;
        Ogre::Real    yaw;
        CPGInstanceEditor *instance;
    };

    typedef Ogre::map<int, PGInstanceInfo>::type PGInstanceList;

    class AddInstanceUndo;
    class RemoveInstanceUndo;

    class OgitorExport CPGInstanceManager : public CBaseEditor, public MouseListener
    {
        friend class CPGInstanceManagerFactory;
        friend class AddInstanceUndo;
        friend class RemoveInstanceUndo;
    public:

        virtual void     createProperties(OgitorsPropertyValueMap &params);
        /** @copydoc CBaseEditor::load(bool) */
        virtual bool     load(bool async = true);
        /** @copydoc CBaseEditor::unLoad() */
        virtual bool     unLoad();
        virtual void     showBoundingBox(bool bShow);
        virtual void     setSelectedImpl(bool bSelected);
        virtual bool     setLayerImpl(unsigned int newlayer);
        virtual void     onSave(bool forced = false);
        virtual TiXmlElement* exportDotScene(TiXmlElement *pParent);

        /** @copydoc CBaseEditor::update(float) */
        virtual bool     update(float timePassed);
        /** @copydoc CBaseEditor::getObjectContextMenu(UTFStringVector &) */
        virtual bool     getObjectContextMenu(UTFStringVector &menuitems);
        /** @copydoc CBaseEditor::onObjectContextMenu(int) */
        virtual void     onObjectContextMenu(int menuresult);

        virtual Ogre::AxisAlignedBox getAABB();

        Ogre::Entity *getEntityHandle() { return mEntityHandle; };
        int  addInstance(const Ogre::Vector3& pos, const Ogre::Real& scale, const Ogre::Real& yaw);
        void removeInstance(int index);
        void modifyInstancePosition(int index, const Ogre::Vector3& pos);
        void modifyInstanceScale(int index, Ogre::Real scale);
        void modifyInstanceYaw(int index, Ogre::Real yaw);
        PGInstanceInfo getInstanceInfo(int index);

        inline bool  isUsingPlaceHolderMesh() { return mUsingPlaceHolderMesh; };
        inline bool  getCastShadows() { return mCastShadows->get(); };
        inline void  setCastShadows(bool shadows) { mCastShadows->set(shadows); };
        
    protected:
        Forests::PagedGeometry      *mPGHandle;
        Forests::TreeLoader3D       *mHandle;
        Ogre::Entity                *mEntityHandle;
        bool                         mPlacementMode;
        PGInstanceList               mInstanceList;
        int                          mNextInstanceIndex;
        bool                         mHideChildrenInProgress;
        Ogre::String                 mLastFileName;
        bool                         mUsingPlaceHolderMesh;
        Ogre::String                 mTempFileName;
        bool                         mShowChildren;

        OgitorsProperty<Ogre::String> *mModel; 
        OgitorsProperty<int> *mPageSize; 
        OgitorsProperty<int> *mBatchDistance; 
        OgitorsProperty<int> *mImpostorDistance; 
        OgitorsProperty<Ogre::Vector4> *mBounds; 
        OgitorsProperty<bool> *mCastShadows; 
        OgitorsProperty<bool> *mTempModified;
        OgitorsProperty<Ogre::Real> *mMinScale;
        OgitorsProperty<Ogre::Real> *mMaxScale;
        OgitorsProperty<Ogre::Real> *mMinYaw;
        OgitorsProperty<Ogre::Real> *mMaxYaw;
        
        CPGInstanceManager(CBaseEditorFactory *factory);
        virtual     ~CPGInstanceManager();

        bool _setModel(OgitorsPropertyBase* property, const Ogre::String& value);
        bool _setPageSize(OgitorsPropertyBase* property, const int& value);
        bool _setBatchDistance(OgitorsPropertyBase* property, const int& value);
        bool _setImpostorDistance(OgitorsPropertyBase* property, const int& value);
        bool _setBounds(OgitorsPropertyBase* property, const Ogre::Vector4& value);
        bool _setCastShadows(OgitorsPropertyBase* property, const bool& value);
        void _onLoad();
        void _save(Ogre::String filename);
        void _createChildEditor(int index, Ogre::Vector3 pos, Ogre::Real scale, Ogre::Real yaw);
        void _deleteChildEditor(int index);

        /**
        * Delegate function for mouse move event
        * @param viewport window to receive the input
        * @param point new mouse position
        * @param buttons mouse button flags 
        */
        virtual void OnMouseMove (CViewportEditor *viewport, Ogre::Vector2 point, unsigned int buttons);
        /**
        * Delegate function for mouse leave event
        * @param viewport window to receive the input
        * @param point new mouse position
        * @param buttons mouse button flags  
        */
        virtual void OnMouseLeave (CViewportEditor *viewport, Ogre::Vector2 point, unsigned int buttons);
        /**
        * Delegate function for mouse left button down event
        * @param viewport window to receive the input
        * @param point new mouse position
        * @param buttons mouse button flags  
        */
        virtual void OnMouseLeftDown (CViewportEditor *viewport, Ogre::Vector2 point, unsigned int buttons);
        /**
        * Delegate function for mouse left button up event
        * @param viewport window to receive the input
        * @param point new mouse position
        * @param buttons mouse button flags  
        */
        virtual void OnMouseLeftUp (CViewportEditor *viewport, Ogre::Vector2 point, unsigned int buttons);
        /**
        * Delegate function for mouse right button down event
        * @param viewport window to receive the input
        * @param point new mouse position
        * @param buttons mouse button flags  
        */
        virtual void OnMouseRightDown (CViewportEditor *viewport, Ogre::Vector2 point, unsigned int buttons);
        /**
        * Delegate function for mouse right button up event
        * @param viewport window to receive the input
        * @param point new mouse position
        * @param buttons mouse button flags
        */
        virtual void OnMouseRightUp (CViewportEditor *viewport, Ogre::Vector2 point, unsigned int buttons);
        /**
        * Delegate function for mouse middle button down event
        * @param viewport window to receive the input
        * @param point new mouse position
        * @param buttons mouse button flags  
        */
        virtual void OnMouseMiddleDown (CViewportEditor *viewport, Ogre::Vector2 point, unsigned int buttons);
        /**
        * Delegate function for mouse middle button up event
        * @param viewport window to receive the input
        * @param point new mouse position
        * @param buttons mouse button flags  
        */
        virtual void OnMouseMiddleUp (CViewportEditor *viewport, Ogre::Vector2 point, unsigned int buttons);
        /**
        * Delegate function for mouse wheel movement (up or down)
        * @param viewport window to receive the input
        * @param point new mouse position
        * @param delta change of mouse scrolling position
        * @param buttons mouse button flags  
        */
        virtual void OnMouseWheel (CViewportEditor *viewport, Ogre::Vector2 point, float delta, unsigned int buttons);
    };

    class OgitorExport CPGInstanceManagerFactory: public CBaseEditorFactory
    {
    public:
        CPGInstanceManagerFactory(OgitorsView *view = 0);
        virtual ~CPGInstanceManagerFactory() {};
        /** @copydoc CBaseEditorFactory::duplicate(OgitorsView *view) */
        virtual CBaseEditorFactory* duplicate(OgitorsView *view);
        virtual CBaseEditor *CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params);
    };
}
