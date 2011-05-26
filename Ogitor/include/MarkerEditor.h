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

    class OgitorExport CMarkerEditor: public CNodeEditor
    {
        friend class CMarkerEditorFactory;
    public:
        /** 
        * Prevent CNodeEditor descendants to behave as nodes
        */
        virtual bool isNodeType() {return false;};
        virtual TiXmlElement* exportDotScene(TiXmlElement *pParent);
        /**
        * Fetches marker color
        * @return marker color
        */
        inline Ogre::ColourValue getColour() { return mColour->get(); }
        /** @copydoc CBaseEditor::load(bool) */
        virtual bool load(bool async = true);
        /** @copydoc CBaseEditor::unLoad() */
        virtual bool unLoad();
        /** @copydoc CBaseEditor::createProperties(OgitorsPropertyValueMap &) */
        virtual void createProperties(OgitorsPropertyValueMap &params);
        /** @copydoc CBaseEditor::getAABB() */
        virtual Ogre::AxisAlignedBox getAABB() {if(mMarkerHandle) return mMarkerHandle->getBoundingBox();else return Ogre::AxisAlignedBox::BOX_NULL;};

    protected:
        Ogre::Entity      *mMarkerHandle;               /** Marker handle */
        Ogre::MaterialPtr  mMaterial;                   /** Marker material handle */

        OgitorsProperty<Ogre::ColourValue> *mColour;    /** Marker color property handle */

        /**
        * Constructor
        * @param factory Handle to marker editor factory
        */
        CMarkerEditor(CBaseEditorFactory *factory);
        /**
        * Destructor
        */
        ~CMarkerEditor() {};

        virtual bool setLayerImpl(unsigned int newlayer);
        /**
        * Property setter for marker color (internal)
        * @param property Handle to property responsible for marker color
        * @param position new marker color
        * @return true if property handle is valid 
        */
        bool _setColour(OgitorsPropertyBase* property, const Ogre::ColourValue& colour);
    };

    //! Marker editor factory class
    /*!  
        A class that is responsible for instantiating marker editor class(es)
    */
    class OgitorExport CMarkerEditorFactory: public CNodeEditorFactory
    {
    public:
        /** @copydoc CBaseEditorFactory::CBaseEditorFactory() */
        CMarkerEditorFactory(OgitorsView *view = 0);
        /** @copydoc CBaseEditorFactory::~CBaseEditorFactory() */
        virtual ~CMarkerEditorFactory() {};
        /** @copydoc CBaseEditorFactory::duplicate(OgitorsView *view) */
        virtual CBaseEditorFactory* duplicate(OgitorsView *view);
        /** @copydoc CBaseEditorFactory::CreateObject(CBaseEditor **, OgitorsPropertyValueMap &) */
        virtual CBaseEditor *CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params);
        /** @copydoc CBaseEditorFactory::GetPlaceHolderName() */
        virtual Ogre::String GetPlaceHolderName() {return "scbMarker.mesh";};
    };
}
