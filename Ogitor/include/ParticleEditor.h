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
    //! Particle editor class
    /*!  
        A class that handles particle system(s) editing
    */
    class OgitorExport CParticleEditor : public CNodeEditor
    {
        friend class CParticleEditorFactory;
    public:
        /** 
        * Prevent CNodeEditor descendants to behave as nodes
        */
        virtual bool isNodeType() {return false;};
        virtual TiXmlElement* exportDotScene(TiXmlElement *pParent);
        /** @copydoc CBaseEditor::setLayerImpl(unsigned int) */
        virtual bool setLayerImpl(unsigned int newlayer);
        /** @copydoc CBaseEditor::createProperties(OgitorsPropertyValueMap &) */
        virtual void createProperties(OgitorsPropertyValueMap &params);
        /** @copydoc CBaseEditor::load(bool) */
        virtual bool load(bool async = true);
        /** @copydoc CBaseEditor::unLoad() */
        virtual bool unLoad();
        /** @copydoc CBaseEditor::getAABB() */
        virtual Ogre::AxisAlignedBox getAABB();
        /** @copydoc CBaseEditor::getHandle() */
        inline virtual void    *getHandle() {return static_cast<void*>(mParticleHandle);};
        inline bool  getCastShadows() { return mCastShadows->get(); };
        inline void  setCastShadows(bool shadows) { mCastShadows->set(shadows); };

    protected:
        Ogre::ParticleSystem *mParticleHandle;              /** Particle system handle */
        Ogre::Entity         *mEntityHandle;                /** Entity handle */

        OgitorsProperty<Ogre::String> *mParticleSystem;     /** Particle system property handle */
        OgitorsProperty<bool>         *mCastShadows;        /** Shadow casting property handle */

        /**
        * Constructor
        * @param factory Handle to terrain editor factory
        */
        CParticleEditor(CBaseEditorFactory *factory);
        /**
        * Destructor
        */
        virtual     ~CParticleEditor() {};

        /**
        * Property setter for certain (selected) particle(internal)
        * @param property Handle to property responsible for certain particle
        * @param particle particle to be re-assigned
        * @return true if property handle is valid 
        */
        bool _setParticle(OgitorsPropertyBase* property, const Ogre::String& particle);
        /**
        * Property setter for shadow casting flag(internal)
        * @param property Handle to property responsible for shadow casting flag
        * @param value new shadow casting flag
        * @return true if property handle is valid 
        */
        bool _setCastShadows(OgitorsPropertyBase* property, const bool& value);
    };

    //! Particle editor factory class
    /*!  
        A class that is responsible for instantiating particle editor class(es)
    */
    class OgitorExport CParticleEditorFactory: public CNodeEditorFactory
    {
    public:
        /** @copydoc CBaseEditorFactory::CBaseEditorFactory() */
        CParticleEditorFactory(OgitorsView *view = 0);
        /** @copydoc CBaseEditorFactory::~CBaseEditorFactory() */
        virtual ~CParticleEditorFactory() {};
        /** @copydoc CBaseEditorFactory::duplicate(OgitorsView *view) */
        virtual CBaseEditorFactory* duplicate(OgitorsView *view);
        /** @copydoc CBaseEditorFactory::CreateObject(CBaseEditor **, OgitorsPropertyValueMap &) */
        virtual CBaseEditor *CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params);
        /** @copydoc CBaseEditorFactory::GetPlaceHolderName() */
        virtual Ogre::String GetPlaceHolderName() {return "scbLight_Omni.mesh";};
    };
}
