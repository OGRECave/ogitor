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

#include "OgitorsPrerequisites.h"
#include "BaseEditor.h"
#include "OgitorsRoot.h"
#include "OgitorsSystem.h"
#include "NodeEditor.h"
#include "ViewportEditor.h"
#include "CameraEditor.h"
#include "EditableMeshEditor.h"


using namespace Ogitors;

//-------------------------------------------------------------------------------
void CEditableMeshEditor::_startEditMode()
{
    if(mInEditMode)
        return;

    mInEditMode = true;
    OgitorsRoot::getSingletonPtr()->CaptureMouse(this);
    showBoundingBox(false);

    _updateMesh();
}
//-------------------------------------------------------------------------------
void CEditableMeshEditor::_stopEditMode()
{
    if(!mInEditMode)
        return;

    mInEditMode = false;
    OgitorsRoot::getSingletonPtr()->ReleaseMouse();

    mCurrentNode = 0;
    mCurrentVertex = 0;
    mIsEditing = false;

    for(unsigned int i = 0;i < mNodes.size();i++)
        mNodes[i]->mIsSelected = false;

    _updateMesh();

    showBoundingBox(true);
}
//-------------------------------------------------------------------------------
bool CEditableMeshEditor::_pickVertex(Ogre::Ray& ray)
{
    Ogre::Real distance = 999999.0f;
    mCurrentVertex = 0;

    Ogre::Vector3 nodePos = mHandle->_getDerivedPosition();
    Ogre::Vector3 vertexPos;

    for(unsigned int i = 0;i < mVertexes.size();i++)
    {
        vertexPos = nodePos + mVertexes[i];
        Ogre::AxisAlignedBox box(vertexPos.x - 0.1f, vertexPos.y - 0.1f, vertexPos.z - 0.1f, vertexPos.x + 0.1f, vertexPos.y + 0.1f, vertexPos.z + 0.1f);
        std::pair<bool, Ogre::Real> result = ray.intersects(box);
        if(result.first && result.second < distance)
        {
            distance = result.second;
            mCurrentVertex = &(mVertexes[i]);
        }
    }
    
    if(distance < 999999.0f)
        return true;

    return false;
}
//-------------------------------------------------------------------------------
void CEditableMeshEditor::_fillTriangles(EditableMeshTriNode *node)
{
    if(!node)
        return;

    if(node->mLeftChild || node->mRightChild)
    {
        _fillTriangles(node->mLeftChild);
        _fillTriangles(node->mRightChild);
    }
    else
        mPickList.push_back(node);
}
//-------------------------------------------------------------------------------
bool CEditableMeshEditor::_pickFace(Ogre::Ray& ray)
{
    Ogre::Real distance = 999999.0f;
    mCurrentNode = 0;
    mPickList.clear();

    for(unsigned int i = 0;i < mRenderNodes.size();i++)
    {
        _fillTriangles(mRenderNodes[i]);
    }

    Ogre::Vector3 nodePos = mHandle->_getDerivedPosition();
    Ogre::Vector3 v1, v2, v3;

    for(unsigned int i = 0;i < mPickList.size();i++)
    {
        v1 = nodePos + (mHandle->_getDerivedOrientation() * (mHandle->_getDerivedScale() * (*(mPickList[i]->mLeft))));
        v2 = nodePos + (mHandle->_getDerivedOrientation() * (mHandle->_getDerivedScale() * (*(mPickList[i]->mRight))));
        v3 = nodePos + (mHandle->_getDerivedOrientation() * (mHandle->_getDerivedScale() * (*(mPickList[i]->mTop))));

        std::pair<bool, Ogre::Real> result = Ogre::Math::intersects(ray, v1, v2, v3, true, true);
        
        if(result.first && result.second < distance)
        {
            distance = result.second;
            mCurrentNode = mPickList[i];
        }
    }
    
    if(distance < 999999.0f)
        return true;

    return false;
}
//-------------------------------------------------------------------------------
void CEditableMeshEditor::OnMouseMove (CViewportEditor *viewport, Ogre::Vector2 point, unsigned int buttons)
{
    Ogre::Ray ray;
    viewport->GetMouseRay(point, ray);

    if(mIsEditing)
    {
        Ogre::Vector3 pos = *mCurrentVertex;
        pos = mHandle->_getDerivedPosition() + (mHandle->_getDerivedOrientation() * (mHandle->_getDerivedScale() * pos));
        Ogre::Quaternion q = getDerivedOrientation();
        Ogre::Vector3 newpos = OgitorsRoot::getSingletonPtr()->GetGizmoIntersectCameraPlane(pos, q, ray);

        Ogre::Quaternion qParent = mHandle->_getDerivedOrientation().Inverse();
        Ogre::Vector3 vParent = mHandle->_getDerivedPosition();
        newpos = (newpos - vParent);
        newpos /= mHandle->_getDerivedScale();
        newpos = qParent * newpos;
        *mCurrentVertex = newpos;
        _updateMesh();
        mNeedsBBoxUpdate = true;
    }
    else if(mSelectionMode == EMSM_FACE)
    {
        EditableMeshTriNode *old = mCurrentNode;     
        
        _pickFace(ray);

        if(old != mCurrentNode)
            _updateMesh();
    }
}
//-------------------------------------------------------------------------------
void CEditableMeshEditor::OnMouseLeave (CViewportEditor *viewport, Ogre::Vector2 point, unsigned int buttons)
{
    mIsEditing = false;
}
//-------------------------------------------------------------------------------
void CEditableMeshEditor::OnMouseLeftDown (CViewportEditor *viewport, Ogre::Vector2 point, unsigned int buttons)
{
    bool updateneeded = false;
    Ogre::Ray ray;
    viewport->GetMouseRay(point, ray);

    if(mSelectionMode == EMSM_VERTEX)
    {
        mIsEditing = _pickVertex(ray);
    }
    else if(mSelectionMode == EMSM_FACE)
    {
        _pickFace(ray);
        if(CViewportEditor::mViewKeyboard[CViewportEditor::mSpecial.SPK_ADD_TO_SELECTION])
        {
            if(mCurrentNode)
            {
                mCurrentNode->mIsSelected = !mCurrentNode->mIsSelected;
                updateneeded = true;
            }
        }
        else
        {
            for(unsigned int i = 0;i < mNodes.size();i++)
            {
                updateneeded |= mNodes[i]->mIsSelected;
                mNodes[i]->mIsSelected = false;
            }

            if(mCurrentNode)
            {
                mCurrentNode->mIsSelected = !mCurrentNode->mIsSelected;
                updateneeded = true;
            }

        }
    }
 
    if(updateneeded)
        _updateMesh();
}
//-------------------------------------------------------------------------------
void CEditableMeshEditor::OnMouseLeftUp (CViewportEditor *viewport, Ogre::Vector2 point, unsigned int buttons)
{
    mIsEditing = false;
}
//-------------------------------------------------------------------------------
void CEditableMeshEditor::OnMouseRightDown (CViewportEditor *viewport, Ogre::Vector2 point, unsigned int buttons)
{
    mSelectionMode = (EditableMeshSelectionMode)((mSelectionMode + 1) & 1);
    mCurrentNode = 0;
    mCurrentVertex = 0;
    _updateMesh();
}
//-------------------------------------------------------------------------------
