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
    class OgitorExport CMultiSelEditor : public CBaseEditor
    {
        friend class CMultiSelEditorFactory;
    public:
        
        virtual void    showBoundingBox(bool bShow);
        void            deleteObjects();
        
        NameObjectPairList getSelection() {return mSelectedObjects;};   
        void getSelection(ObjectVector& list);

        virtual Ogre::AxisAlignedBox  getAABB();
        virtual Ogre::AxisAlignedBox  getWorldAABB() {return mWorldAABB;};
        virtual Ogre::SceneNode      *getNode() {return mNode;};
        virtual Ogre::Vector3          getDerivedPosition() {return mNode->_getDerivedPosition();};
        virtual Ogre::Quaternion      getDerivedOrientation() {return mNode->_getDerivedOrientation();};
        virtual Ogre::Vector3         getDerivedScale() {return mNode->_getDerivedScale();};

        void            setSelection(const ObjectVector& list);
        void            setSelection(NameObjectPairList &newselection);
        void            setSelection(CBaseEditor *object);
        virtual void    setDerivedPosition(Ogre::Vector3 val);
        virtual void    setDerivedOrientation(Ogre::Quaternion val);

        void add(CBaseEditor* object);
        void add(const NameObjectPairList& newselection);
        void add(const ObjectVector& newselection);
        void add(const Ogre::StringVector& newselection);

        void remove(CBaseEditor* object);
        void remove(const NameObjectPairList& newselection);
        void remove(const ObjectVector& newselection);
        void remove(const Ogre::StringVector& newselection);

        bool contains(CBaseEditor* object) const;
        bool containsOrEqual(CBaseEditor* object) const;
        bool isSingle() const;
        bool isEmpty() const;

        CBaseEditor *getFirstObject();
        CBaseEditor *getAsSingle();

    protected:
        NameObjectPairList   mSelectedObjects;
        NameObjectPairList   mModifyList;
        Ogre::AxisAlignedBox mWorldAABB;
        Ogre::SceneNode     *mNode;
        bool                 mDeletionInProgress;
        
        CMultiSelEditor(CBaseEditorFactory *factory, const Ogre::String& name);
        virtual     ~CMultiSelEditor();

        bool _setPosition(OgitorsPropertyBase* property, const Ogre::Vector3& position) 
        {
            setDerivedPosition(position);
            return true;
        }
        bool _setOrientation(OgitorsPropertyBase* property, const Ogre::Quaternion& orientation) 
        {
            setDerivedOrientation(orientation);
            return true;
        }
        bool _setScale(OgitorsPropertyBase* property, const Ogre::Vector3& val);

        void _createModifyList();
        void _clearSelection();
    };

    class OgitorExport CMultiSelEditorFactory: public CBaseEditorFactory
    {
    public:
        CMultiSelEditorFactory(OgitorsView *view = 0);
        virtual ~CMultiSelEditorFactory() {};
        /** @copydoc CBaseEditorFactory::duplicate(OgitorsView *view) */
        virtual CBaseEditorFactory* duplicate(OgitorsView *view);
        virtual CBaseEditor *CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params);
    };
}
