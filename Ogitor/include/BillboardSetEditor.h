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
    //! Billboard editor class
    /*!  
        Billboard editor class that assists in editing billboard(s)
    */
    class OgitorExport CBillboardSetEditor: public CNodeEditor
    {
        friend class CBillboardSetEditorFactory;
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
        /** @copydoc CBaseEditor::getHandle() */
        inline virtual void *getHandle() {return static_cast<void*>(mBillboardSetHandle);};
        /**
        * Fetches axis aligned bounding box for while billboard set
        * @return axis aligned bounding box for while billboard set
        */
        virtual Ogre::AxisAlignedBox getAABB() {if(mBillboardSetHandle) return mBillboardSetHandle->getBoundingBox(); else return Ogre::AxisAlignedBox::BOX_NULL;};
        
        /** @copydoc CBaseEditor::onDropMaterial(Ogre::Ray ray, Ogre::Vector3, const Ogre::String&) */
        virtual void  onDropMaterial(Ogre::Ray ray, Ogre::Vector3 position, const Ogre::String& materialname);
        /** @copydoc CBaseEditor::getPropertyContextMenu(Ogre::String, UTFStringVector &) */
        virtual bool  getPropertyContextMenu(Ogre::String propertyName, UTFStringVector &menuitems);
        /** @copydoc CBaseEditor::onPropertyContextMenu(Ogre::String, int) */
        virtual void  onPropertyContextMenu(Ogre::String propertyName, int menuresult);

    protected:
        Ogre::BillboardSet                *mBillboardSetHandle; /** Actual billboard set handle */

        OgitorsProperty<int>            *mBillboardType;      /** Billboard type property handle */
        OgitorsProperty<bool>           *mSorting;            /** Sorting type property handle */
        OgitorsProperty<int>            *mOrigin;             /** Origin type property handle */
        OgitorsProperty<int>            *mRotation;           /** Rotation property handle */
        OgitorsProperty<Ogre::Real>     *mDefaultWidth;       /** Default width property handle */
        OgitorsProperty<Ogre::Real>     *mDefaultHeight;      /** Default height property handle */
        OgitorsProperty<Ogre::String>   *mMaterial;           /** Material name property handle */
        OgitorsProperty<bool>           *mPointRendering;     /** Point rendering type property handle */
        OgitorsProperty<Ogre::Real>     *mRenderDistance;     /** Rendering distance property handle */
        OgitorsProperty<int>            *mBillboardCount;     /** Billboard count property handle */
        
        /** 
        * Constructor 
        */
        CBillboardSetEditor(CBaseEditorFactory *factory);
        /** 
        * Destructor
        */
        virtual ~CBillboardSetEditor();
        
        virtual bool setLayerImpl(unsigned int newlayer);
        /**
        * Sets billboard type for billboard set
        * @param property the sender
        * @param value billboard type (@see Ogre::BillboardType)
        */
        bool _setBillboardType(OgitorsPropertyBase* property, const int& value);
        /**
        * Sets billboard sorting flag for billboard set
        * @param property the sender
        * @param bSorting sorting flag for billboard set
        */
        bool _setSorting(OgitorsPropertyBase* property, const bool& bSorting);
        /**
        * Sets billboard origin for billboard set
        * @param property the sender
        * @param origin billboard origin (@see Ogre::BillboardOrigin)
        */
        bool _setOrigin(OgitorsPropertyBase* property, const int& origin);
        /**
        * Sets billboard rotation for billboard set
        * @param property the sender
        * @param rotation billboard rotation (@see Ogre::BillboardRotationType)
        */
        bool _setRotation(OgitorsPropertyBase* property, const int& rotation);
        /**
        * Sets billboard default width for billboard set
        * @param property the sender
        * @param width billboard default width
        */
        bool _setWidth(OgitorsPropertyBase* property, const Ogre::Real& width);
        /**
        * Sets billboard default height for billboard set
        * @param property the sender
        * @param height billboard default height
        */
        bool _setHeight(OgitorsPropertyBase* property, const Ogre::Real& height);
        /**
        * Sets billboard default material name for billboard set
        * @param property the sender
        * @param value billboard default material name
        */
        bool _setMaterial(OgitorsPropertyBase* property, const Ogre::String& value);
        /**
        * Sets billboard point rendering flag for billboard set
        * @param property the sender
        * @param point billboard point rendering flag; if true, will render all billboards as point sprites, otherwise as normal 'quads'.
        */
        bool _setPointRendering(OgitorsPropertyBase* property, const bool& point);
        /**
        * Sets the distance at which the object is no longer rendered for billboard set
        * @param property the sender
        * @param distance distance at which the object is no longer rendered for billboard set
        */
        bool _setRenderingDistance(OgitorsPropertyBase* property, const Ogre::Real& distance);

        /**
        * Sets the position of a billboard in the set
        * @param property the sender
        * @param value new position of the billboard
        */
        bool _setBillboardPosition(OgitorsPropertyBase* property, const Ogre::Vector3& value);
        /**
        * Sets the colour of a billboard in the set
        * @param property the sender
        * @param value new colour of the billboard
        */
        bool _setBillboardColour(OgitorsPropertyBase* property, const Ogre::ColourValue& value);
        /**
        * Sets the rotation of a billboard in the set (Degrees)
        * @param property the sender
        * @param value new rotation angle (Degrees) of the billboard
        */
        bool _setBillboardRotation(OgitorsPropertyBase* property, const Ogre::Real& value);
        /**
        * Sets the dimensions of a billboard in the set
        * @param property the sender
        * @param value new dimensions (Width, Height) of the billboard
        */
        bool _setBillboardDimensions(OgitorsPropertyBase* property, const Ogre::Vector2& value);
        /**
        * Sets the texture coordinate index of a billboard in the set
        * @param property the sender
        * @param value new texture coordinate index of the billboard
        */
        bool _setBillboardTexCoordIndex(OgitorsPropertyBase* property, const int& value);
        /**
        * Sets the texture rectangle of a billboard in the set
        * @param property the sender
        * @param value new texture rectangle (u0,v0,u1,v1) of the billboard
        */
        bool _setBillboardTexRect(OgitorsPropertyBase* property, const Ogre::Vector4& value);
        /**
        * Removes a billboard from the set
        * @param id index of the billboard to be removed
        */
        void _removeBillboard(int id);
        /**
        * Adds a billboard to the set
        */
        void _addBillboard();
    };

    //! Billboard set editor factory class
    /*!  
        A class that instantiates billboard set editor and tracks it thereafter
    */
    class OgitorExport CBillboardSetEditorFactory: public CNodeEditorFactory
    {
    public:
        /**
        * Constructor
        */
        CBillboardSetEditorFactory(OgitorsView * view = 0);
        /**
        * Destructor
        */
        virtual ~CBillboardSetEditorFactory() {};
        /** @copydoc CBaseEditorFactory::duplicate(OgitorsView *view) */
        virtual CBaseEditorFactory* duplicate(OgitorsView *view);
        /** @copydoc CBaseEditorFactory::CreateObject(CBaseEditor **, Ogre::NameValuePairList &) */
        virtual CBaseEditor *CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params);
        /** @copydoc CBaseEditorFactory::GetPlaceHolderName() */
        virtual Ogre::String GetPlaceHolderName() {return "scbLight_Omni.mesh";};
        /**
        * Fetches billboard types property list
        * @return billboard types property list
        */
        static PropertyOptionsVector *getBillboardTypes();
        /**
        * Fetches billboard types property list
        * @return billboard types property list
        */
        static PropertyOptionsVector *getBillboardOriginTypes();
        static PropertyOptionsVector *getmBillboardRotationTypes();

    protected:
        static PropertyOptionsVector mBillboardTypes;
        static PropertyOptionsVector mBillboardOriginTypes;
        static PropertyOptionsVector mBillboardRotationTypes;
    };
}
