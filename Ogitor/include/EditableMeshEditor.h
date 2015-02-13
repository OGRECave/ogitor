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
    struct EditableMeshTriNode
    {
        EditableMeshTriNode *mNeighbourLeft;
        EditableMeshTriNode *mNeighbourRight;
        EditableMeshTriNode *mNeighbourBottom;
        EditableMeshTriNode *mLeftChild;
        EditableMeshTriNode *mRightChild;
        Ogre::Vector3 *mLeft;
        Ogre::Vector3 *mRight;
        Ogre::Vector3 *mTop;
        bool           mInverseBinding;
        bool           mIsSelected;
        Ogre::String   mMaterial;
    };
    
    typedef std::vector<Ogre::Vector3> EditableMeshVertexData; 
    typedef std::vector<EditableMeshTriNode*> EditableMeshTriNodeData; 

    enum EditableMeshSelectionMode
    {
        EMSM_VERTEX = 0,
        EMSM_FACE = 1
    };

    class OgitorExport CEditableMeshEditor : public CNodeEditor, public MouseListener, public KeyboardListener
    {
        friend class CEditableMeshEditorFactory;
    public:

        virtual void            createProperties(OgitorsPropertyValueMap &params);
        /// Virtual Function that loads (Creates) underlying object 
        virtual bool            load(bool async = true);
        /// Virtual Function that unloads (Deletes) underlying object 
        virtual bool            unLoad();

        virtual void            setSelectedImpl(bool bSelected);

        virtual void            showBoundingBox(bool bShow);
        virtual Ogre::AxisAlignedBox getAABB();

    protected:
        Ogre::ManualObject        *mMeshHandle;
        Ogre::AxisAlignedBox       mBoundingBox;
        EditableMeshVertexData     mVertexes;
        EditableMeshTriNodeData    mRenderNodes;
        EditableMeshTriNodeData    mNodes;
        bool                       mIsEditing;
        bool                       mInEditMode;
        bool                       mNeedsBBoxUpdate;
        Ogre::Vector3             *mCurrentVertex;
        EditableMeshTriNode       *mCurrentNode;
        EditableMeshSelectionMode  mSelectionMode;
        EditableMeshTriNodeData    mPickList;
        

        CEditableMeshEditor(CBaseEditorFactory *factory);
        virtual     ~CEditableMeshEditor();

        void _startEditMode();
        void _stopEditMode();
        void _updateMesh();
        void _renderNode(EditableMeshTriNode *node);
        void _renderVertices();
        bool _pickVertex(Ogre::Ray& ray);
        bool _pickFace(Ogre::Ray& ray);
        void _fillTriangles(EditableMeshTriNode *node);
        Ogre::Vector3 *_addVertex(const Ogre::Vector3& vertex)
        {
                mNeedsBBoxUpdate = true;
                mVertexes.push_back(vertex);
                return &(mVertexes[mVertexes.size() - 1]);
        }

        void OnMouseMove (CViewportEditor *viewport, Ogre::Vector2 point, unsigned int buttons);
        void OnMouseLeave (CViewportEditor *viewport, Ogre::Vector2 point, unsigned int buttons);
        void OnMouseLeftDown (CViewportEditor *viewport, Ogre::Vector2 point, unsigned int buttons);
        void OnMouseLeftUp (CViewportEditor *viewport, Ogre::Vector2 point, unsigned int buttons);
        void OnMouseRightDown (CViewportEditor *viewport, Ogre::Vector2 point, unsigned int buttons);
        void OnMouseRightUp (CViewportEditor *viewport, Ogre::Vector2 point, unsigned int buttons) {};
        void OnMouseMiddleDown (CViewportEditor *viewport, Ogre::Vector2 point, unsigned int buttons) {};
        void OnMouseMiddleUp (CViewportEditor *viewport, Ogre::Vector2 point, unsigned int buttons) {};
        void OnMouseWheel (CViewportEditor *viewport, Ogre::Vector2 point, float delta, unsigned int buttons) {};
        bool OnKeyDown (unsigned short key);
        bool OnKeyUp (unsigned short key) { return false; }

    };

    class OgitorExport CEditableMeshEditorFactory: public CNodeEditorFactory
    {
    public:
        CEditableMeshEditorFactory(OgitorsView *view = 0);
        virtual ~CEditableMeshEditorFactory() {};
        /** @copydoc CBaseEditorFactory::duplicate(OgitorsView *view) */
        virtual CBaseEditorFactory* duplicate(OgitorsView *view);
        virtual CBaseEditor *CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params);
        virtual Ogre::String GetPlaceHolderName() {return "scbLight_Omni.mesh";};
    };

}
