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
    class LightVisualHelper;

    //! Light editor class
    /*!  
        Light editor class that assists in editing Light(s)
    */
    class OgitorExport CLightEditor: public CBaseEditor
    {
        friend class CLightEditorFactory;
    public:

        /// Returns the Factory associated with this object
        static CBaseEditorFactory *getFactory();

        inline Ogre::Vector3     getPosition() { return mPosition->get(); }
        inline Ogre::Quaternion  getOrientation() {return mOrientation->get(); }
        inline Ogre::Vector3     getDirection() {return mDirection->get(); }
        inline int               getLightType() { return mLightType->get(); }
        inline Ogre::ColourValue getDiffuse() { return mDiffuse->get(); }
        inline Ogre::ColourValue getSpecular() { return mSpecular->get(); }
        inline Ogre::Vector3     getRange() { return mRange->get(); }
        inline Ogre::Vector4     getAttenuation() {return mAttenuation->get(); }
        inline Ogre::Real        getPower() { return mPower->get(); }
        inline bool              getCastShadows() { return mCastShadows->get(); };

        inline void              setPosition(const Ogre::Vector3& position) { mPosition->set(position); }
        inline void              setOrientation(const Ogre::Quaternion& orientation) { mOrientation->set(orientation); }
        inline void              setDirection(const Ogre::Vector3& direction) { mDirection->set(direction); }
        inline void              setLightType(int type) { mLightType->set(type); };
        inline void              setDiffuse(const Ogre::ColourValue& value) { mDiffuse->set(value); }
        inline void              setSpecular(const Ogre::ColourValue& value) { mSpecular->set(value); }
        inline void              setRange(const Ogre::Vector3& value) { mRange->set(value); }
        inline void              setAttenuation(const Ogre::Vector4& value) { mAttenuation->set(value); }
        inline void              setPower(Ogre::Real value) { mPower->set(value); }
        inline void              setCastShadows(bool shadows) { mCastShadows->set(shadows); };

        virtual TiXmlElement*    exportDotScene(TiXmlElement *pParent);
        /** @copydoc CBaseEditor::setParentImpl(CBaseEditor *,CBaseEditor *) */
        virtual void             setParentImpl(CBaseEditor *oldparent, CBaseEditor *newparent);
        /** @copydoc CBaseEditor::setLayerImpl(unsigned int) */
        virtual bool             setLayerImpl(unsigned int newlayer);
        /** @copydoc CBaseEditor::load(bool) */
        virtual bool             load(bool async = true);
        /** @copydoc CBaseEditor::unLoad() */
        virtual bool             unLoad();
        /** @copydoc CBaseEditor::createProperties(Ogre::NameValuePairList &) */
        virtual void             createProperties(OgitorsPropertyValueMap &params);
        /** @copydoc CBaseEditor::getHandle() */
        inline virtual void     *getHandle() {return static_cast<void*>(mHandle);};
        /** @copydoc CBaseEditor::getAABB() */
        virtual Ogre::AxisAlignedBox getAABB();
        /** @copydoc CBaseEditor::getWorldAABB() */
        virtual Ogre::AxisAlignedBox getWorldAABB();
        /** @copydoc CBaseEditor::getDerivedPosition() */
        virtual Ogre::Vector3    getDerivedPosition();
        /** @copydoc CBaseEditor::getDerivedOrientation() */
        virtual Ogre::Quaternion getDerivedOrientation();
        /** @copydoc CBaseEditor::setDerivedPosition(Ogre::Vector3) */
        virtual void             setDerivedPosition(Ogre::Vector3 val);

        void _setHandle(Ogre::Light* light) { mHandle = light; }

    protected:
        Ogre::Light *mHandle;             /** Actual light handle */
	Ogre::SceneNode                    *mLightNode;

        OgitorsProperty<Ogre::Vector3>     *mPosition;           /** Light position (wrapper) */
        OgitorsProperty<Ogre::Quaternion>  *mOrientation;        /** Light orientation (wrapper) */
        OgitorsProperty<Ogre::Vector3>     *mDirection;          /** Light direction (wrapper) */
        OgitorsProperty<int>               *mLightType;          /** Light type (wrapper) */
        OgitorsProperty<Ogre::ColourValue> *mDiffuse;            /** Light diffuse color (wrapper) */
        OgitorsProperty<Ogre::ColourValue> *mSpecular;           /** Light specular color (wrapper) */
        OgitorsProperty<Ogre::Vector3>     *mRange;              /** Light range (wrapper) */
        OgitorsProperty<Ogre::Vector4>     *mAttenuation;        /** Light attenuation (wrapper) */
        OgitorsProperty<Ogre::Real>        *mPower;              /** Light power/intensity (wrapper) */
        OgitorsProperty<bool>              *mCastShadows;        /** Light shadow casting flag */
        
        /**
        * Constructor
        */
        CLightEditor(CBaseEditorFactory *factory);
        /**
        * Destructor
        */
        virtual           ~CLightEditor();

        /**
        * Sets light position
        * @param position light position
        */
        bool _setPosition(OgitorsPropertyBase* property, const Ogre::Vector3& position);
        /**
        * Sets light position
        * @param orientation light orientation
        */
        bool _setOrientation(OgitorsPropertyBase* property, const Ogre::Quaternion& orientation);
        /**
        * Sets light direction
        * @param value light direction
        */
        bool _setDirection(OgitorsPropertyBase* property, const Ogre::Vector3& value);
        /**
        * Sets light type (@see Ogre::Light::LightTypes)
        * @param value light type
        */
        bool _setLightType(OgitorsPropertyBase* property, const int& value);
        /**
        * Sets light diffuse color
        * @param value light diffuse color
        */
        bool _setDiffuse(OgitorsPropertyBase* property, const Ogre::ColourValue& value);
        /**
        * Sets light specular color
        * @param value light specular color
        */
        bool _setSpecular(OgitorsPropertyBase* property, const Ogre::ColourValue& value);
        /**
        * Sets light range
        * @param value light range
        */
        bool _setRange(OgitorsPropertyBase* property, const Ogre::Vector3& value);
        /**
        * Sets light attenuation range
        * @param value light attenuation range
        */
        bool _setAttenuation(OgitorsPropertyBase* property, const Ogre::Vector4& value);
        /**
        * Sets light power (intensity)
        * @param value light power
        */
        bool _setPower(OgitorsPropertyBase* property, const Ogre::Real& value);
        /**
        * Sets casting shadows flag; if true, light will be casting shadows
        * @param value casting shadows flag
        */
        bool _setCastShadows(OgitorsPropertyBase* property, const bool& value);
        /**
        * Calculates light current orientation
        */
        void _calculateOrientation();
    };

    //! Light editor factory class
    /*!  
        A class that instantiates light editor and tracks it thereafter
    */
    class OgitorExport CLightEditorFactory: public CBaseEditorFactory
    {
    public:
        /**
        * Constructor
        */
        CLightEditorFactory(OgitorsView *view = 0);
        /**
        * Destructor
        */
        virtual ~CLightEditorFactory() {};
        /** @copydoc CBaseEditorFactory::duplicate(OgitorsView *view) */
        virtual CBaseEditorFactory* duplicate(OgitorsView *view);
        /** @copydoc CBaseEditorFactory::CreateObject(CBaseEditor **, Ogre::NameValuePairList &) */
        virtual CBaseEditor *CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params);
        /** @copydoc CBaseEditorFactory::GetPlaceHolderName() */
        virtual Ogre::String GetPlaceHolderName() {return "scbLight_Omni.mesh";};

        static PropertyOptionsVector *getLightTypes() { return &mLightTypes; }

    protected:
        static PropertyOptionsVector mLightTypes;
    };
}
