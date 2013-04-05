/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2013 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
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

    class OgitorExport CPGInstanceEditor : public CBaseEditor
    {
        friend class CPGInstanceEditorFactory;
    public:

        virtual bool     isSerializable() {return false;}; // Do not save this object
        virtual void     createProperties(OgitorsPropertyValueMap &params);
        /** @copydoc CBaseEditor::load(bool) */
        virtual bool     load(bool async = true);
        /** @copydoc CBaseEditor::unLoad() */
        virtual bool     unLoad();
        virtual void     showBoundingBox(bool bShow);
        
        virtual Ogre::AxisAlignedBox getAABB();
        virtual Ogre::SceneNode     *getNode() {return mHandle;};
        virtual Ogre::Vector3        getDerivedPosition();
        virtual Ogre::Quaternion     getDerivedOrientation();
        virtual Ogre::Vector3        getDerivedScale();

    protected:
        Ogre::SceneNode       *mHandle;
        int                    mIndex;
        
        OgitorsProperty<Ogre::Vector3>     *mPosition;             /** Position of Instance */
        OgitorsProperty<Ogre::Quaternion>  *mOrientation;          /** Node orientation (wrapper) */
        OgitorsProperty<Ogre::Vector3>     *mScale;                /** Node Scale (wrapper) */
        OgitorsProperty<Ogre::Real>        *mUniformScale;         /** Uniform Scale of Instance */
        OgitorsProperty<Ogre::Real>        *mYaw;                  /** Yaw of Instance*/

        CPGInstanceEditor(CBaseEditorFactory *factory);
        virtual     ~CPGInstanceEditor();

        bool _setPosition(OgitorsPropertyBase* property, const Ogre::Vector3& position);
        bool _setOrientation(OgitorsPropertyBase* property, const Ogre::Quaternion& orientation);
        bool _setScale(OgitorsPropertyBase* property, const Ogre::Vector3& scale);
        bool _setUniformScale(OgitorsPropertyBase* property, const Ogre::Real& scale);
        bool _setYaw(OgitorsPropertyBase* property, const Ogre::Real& yaw);
    };

    class OgitorExport CPGInstanceEditorFactory: public CBaseEditorFactory
    {
    public:
        CPGInstanceEditorFactory(OgitorsView *view = 0);
        virtual ~CPGInstanceEditorFactory() {};
        /** @copydoc CBaseEditorFactory::duplicate(OgitorsView *view) */
        virtual CBaseEditorFactory* duplicate(OgitorsView *view);
        virtual CBaseEditor *CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params);
    };
}
