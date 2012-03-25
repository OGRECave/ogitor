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
    //! Plane editor class
    /*!  
    A class that handles plane(s) editing
    */
    class OgitorExport CPlaneEditor : public CNodeEditor
    {
        friend class CPlaneEditorFactory;
    public:
        /** 
        * Prevent CNodeEditor descendants to behave as nodes
        */
        virtual bool isNodeType() {return false;};
        virtual TiXmlElement* exportDotScene(TiXmlElement *pParent);
        /** @copydoc CBaseEditor::createProperties(OgitorsPropertyValueMap &) */
        virtual void createProperties(OgitorsPropertyValueMap &params);
        /** @copydoc CBaseEditor::load(bool) */
        virtual bool load(bool async = true);
        /** @copydoc CBaseEditor::unLoad() */
        virtual bool unLoad();
        /** @copydoc CBaseEditor::onDropMaterial(Ogre::Ray, Ogre::Vector3, const Ogre::String&) */
        virtual void onDropMaterial(Ogre::Ray ray, Ogre::Vector3 position, const Ogre::String& materialname);
        /** @copydoc CBaseEditor::getAABB() */
        virtual Ogre::AxisAlignedBox getAABB() {if(mPlaneHandle) return mPlaneHandle->getBoundingBox();else return Ogre::AxisAlignedBox::BOX_NULL;};
        /** @copydoc CBaseEditor::getHandle() */
        inline virtual void    *getHandle() {return static_cast<void*>(mPlaneHandle);};
        inline bool  getCastShadows() { return mCastShadows->get(); };
        inline void  setCastShadows(bool shadows) { mCastShadows->set(shadows); };

    protected:
        Ogre::Entity *mPlaneHandle;                         /** Plane handle */
        OgitorsProperty<Ogre::Vector3> *mNormal;            /** Plane normal property handle */
        OgitorsProperty<Ogre::Real>    *mDistance;          /** Plane distance property handle */
        OgitorsProperty<Ogre::Real>    *mWidth;             /** Plane width property handle */
        OgitorsProperty<Ogre::Real>    *mHeight;            /** Plane height property handle */
        OgitorsProperty<int>           *mxSegments;         /** Plane segments (X) property handle */
        OgitorsProperty<int>           *mySegments;         /** Plane segments (Y) property handle */
        OgitorsProperty<int>           *mNumCoordSets;      /** Plane number of coordinate set(s) property handle */
        OgitorsProperty<int>           *muTile;             /** Plane UV tiling U attribute property handle */
        OgitorsProperty<int>           *mvTile;             /** Plane UV tiling V attribute property handle */
        OgitorsProperty<bool>          *mHasNormals;        /** Plane normal(s) flag property handle */
        OgitorsProperty<Ogre::String>  *mMaterial;          /** Plane material name property handle */
        OgitorsProperty<bool>          *mCastShadows;       /** Plane shadow casting flag property handle */

        /**
        * Constructor
        * @param factory Handle to terrain editor factory
        */
        CPlaneEditor(CBaseEditorFactory *factory);
        /**
        * Destructor
        */
        virtual       ~CPlaneEditor() {};

        virtual bool setLayerImpl(unsigned int newlayer);
        /**
        * Property setter for plane normal (internal)
        * @param property Handle to property responsible for plane normal
        * @param value new plane normal
        * @return true if property handle is valid 
        */
        bool  _setNormal(OgitorsPropertyBase* property, const Ogre::Vector3& value);
        /**
        * Property setter for plane distance (internal)
        * @param property Handle to property responsible for plane distance
        * @param value new plane distance
        * @return true if property handle is valid 
        */
        bool  _setDistance(OgitorsPropertyBase* property, const Ogre::Real& value);
        /**
        * Property setter for plane width (internal)
        * @param property Handle to property responsible for plane width
        * @param value new plane width
        * @return true if property handle is valid 
        */
        bool  _setWidth(OgitorsPropertyBase* property, const Ogre::Real& value);
        /**
        * Property setter for plane height (internal)
        * @param property Handle to property responsible for plane height
        * @param value new plane height
        * @return true if property handle is valid 
        */
        bool  _setHeight(OgitorsPropertyBase* property, const Ogre::Real& value);
        /**
        * Property setter for plane X segment(s) count (internal)
        * @param property Handle to property responsible for plane X segment(s) count
        * @param value new plane X segment(s) count
        * @return true if property handle is valid 
        */
        bool  _setXSegments(OgitorsPropertyBase* property, const int& value);
        /**
        * Property setter for plane Y segment(s) count (internal)
        * @param property Handle to property responsible for plane Y segment(s) count
        * @param value new plane Y segment(s) count
        * @return true if property handle is valid 
        */
        bool  _setYSegments(OgitorsPropertyBase* property, const int& value);
        /**
        * Property setter for plane coordinate set(s) count (internal)
        * @param property Handle to property responsible for plane coordinate set(s) count
        * @param value new plane coordinate set(s) count
        * @return true if property handle is valid 
        */
        bool  _setNumCoordSets(OgitorsPropertyBase* property, const int& value);
        /**
        * Property setter for plane UV tiling U attribute(internal)
        * @param property Handle to property responsible for plane UV tiling U attribute
        * @param value new plane UV tiling U attribute
        * @return true if property handle is valid 
        */
        bool  _setUTile(OgitorsPropertyBase* property, const int& value);
        /**
        * Property setter for plane UV tiling V attribute(internal)
        * @param property Handle to property responsible for plane UV tiling V attribute
        * @param value new plane UV tiling V attribute
        * @return true if property handle is valid 
        */
        bool  _setVTile(OgitorsPropertyBase* property, const int& value);
        /**
        * Property setter for plane normal(s) flag(internal)
        * @param property Handle to property responsible for plane normal(s) flag
        * @param value new plane normal(s) flag
        * @return true if property handle is valid 
        */
        bool  _setHasNormals(OgitorsPropertyBase* property, const bool& value);
        /**
        * Property setter for plane material name(internal)
        * @param property Handle to property responsible for plane material name
        * @param value new plane material name
        * @return true if property handle is valid 
        */
        bool  _setMaterial(OgitorsPropertyBase* property, const Ogre::String& value);
        /**
        * Property setter for plane shadow casting flag(internal)
        * @param property Handle to property responsible for plane shadow casting flag
        * @param value new plane shadow casting flag
        * @return true if property handle is valid 
        */
        bool  _setCastShadows(OgitorsPropertyBase* property, const bool& value);
        /**
        * Creates new plane
        * @return new plane
        */
        Ogre::Entity *_createPlane();
    };

    //! Plane editor factory class
    /*!  
    A class that is responsible for instantiating plane editor class(es)
    */
    class OgitorExport CPlaneEditorFactory: public CNodeEditorFactory
    {
    public:
        /** @copydoc CBaseEditorFactory::CBaseEditorFactory() */
        CPlaneEditorFactory(OgitorsView *view = 0);
        /** @copydoc CBaseEditorFactory::~CBaseEditorFactory() */
        virtual ~CPlaneEditorFactory() {};
        /** @copydoc CBaseEditorFactory::duplicate(OgitorsView *view) */
        virtual CBaseEditorFactory* duplicate(OgitorsView *view);
        /** @copydoc CBaseEditorFactory::CreateObject(CBaseEditor **, OgitorsPropertyValueMap &) */
        virtual CBaseEditor *CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params);
        /** @copydoc CBaseEditorFactory::GetPlaceHolderName() */
        virtual Ogre::String GetPlaceHolderName() {return "scbLight_Omni.mesh";};
    };

}
