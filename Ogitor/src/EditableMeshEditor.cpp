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

#include "OgitorsPrerequisites.h"
#include "BaseEditor.h"
#include "OgitorsRoot.h"
#include "OgitorsSystem.h"
#include "NodeEditor.h"
#include "EditableMeshEditor.h"


using namespace Ogitors;

//-------------------------------------------------------------------------------
CEditableMeshEditor::CEditableMeshEditor(CBaseEditorFactory *factory) : CNodeEditor(factory),
mMeshHandle(0), mBoundingBox(Ogre::AxisAlignedBox::BOX_NULL), 
mIsEditing(false), mInEditMode(false), mNeedsBBoxUpdate(true), 
mCurrentVertex(0), mCurrentNode(0), mSelectionMode(EMSM_VERTEX)
{
    mUsesGizmos = true;
    mUsesHelper = false;
}
//-------------------------------------------------------------------------------
CEditableMeshEditor::~CEditableMeshEditor()
{
    for(unsigned int i = 0;i < mNodes.size();i++)
        delete mNodes[i];
}
//-------------------------------------------------------------------------------
void CEditableMeshEditor::setSelectedImpl(bool bSelected)
{
    if(bSelected)
    {
        OgitorsRoot::getSingletonPtr()->CaptureKeyboard(this);
    }
    else
    {
        OgitorsRoot::getSingletonPtr()->ReleaseKeyboard();
        _stopEditMode();
    }
    CBaseEditor::setSelectedImpl(bSelected);
}
//-------------------------------------------------------------------------------
void CEditableMeshEditor::showBoundingBox(bool bShow) 
{
    if(!mBoxParentNode)
        createBoundingBox();

    if(bShow)
    {
        adjustBoundingBox();
        if(mBBoxNode)
            mBBoxNode->setVisible(true);
    }
    else if(mBBoxNode)
        mBBoxNode->setVisible(false);
}//-------------------------------------------------------------------------------
Ogre::AxisAlignedBox CEditableMeshEditor::getAABB() 
{
    if(mNeedsBBoxUpdate)
    {
        mNeedsBBoxUpdate = false;
        mBoundingBox = Ogre::AxisAlignedBox::BOX_NULL;
        for(unsigned int i = 0;i < mVertexes.size();i++)
            mBoundingBox.merge(mVertexes[i]);
    }
    return mBoundingBox;
}
//-------------------------------------------------------------------------------
bool CEditableMeshEditor::OnKeyDown (unsigned short key)
{
    if(key == 259)
    {
        if(mInEditMode)
            _stopEditMode();
        else
            _startEditMode();

        return true;
    }
    return false;
}
//-------------------------------------------------------------------------------
void CEditableMeshEditor::_renderNode(EditableMeshTriNode *node)
{
    if(!node)
        return;
    if(node->mLeftChild || node->mRightChild)
    {
        _renderNode(node->mLeftChild);
        _renderNode(node->mRightChild);
    }
    else
    {
        if(mInEditMode && node != mCurrentNode && !node->mIsSelected)
        {
            mMeshHandle->begin("EditableMeshEditingMaterial", Ogre::RenderOperation::OT_LINE_STRIP);
            mMeshHandle->position(*(node->mLeft));
            mMeshHandle->position(*(node->mRight));
            mMeshHandle->position(*(node->mTop));
            mMeshHandle->position(*(node->mLeft));
            mMeshHandle->end();
        }
        else
        {
            if(mInEditMode && node == mCurrentNode)
                mMeshHandle->begin("EditableMeshHighlightMaterial", Ogre::RenderOperation::OT_TRIANGLE_LIST);
            else if(mInEditMode && node->mIsSelected)
                mMeshHandle->begin("EditableMeshSelectionMaterial", Ogre::RenderOperation::OT_TRIANGLE_LIST);
            else
                mMeshHandle->begin(node->mMaterial, Ogre::RenderOperation::OT_TRIANGLE_LIST);

            if(node->mInverseBinding)
            {
                Ogre::Vector3 dir0 = *(node->mRight) - *(node->mLeft);
                Ogre::Vector3 dir1 = *(node->mLeft) - *(node->mTop);
                Ogre::Vector3 normal = dir0.crossProduct(dir1).normalisedCopy();
                
                mMeshHandle->position(*(node->mLeft));
                mMeshHandle->normal(normal);
                mMeshHandle->position(*(node->mTop));
                mMeshHandle->normal(normal);
                mMeshHandle->position(*(node->mRight));
                mMeshHandle->normal(normal);
            }
            else
            {
                Ogre::Vector3 dir0 = *(node->mTop) - *(node->mLeft);
                Ogre::Vector3 dir1 = *(node->mLeft) - *(node->mRight);
                Ogre::Vector3 normal = dir0.crossProduct(dir1).normalisedCopy();
                
                mMeshHandle->position(*(node->mLeft));
                mMeshHandle->normal(normal);
                mMeshHandle->position(*(node->mRight));
                mMeshHandle->normal(normal);
                mMeshHandle->position(*(node->mTop));
                mMeshHandle->normal(normal);
            }
            mMeshHandle->end();
        }
    }
}
//-------------------------------------------------------------------------------
void CEditableMeshEditor::_renderVertices()
{
    mMeshHandle->begin("EditableMeshVerticeMaterial", Ogre::RenderOperation::OT_POINT_LIST);
    
    for(unsigned int i = 0;i < mVertexes.size();i++)
    {
        mMeshHandle->position(mVertexes[i]);
    }

    mMeshHandle->end();
}
//-------------------------------------------------------------------------------
void CEditableMeshEditor::_updateMesh()
{
    mMeshHandle->clear();
    for(unsigned int i = 0;i < mRenderNodes.size();i++)
    {
        _renderNode(mRenderNodes[i]);
    }
    mMeshHandle->setQueryFlags(QUERYFLAG_MOVABLE);

    if(mInEditMode)
        _renderVertices();
}
//-------------------------------------------------------------------------------
void CEditableMeshEditor::createProperties(OgitorsPropertyValueMap &params)
{
    mVertexes.clear();
    _addVertex(Ogre::Vector3(-0.5f,  0.5f,  0.5f));
    _addVertex(Ogre::Vector3(-0.5f,  0.5f, -0.5f));
    _addVertex(Ogre::Vector3( 0.5f,  0.5f, -0.5f));
    _addVertex(Ogre::Vector3( 0.5f,  0.5f,  0.5f));

    _addVertex(Ogre::Vector3(-0.5f, -0.5f,  0.5f));
    _addVertex(Ogre::Vector3(-0.5f, -0.5f, -0.5f));
    _addVertex(Ogre::Vector3( 0.5f, -0.5f, -0.5f));
    _addVertex(Ogre::Vector3( 0.5f, -0.5f,  0.5f));

    mNodes.clear();
    for(int i = 0;i < 12;i++)
    {
        EditableMeshTriNode *node = new EditableMeshTriNode;
        mNodes.push_back(node);
        mRenderNodes.push_back(node);
        node->mLeftChild = 0;
        node->mRightChild = 0;
        node->mInverseBinding = false;
        node->mIsSelected = false;
        node->mMaterial = "EditableMeshDefaultMaterial";
    }
    
    mNodes[0]->mNeighbourLeft = mNodes[2];
    mNodes[0]->mNeighbourRight = mNodes[5];
    mNodes[0]->mNeighbourBottom = mNodes[1];
    mNodes[0]->mLeft = &(mVertexes[0]);
    mNodes[0]->mRight = &(mVertexes[2]);
    mNodes[0]->mTop = &(mVertexes[1]);

    mNodes[1]->mNeighbourLeft = mNodes[6];
    mNodes[1]->mNeighbourRight = mNodes[9];
    mNodes[1]->mNeighbourBottom = mNodes[0];
    mNodes[1]->mLeft = &(mVertexes[2]);
    mNodes[1]->mRight = &(mVertexes[0]);
    mNodes[1]->mTop = &(mVertexes[3]);

    mNodes[2]->mNeighbourLeft = mNodes[0];
    mNodes[2]->mNeighbourRight = mNodes[9];
    mNodes[2]->mNeighbourBottom = mNodes[3];
    mNodes[2]->mLeft = &(mVertexes[1]);
    mNodes[2]->mRight = &(mVertexes[4]);
    mNodes[2]->mTop = &(mVertexes[0]);

    mNodes[3]->mNeighbourLeft = mNodes[10];
    mNodes[3]->mNeighbourRight = mNodes[4];
    mNodes[3]->mNeighbourBottom = mNodes[2];
    mNodes[3]->mLeft = &(mVertexes[4]);
    mNodes[3]->mRight = &(mVertexes[1]);
    mNodes[3]->mTop = &(mVertexes[5]);

    mNodes[4]->mNeighbourLeft = mNodes[3];
    mNodes[4]->mNeighbourRight = mNodes[11];
    mNodes[4]->mNeighbourBottom = mNodes[5];
    mNodes[4]->mLeft = &(mVertexes[1]);
    mNodes[4]->mRight = &(mVertexes[6]);
    mNodes[4]->mTop = &(mVertexes[5]);

    mNodes[5]->mNeighbourLeft = mNodes[6];
    mNodes[5]->mNeighbourRight = mNodes[0];
    mNodes[5]->mNeighbourBottom = mNodes[4];
    mNodes[5]->mLeft = &(mVertexes[6]);
    mNodes[5]->mRight = &(mVertexes[1]);
    mNodes[5]->mTop = &(mVertexes[2]);

    mNodes[6]->mNeighbourLeft = mNodes[1];
    mNodes[6]->mNeighbourRight = mNodes[5];
    mNodes[6]->mNeighbourBottom = mNodes[7];
    mNodes[6]->mLeft = &(mVertexes[3]);
    mNodes[6]->mRight = &(mVertexes[6]);
    mNodes[6]->mTop = &(mVertexes[2]);

    mNodes[7]->mNeighbourLeft = mNodes[11];
    mNodes[7]->mNeighbourRight = mNodes[8];
    mNodes[7]->mNeighbourBottom = mNodes[6];
    mNodes[7]->mLeft = &(mVertexes[6]);
    mNodes[7]->mRight = &(mVertexes[3]);
    mNodes[7]->mTop = &(mVertexes[7]);

    mNodes[8]->mNeighbourLeft = mNodes[7];
    mNodes[8]->mNeighbourRight = mNodes[10];
    mNodes[8]->mNeighbourBottom = mNodes[9];
    mNodes[8]->mLeft = &(mVertexes[3]);
    mNodes[8]->mRight = &(mVertexes[4]);
    mNodes[8]->mTop = &(mVertexes[7]);

    mNodes[9]->mNeighbourLeft = mNodes[2];
    mNodes[9]->mNeighbourRight = mNodes[1];
    mNodes[9]->mNeighbourBottom = mNodes[8];
    mNodes[9]->mLeft = &(mVertexes[4]);
    mNodes[9]->mRight = &(mVertexes[3]);
    mNodes[9]->mTop = &(mVertexes[0]);
    
    mNodes[10]->mNeighbourLeft = mNodes[3];
    mNodes[10]->mNeighbourRight = mNodes[8];
    mNodes[10]->mNeighbourBottom = mNodes[11];
    mNodes[10]->mLeft = &(mVertexes[5]);
    mNodes[10]->mRight = &(mVertexes[7]);
    mNodes[10]->mTop = &(mVertexes[4]);

    mNodes[11]->mNeighbourLeft = mNodes[7];
    mNodes[11]->mNeighbourRight = mNodes[4];
    mNodes[11]->mNeighbourBottom = mNodes[10];
    mNodes[11]->mLeft = &(mVertexes[7]);
    mNodes[11]->mRight = &(mVertexes[5]);
    mNodes[11]->mTop = &(mVertexes[6]);

    mProperties.initValueMap(params);
}    
//-----------------------------------------------------------------------------------------
bool CEditableMeshEditor::load(bool async)
{
    if(mLoaded->get())
        return true;
    
    if(CNodeEditor::load())
    {
        mMeshHandle = OgitorsRoot::getSingletonPtr()->GetSceneManager()->createManualObject(mName->get());
        mMeshHandle->setDynamic(true);
        mMeshHandle->setQueryFlags(QUERYFLAG_MOVABLE);
       
        _updateMesh();
        mHandle->attachObject(mMeshHandle);
    }
    else
        return false;

    return true;
}
//-----------------------------------------------------------------------------------------
bool CEditableMeshEditor::unLoad()
{
    if(!mLoaded->get())
        return true;

    destroyBoundingBox();

    if(mMeshHandle)
    {
        mMeshHandle->detachFromParent();
        mMeshHandle->_getManager()->destroyManualObject(mMeshHandle);
        mMeshHandle = 0;
    }

    return CNodeEditor::unLoad();
}
//-----------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------
//--------------CPARTICLEEDITORFACTORY-------------------------------------------
//-------------------------------------------------------------------------------
CEditableMeshEditorFactory::CEditableMeshEditorFactory(OgitorsView *view) : CNodeEditorFactory(view)
{
    mTypeName = "Editable Mesh Object";
    mEditorType = ETYPE_MOVABLE;
    mAddToObjectList = true;
    mRequirePlacement = true;
    mIcon = "entity.svg";
    mCapabilities = CAN_MOVE | CAN_SCALE | CAN_ROTATE | CAN_DELETE  | CAN_FOCUS | CAN_DRAG;
}
//-----------------------------------------------------------------------------------------
CBaseEditorFactory *CEditableMeshEditorFactory::duplicate(OgitorsView *view)
{
    CBaseEditorFactory *ret = new CEditableMeshEditorFactory(view);
    ret->mTypeID = mTypeID;

    return ret;
}
//-----------------------------------------------------------------------------------------
CBaseEditor *CEditableMeshEditorFactory::CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params)
{
    CEditableMeshEditor *object = new CEditableMeshEditor(this);

    OgitorsPropertyValueMap::iterator ni;

    if ((ni = params.find("init")) != params.end())
    {
        OgitorsPropertyValue value = EMPTY_PROPERTY_VALUE;
        value.val = Ogre::Any(CreateUniqueID("EditableMesh"));
        params["name"] = value;
        params.erase(ni);
    }

    object->createProperties(params);
    object->mParentEditor->init(*parent);

    mInstanceCount++;
    return object;
}
//-------------------------------------------------------------------------------
