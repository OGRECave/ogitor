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
    //! Entity editor class
    /*!  
        Entity editor class that assists in editing Entity(ies)
    */
    class OgitorExport CEntityEditor : public CNodeEditor
    {
        friend class CEntityEditorFactory;
    public:
        /** 
        * Prevent CNodeEditor descendants to behave as nodes
        */
        virtual bool isNodeType() {return false;};
        /** @copydoc CBaseEditor::load(bool) */
        virtual bool load(bool async = true);
        /** @copydoc CBaseEditor::unLoad() */
        virtual bool unLoad();
        /** @copydoc CBaseEditor::createProperties(Ogre::NameValuePairList &) */
        virtual void createProperties(OgitorsPropertyValueMap &params);

        /** @copydoc CBaseEditor::onDropMaterial(Ogre::Ray ray, Ogre::Vector3, const Ogre::String&) */
        virtual void onDropMaterial(Ogre::Ray ray, Ogre::Vector3 position, const Ogre::String& materialname);
        /** @copydoc CBaseEditor::getAABB() */
        virtual Ogre::AxisAlignedBox getAABB() {if(mEntityHandle) return mEntityHandle->getBoundingBox();else return Ogre::AxisAlignedBox::BOX_NULL;};
        /** @copydoc CBaseEditor::getHandle() */
        inline virtual void    *getHandle() {return static_cast<void*>(mEntityHandle);};

        void _setEntity(Ogre::Entity* entity) { mEntityHandle = entity; }

        inline bool  isUsingPlaceHolderMesh() { return mUsingPlaceHolderMesh; };

        inline int          getSubEntityCount() { return mSubEntityCount->get(); };
        inline Ogre::String getMeshName() { return mMeshFile->get(); };
        inline void         setMeshName(const Ogre::String& name) { return mMeshFile->set(name); };
        inline bool         getCastShadows() { return mCastShadows->get(); };
        inline void         setCastShadows(bool shadows) { mCastShadows->set(shadows); };

    protected:
        Ogre::Entity *mEntityHandle;                       /** Actual mesh handle */
        OgitorsScopedConnection mShadowsConnection[2];
        bool          mUsingPlaceHolderMesh;
        
        OgitorsProperty<Ogre::String> *mMeshFile;          /** Mesh file name */
        OgitorsProperty<bool>         *mCastShadows;       /** Casting shadows flag */
        OgitorsProperty<int>          *mSubEntityCount;    /** Number of Sub Entities */
        OgitorsProperty<Ogre::Real>   *mRenderingDistance; /** Maximum Rendering Distance for all entities */

        /** 
        * Constructor
        */
        CEntityEditor(CBaseEditorFactory *factory);
        /** 
        * Destructor
        */
        virtual     ~CEntityEditor() {};

        virtual TiXmlElement* exportDotScene(TiXmlElement *pParent);

        virtual bool setLayerImpl(unsigned int newlayer);
        /**
        * Sets mesh file name
        * @param value mesh file name
        */
        bool         _setMeshFile(OgitorsPropertyBase* property, const Ogre::String& value);
        /**
        * Sets casting shadows flag; if true, entities will be casting shadows
        * @param value casting shadows flag
        */
        bool         _setCastShadows(OgitorsPropertyBase* property, const bool& value);
        /**
        * Sets material of sub entity
        * @param value material name
        */
        bool         _setSubMaterial(OgitorsPropertyBase* property, const Ogre::String& value);
        /**
        * Sets visibility of sub entity
        * @param value visibility flag
        */
        bool         _setSubVisible(OgitorsPropertyBase* property, const bool& value);
        /**
        * Property setter for maximum rendering distance for meshes
        * @param property Handle to property responsible for max rendering distance
        * @param value new max rendering distance
        * @return true if property handle is valid 
        */
        bool _setRenderingDistance(OgitorsPropertyBase* property, const Ogre::Real& distance);

        void OnShadowsChange(const OgitorsPropertyBase* property, Ogre::Any value);
        void OnShadowsTechniqueChange(const OgitorsPropertyBase* property, Ogre::Any value);
    };

    //! Entity editor factory class
    /*!  
        A class that instantiates Entity editor and tracks it thereafter
    */
    class OgitorExport CEntityEditorFactory: public CNodeEditorFactory
    {
    public:
        /** @copydoc CBaseEditorFactory::CBaseEditorFactory() */
        CEntityEditorFactory(OgitorsView *view = 0);
        /** @copydoc CBaseEditorFactory::~CBaseEditorFactory() */
        virtual ~CEntityEditorFactory() {};
        /** @copydoc CBaseEditorFactory::duplicate(OgitorsView *view) */
        virtual CBaseEditorFactory* duplicate(OgitorsView *view);
        /** @copydoc CBaseEditorFactory::CreateObject(CBaseEditor **, Ogre::NameValuePairList &) */
        virtual CBaseEditor *CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params);
    };

}
