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

namespace Ogitors
{

    class OgitorExport CNodeEditor : public CBaseEditor
    {
        friend class CNodeEditorFactory;
    public:

        inline Ogre::Vector3    getPosition() { return mPosition->get(); }
        inline Ogre::Vector3    getScale() { return mScale->get(); }
        inline Ogre::Quaternion getOrientation() { return mOrientation->get(); }
        inline void             setPosition(const Ogre::Vector3& position) { mPosition->set(position); }
        inline void             setScale(const Ogre::Vector3& scale) { mScale->set(scale); }
        inline void             setOrientation(const Ogre::Quaternion& orientation) { mOrientation->set(orientation); }

        /** @copydoc CBaseEditor::setParentImpl(CBaseEditor *,CBaseEditor *) */
        virtual void     setParentImpl(CBaseEditor *oldparent, CBaseEditor *newparent);
        virtual void     createProperties(OgitorsPropertyValueMap &params);
        /** @copydoc CBaseEditor::load(bool) */
        virtual bool     load(bool async = true);
        /** @copydoc CBaseEditor::unLoad() */
        virtual bool     unLoad();
        /** @copydoc CBaseEditor::isAutoTrackTarget() */
        virtual bool     isAutoTrackTarget() {return true;};
        /** @copydoc CBaseEditor::isNodeType() */
        virtual bool     isNodeType() {return true;};

        void _setHandle(Ogre::SceneNode* node) { mHandle = node; }

        virtual TiXmlElement*        exportDotScene(TiXmlElement *pParent);
        /// This function is called when user right clicks a property to get that property specific popup menu
        /// returns false if no menu present (Disabled Menu Items has a "D" prefix where Enabled Menu Items have "E" prefix
        virtual bool                 getObjectContextMenu(UTFStringVector &menuitems);
        /// This function is called when user selects a menuitem from ContextMenu
        virtual void                 onObjectContextMenu(int menuresult);
        /// General Purpose Update Function Used By OgitorsRoot Override for Descendants
        virtual bool                 postSceneUpdate(Ogre::SceneManager *SceneMngr, Ogre::Camera *Camera, Ogre::RenderWindow *RenderWindow);
        /// Gets the Handle to encapsulated object
        inline virtual void         *getHandle() {return static_cast<void*>(mHandle);};
        virtual Ogre::Vector3        getDerivedPosition();
        virtual Ogre::Quaternion     getDerivedOrientation();
        virtual Ogre::Vector3        getDerivedScale();
        virtual Ogre::AxisAlignedBox getAABB();
        virtual Ogre::AxisAlignedBox getWorldAABB();
        virtual Ogre::SceneNode     *getNode() {return mHandle;};
        virtual Ogre::SceneManager  *getSceneManager() {return mHandle->getCreator();};

    protected:
        Ogre::SceneNode *mHandle;
        OgitorsProperty<Ogre::Vector3>     *mPosition;             /** Node position (wrapper) */
        OgitorsProperty<Ogre::Quaternion>  *mOrientation;          /** Node orientation (wrapper) */
        OgitorsProperty<Ogre::Vector3>     *mScale;                /** Node Scale (wrapper) */
        OgitorsProperty<Ogre::String>      *mAutoTrackTarget;      /** Node tracking target name (wrapper) */
        CBaseEditor                        *mAutoTrackTargetPtr;   /** Node tracking target object pointer (wrapper) */
        OgitorsScopedConnection             mAutoTrackTargetConnection[3];
        
        CNodeEditor(CBaseEditorFactory *factory);
        virtual     ~CNodeEditor();

        bool _setPosition(OgitorsPropertyBase* property, const Ogre::Vector3& position);
        bool _setOrientation(OgitorsPropertyBase* property, const Ogre::Quaternion& orientation);
        bool _setScale(OgitorsPropertyBase* property, const Ogre::Vector3& scale);
        bool _setAutoTrackTarget(OgitorsPropertyBase* property, const Ogre::String& targetname);

        /// Auto Track Target Observation Related
        void OnTrackTargetDestroyed(const OgitorsPropertyBase* property, Ogre::Any value);
        void OnTrackTargetPositionChange(const OgitorsPropertyBase* property, Ogre::Any value);
        void OnTrackTargetNameChange(const OgitorsPropertyBase* property, Ogre::Any value);
    };

    class OgitorExport CNodeEditorFactory: public CBaseEditorFactory
    {
    public:
        CNodeEditorFactory(OgitorsView *view = 0);
        virtual ~CNodeEditorFactory() {};
        /** @copydoc CBaseEditorFactory::duplicate(OgitorsView *view) */
        virtual CBaseEditorFactory* duplicate(OgitorsView *view);
        virtual CBaseEditor *CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params);
        virtual Ogre::String GetPlaceHolderName() {return "scbLight_Omni.mesh";};
    };
}

