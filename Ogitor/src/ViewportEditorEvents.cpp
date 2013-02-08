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
#include "CameraEditor.h"
#include "OgitorsUndoManager.h"
#include "TerrainEditor.h"
#include "MultiSelEditor.h"
#include "ViewportEditor.h"
#include "TerrainPageEditor.h"
#include "TerrainGroupEditor.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#include "OgreTerrain.h"
#include "OgreTerrainGroup.h"
#else
#include "Terrain/OgreTerrain.h"
#include "Terrain/OgreTerrainGroup.h"
#endif

namespace Ogitors
{
    OgitorsUndoManager *CViewportEditor::mUndoManager = 0;
    NameObjectPairList  CViewportEditor::mHighLighted;
    int                 CViewportEditor::mEditorToolEx = TOOL_EX_NONE;
    int                 CViewportEditor::mEditorAxis = 0;
    float               CViewportEditor::mCameraSpeed = 1.0f;
    bool                CViewportEditor::mIsEditing = false;
    bool                CViewportEditor::mIsSettingPos = false;
    bool               *CViewportEditor::mViewKeyboard = 0;
    OgitorsSpecialKeys  CViewportEditor::mSpecial;
    float               CViewportEditor::mSnapMultiplier = 1.0f;

    //-------------------------------------------------------------------------------
    void CViewportEditor::ResetCommonValues()
    {
        mUndoManager = OgitorsUndoManager::getSingletonPtr();
        mHighLighted.clear();
        mEditorToolEx = TOOL_EX_NONE;
        mEditorAxis = 0;
        mCameraSpeed = 1.0f;
        mIsEditing = false;
        mIsSettingPos = false;
    }
    //-------------------------------------------------------------------------------
    void  CViewportEditor::SetEditorTool(unsigned int tool)
    {
        Ogitors::CMultiSelEditor *selected = mOgitorsRoot->GetSelection();

        if(mViewGrid)
        {
            if(tool == Ogitors::TOOL_MOVE && !selected->isEmpty() && !selected->getAsSingle()->isTerrainType())
                mViewGrid->setTrackingObject(selected->getAsSingle());
            else
                mViewGrid->setTrackingObject(0);
        }

        if(!selected->isEmpty())
        {
            Ogre::Ray ray;
            GetMouseRay(mLastMouse, ray);
            SaveEditorStates(ray);
        }

        mOgitorsRoot->SetGizmoMode(tool);

        if(mOgitorsRoot->GetTerrainEditor()) 
            mOgitorsRoot->GetTerrainEditor()->setEditMode(0);

        mOgitorsRoot->SetEditorTool(tool);
    }
    //-------------------------------------------------------------------------------
    unsigned int CViewportEditor::GetEditorTool()
    {
        return OgitorsRoot::getSingletonPtr()->GetEditorTool();
    }
    //-------------------------------------------------------------------------------
    void CViewportEditor::OnKeyDown (unsigned int key)
    {
        mViewKeyboard[key] = true;

        if(!mOgitorsRoot->GetSelection()->isEmpty())
        {
            if(mViewKeyboard[mSpecial.SPK_SWITCH_AXIS] && GetEditorTool() >= TOOL_MOVE && GetEditorTool() <= TOOL_SCALE)
            {
                mViewKeyboard[mSpecial.SPK_SWITCH_AXIS] = false;
                if(mEditorAxis == 0)
                    mEditorAxis = AXIS_X;
                else if(mEditorAxis == AXIS_Z)
                    mEditorAxis = 0;
                else
                    mEditorAxis = (mEditorAxis << 1) % 7;

                mOgitorsRoot->HighlightGizmo(mEditorAxis);
                Ogre::Ray mouseRay;
                if(GetMouseRay(mLastMouse, mouseRay)) 
                    SaveEditorStates(mouseRay);
            }

            if(mViewKeyboard[mSpecial.SPK_DELETE]) 
            { 
                mViewKeyboard[mSpecial.SPK_DELETE] = false;
                DeleteSelectedObject();
            }

            if(mViewKeyboard[mSpecial.SPK_FOCUS_OBJECT]) 
            {   
                mViewKeyboard[mSpecial.SPK_FOCUS_OBJECT] = false;
                FocusSelectedObject();
            }
        } 
    }
    //-------------------------------------------------------------------------------
    void CViewportEditor::OnKeyUp (unsigned int key)
    {
        mViewKeyboard[key] = false;
    }
    //-------------------------------------------------------------------------------
    void CViewportEditor::OnMouseMove (Ogre::Vector2 point, unsigned int buttons, bool imitate)
    {
        mMouseMovedSignal = true;
        mLastButtons = buttons;
        if(mIsSettingPos)
        {
            mIsSettingPos = false;
            mLastMouse = point;
            return;
        }

        float DeltaX = (point.x - mLastMouse.x) * 0.5f;
        float DeltaY = (point.y - mLastMouse.y) * 0.5f;

        OgitorsRoot *ogRoot = mOgitorsRoot;
        CMultiSelEditor *multisel = ogRoot->GetSelection();

        if(!imitate)
        {
            if(buttons & OMB_MIDDLE)
            {
                Ogre::Vector3 vPos = mActiveCamera->getDerivedPosition();
                Ogre::Vector3 vDelta = Ogre::Vector3(DeltaX * mCameraSpeed / 3.0f,-DeltaY * mCameraSpeed / 3.0f,0);

                mActiveCamera->setDerivedPosition(vPos + (mActiveCamera->getDerivedOrientation() * vDelta));

                mNewCamPosition = Ogre::Vector3::ZERO;
                if(ogRoot->GetTerrainEditor()) 
                    ogRoot->GetTerrainEditor()->stopEdit();
                OgitorsSystem::getSingletonPtr()->ShowMouseCursor(false);

#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE
                point.x -= (DeltaX * 2.0f);
                point.y -= (DeltaY * 2.0f);

                mIsSettingPos = true;
                OgitorsSystem::getSingletonPtr()->SetMousePosition(point + Ogre::Vector2(mHandle->getActualLeft(),mHandle->getActualTop()));
#else
                mLastMouse = point;
#endif
            }
            else if(buttons & OMB_RIGHT)
            {
                OgitorsSystem::getSingletonPtr()->ShowMouseCursor(false);
                if(mViewKeyboard[mSpecial.SPK_OBJECT_CENTERED_MOVEMENT] && !multisel->isEmpty() && multisel->getAsSingle()->supports(CAN_FOCUS))
                {
                    Ogre::Vector3 lookat =  multisel->getAsSingle()->getDerivedPosition();
                    Ogre::Vector3 vDistance = (mActiveCamera->getDerivedPosition() - lookat);
                    float fDist = vDistance.length();
                    vDistance = (mActiveCamera->getCamera()->getDerivedRight() * -DeltaX * fDist / 48.0f) + (mActiveCamera->getCamera()->getDerivedUp() * DeltaY * fDist / 96.0f) + vDistance;
                    vDistance.normalise();
                    Ogre::Vector3 vNewPos = lookat + (vDistance * fDist);
                    mActiveCamera->setDerivedPosition(vNewPos);
                    mActiveCamera->lookAt(lookat);
                    mNewCamPosition = Ogre::Vector3::ZERO;
                }
                else
                {
                    mActiveCamera->yaw(Ogre::Degree(-DeltaX / 4.0f));
                    mActiveCamera->pitch(Ogre::Degree(-DeltaY / 4.0f));
                }
                OgitorsSystem::getSingletonPtr()->ShowMouseCursor(false);

#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE
                point.x -= (DeltaX * 2.0f);
                point.y -= (DeltaY * 2.0f);

                mIsSettingPos = true;
                OgitorsSystem::getSingletonPtr()->SetMousePosition(point + Ogre::Vector2(mHandle->getActualLeft(),mHandle->getActualTop()));
#else
                mLastMouse = point;
#endif
                Ogre::Ray mouseRay;
                if(!GetMouseRay(point, mouseRay)) return;
                if(!ogRoot->GetSelection()->isEmpty())
                    mLastUsedPlane = mOgitorsRoot->FindGizmoTranslationPlane(mouseRay,mEditorAxis);
            }
        }

        Ogre::Ray mouseRay;
        if(!GetMouseRay(point, mouseRay)) return;

        if(ogRoot->GetTerrainEditor() && ogRoot->GetTerrainEditor()->getEditMode() && ogRoot->GetTerrainEditor()->isSelected()) 
            ogRoot->GetTerrainEditor()->setMousePosition(mouseRay);

        unsigned int CURRENT_EDITOR_TOOL = GetEditorTool();

        if(CURRENT_EDITOR_TOOL != TOOL_ROTATE || multisel->isEmpty()) 
            mLastMouse = point;

        if(!multisel->isEmpty())
        {
            if(mEditorAxis == 0)
            {
                int Axis = 0;
                ogRoot->PickGizmos(mouseRay, Axis);
                ogRoot->HighlightGizmo(Axis);
            }
            else
            {
                ogRoot->HighlightGizmo(mEditorAxis);
            }
        }

        if(mIsEditing && !multisel->isEmpty() && !multisel->getAsSingle()->getLocked())
        {
            if((CURRENT_EDITOR_TOOL == TOOL_MOVE) && multisel->getAsSingle()->supports(CAN_MOVE))
            {
                if(mEditorToolEx == TOOL_EX_CLONE) 
                {
                    CloneMove(false);
                }
                else if(mEditorToolEx == TOOL_EX_INSTANCECLONE) 
                {
                    CloneMove(true);
                }

                if(mFirstTimeTranslation)
                    PrepareTranslationUndo();

                Ogre::Vector3 vNewPos;

                if(mEditorAxis == 0)
                {
                    Ogre::Vector3 vTempPos;

                    if((mOgitorsRoot->GetSnapGroundState() || mViewKeyboard[mSpecial.SPK_SNAP]) && GetHitPosition(mouseRay, vTempPos, multisel->getAsSingle()->getName()))
                    {
                        vNewPos = vTempPos;
                        mLastDerivedPosition = vNewPos;
                        mLast3DDelta = mOgitorsRoot->GetGizmoIntersect(mouseRay,mLastUsedPlane,mEditorAxis,mLastDerivedPosition);
                        mLastUsedPlane = mOgitorsRoot->FindGizmoTranslationPlane(mouseRay,mEditorAxis);
                    }
                    else
                    {
                        vNewPos = mOgitorsRoot->GetGizmoIntersect(mouseRay,mLastUsedPlane,mEditorAxis,mLastDerivedPosition);
                        vNewPos = vNewPos - mLast3DDelta + mLastDerivedPosition;
                    }
                }
                else
                {
                    vNewPos = mOgitorsRoot->GetGizmoIntersect(mouseRay,mLastUsedPlane,mEditorAxis,mLastDerivedPosition);
                    vNewPos = vNewPos - mLast3DDelta + mLastDerivedPosition;
                    if(mViewKeyboard[mSpecial.SPK_SNAP])
                    {
                        int X = vNewPos.x / (ViewportGrid::getGridSpacing() * mSnapMultiplier);
                        int Z = vNewPos.z / (ViewportGrid::getGridSpacing() * mSnapMultiplier);
                        int Y = vNewPos.y / (ViewportGrid::getGridSpacing() * mSnapMultiplier);
                        vNewPos.x = X * (ViewportGrid::getGridSpacing() * mSnapMultiplier);
                        vNewPos.z = Z * (ViewportGrid::getGridSpacing() * mSnapMultiplier);
                        vNewPos.y = Y * (ViewportGrid::getGridSpacing() * mSnapMultiplier);
                    }
                }

                multisel->getAsSingle()->setDerivedPosition(vNewPos);

                if(mFirstTimeTranslation)
                {
                    mFirstTimeTranslation = false;
                    OgitorsUndoManager::getSingletonPtr()->EndCollection(true);
                }
            }
            else if((CURRENT_EDITOR_TOOL == TOOL_SCALE) && multisel->getAsSingle()->supports(CAN_SCALE))
            {
                if(mFirstTimeTranslation)
                    PrepareTranslationUndo();

                Ogre::Vector3 vNewDist = ogRoot->GetGizmoIntersect(mouseRay,mLastUsedPlane,mEditorAxis,mLastDerivedPosition);
                Ogre::Vector3 vScale = mLastScale;
                float fNewDist = vNewDist.length();
                float fLength = mLast3DDelta.length();

                if(mViewKeyboard[mSpecial.SPK_SNAP] || mEditorAxis == 0)
                {
                    vScale *= (fNewDist / fLength);
                }
                else
                {
                    if(mEditorAxis & AXIS_X) vScale.x *= (fNewDist / fLength);
                    if(mEditorAxis & AXIS_Y) vScale.y *= (fNewDist / fLength);
                    if(mEditorAxis & AXIS_Z) vScale.z *= (fNewDist / fLength);
                }
                multisel->getAsSingle()->getProperties()->setValue("scale",vScale);

                if(mFirstTimeTranslation)
                {
                    mFirstTimeTranslation = false;
                    OgitorsUndoManager::getSingletonPtr()->EndCollection(true);
                }
            }
            else if((CURRENT_EDITOR_TOOL == TOOL_ROTATE) && multisel->getAsSingle()->supports(CAN_ROTATE))
            {
                if(mEditorAxis == 0)
                    mEditorAxis = AXIS_Y;

                int axisSave = mEditorAxis;

                if(!multisel->getAsSingle()->supports(CAN_ROTATEX))
                    mEditorAxis &= ~AXIS_X;
                if(!multisel->getAsSingle()->supports(CAN_ROTATEY))
                    mEditorAxis &= ~AXIS_Y;
                if(!multisel->getAsSingle()->supports(CAN_ROTATEZ))
                    mEditorAxis &= ~AXIS_Z;

                if(mEditorAxis)
                {
                    if(mFirstTimeTranslation)
                        PrepareTranslationUndo();

                    Ogre::Quaternion q1 = mLastDerivedOrient;

                    if(mViewKeyboard[mSpecial.SPK_SNAP])
                    {
                        Ogre::Real divider = 5.0f;

                        DeltaY = (int)(DeltaY / divider);
                        DeltaY *= divider;
                    }

                    if(ogRoot->GetWorldSpaceGizmoOrientation())
                    {
                        switch(mEditorAxis)
                        {
                        case AXIS_X:q1 = Ogre::Quaternion(Ogre::Degree(-DeltaY),Ogre::Vector3(1,0,0)) * q1;break;
                        case AXIS_Y:q1 = Ogre::Quaternion(Ogre::Degree(-DeltaY),Ogre::Vector3(0,1,0)) * q1;break;
                        case AXIS_Z:q1 = Ogre::Quaternion(Ogre::Degree(-DeltaY),Ogre::Vector3(0,0,1)) * q1;break;
                        }
                    }
                    else
                    {
                        switch(mEditorAxis)
                        {
                        case AXIS_X:q1 = q1 * Ogre::Quaternion(Ogre::Degree(-DeltaY),Ogre::Vector3(1,0,0));break;
                        case AXIS_Y:q1 = q1 * Ogre::Quaternion(Ogre::Degree(-DeltaY),Ogre::Vector3(0,1,0));break;
                        case AXIS_Z:q1 = q1 * Ogre::Quaternion(Ogre::Degree(-DeltaY),Ogre::Vector3(0,0,1));break;
                        }
                    }

                    multisel->getAsSingle()->setDerivedOrientation(q1);
                    if(mFirstTimeTranslation)
                    {
                        mFirstTimeTranslation = false;
                        OgitorsUndoManager::getSingletonPtr()->EndCollection(true);
                    }
                }

                mEditorAxis = axisSave;
            }
        }

        bool multiselectmode = !mIsEditing;
        multiselectmode &= (CURRENT_EDITOR_TOOL >= TOOL_SELECT && CURRENT_EDITOR_TOOL <= TOOL_SCALE);
        multiselectmode &= (buttons & OMB_LEFT);

        if(multiselectmode)
        {
            if(mLastClickPoint.x > 0 && mLastClickPoint.y > 0 )
            {
                float width = mHandle->getActualWidth();
                float height = mHandle->getActualHeight();

                float sx = mLastClickPoint.x / width * 2.0f;
                float sy = mLastClickPoint.y / height * 2.0f;
                float ex = mLastMouse.x / width * 2.0f;
                float ey = mLastMouse.y / height * 2.0f; 

                ogRoot->ShowSelectionRect(Ogre::Vector4(sx - 1.0f,1.0f - sy,ex - 1.0f,1.0f - ey));

                DoVolumeSelect();
                mVolumeSelecting = true;
            }
        }
        else
            HighlightObjectAtPosition(mouseRay);
    }
    //-------------------------------------------------------------------------------
    void CViewportEditor::OnMouseLeave (Ogre::Vector2 point, unsigned int buttons)
    {
        if(mIsEditing)
        {
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE
            mIsSettingPos = true;
            OgitorsSystem::getSingletonPtr()->SetMousePosition(mLastMouse + Ogre::Vector2(mHandle->getActualLeft(),mHandle->getActualTop()));
            return;
#endif
        }

        mLastButtons = 0;
        NameObjectPairList::const_iterator it = mHighLighted.begin();

        while(it != mHighLighted.end())
        {
            it->second->setHighlighted(false);
            it++;
        }
        mHighLighted.clear();

        mLastMouse = Ogre::Vector2(-1,-1);

        mIsEditing = false;

        if(mOgitorsRoot->GetTerrainEditor()) 
            mOgitorsRoot->GetTerrainEditor()->stopEdit();

        OgitorsSystem::getSingletonPtr()->ShowMouseCursor(true);

        mOgitorsRoot->HideSelectionRect();
        mVolumeSelecting = false;
    }
    //-------------------------------------------------------------------------------
    void CViewportEditor::OnMouseLeftDown (Ogre::Vector2 point, unsigned int buttons)
    {
        mMouseMovedSignal = false;
        mLastMouse = point;
        mLastClickPoint = point;
        mLastButtons = buttons;

        if(buttons & OMB_RIGHT) return;

        mIsEditing = false;

        if(GetEditorTool() == TOOL_SELECT)
            return;

        Ogre::Ray mouseRay;
        GetMouseRay(mLastMouse, mouseRay);

        CMultiSelEditor *multisel = mOgitorsRoot->GetSelection();

        bool pickTerrain = GetEditorTool() >= TOOL_DEFORM && GetEditorTool() <= TOOL_SMOOTH;

        ITerrainEditor *terED = mOgitorsRoot->GetTerrainEditor();
        if(pickTerrain &&  terED && terED->isSelected() && !mViewKeyboard[mSpecial.SPK_ALWAYS_SELECT])
        {
            terED->startEdit();
        }

        CBaseEditor *newsel = GetObjectUnderMouse(mouseRay, true, pickTerrain);

        if(!multisel->isEmpty() && multisel->containsOrEqual(newsel))
        {
            SaveEditorStates(mouseRay);

            if(mViewKeyboard[mSpecial.SPK_CLONE] && mViewKeyboard[mSpecial.SPK_ALWAYS_SELECT] && GetEditorTool() == TOOL_MOVE  && multisel->getAsSingle()->supports(CAN_CLONE))
            {
                mEditorToolEx = TOOL_EX_INSTANCECLONE;
            }
            else if(mViewKeyboard[mSpecial.SPK_CLONE] && GetEditorTool() == TOOL_MOVE  && multisel->getAsSingle()->supports(CAN_CLONE))
            {
                mEditorToolEx = TOOL_EX_CLONE;
            }

            mIsEditing = true;
            mFirstTimeTranslation = true;
        }
    }
    //-------------------------------------------------------------------------------
    void CViewportEditor::OnMouseLeftUp (Ogre::Vector2 point, unsigned int buttons)
    {
        mLastMouse = point;
        mLastClickPoint = point;
        mLastButtons = buttons;

        Ogre::Ray mouseRay;
        GetMouseRay(mLastMouse, mouseRay);

        ITerrainEditor *terED = mOgitorsRoot->GetTerrainEditor();

        if(terED) 
            terED->stopEdit();

        mEditorAxis = 0;

        mOgitorsRoot->HighlightGizmo(0);

        mOgitorsRoot->HideSelectionRect();

        if(mVolumeSelecting)
        {
            CMultiSelEditor *multisel = mOgitorsRoot->GetSelection();

            if(mViewKeyboard[mSpecial.SPK_ADD_TO_SELECTION]  && !mViewKeyboard[mSpecial.SPK_SUBTRACT_FROM_SELECTION])
            {
                if(multisel->getAsSingle() != multisel && mHighLighted.find(multisel->getAsSingle()->getName()) == mHighLighted.end())
                    mHighLighted.insert(NameObjectPairList::value_type(multisel->getAsSingle()->getName(), multisel->getAsSingle()));
                else
                {
                    NameObjectPairList theList = multisel->getSelection();
                    NameObjectPairList::const_iterator it = theList.begin();
                    while(it != theList.end())
                    {
                        if(mHighLighted.find(it->first) == mHighLighted.end())
                            mHighLighted.insert(NameObjectPairList::value_type(it->first, it->second));
                        it++;
                    }
                }
            }
            else if(!mViewKeyboard[mSpecial.SPK_ADD_TO_SELECTION]  && mViewKeyboard[mSpecial.SPK_SUBTRACT_FROM_SELECTION])
            {
                if(multisel->getAsSingle() != multisel)
                    mHighLighted.clear();
                else
                {
                    NameObjectPairList theList = multisel->getSelection();
                    NameObjectPairList::const_iterator it = mHighLighted.begin();
                    NameObjectPairList::iterator dit;
                    while(it != mHighLighted.end())
                    {
                        it->second->setHighlighted(false);
                        dit = theList.find(it->first);
                        if(dit != theList.end())
                            theList.erase(dit);
                        it++;
                    }
                    mHighLighted = theList;
                }
            }

            NameObjectPairList::const_iterator it = mHighLighted.begin();
            while(it != mHighLighted.end())
            {
                it->second->setHighlighted(false);
                it++;
            }

            multisel->setSelection(mHighLighted);

            if(GetEditorTool() == TOOL_MOVE && !multisel->isEmpty())
                mViewGrid->setTrackingObject(multisel->getAsSingle());
            else
                mViewGrid->setTrackingObject(0);

            mHighLighted.clear();

            mEditorAxis = 0;
            mVolumeSelecting = false;
        }
        else if(!mIsEditing)
        {
            if(mViewKeyboard[mSpecial.SPK_ALWAYS_SELECT] && terED && terED->getEditMode() > 0)
            {
                SetEditorTool(TOOL_SELECT);
            }
            if((GetEditorTool() >= TOOL_SELECT && GetEditorTool() <= TOOL_SCALE) || mViewKeyboard[mSpecial.SPK_ALWAYS_SELECT])
            { 
                DoSelect(mouseRay);
            }
        }
        mIsEditing = false;
    }
    //-------------------------------------------------------------------------------
    void CViewportEditor::OnMouseRightDown (Ogre::Vector2 point, unsigned int buttons)
    {
        mLastMouse = point;
        mLastButtons = buttons;
        if(!mIsEditing && buttons == OMB_RIGHT)
        {
            mMouseMovedSignal = false;
        }
    }
    //-------------------------------------------------------------------------------
    void CViewportEditor::OnMouseRightUp (Ogre::Vector2 point, unsigned int buttons)
    {
        mLastMouse = point;
        mLastButtons = buttons;
        mEditorToolEx = TOOL_EX_NONE;
        OgitorsSystem::getSingletonPtr()->SetMouseCursor(MOUSE_ARROW);
        OgitorsSystem::getSingletonPtr()->ShowMouseCursor(true);

        Ogre::Ray mouseRay;
        GetMouseRay(mLastMouse, mouseRay);

        if(!mIsEditing && !mMouseMovedSignal && buttons == OMB_RIGHT)
        {
            CMultiSelEditor *multisel = mOgitorsRoot->GetSelection();
            CBaseEditor *newselection = GetObjectUnderMouse(mouseRay, true, true);
            if(!multisel->contains(newselection))
            {
                multisel->setSelection(newselection);

                if(newselection && !newselection->isTerrainType() && GetEditorTool() == TOOL_MOVE)
                    mViewGrid->setTrackingObject(newselection);
                else
                    mViewGrid->setTrackingObject(0);
            }
        }
    }
    //-------------------------------------------------------------------------------
    void CViewportEditor::OnMouseMiddleDown (Ogre::Vector2 point, unsigned int buttons)
    {
        mLastMouse = point;
        mLastButtons = buttons;
    }
    //-------------------------------------------------------------------------------
    void CViewportEditor::OnMouseMiddleUp (Ogre::Vector2 point, unsigned int buttons)
    {
        mLastMouse = point;
        mLastButtons = buttons;
        OgitorsSystem::getSingletonPtr()->ShowMouseCursor(true);
    }
    //-------------------------------------------------------------------------------
    void CViewportEditor::OnMouseWheel (Ogre::Vector2 point, float delta, unsigned int buttons)
    {
        mLastMouse = point;
        mLastButtons = buttons;

        if(!mActiveCamera) return;
        Ogre::Vector3 vPos = mActiveCamera->getDerivedPosition();

        CMultiSelEditor *multisel = mOgitorsRoot->GetSelection();

        if(mViewKeyboard[mSpecial.SPK_OBJECT_CENTERED_MOVEMENT] && !multisel->isEmpty() && multisel->getAsSingle()->supports(CAN_FOCUS))
        {
            Ogre::Vector3 vObjPos = multisel->getAsSingle()->getDerivedPosition();
            mActiveCamera->lookAt(vObjPos);
            Ogre::Vector3 vDelta = vObjPos - vPos;
            float fDelta = delta / 400.0f * mCameraSpeed;
            if(delta > 0)
            {
                vDelta *= std::min(fDelta,0.8f);
            }
            else
                vDelta *= std::max(fDelta,-0.7f);

            vPos += vDelta;
            if((vPos - vObjPos).length() < 1.0f)
            {
                vDelta = vPos - vObjPos;
                vDelta.normalise();
                vPos = vObjPos + vDelta;
            }
        }
        else
        {
            Ogre::Vector3 vDelta = Ogre::Vector3(0,0,delta / 32.0f) * mCameraSpeed;
            vPos = vPos - (mActiveCamera->getDerivedOrientation() * vDelta);
        } 
        mNewCamPosition = vPos;
    }
    //-------------------------------------------------------------------------------
    bool CViewportEditor::GetMouseRay( Ogre::Vector2 point, Ogre::Ray &mRay )
    {
        if(!mActiveCamera) 
            return false;

        float width = mHandle->getActualWidth();
        float height = mHandle->getActualHeight();
        mRay = mActiveCamera->getCamera()->getCameraToViewportRay(point.x / width, point.y / height);

        return true;
    }
    //-------------------------------------------------------------------------------
    void CViewportEditor::SaveEditorStates(Ogre::Ray &mouseRay)
    {
        CBaseEditor *selected = mOgitorsRoot->GetSelection()->getAsSingle();

        if( selected )
        {
            if(selected->getProperties()->hasProperty("scale"))
                selected->getProperties()->getValue("scale",mLastScale);
            else
                mLastScale = Ogre::Vector3(1,1,1);

            if(selected->getProperties()->hasProperty("position"))
                selected->getProperties()->getValue("position",mLastPosition);
            else
                mLastPosition = Ogre::Vector3::ZERO;

            if(selected->getProperties()->hasProperty("orientation"))
                selected->getProperties()->getValue("orientation",mLastOrient);
            else
                mLastOrient = Ogre::Quaternion::IDENTITY;

            mLastDerivedOrient = selected->getDerivedOrientation();
            mLastDerivedPosition = selected->getDerivedPosition();
            mLastUsedPlane = mOgitorsRoot->FindGizmoTranslationPlane(mouseRay,mEditorAxis);
            mLast3DDelta = mOgitorsRoot->GetGizmoIntersect(mouseRay,mLastUsedPlane,mEditorAxis,mLastDerivedPosition);
        }
    }
    //-------------------------------------------------------------------------------
    void CViewportEditor::HighlightObjectAtPosition(Ogre::Ray &mouseRay)
    {
        CBaseEditor *selected = GetObjectUnderMouse(mouseRay, false, false);

        NameObjectPairList::const_iterator it = mHighLighted.begin();
        while(it != mHighLighted.end())
        {
            if(it->second != selected)
                it->second->setHighlighted(false);
            it++;
        }

        mHighLighted.clear();

        if(selected)
        {
            selected->setHighlighted(true);
            mHighLighted.insert(NameObjectPairList::value_type(selected->getName(),selected));
        }
    }
    //-------------------------------------------------------------------------------
    CBaseEditor* CViewportEditor::GetObjectUnderMouse(Ogre::Ray &mouseRay, bool pickwidgets, bool pickterrain)
    {
        Ogre::Entity *result;
        Ogre::Vector3 hitlocation;
        CBaseEditor *selected = 0;

        if(pickwidgets && mOgitorsRoot->PickGizmos(mouseRay, mEditorAxis))    
        {
            selected = mOgitorsRoot->GetSelection()->getAsSingle();
        }
        else
        {
            if(OgitorsUtils::PickEntity(mouseRay,&result,hitlocation)&&result->getName() != "HydraxMeshEnt")    
            {
                Ogre::String sName = result->getName();
                selected = mOgitorsRoot->FindObject(sName);
            }

            if(pickterrain && !selected && mOgitorsRoot->GetTerrainEditor() && 
                mOgitorsRoot->GetTerrainEditor()->hitTest(mouseRay))
            {
                /* Set default selection to the TerrainGroup unless we find the page */
                selected = mOgitorsRoot->GetTerrainEditorObject(); 

                Ogre::String name = mOgitorsRoot->GetTerrainEditorObject()->getTypeName();

                /* Only attempt to select the page if it's a Terrain Group Object type */
                if(name == "Terrain Group") 
                {
                    CTerrainGroupEditor* terrainGroupEditor = (CTerrainGroupEditor*)selected;
                    Ogre::TerrainGroup* handle = (Ogre::TerrainGroup*)terrainGroupEditor->getHandle();

                    /* Calculate the page we intersect */
                    Ogre::TerrainGroup::RayResult rayresult = handle->rayIntersects(mouseRay);
                    if(rayresult.hit) 
                    {
                        long x, y;
                        handle->convertWorldPositionToTerrainSlot(rayresult.position, &x, &y);

                        /* Loop over all pages and find the one matching the X and Y value retrieved before */
                        CTerrainPageEditor* terrainPageEditor;
                        Ogitors::NameObjectPairList &list = terrainGroupEditor->getChildren();
                        Ogitors::NameObjectPairList::iterator it = list.begin();
                        while(it != list.end()) 
                        {
                            terrainPageEditor = (CTerrainPageEditor*)it->second;

                            if(terrainPageEditor->getPageX() == x && terrainPageEditor->getPageY() == y)  
                            {
                                selected = it->second;
                                break;
                            }                        
                            else
                                it++;
                        }
                    }
                }
            }
        }

        return selected;
    }
    //-------------------------------------------------------------------------------
    void CViewportEditor::DoSelect(Ogre::Ray &mouseRay)
    {
        CMultiSelEditor *multisel = mOgitorsRoot->GetSelection();
        CBaseEditor *newselection = GetObjectUnderMouse(mouseRay, true, true);

        bool valid = false;

        if(GetEditorTool() == TOOL_SELECT || !mMouseMovedSignal)
            valid = newselection && (multisel->getAsSingle() != newselection);
        else
            valid = newselection && !multisel->containsOrEqual(newselection);

        if(valid)
        {
            if(mViewKeyboard[mSpecial.SPK_ADD_TO_SELECTION]  && !mViewKeyboard[mSpecial.SPK_SUBTRACT_FROM_SELECTION])
            {
                mHighLighted.clear();
                mHighLighted.insert(NameObjectPairList::value_type(newselection->getName(), newselection));
                if(multisel->getAsSingle() != multisel)
                {
                    mHighLighted.insert(NameObjectPairList::value_type(multisel->getAsSingle()->getName(), multisel->getAsSingle()));
                }
                else
                {
                    NameObjectPairList theList = multisel->getSelection();
                    NameObjectPairList::const_iterator it = theList.begin();
                    while(it != theList.end())
                    {
                        if(mHighLighted.find(it->first) == mHighLighted.end())
                            mHighLighted.insert(NameObjectPairList::value_type(it->first, it->second));
                        it++;
                    }
                }

                NameObjectPairList::const_iterator it = mHighLighted.begin();
                while(it != mHighLighted.end())
                {
                    it->second->setHighlighted(false);
                    it++;
                }

                multisel->setSelection(mHighLighted);

                if(GetEditorTool() == TOOL_MOVE && !multisel->isEmpty())
                    mViewGrid->setTrackingObject(multisel->getAsSingle());
                else
                    mViewGrid->setTrackingObject(0);

                mHighLighted.clear();
                mEditorAxis = 0;

                return;
            }
            else if(!mViewKeyboard[mSpecial.SPK_ADD_TO_SELECTION]  && mViewKeyboard[mSpecial.SPK_SUBTRACT_FROM_SELECTION])
            {
                mHighLighted.clear();
                if(multisel->getAsSingle() != multisel)
                {
                    mHighLighted.insert(NameObjectPairList::value_type(multisel->getAsSingle()->getName(), multisel->getAsSingle()));
                }
                else
                {
                    NameObjectPairList theList = multisel->getSelection();
                    NameObjectPairList::iterator dit;

                    dit = theList.find(newselection->getName());
                    if(dit != theList.end())
                        theList.erase(dit);

                    mHighLighted = theList;
                }

                NameObjectPairList::const_iterator it = mHighLighted.begin();
                while(it != mHighLighted.end())
                {
                    it->second->setHighlighted(false);
                    it++;
                }

                multisel->setSelection(mHighLighted);

                if(GetEditorTool() == TOOL_MOVE && !multisel->isEmpty())
                    mViewGrid->setTrackingObject(multisel->getAsSingle());
                else
                    mViewGrid->setTrackingObject(0);


                mHighLighted.clear();
                mEditorAxis = 0;

                return;
            }

            multisel->setSelection(newselection);

            if(!newselection->isTerrainType() && GetEditorTool() == TOOL_MOVE)
                mViewGrid->setTrackingObject(newselection);
            else
                mViewGrid->setTrackingObject(0);

            mEditorAxis = 0;

            if(mViewKeyboard[mSpecial.SPK_ALWAYS_SELECT]) 
                FocusSelectedObject();    
        }
        else if(!multisel->isEmpty() && !newselection) 
            multisel->setSelection(0);
    }
    //-------------------------------------------------------------------------------
    void CViewportEditor::CloneMove(bool instance)
    {
        mEditorToolEx = TOOL_EX_NONE;
        if(instance)
            mOgitorsRoot->InstanceCloneEditorObject(mOgitorsRoot->GetSelection()->getAsSingle(),true,true);
        else
            mOgitorsRoot->CloneEditorObject(mOgitorsRoot->GetSelection()->getAsSingle(),true,true);

        mViewGrid->setTrackingObject(mOgitorsRoot->GetSelection()->getAsSingle());
    }
    //-------------------------------------------------------------------------------
    static Ogre::Vector3 oldCamPos = Ogre::Vector3::ZERO;

    void CViewportEditor::UpdateAutoCameraPosition(float time)
    {
        if(!mActiveCamera)
            return;
        if(mNewCamPosition != Ogre::Vector3::ZERO)
        {
            Ogre::Vector3 curpos = mActiveCamera->getDerivedPosition();
            Ogre::Vector3 diff = mNewCamPosition - curpos;

            if(diff.length() > 0.03f)
            {
                curpos += diff * std::min(time * 3.0f, 1.0f);
            }
            else
            {
                mNewCamPosition = Ogre::Vector3::ZERO;
            }

            mActiveCamera->setDerivedPosition(curpos);

            if(!mVolumeSelecting)
            {
                Ogre::Ray mouseRay;
                GetMouseRay(mLastMouse, mouseRay);
                HighlightObjectAtPosition(mouseRay);
            }

            OnMouseMove(mLastMouse, mLastButtons, true);
        }
    }
    //-------------------------------------------------------------------------------
    void CViewportEditor::FocusSelectedObject()
    {
        if(mOgitorsRoot->GetSelection()->isEmpty() || mOgitorsRoot->GetWalkAroundMode()) 
            return;

        if(mOgitorsRoot->GetSelection()->getAsSingle()->supports(CAN_FOCUS))
        {
            CBaseEditor *focus_object = mOgitorsRoot->GetSelection()->getAsSingle();

            if( focus_object->getEditorType() == ETYPE_TERRAIN_PAGE )
            {
                Ogre::Vector3 pos;

                Ogre::Terrain *ter = static_cast<Ogre::Terrain*>(focus_object->getHandle());
                pos = ter->getWorldAABB().getCenter();
                pos.y = ter->getHeightAtTerrainPosition( 0.5f, 0.5f ) + 50.0f;
                
                Ogre::Vector3 lookat = (mActiveCamera->getCamera()->getDerivedDirection() * 5.0f);
                mNewCamPosition = pos - lookat;
            }
            else
            {
                Ogre::AxisAlignedBox bbox = focus_object->getWorldAABB();

                if(bbox == Ogre::AxisAlignedBox::BOX_NULL)
                    return;

                Ogre::Vector3 vSize = bbox.getHalfSize();
                Ogre::Vector3 vCenter = bbox.getCenter(); 

                vSize += Ogre::Vector3(vSize.z, vSize.z, vSize.z);

                float dist = std::max(std::max(vSize.x,vSize.y),vSize.z) * 2.0f;

                Ogre::Vector3 lookat = (mActiveCamera->getCamera()->getDerivedDirection() * dist);
                mNewCamPosition = vCenter - lookat;
            }
        }
    }
    //-------------------------------------------------------------------------------
    void CViewportEditor::DeleteSelectedObject(bool silent, bool withundo)
    {
        if(mOgitorsRoot->GetSelection()->isEmpty())
            return;

        CBaseEditor *selected = mOgitorsRoot->GetSelection()->getAsSingle();

        if(selected->supports(CAN_DELETE))
        {
            bool cont = false;
            if(!silent)
            {
                Ogre::UTFString strWarn;
                Ogre::String strName = selected->getName();
                if(selected->isNodeType())
                    strWarn = OTR("Are you sure you want to delete %s and all of its children?");
                else if(selected->getEditorType() == ETYPE_MULTISEL)
                    strWarn = OTR("Are you sure you want to delete all selected objects?");
                else
                    strWarn = OTR("Are you sure you want to delete %s?");

                int pos = strWarn.find("%s");

                if(pos != -1)
                {
                    strWarn.erase(pos,2);
                    strWarn.insert(pos, strName);
                }
                if(OgitorsSystem::getSingletonPtr()->DisplayMessageDialog(strWarn, DLGTYPE_YESNO) == DLGRET_YES)
                    cont = true;
            }
            else
                cont = true;

            if(cont)
            {
                CBaseEditor *delED = selected;

                if(delED->isTerrainType()) 
                {
                    if(GetEditorTool() >= TOOL_DEFORM)
                        SetEditorTool(TOOL_SELECT);
                }

                mViewGrid->setTrackingObject(0);

                if(delED->getEditorType() == ETYPE_MULTISEL)
                {
                    CMultiSelEditor *msed = static_cast<CMultiSelEditor*>(delED);
                    msed->deleteObjects();
                }
                else
                    mOgitorsRoot->DestroyEditorObject(delED, true, true);

                mEditorAxis = 0;
            } 
        }
    }
    //-------------------------------------------------------------------------------
    void CViewportEditor::ProcessKeyActions(unsigned int timesince)
    {
        if(!mViewKeyboard)
            return;
        double timeMod = (double)timesince / 100.0f * mCameraSpeed;

        if(mActiveCamera)
        {
            Ogre::Vector3 vPos = mActiveCamera->getPosition();

            if(mViewKeyboard[mSpecial.SPK_FORWARD])
            {
                if(mNewCamPosition == Ogre::Vector3::ZERO)
                    mNewCamPosition = mActiveCamera->getDerivedPosition();

                if(mOgitorsRoot->GetWalkAroundMode())
                {
                    Ogre::Vector3 dir = mActiveCamera->getCamera()->getDerivedDirection();
                    dir.y = 0.0f;
                    mNewCamPosition += dir.normalisedCopy() * timeMod;
                }
                else
                    mNewCamPosition += mActiveCamera->getCamera()->getDerivedDirection() * timeMod;
            }
            if(mViewKeyboard[mSpecial.SPK_BACKWARD])
            {
                if(mNewCamPosition == Ogre::Vector3::ZERO)
                    mNewCamPosition = mActiveCamera->getDerivedPosition();

                if(mOgitorsRoot->GetWalkAroundMode())
                {
                    Ogre::Vector3 dir = mActiveCamera->getCamera()->getDerivedDirection();
                    dir.y = 0.0f;
                    mNewCamPosition -= dir.normalisedCopy() * timeMod;
                }
                else
                    mNewCamPosition -= mActiveCamera->getCamera()->getDerivedDirection() * timeMod;
            }
            if(mViewKeyboard[mSpecial.SPK_LEFT])
            {
                if(mOgitorsRoot->GetWalkAroundMode())
                {
                    Ogre::Quaternion orient = Ogre::Quaternion(Ogre::Degree((float)timesince / 10.0f),Ogre::Vector3(0,1,0)) * mActiveCamera->getOrientation();
                    mActiveCamera->setOrientation(orient);

                    Ogre::Ray mouseRay;
                    if(GetMouseRay(mLastMouse, mouseRay))
                    {
                        if(!mOgitorsRoot->GetSelection()->isEmpty())
                            mLastUsedPlane = mOgitorsRoot->FindGizmoTranslationPlane(mouseRay,mEditorAxis);
                    }
                }
                else
                {
                    if(mNewCamPosition == Ogre::Vector3::ZERO)
                        mNewCamPosition = mActiveCamera->getDerivedPosition();
                    mNewCamPosition -= mActiveCamera->getCamera()->getDerivedRight() * timeMod;
                }
            }
            if(mViewKeyboard[mSpecial.SPK_RIGHT])
            {
                if(mOgitorsRoot->GetWalkAroundMode())
                {
                    Ogre::Quaternion orient = Ogre::Quaternion(Ogre::Degree((float)timesince / -10.0f),Ogre::Vector3(0,1,0)) * mActiveCamera->getOrientation();
                    mActiveCamera->setOrientation(orient);

                    Ogre::Ray mouseRay;
                    if(GetMouseRay(mLastMouse, mouseRay))
                    {
                        if(!mOgitorsRoot->GetSelection()->isEmpty())
                            mLastUsedPlane = mOgitorsRoot->FindGizmoTranslationPlane(mouseRay,mEditorAxis);
                    }
                }
                else
                {
                    if(mNewCamPosition == Ogre::Vector3::ZERO)
                        mNewCamPosition = mActiveCamera->getDerivedPosition();
                    mNewCamPosition += mActiveCamera->getCamera()->getDerivedRight() * timeMod;
                }
            }
            if(mViewKeyboard[mSpecial.SPK_UP])
            {
                if(mOgitorsRoot->GetWalkAroundMode())
                {
                    mOgitorsRoot->GetProjectOptions()->WalkAroundHeight += timeMod;
                }
                else
                {
                    if(mNewCamPosition == Ogre::Vector3::ZERO)
                        mNewCamPosition = mActiveCamera->getDerivedPosition();
                    mNewCamPosition += Ogre::Vector3(0,1,0) * timeMod;
                }
            }
            if(mViewKeyboard[mSpecial.SPK_DOWN])
            {
                if(mOgitorsRoot->GetWalkAroundMode())
                {
                    mOgitorsRoot->GetProjectOptions()->WalkAroundHeight = std::max(mOgitorsRoot->GetProjectOptions()->WalkAroundHeight - (float)timeMod, 0.0f);
                }
                else
                {
                    if(mNewCamPosition == Ogre::Vector3::ZERO)
                        mNewCamPosition = mActiveCamera->getDerivedPosition();
                    mNewCamPosition -= Ogre::Vector3(0,1,0) * timeMod;
                }
            }

            if(mOgitorsRoot->GetWalkAroundMode())
            {
                if(mOgitorsRoot->GetTerrainEditor())
                {
                    if(mNewCamPosition == Ogre::Vector3::ZERO)
                        mNewCamPosition = mActiveCamera->getDerivedPosition();

                    Ogitors::PGHeightFunction *func = mOgitorsRoot->GetTerrainEditor()->getHeightFunction();
                    mNewCamPosition.y = mOgitorsRoot->GetProjectOptions()->WalkAroundHeight + (*func)(mNewCamPosition.x, mNewCamPosition.z, 0);
                }
            }

        } // if get camera
    }
    //-------------------------------------------------------------------------------
    void CViewportEditor::LoadEditorObjects()
    {
        mViewGrid = OGRE_NEW ViewportGrid(mOgitorsRoot->GetSceneManager(), mHandle);
        mViewGrid->setRenderLayer(ViewportGrid::RL_INFRONT);
        mViewGrid->setColour(Ogre::ColourValue(0.8f,0.8f,0.8f,1));
        mViewGrid->setPerspectiveSize(20);
        mViewGrid->setDivision(2);
        mViewGrid->setEnabled(true);
    }

    void CViewportEditor::DoVolumeSelect()
    {
        if(!mActiveCamera)
            return;

        Ogre::Real left = std::min(mLastMouse.x,mLastClickPoint.x) + 1;
        Ogre::Real right = std::max(mLastMouse.x,mLastClickPoint.x) - 1;
        Ogre::Real top = std::min(mLastMouse.y,mLastClickPoint.y) + 1;
        Ogre::Real bottom = std::max(mLastMouse.y,mLastClickPoint.y) - 1;

        float width = mHandle->getActualWidth();
        float height = mHandle->getActualHeight();

        if((right - left) > 2 && (bottom - top) > 2)
        {
            NameObjectPairList SelectionResult;

            mOgitorsRoot->VolumeSelect(mActiveCamera->getCamera(), left / width, right / width, top / height, bottom / height, SelectionResult);

            NameObjectPairList::const_iterator it = mHighLighted.begin();
            while(it != mHighLighted.end())
            {
                NameObjectPairList::const_iterator ith = SelectionResult.find(it->first);
                if(ith == SelectionResult.end())
                {
                    it->second->setHighlighted(false);
                }
                it++;
            }

            it = SelectionResult.begin();
            while(it != SelectionResult.end())
            {
                NameObjectPairList::const_iterator ith = mHighLighted.find(it->first);
                if(ith == mHighLighted.end())
                {
                    it->second->setHighlighted(true);
                }
                it++;
            }

            mHighLighted.clear();
            mHighLighted = SelectionResult;
        }
    }
    //-------------------------------------------------------------------------------
    void CViewportEditor::PrepareTranslationUndo()
    {
        CBaseEditor *selected = mOgitorsRoot->GetSelection()->getAsSingle();
        if(!selected)
            return;

        switch(GetEditorTool())
        {
        case TOOL_MOVE:OgitorsUndoManager::getSingletonPtr()->BeginCollection(selected->getName() + " Move");break;
        case TOOL_ROTATE:OgitorsUndoManager::getSingletonPtr()->BeginCollection(selected->getName() + " Rotate");break;
        case TOOL_SCALE:OgitorsUndoManager::getSingletonPtr()->BeginCollection(selected->getName() + " Scale");break;
        };
    }
    //-------------------------------------------------------------------------------
    bool CViewportEditor::GetHitPosition(Ogre::Ray &mouseRay, Ogre::Vector3& position, const Ogre::String& excludeobject)
    {
        Ogre::Vector3 vFinalPosition = Ogre::Vector3(-999999,-999999,-999999);
        float distance = -1.0f;
        bool hitfound = false;

        Ogre::Vector3 vPos;
        if(mOgitorsRoot->GetTerrainEditor() && mOgitorsRoot->GetTerrainEditor()->hitTest(mouseRay,&vPos))
        {
            distance = (vPos - mActiveCamera->getPosition()).length();
            vFinalPosition = vPos;
            hitfound = true;
        }

        Ogre::Entity *result;
        if(OgitorsUtils::PickEntity(mouseRay,&result,vPos, excludeobject, distance))    
        {
            vFinalPosition = vPos;
            hitfound = true;
        }

        if(hitfound)
        {
            position = vFinalPosition;
            return true;
        }
        return false;
    }
    //-------------------------------------------------------------------------------
    bool CViewportEditor::GetHitPosition(Ogre::Ray &mouseRay, Ogre::Vector3& position, const Ogre::StringVector& excludeobjects)
    {
        Ogre::Vector3 vFinalPosition = Ogre::Vector3(-999999,-999999,-999999);
        float distance = -1.0f;
        bool hitfound = false;

        Ogre::Vector3 vPos;
        if(mOgitorsRoot->GetTerrainEditor() && mOgitorsRoot->GetTerrainEditor()->hitTest(mouseRay,&vPos))
        {
            distance = (vPos - mActiveCamera->getPosition()).length();
            vFinalPosition = vPos;
            hitfound = true;
        }

        Ogre::Entity *result;
        if(OgitorsUtils::PickEntity(mouseRay,&result,vPos, excludeobjects, distance))    
        {
            vFinalPosition = vPos;
            hitfound = true;
        }

        if(hitfound)
        {
            position = vFinalPosition;
            return true;
        }
        return false;
    }
    //-------------------------------------------------------------------------------
    void CViewportEditor::OnObjectDestroyed(CBaseEditor *object)
    {
        NameObjectPairList::iterator it = mHighLighted.find(object->getName());
        if(it != mHighLighted.end())
            mHighLighted.erase(it);
    }
    //-------------------------------------------------------------------------------
}

