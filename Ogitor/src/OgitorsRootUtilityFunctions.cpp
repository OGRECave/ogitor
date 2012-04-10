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
#include "OgitorsRoot.h"
#include "OgitorsSystem.h"
#include "BaseSerializer.h"
#include "BaseEditor.h"
#include "VisualHelper.h"
#include "OFSSceneSerializer.h"
#include "SceneManagerEditor.h"
#include "ViewportEditor.h"
#include "NodeEditor.h"
#include "EntityEditor.h"
#include "CameraEditor.h"
#include "MultiSelEditor.h"
#include "TerrainEditor.h"
#include "AxisGizmo.h"
#include "PGInstanceManager.h"

#include "ofs.h"

using namespace Ogre;
using namespace Ogitors;

//-----------------------------------------------------------------------------------------
Ogre::String OgitorsRoot::CreateUniqueID(Ogre::String n1, Ogre::String n2, int seed)
{
    int temp = seed;

    if(seed < 0)
        seed = 0;

    Ogre::String tail =  "#" + Ogre::StringConverter::toString(seed);
    if(n2 != "")
    {
        while(FindObject(n1 + tail) || FindObject(n2 + tail))
        {
            tail = "#" + Ogre::StringConverter::toString(++seed);
        }
    }
    else
    {
        while(FindObject(n1 + tail))
        {
            tail = "#" +  Ogre::StringConverter::toString(++seed);
        }
    }
    
    return tail;
}
//-----------------------------------------------------------------------------------------
bool OgitorsRoot::PickGizmos(Ogre::Ray &ray, int &Axis)
{
    if(!mGizmoEntities[0] || !mGizmoEntities[0]->isVisible())
        return false;

    Ogre::Real closest_distance = -1.0f;
    Ogre::Vector3 closest_result;

    for(int widx = 0;widx < 6;widx++)
    {
        // get the entity to check
        Ogre::Entity *pentity = mGizmoEntities[widx];

        std::pair<bool, Ogre::Real> hit = Ogre::Math::intersects(ray, pentity->getWorldBoundingBox());
        if(!hit.first)
            continue;

        size_t vertex_count;
        size_t index_count;
        Ogre::Vector3 *vertices;
        unsigned long *indices;

        // get the mesh information
        OgitorsUtils::GetMeshDataEx(pentity, vertex_count, index_count, 
                                  pentity->getParentNode()->_getDerivedPosition(),
                                  pentity->getParentNode()->_getDerivedOrientation(),
                                  pentity->getParentNode()->_getDerivedScale());

        OgitorsUtils::GetBuffers(&vertices, &indices);
        // test for hitting individual triangles on the mesh
        bool new_closest_found = false;
        for (int i = 0; i < static_cast<int>(index_count); i += 3)
        {
            // check for a hit against this triangle
            hit = Ogre::Math::intersects(ray, vertices[indices[i]],
            vertices[indices[i+1]], vertices[indices[i+2]], true, true);
            // if it was a hit check if its the closest
            if (hit.first)
            {
                if ((closest_distance < 0.0f) || (hit.second < closest_distance))
                {
                    // this is the closest so far, save it off
                    closest_distance = hit.second;
                    new_closest_found = true;
                }
            }
        }

        // if we found a new closest raycast for this object, update the
        // closest_result before moving on to the next object.
        if (new_closest_found)
        {
            closest_result = ray.getPoint(closest_distance);
            switch(widx)
            {
            case 0:Axis = AXIS_X;break;
            case 1:Axis = AXIS_Y;break;
            case 2:Axis = AXIS_Z;break;
            case 3:Axis = AXIS_XY;break;
            case 4:Axis = AXIS_YZ;break;
            case 5:Axis = AXIS_XZ;break;
            }
        }
    }

    return (closest_distance >= 0.0f);
}
//-----------------------------------------------------------------------------------------
Plane OgitorsRoot::FindGizmoTranslationPlane(Ogre::Ray &pickRay, int TranslationAxis)
{
    Quaternion qOrient = mMultiSelection->getAsSingle()->getDerivedOrientation();
    
    if(mWorldSpaceGizmoOrientation)
        qOrient = Ogre::Quaternion::IDENTITY;

    Vector3 vPos = mMultiSelection->getAsSingle()->getDerivedPosition();
    Vector3 vCamBack = GetViewport()->getCameraEditor()->getCamera()->getDerivedDirection();
    vCamBack = -vCamBack;
    if(!TranslationAxis) return Plane(vCamBack,vPos);
    //if(!TranslationAxis) return Plane(qOrient.yAxis(), vPos);

    Plane planeX(qOrient.xAxis(), vPos);
    Plane planeY(qOrient.yAxis(), vPos);
    Plane planeZ(qOrient.zAxis(), vPos);

    float vX = planeX.projectVector(pickRay.getDirection()).length();
    float vY = planeY.projectVector(pickRay.getDirection()).length();
    float vZ = planeZ.projectVector(pickRay.getDirection()).length();

    if(TranslationAxis & AXIS_X) vX = 10000.0f;
    if(TranslationAxis & AXIS_Y) vY = 10000.0f;
    if(TranslationAxis & AXIS_Z) vZ = 10000.0f;

    if (vX < vY && vX < vZ)
        return planeX;
    else
    {
        if (vY < vX && vY < vZ )
            return planeY;
        else
            return planeZ;
    }
}
//--------------------------------------------------------------------------------
Vector3 OgitorsRoot::GetGizmoIntersect(Ogre::Ray &pickRay, Ogre::Plane &planetouse, int TranslationAxis, Ogre::Vector3 &vLastPosition)
{
    std::pair<bool,Ogre::Real> result;

    result = pickRay.intersects(planetouse);

    if (result.first)
    {
        Vector3 AxisX = Vector3::ZERO;
        Vector3 AxisY = Vector3::ZERO;
        Vector3 AxisZ = Vector3::ZERO;

        Quaternion qOrient = mMultiSelection->getAsSingle()->getDerivedOrientation();
    
        if(mWorldSpaceGizmoOrientation)
             qOrient = Ogre::Quaternion::IDENTITY;

        if(TranslationAxis)
        {
            if(TranslationAxis & AXIS_X) AxisX = qOrient.xAxis();
            if(TranslationAxis & AXIS_Y) AxisY = qOrient.yAxis();
            if(TranslationAxis & AXIS_Z) AxisZ = qOrient.zAxis();
        }
        else
        {
            AxisX = qOrient.xAxis();
            AxisY = qOrient.yAxis();
            AxisZ = qOrient.zAxis();
        }

        Vector3 Proj = pickRay.getPoint(result.second) - vLastPosition;
        Vector3 vPos1 = (AxisX.dotProduct(Proj) * AxisX);
        Vector3 vPos2 = (AxisY.dotProduct(Proj) * AxisY);
        Vector3 vPos3 = (AxisZ.dotProduct(Proj) * AxisZ);
        Vector3 vPos = vPos1 + vPos2 + vPos3;

        mLastTranslationDelta = vPos;
        return vPos;
    }
    return mLastTranslationDelta;
}
//-----------------------------------------------------------------------------------------
Vector3 OgitorsRoot::GetGizmoIntersectCameraPlane(CBaseEditor *object, Ogre::Ray &pickRay)
{
    std::pair<bool,Ogre::Real> result;

    Ogre::Vector3 vPos = object->getDerivedPosition();
    result = pickRay.intersects(Ogre::Plane(-(mActiveViewport->getCameraEditor()->getCamera()->getDerivedDirection()), vPos));

    if (result.first)
    {
        Quaternion qOrient = object->getDerivedOrientation();
    
        if(mWorldSpaceGizmoOrientation)
             qOrient = Ogre::Quaternion::IDENTITY;

        Vector3 AxisX = qOrient.xAxis();
        Vector3 AxisY = qOrient.yAxis();
        Vector3 AxisZ = qOrient.zAxis();

        Vector3 Proj = pickRay.getPoint(result.second) - vPos;
        Vector3 vPos1 = (AxisX.dotProduct(Proj) * AxisX);
        Vector3 vPos2 = (AxisY.dotProduct(Proj) * AxisY);
        Vector3 vPos3 = (AxisZ.dotProduct(Proj) * AxisZ);
        vPos += vPos1 + vPos2 + vPos3;
    }

    return vPos;
}
//-----------------------------------------------------------------------------------------
Vector3 OgitorsRoot::GetGizmoIntersectCameraPlane(Ogre::Vector3& pos, Ogre::Quaternion& orient, Ogre::Ray &pickRay)
{
    std::pair<bool,Ogre::Real> result;

    Ogre::Vector3 vPos = pos;
    result = pickRay.intersects(Ogre::Plane(-(mActiveViewport->getCameraEditor()->getCamera()->getDerivedDirection()), vPos));

    if (result.first)
    {
        Vector3 AxisX = orient.xAxis();
        Vector3 AxisY = orient.yAxis();
        Vector3 AxisZ = orient.zAxis();

        Vector3 Proj = pickRay.getPoint(result.second) - vPos;
        Vector3 vPos1 = (AxisX.dotProduct(Proj) * AxisX);
        Vector3 vPos2 = (AxisY.dotProduct(Proj) * AxisY);
        Vector3 vPos3 = (AxisZ.dotProduct(Proj) * AxisZ);
        vPos += vPos1 + vPos2 + vPos3;
    }

    return vPos;
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::FillResourceGroup(Ogre::ResourceGroupManager *mngr, Ogre::StringVector &list, Ogre::String path, Ogre::String group)
{
    Ogre::String strTemp;
    unsigned int itemcount;
    Ogre::String stype;

    itemcount = list.size();

    Ogre::String ofspath = path + "::";

    for(unsigned int i = 0;i < itemcount;i++)
    {
        strTemp = list[i];
        mngr->addResourceLocation(ofspath + strTemp, "Ofs", group);
    }
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::WriteCameraPositions(std::ostream &outstream, const PROJECTOPTIONS *pOpt)
{
    char savestr[500];
    int i;
    
    sprintf_s(savestr,500,"  <CAMERASPEED value=\"%s\"></CAMERASPEED>\n", Ogre::StringConverter::toString(pOpt->CameraSpeed).c_str());
    outstream << savestr;
    sprintf_s(savestr,500,"  <CAMERAPOSITIONS count=\"%d\">\n", pOpt->CameraSaveCount);
    outstream << savestr;
    for(i =0;i < pOpt->CameraSaveCount;i++)
    {
        sprintf_s(savestr,500,"    <OPTION id=\"%d\" position=\"%s\" orientation=\"%s\"></OPTION>\n",i,Ogre::StringConverter::toString(pOpt->CameraPositions[i]).c_str(),Ogre::StringConverter::toString(pOpt->CameraOrientations[i]).c_str());
        outstream << savestr;
    }
    if(!pOpt->IsNewProject)
    {
        Ogre::Vector3 vCamPos = GetViewport()->getCameraEditor()->getDerivedPosition();
        Ogre::Quaternion qCamOrient = GetViewport()->getCameraEditor()->getDerivedOrientation();
        sprintf_s(savestr,500,"    <OPTION id=\"%d\" position=\"%s\" orientation=\"%s\"></OPTION>\n",i,Ogre::StringConverter::toString(vCamPos).c_str(),Ogre::StringConverter::toString(qCamOrient).c_str());
        outstream << savestr;
    }
    else
    {
        sprintf_s(savestr,500,"    <OPTION id=\"%d\" position=\"5 10 5\" orientation=\"0.937683 -0.155887 -0.306458 -0.050973\"></OPTION>\n",i);
        outstream << savestr;
    }
    outstream << "  </CAMERAPOSITIONS>\n";
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::WriteProjectOptions(std::ostream &outstream, const PROJECTOPTIONS *pOpt)
{
    char buffer[5000];
    unsigned int i;

    outstream << "  <PROJECT>\n";
    sprintf_s(buffer,5000,"    <SCENEMANAGER value=\"%s\"></SCENEMANAGER>\n",pOpt->SceneManagerName.c_str());
    outstream << buffer;
    sprintf_s(buffer,5000,"    <TERRAINDIR value=\"%s\"></TERRAINDIR>\n",pOpt->TerrainDirectory.c_str());
    outstream << buffer;
    sprintf_s(buffer,5000,"    <HYDRAXDIR value=\"%s\"></HYDRAXDIR>\n",pOpt->HydraxDirectory.c_str());
    outstream << buffer;
    sprintf_s(buffer,5000,"    <CAELUMDIR value=\"%s\"></CAELUMDIR>\n",pOpt->CaelumDirectory.c_str());
    outstream << buffer;
    sprintf_s(buffer,5000,"    <PAGEDGEOMETRYDIR value=\"%s\"></PAGEDGEOMETRYDIR>\n",pOpt->PagedGeometryDirectory.c_str());
    outstream << buffer;
    outstream << "    <RESOURCEDIRECTORIES>\n";
    Ogre::String strtype;
    Ogre::String value;
    for(i = 0;i < pOpt->ResourceDirectories.size();i++)
    {
        value = pOpt->ResourceDirectories[i];
        sprintf_s(buffer,5000,"      <DIRECTORY type=\"Ofs\" value=\"%s\"></DIRECTORY>\n", value.c_str());
        outstream << buffer;
    }
    outstream << "    </RESOURCEDIRECTORIES>\n";

    WriteCameraPositions(outstream, pOpt);

    outstream << "    <LAYERS>\n";
    for(i = 0;i < 31;i++)
    {
        sprintf_s(buffer,5000,"      <LAYER id=\"%d\" name=\"%s\" visible=\"%s\"></LAYER>\n", i,pOpt->LayerNames[i].c_str(), Ogre::StringConverter::toString(pOpt->LayerVisible[i]).c_str());
        outstream << buffer;
    }
    outstream << "    </LAYERS>\n";

    sprintf_s(buffer,5000,"  <LAYERCOUNT value=\"%s\"></LAYERCOUNT>\n",Ogre::StringConverter::toString(pOpt->LayerCount).c_str());
    outstream << buffer;
    sprintf_s(buffer,5000,"  <SELECTIONRECTCOLOUR value=\"%s\"></SELECTIONRECTCOLOUR>\n",Ogre::StringConverter::toString(pOpt->SelectionRectColour).c_str());
    outstream << buffer;
    sprintf_s(buffer,5000,"  <SELECTIONCOLOUR value=\"%s\"></SELECTIONCOLOUR>\n",Ogre::StringConverter::toString(pOpt->SelectionBBColour).c_str());
    outstream << buffer;
    sprintf_s(buffer,5000,"  <HIGHLIGHTCOLOUR value=\"%s\"></HIGHLIGHTCOLOUR>\n",Ogre::StringConverter::toString(pOpt->HighlightBBColour).c_str());
    outstream << buffer;
    sprintf_s(buffer,5000,"  <SELECTHIGHLIGHTCOLOUR value=\"%s\"></SELECTHIGHLIGHTCOLOUR>\n",Ogre::StringConverter::toString(pOpt->SelectHighlightBBColour).c_str());
    outstream << buffer;
    sprintf_s(buffer,5000,"  <GRIDCOLOUR value=\"%s\"></GRIDCOLOUR>\n",Ogre::StringConverter::toString(pOpt->GridColour).c_str());
    outstream << buffer;
    sprintf_s(buffer,5000,"  <GRIDSPACING value=\"%s\"></GRIDSPACING>\n",Ogre::StringConverter::toString(pOpt->GridSpacing).c_str());
    outstream << buffer;
    sprintf_s(buffer,5000,"  <SNAPANGLE value=\"%s\"></SNAPANGLE>\n",Ogre::StringConverter::toString(pOpt->SnapAngle).c_str());
    outstream << buffer;
    sprintf_s(buffer,5000,"  <WALKAROUNDHEIGHT value=\"%s\"></WALKAROUNDHEIGHT>\n",Ogre::StringConverter::toString(pOpt->WalkAroundHeight).c_str());
    outstream << buffer;
    sprintf_s(buffer,5000,"  <VOLUMESELECTIONDEPTH value=\"%s\"></VOLUMESELECTIONDEPTH>\n",Ogre::StringConverter::toString(pOpt->VolumeSelectionDepth).c_str());
    outstream << buffer;
    sprintf_s(buffer,5000,"  <OBJECTCOUNT value=\"%s\"></OBJECTCOUNT>\n",Ogre::StringConverter::toString(pOpt->ObjectCount).c_str());
    outstream << buffer;
    sprintf_s(buffer,5000,"  <AUTOBACKUP value=\"%s\"></AUTOBACKUP>\n",Ogre::StringConverter::toString(pOpt->AutoBackupEnabled).c_str());
    outstream << buffer;
    sprintf_s(buffer,5000,"  <AUTOBACKUPPERIOD value=\"%s\"></AUTOBACKUPPERIOD>\n",Ogre::StringConverter::toString(pOpt->AutoBackupPeriod).c_str());
    outstream << buffer;
    sprintf_s(buffer,5000,"  <AUTOBACKUPPERIODTYPE value=\"%s\"></AUTOBACKUPPERIODTYPE>\n",Ogre::StringConverter::toString(pOpt->AutoBackupPeriodType).c_str());
    outstream << buffer;
    sprintf_s(buffer,5000,"  <AUTOBACKUPFOLDER value=\"%s\"></AUTOBACKUPFOLDER>\n",pOpt->AutoBackupFolder.c_str());
    outstream << buffer;
    sprintf_s(buffer,5000,"  <AUTOBACKUPNUMBER value=\"%s\"></AUTOBACKUPNUMBER>\n",Ogre::StringConverter::toString(pOpt->AutoBackupNumber).c_str());
    outstream << buffer;
    outstream << "  </PROJECT>\n";
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::AdjustUserResourceDirectories(Ogre::String oldpath)
{
    unsigned int i;
    Ogre::String typestr;
    Ogre::String value;

    Ogre::String newdir = mProjectOptions.ProjectDir;
    Ogre::String result;

    for(i = 0;i < mProjectOptions.ResourceDirectories.size();i++)
    {
        value = mProjectOptions.ResourceDirectories[i];
        typestr = value.substr(0,3);
        value.erase(0,3);
        
        if(value.substr(0,1) == "." && value.substr(1,1) != ".") 
            continue;

        if(value.substr(0,2) == "..") 
            value = oldpath + "/" + value;

        Ogre::String pathto = value;

        result = OgitorsUtils::GetRelativePath(newdir, pathto);
        mProjectOptions.ResourceDirectories[i] = typestr + result;
    }
}
//----------------------------------------------------------------------------
void OgitorsRoot::VolumeSelect(Ogre::Camera *cam, Ogre::Real left, Ogre::Real right, Ogre::Real top, Ogre::Real bottom, NameObjectPairList &result)
{
    Ogre::Real front_dist = 3.0f;
    Ogre::Real back_dist = 3.0f + mProjectOptions.VolumeSelectionDepth;

    Ogre::Ray topLeft, topRight, bottomLeft, bottomRight;

    topLeft = cam->getCameraToViewportRay(left, top);
    topRight = cam->getCameraToViewportRay(right, top);
    bottomLeft = cam->getCameraToViewportRay(left, bottom);
    bottomRight = cam->getCameraToViewportRay(right, bottom);

    Ogre::PlaneBoundedVolume vol;
    // front plane
    vol.planes.push_back(Ogre::Plane(topLeft.getPoint(front_dist)   , topRight.getPoint(front_dist)   , bottomLeft.getPoint(front_dist)));
    // back plane
    vol.planes.push_back(Ogre::Plane(topLeft.getPoint(back_dist)    , bottomLeft.getPoint(back_dist)  , topRight.getPoint(back_dist)));
    // top plane
    vol.planes.push_back(Ogre::Plane(topLeft.getPoint(front_dist)   , topLeft.getPoint(back_dist)     , topRight.getPoint(front_dist)));
    // bottom plane
    vol.planes.push_back(Ogre::Plane(bottomLeft.getPoint(front_dist), bottomRight.getPoint(front_dist), bottomLeft.getPoint(back_dist)));
    // left plane
    vol.planes.push_back(Ogre::Plane(topLeft.getPoint(front_dist)   , bottomLeft.getPoint(front_dist) , topLeft.getPoint(back_dist)));
    // right plane
    vol.planes.push_back(Ogre::Plane(topRight.getPoint(front_dist)  , topRight.getPoint(back_dist)    , bottomRight.getPoint(front_dist)));     

    Ogre::PlaneBoundedVolumeList volList;
    volList.push_back(vol);

    Ogre::PlaneBoundedVolumeListSceneQuery *VolQuery = GetSceneManager()->createPlaneBoundedVolumeQuery(Ogre::PlaneBoundedVolumeList());
    VolQuery->setVolumes(volList);
    VolQuery->setQueryMask(QUERYFLAG_MOVABLE);
    Ogre::SceneQueryResult qryresult = VolQuery->execute();

    unsigned int mVisibilityMask = GetSceneManager()->getVisibilityMask();
    Ogre::Matrix4 viewMatrix = cam->getViewMatrix(true);
    Ogre::Matrix4 projMatrix = cam->getProjectionMatrix();
    Ogre::Vector3 camPos = cam->getDerivedPosition();

    left = (2.0f * left) - 1.0f;
    right = (2.0f * right) - 1.0f;
    top = (2.0f * (1.0f - top)) - 1.0f;
    bottom = (2.0f * (1.0f - bottom)) - 1.0f;

    result.clear();
    Ogre::SceneQueryResultMovableList::iterator itr;
    for (itr = qryresult.movables.begin(); itr != qryresult.movables.end(); ++itr)
    {
        // only check this result if its a hit against an entity
        if ((*itr)->getMovableType().compare("Entity") == 0)
        {
            // get the entity to check
            Ogre::Entity *pentity = static_cast<Ogre::Entity*>(*itr);

            if(!(pentity->getVisibilityFlags() & mVisibilityMask))
                continue;

            if(!pentity->getVisible()) 
                continue;

            // mesh data to retrieve
            size_t vertex_count;
            size_t index_count;

            // get the mesh information
            OgitorsUtils::GetMeshData(pentity->getMesh(), vertex_count, index_count, 
                        pentity->getParentNode()->_getDerivedPosition(),
                        pentity->getParentNode()->_getDerivedOrientation(),
                        pentity->getParentNode()->_getDerivedScale());


            Ogre::Vector3 *mVertexBuffer;
            unsigned long *mIndexBuffer;
            
            OgitorsUtils::GetBuffers(&mVertexBuffer, &mIndexBuffer);

#ifdef _DEBUG
            Ogre::Real minx = 1000.0f;
            Ogre::Real miny = 1000.0f;
            Ogre::Real maxx = -1000.0f;
            Ogre::Real maxy = -1000.0f;
#endif
            bool hitfound = false;
            for (int i = 0; i < static_cast<int>(index_count); i++)
            {
                Ogre::Vector3 vertexPos = mVertexBuffer[mIndexBuffer[i]];

                if((vertexPos - camPos).length() > back_dist)
                    continue;

                Vector3 eyeSpacePos = viewMatrix * vertexPos;
                // z < 0 means in front of cam
                if (eyeSpacePos.z < 0) 
                {
                    // calculate projected pos
                    Vector3 screenSpacePos = projMatrix * eyeSpacePos;
                    
#ifdef _DEBUG
                    if(screenSpacePos.x > maxx)
                        maxx = screenSpacePos.x;
                    if(screenSpacePos.x < minx)
                        minx = screenSpacePos.x;
                    if(screenSpacePos.y > maxy)
                        maxy = screenSpacePos.y;
                    if(screenSpacePos.x < miny)
                        miny = screenSpacePos.y;
#endif                    
                    if(screenSpacePos.x > left && screenSpacePos.x < right && screenSpacePos.y < top && screenSpacePos.y > bottom)
                    {
                        hitfound = true;
                        break;
                    }
                } 
            }

            if(!hitfound)
                continue;
#ifdef _DEBUG
            else
            {
                minx = minx;
            }
#endif
        }

        Ogre::String objname = (*itr)->getName();
        
        CBaseEditor *object = FindObject(objname);

        if(object == GetViewport()->getCameraEditor())
            continue;

        if(object)
        {
            result.insert(NameObjectPairList::value_type(object->getName(),object));
        }
    }
    GetSceneManager()->destroyQuery(VolQuery);
}
/*
bool    ProjectPos    (Camera* cam,const Ogre::Vector3& pos,Ogre::Real& x,Ogre::Real& y) {
    Vector3 eyeSpacePos = cam->getViewMatrix(true) * pos;
    // z < 0 means in front of cam
    if (eyeSpacePos.z < 0) {
        // calculate projected pos
        Vector3 screenSpacePos = cam->getProjectionMatrix() * eyeSpacePos;
        x = screenSpacePos.x;
        y = screenSpacePos.y;
        return true;
    } else {
        x = (-eyeSpacePos.x > 0) ? -1 : 1;
        y = (-eyeSpacePos.y > 0) ? -1 : 1;
        return false;
    }
}*/
//-----------------------------------------------------------------------------------------
void OgitorsRoot::CreateGizmo()
{
    Ogre::MaterialPtr gizmoMaterial = MaterialManager::getSingletonPtr()->createOrRetrieve("AxisGizmo_Material", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME).first;
    gizmoMaterial->setReceiveShadows(false);
    gizmoMaterial->getTechnique(0)->getPass(0)->setLightingEnabled(false); 
    gizmoMaterial->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
    gizmoMaterial->getTechnique(0)->getPass(0)->setCullingMode(CULL_NONE);
    gizmoMaterial->getTechnique(0)->getPass(0)->setVertexColourTracking(Ogre::TVC_DIFFUSE);

    AxisGizmo::createMesh(GetSceneManager(), "OgitorAxisGizmoMesh");
    AxisGizmo::createPlaneMesh(GetSceneManager(), "OgitorAxisPlaneMesh");

    mGizmoNode = GetSceneManager()->getRootSceneNode()->createChildSceneNode("scbWidgetNode",Vector3(0,0,0),Quaternion::IDENTITY);

    mGizmoX = mGizmoNode->createChildSceneNode("scbnwx",Vector3(0,0,0),Quaternion::IDENTITY);
    mGizmoY = mGizmoNode->createChildSceneNode("scbnwy",Vector3(0,0,0),Quaternion::IDENTITY);
    mGizmoZ = mGizmoNode->createChildSceneNode("scbnwz",Vector3(0,0,0),Quaternion::IDENTITY);

    Quaternion q1;
    Quaternion q2;

    q1.FromAngleAxis(Degree(90), Ogre::Vector3(0,0,1));
    q2.FromAngleAxis(Degree(90), Ogre::Vector3(1,0,0));
    mGizmoY->setOrientation(q1 * q2);

    q1.FromAngleAxis(Degree(-90), Ogre::Vector3(0,1,0));
    q2.FromAngleAxis(Degree(-90), Ogre::Vector3(1,0,0));
    mGizmoZ->setOrientation(q1 * q2);

    //Entities
    mGizmoEntities[0] = GetSceneManager()->createEntity("scbwx", "OgitorAxisGizmoMesh", "EditorResources");
    mGizmoEntities[1] = GetSceneManager()->createEntity("scbwy", "OgitorAxisGizmoMesh", "EditorResources");
    mGizmoEntities[2] = GetSceneManager()->createEntity("scbwz", "OgitorAxisGizmoMesh", "EditorResources");
    mGizmoEntities[3] = GetSceneManager()->createEntity("scbwt", "OgitorAxisPlaneMesh", "EditorResources");
    mGizmoEntities[4] = GetSceneManager()->createEntity("scbwu", "OgitorAxisPlaneMesh", "EditorResources");
    mGizmoEntities[5] = GetSceneManager()->createEntity("scbwv", "OgitorAxisPlaneMesh", "EditorResources");

    //XX arrows
    mGizmoEntities[0]->setCastShadows(false);
    mGizmoEntities[0]->setMaterialName("MAT_GIZMO_X");
    mGizmoEntities[0]->setRenderQueueGroup(RENDER_QUEUE_SKIES_LATE);
    mGizmoEntities[0]->setQueryFlags(QUERYFLAG_WIDGET);
    mGizmoX->attachObject(mGizmoEntities[0]);

    //YY arrows
    mGizmoEntities[1]->setCastShadows(false);
    mGizmoEntities[1]->setMaterialName("MAT_GIZMO_Y");
    mGizmoEntities[1]->setRenderQueueGroup(RENDER_QUEUE_SKIES_LATE);
    mGizmoEntities[1]->setQueryFlags(QUERYFLAG_WIDGET);
    mGizmoY->attachObject(mGizmoEntities[1]);

    //ZZ arrows
    mGizmoEntities[2]->setCastShadows(false);
    mGizmoEntities[2]->setMaterialName("MAT_GIZMO_Z");
    mGizmoEntities[2]->setRenderQueueGroup(RENDER_QUEUE_SKIES_LATE);
    mGizmoEntities[2]->setQueryFlags(QUERYFLAG_WIDGET);
    mGizmoZ->attachObject(mGizmoEntities[2]);

    //XY Plane
    mGizmoEntities[3]->setCastShadows(false);
    mGizmoEntities[3]->setMaterialName("MAT_GIZMO_XY");
    mGizmoEntities[3]->setRenderQueueGroup(RENDER_QUEUE_SKIES_LATE);
    mGizmoEntities[3]->setQueryFlags(QUERYFLAG_WIDGET);
    mGizmoX->attachObject(mGizmoEntities[3]);

    //YZ Plane
    mGizmoEntities[4]->setCastShadows(false);
    mGizmoEntities[4]->setMaterialName("MAT_GIZMO_YZ");
    mGizmoEntities[4]->setRenderQueueGroup(RENDER_QUEUE_SKIES_LATE);
    mGizmoEntities[4]->setQueryFlags(QUERYFLAG_WIDGET);
    mGizmoY->attachObject(mGizmoEntities[4]);

    //ZX Plane
    mGizmoEntities[5]->setCastShadows(false);
    mGizmoEntities[5]->setMaterialName("MAT_GIZMO_ZX");
    mGizmoEntities[5]->setRenderQueueGroup(RENDER_QUEUE_SKIES_LATE);
    mGizmoEntities[5]->setQueryFlags(QUERYFLAG_WIDGET);
    mGizmoZ->attachObject(mGizmoEntities[5]);

    mGizmoNode->setVisible(false);
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::UpdateGizmo()
{
    CViewportEditor *viewport = GetViewport();

    if(mGizmoEntities[0])
    {
        Ogre::Vector3 cameraPos = viewport->getCameraEditor()->getPosition();
        float dist1 = (cameraPos - mGizmoEntities[0]->getWorldBoundingBox().getCenter()).length();
        float dist2 = (cameraPos - mGizmoEntities[1]->getWorldBoundingBox().getCenter()).length();
        float dist3 = (cameraPos - mGizmoEntities[2]->getWorldBoundingBox().getCenter()).length();

        int orders[3];
        int increment = -1;

        if(viewport->GetEditorTool() == TOOL_ROTATE)
        {
            increment = 1;
            orders[0] = orders[1] = orders[2] = 93;
        }
        else
        {
            increment = -1;
            orders[0] = orders[1] = orders[2] = 95;
        }


        if(dist1 > dist2) 
            orders[0] += increment;
        else
            orders[1] += increment;
        
        if(dist1 > dist3) 
            orders[0] += increment;
        else
            orders[2] += increment;

        if(dist2 > dist3) 
            orders[1] += increment;
        else
            orders[2] += increment;

        mGizmoEntities[0]->setRenderQueueGroup(orders[0]);
        mGizmoEntities[1]->setRenderQueueGroup(orders[1]);
        mGizmoEntities[2]->setRenderQueueGroup(orders[2]);
    }

    if(mMultiSelection->getAsSingle()->usesGizmos() && mMultiSelection->getAsSingle() != viewport->getCameraEditor()) 
    {
        if(!mMultiSelection->getAsSingle()->isLoaded())
        {
            mGizmoNode->setVisible(false);
            return;
        }

        Ogre::Vector3 position;
        if(mMultiSelection->getAsSingle()->usesHelper())
            position = mMultiSelection->getAsSingle()->getHelper()->getNode()->_getDerivedPosition();
        else
            position = mMultiSelection->getAsSingle()->getDerivedPosition();

        Ogre::Vector4 rect;
        viewport->getRect(rect);

        float minsize = std::min(rect.z, rect.w);
        float distance = (position - viewport->getCameraEditor()->getDerivedPosition()).length();
        
        distance /= (minsize / 20.0f);
        distance *= mGizmoScale;
        
        Ogre::Quaternion orient = Ogre::Quaternion::IDENTITY;

        if(!mWorldSpaceGizmoOrientation || (CViewportEditor::GetEditorTool() == TOOL_SCALE))
            orient = mMultiSelection->getAsSingle()->getDerivedOrientation();

        mGizmoNode->setPosition(position);
        mGizmoNode->setOrientation(orient);
        mGizmoNode->setScale(distance, distance, distance);
        mGizmoNode->setVisible(true);
    }
    else
        mGizmoNode->setVisible(false);
 }
//-----------------------------------------------------------------------------------------
void OgitorsRoot::HighlightGizmo(int ID)
{
    if(mOldGizmoAxis == ID) 
        return;
    
    mOldGizmoAxis = ID;
    Entity* wx = mGizmoEntities[0];
    Entity* wy = mGizmoEntities[1];
    Entity* wz = mGizmoEntities[2];

    if(ID & AXIS_X)
        wx->setMaterialName("MAT_GIZMO_X_L");
    else
        wx->setMaterialName("MAT_GIZMO_X");
    if(ID & AXIS_Y)
        wy->setMaterialName("MAT_GIZMO_Y_L");
    else
        wy->setMaterialName("MAT_GIZMO_Y");
    if(ID & AXIS_Z)
        wz->setMaterialName("MAT_GIZMO_Z_L");
    else
        wz->setMaterialName("MAT_GIZMO_Z");

    if((ID & AXIS_XY) == AXIS_XY)
        mGizmoEntities[3]->setMaterialName("MAT_GIZMO_XY_L");
    else
        mGizmoEntities[3]->setMaterialName("MAT_GIZMO_XY");
    if((ID & AXIS_YZ) == AXIS_YZ)
        mGizmoEntities[4]->setMaterialName("MAT_GIZMO_YZ_L");
    else
        mGizmoEntities[4]->setMaterialName("MAT_GIZMO_YZ");
    if((ID & AXIS_XZ) == AXIS_XZ)
        mGizmoEntities[5]->setMaterialName("MAT_GIZMO_ZX_L");
    else
        mGizmoEntities[5]->setMaterialName("MAT_GIZMO_ZX");
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::SetGizmoScale(Ogre::Real value)
{
    mGizmoScale = value;
    
    if(!mGizmoNode)
        return;

    if(!mMultiSelection->isEmpty())
    {
        UpdateGizmo();
        mGizmoNode->setVisible(true);
    }
    else
        mGizmoNode->setVisible(false);
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::SetGizmoMode(int mode)
{
    if(mOldGizmoMode == mode) 
        return;
    mOldGizmoMode = mode;
    Entity* wx = mGizmoEntities[0];
    Entity* wy = mGizmoEntities[1];
    Entity* wz = mGizmoEntities[2];

    mGizmoEntities[3]->getSubEntity(0)->setVisible(false);
    mGizmoEntities[4]->getSubEntity(0)->setVisible(false);
    mGizmoEntities[5]->getSubEntity(0)->setVisible(false);

    wx->getSubEntity(0)->setVisible(true);
    wx->getSubEntity(1)->setVisible(false);
    wx->getSubEntity(2)->setVisible(false);
    wx->getSubEntity(3)->setVisible(false);
    wx->getSubEntity(4)->setVisible(false);
    wy->getSubEntity(0)->setVisible(true);
    wy->getSubEntity(1)->setVisible(false);
    wy->getSubEntity(2)->setVisible(false);
    wy->getSubEntity(3)->setVisible(false);
    wy->getSubEntity(4)->setVisible(false);
    wz->getSubEntity(0)->setVisible(true);
    wz->getSubEntity(1)->setVisible(false);
    wz->getSubEntity(2)->setVisible(false);
    wz->getSubEntity(3)->setVisible(false);
    wz->getSubEntity(4)->setVisible(false);
    switch(mode)
    {
    case TOOL_MOVE:
        {
            wx->getSubEntity(2)->setVisible(true);
            wy->getSubEntity(2)->setVisible(true);
            wz->getSubEntity(2)->setVisible(true);
            mGizmoEntities[3]->getSubEntity(0)->setVisible(true);
            mGizmoEntities[4]->getSubEntity(0)->setVisible(true);
            mGizmoEntities[5]->getSubEntity(0)->setVisible(true);
            break;
        }
    case TOOL_ROTATE:
        {
            wx->getSubEntity(0)->setVisible(false);
            wy->getSubEntity(0)->setVisible(false);
            wz->getSubEntity(0)->setVisible(false);
            wx->getSubEntity(1)->setVisible(true);
            wy->getSubEntity(1)->setVisible(true);
            wz->getSubEntity(1)->setVisible(true);
            wx->getSubEntity(3)->setVisible(true);
            wy->getSubEntity(3)->setVisible(true);
            wz->getSubEntity(3)->setVisible(true);
            break;
        }
    case TOOL_SCALE:
        {
            wx->getSubEntity(4)->setVisible(true);
            wy->getSubEntity(4)->setVisible(true);
            wz->getSubEntity(4)->setVisible(true);
            mGizmoEntities[3]->getSubEntity(0)->setVisible(true);
            mGizmoEntities[4]->getSubEntity(0)->setVisible(true);
            mGizmoEntities[5]->getSubEntity(0)->setVisible(true);
            break;
        }
    }
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::DestroyGizmo()
{
    if(!mGizmoNode) 
        return;

    for(int i= 0;i < 6;i++)
    {
        mGizmoEntities[i]->detachFromParent();
        mGizmoEntities[i]->_getManager()->destroyEntity(mGizmoEntities[i]);
    }
    
    mGizmoNode->removeAndDestroyChild(mGizmoX->getName());
    mGizmoNode->removeAndDestroyChild(mGizmoY->getName());
    mGizmoNode->removeAndDestroyChild(mGizmoZ->getName());

    mGizmoNode->getParentSceneNode()->removeAndDestroyChild(mGizmoNode->getName());
    mGizmoNode = 0;
    mGizmoX = 0;
    mGizmoY = 0;
    mGizmoZ = 0;
      mGizmoEntities[0] = mGizmoEntities[1] = mGizmoEntities[2] = mGizmoEntities[3] = mGizmoEntities[4] = mGizmoEntities[5] = 0;
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::OnKeyDown (unsigned int key)
{
    if(mKeyboardListener && mKeyboardListener->OnKeyDown(key))
        return;

    if(mActiveViewport)
        mActiveViewport->OnKeyDown(key);
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::OnKeyUp (unsigned int key)
{
    if(mKeyboardListener && mKeyboardListener->OnKeyUp(key))
        return;

    if(mActiveViewport)
        mActiveViewport->OnKeyUp(key);
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::OnMouseWheel (Ogre::Vector2 point, float delta, unsigned int buttons)
{
    if(mActiveViewport)
    {
        Ogre::Vector4 rect;
        mActiveViewport->getRect(rect);

        if(mActiveDragSource)
        {
            DragDropHandlerMap::iterator it = mDragDropHandlers.begin();
            while(it != mDragDropHandlers.end())
            {
                if(it->first == mActiveDragSource)
                {
                    it->second->OnDragWheel((Ogre::Viewport*)mActiveViewport->getHandle(), delta);
                    return;
                }
                it++;
            }
        }

        if(mMouseListener)
            mMouseListener->OnMouseWheel(mActiveViewport, point - Ogre::Vector2(rect.x,rect.y), delta, buttons);
        else
            mActiveViewport->OnMouseWheel(point - Ogre::Vector2(rect.x,rect.y), delta, buttons);
    }
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::OnMouseMove (Ogre::Vector2 point, unsigned int buttons)
{
    if(!mActiveViewport)
        return;
    Ogre::Vector4 rect;
    mActiveViewport->getRect(rect);

    if((rect.x <= point.x) && (rect.y <= point.y) && ((rect.x + rect.z) >= point.x) && ((rect.y + rect.w) >= point.y))
    {
        if(mMouseListener)
            mMouseListener->OnMouseMove(mActiveViewport, point - Ogre::Vector2(rect.x,rect.y), buttons);
        else
            mActiveViewport->OnMouseMove(point - Ogre::Vector2(rect.x,rect.y), buttons);
    }
    else
    {
        if(mMouseListener)
            mMouseListener->OnMouseLeave(mActiveViewport, point - Ogre::Vector2(rect.x,rect.y), buttons);
        else
            mActiveViewport->OnMouseLeave(point - Ogre::Vector2(rect.x,rect.y), buttons);
    }
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::OnMouseLeave (Ogre::Vector2 point, unsigned int buttons)
{
    if(mActiveViewport)
    {
        Ogre::Vector4 rect;
        mActiveViewport->getRect(rect);
        if(mMouseListener)
            mMouseListener->OnMouseLeave(mActiveViewport, point - Ogre::Vector2(rect.x,rect.y), buttons);
        else
            mActiveViewport->OnMouseLeave(point - Ogre::Vector2(rect.x,rect.y), buttons);
    }
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::OnMouseLeftDown (Ogre::Vector2 point, unsigned int buttons)
{
    Ogre::Vector4 rect;
    NameObjectPairList viewports = GetObjectsByType(ETYPE_VIEWPORT);
    NameObjectPairList::const_iterator it = viewports.begin();

    CViewportEditor *vp = 0;
    int ZOrder = -1000;
    while(it != viewports.end())
    {
        int order = static_cast<CViewportEditor*>(it->second)->getRect(rect);
        if((rect.x <= point.x) && (rect.y <= point.y) && ((rect.x + rect.z) >= point.x) && ((rect.y + rect.w) >= point.y) && (order > ZOrder))
        {
           ZOrder = order;
           vp = static_cast<CViewportEditor*>(it->second);
        }
        it++;
    }
    if(vp)
    {
        mActiveViewport = vp;
        mActiveViewport->getRect(rect);
        if(mMouseListener)
            mMouseListener->OnMouseLeftDown(mActiveViewport, point - Ogre::Vector2(rect.x,rect.y), buttons);
        else
            mActiveViewport->OnMouseLeftDown(point - Ogre::Vector2(rect.x,rect.y), buttons);
    }
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::OnMouseRightDown (Ogre::Vector2 point, unsigned int buttons)
{
    Ogre::Vector4 rect;
    NameObjectPairList viewports = GetObjectsByType(ETYPE_VIEWPORT);
    NameObjectPairList::const_iterator it = viewports.begin();

    CViewportEditor *vp = 0;
    int ZOrder = -1000;
    while(it != viewports.end())
    {
        int order = static_cast<CViewportEditor*>(it->second)->getRect(rect);
        if((rect.x <= point.x) && (rect.y <= point.y) && ((rect.x + rect.z) >= point.x) && ((rect.y + rect.w) >= point.y) && (order > ZOrder))
        {
             ZOrder = order;
             vp = static_cast<CViewportEditor*>(it->second);
        }
        it++;
    }
    if(vp)
    {
        mActiveViewport = vp;
        mActiveViewport->getRect(rect);
        if(mMouseListener)
            mMouseListener->OnMouseRightDown(mActiveViewport, point - Ogre::Vector2(rect.x,rect.y), buttons);
        else
            mActiveViewport->OnMouseRightDown(point - Ogre::Vector2(rect.x,rect.y), buttons);
    }
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::OnMouseMiddleDown (Ogre::Vector2 point, unsigned int buttons)
{
    Ogre::Vector4 rect;
    NameObjectPairList viewports = GetObjectsByType(ETYPE_VIEWPORT);
    NameObjectPairList::const_iterator it = viewports.begin();

    CViewportEditor *vp = 0;
    int ZOrder = -1000;
    while(it != viewports.end())
    {
        int order = static_cast<CViewportEditor*>(it->second)->getRect(rect);
        if((rect.x <= point.x) && (rect.y <= point.y) && ((rect.x + rect.z) >= point.x) && ((rect.y + rect.w) >= point.y) && (order > ZOrder))
        {
            ZOrder = order;
            vp = static_cast<CViewportEditor*>(it->second);
        }
        it++;
    }
    if(vp)
    {
        mActiveViewport = vp;
        mActiveViewport->getRect(rect);
        if(mMouseListener)
            mMouseListener->OnMouseMiddleDown(mActiveViewport, point - Ogre::Vector2(rect.x,rect.y), buttons);
        else
            mActiveViewport->OnMouseMiddleDown(point - Ogre::Vector2(rect.x,rect.y), buttons);
    }
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::OnMouseLeftUp (Ogre::Vector2 point, unsigned int buttons)
{
    if(!mActiveViewport)
        return;
    Ogre::Vector4 rect;
    mActiveViewport->getRect(rect);
    if((rect.x <= point.x) && (rect.y <= point.y) && ((rect.x + rect.z) >= point.x) && ((rect.y + rect.w) >= point.y))
    {
        if(mMouseListener)
            mMouseListener->OnMouseLeftUp(mActiveViewport, point - Ogre::Vector2(rect.x,rect.y), buttons);
        else
            mActiveViewport->OnMouseLeftUp(point - Ogre::Vector2(rect.x,rect.y), buttons);
    }
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::OnMouseMiddleUp (Ogre::Vector2 point, unsigned int buttons)
{
    if(!mActiveViewport)
        return;
    Ogre::Vector4 rect;
    mActiveViewport->getRect(rect);
    if((rect.x <= point.x) && (rect.y <= point.y) && ((rect.x + rect.z) >= point.x) && ((rect.y + rect.w) >= point.y))
    {
        if(mMouseListener)
            mMouseListener->OnMouseMiddleUp(mActiveViewport, point - Ogre::Vector2(rect.x,rect.y), buttons);
        else
            mActiveViewport->OnMouseMiddleUp(point - Ogre::Vector2(rect.x,rect.y), buttons);
    }
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::OnMouseRightUp (Ogre::Vector2 point, unsigned int buttons)
{
    if(!mActiveViewport)
        return;
    Ogre::Vector4 rect;
    mActiveViewport->getRect(rect);
    if((rect.x <= point.x) && (rect.y <= point.y) && ((rect.x + rect.z) >= point.x) && ((rect.y + rect.w) >= point.y))
    {
        if(mMouseListener)
            mMouseListener->OnMouseRightUp(mActiveViewport, point - Ogre::Vector2(rect.x,rect.y), buttons);
        else
            mActiveViewport->OnMouseRightUp(point - Ogre::Vector2(rect.x,rect.y), buttons);
    }
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::RenderWindowResized()
{
    NameObjectPairList viewports = GetObjectsByType(ETYPE_VIEWPORT);
    NameObjectPairList::const_iterator it = viewports.begin();

    while(it != viewports.end())
    {
        static_cast<CViewportEditor*>(it->second)->renderWindowResized();
        it++;
    }
    ClearScreenBackground(true);
}
//-----------------------------------------------------------------------------------------
bool OgitorsRoot::OnDragEnter (void *source)
{
    if(mLoadState != LS_LOADED)
        return false;

    DragDropHandlerMap::iterator it = mDragDropHandlers.begin();
    while(it != mDragDropHandlers.end())
    {
        if(it->first == source)
        {
            mActiveDragSource = source;        
            return it->second->OnDragEnter();
        }
        it++;
    }

    return false;
}
//-----------------------------------------------------------------------------------------
bool OgitorsRoot::OnDragMove (void *source, unsigned int modifier, int x, int y)
{
    Ogre::Vector4 rect;
    NameObjectPairList viewports = GetObjectsByType(ETYPE_VIEWPORT);
    NameObjectPairList::const_iterator it = viewports.begin();

    CViewportEditor *vp = 0;
    int ZOrder = -1000;
    while(it != viewports.end())
    {
        int order = static_cast<CViewportEditor*>(it->second)->getRect(rect);
        if((rect.x <= x) && (rect.y <= y) && ((rect.x + rect.z) >= x) && ((rect.y + rect.w) >= y) && (order > ZOrder))
        {
           ZOrder = order;
           vp = static_cast<CViewportEditor*>(it->second);
        }
        it++;
    }
    if(vp)
    {
        mActiveViewport = vp;
        mActiveViewport->getRect(rect);

        DragDropHandlerMap::iterator it = mDragDropHandlers.begin();
        while(it != mDragDropHandlers.end())
        {
            if(it->first == source)
            {
                Ogre::Vector2 point((x - rect.x) / rect.z,(y - rect.y) / rect.w);
                return it->second->OnDragMove((Ogre::Viewport*)mActiveViewport->getHandle(), modifier, point);
            }
            it++;
        }

        return true;
    }
    return false;
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::OnDragLeave ()
{
    DragDropHandlerMap::iterator it = mDragDropHandlers.begin();
    while(it != mDragDropHandlers.end())
    {
        if(it->first == mActiveDragSource)
        {
            it->second->OnDragLeave();
            break;
        }
        it++;
    }
    mActiveDragSource = 0;
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::OnDragDropped (void *source, int x, int y)
{
    Ogre::Vector4 drect;
    mActiveViewport->getRect(drect);
    Ogre::Vector2 droppos((x - drect.x) / drect.z, (y - drect.y) / drect.w);
    DragDropHandlerMap::iterator it = mDragDropHandlers.begin();
    while(it != mDragDropHandlers.end())
    {
        if(it->first == source)
        {
            it->second->OnDragDropped((Ogre::Viewport*)mActiveViewport->getHandle(), droppos);
            break;
        }
        it++;
    }
    mActiveDragSource = 0;
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::InitRecentFiles(UTFStringVector& list)
{
    mRecentFiles = list;
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::GetRecentFiles(UTFStringVector& list)
{
    list = mRecentFiles;
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::AddToRecentFiles(const UTFString& entry)
{
    if(entry.empty())
        return;
    
    UTFStringVector temp;

    temp.clear();
    temp.push_back(entry);
    for(unsigned int i = 0;i < mRecentFiles.size();i++)
    {
        if(mRecentFiles[i] != entry)
            temp.push_back(mRecentFiles[i]);
    }
    mRecentFiles = temp;
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::UpdateMaterialInScene(Ogre::String materialName)
{
    if(materialName == "" || !IsSceneLoaded())
        return;

    Ogre::NameValuePairList *modelMaterialMap = GetModelMaterialMap();

    // Update entities
    Ogre::SceneManager::MovableObjectIterator it = GetSceneManager()->getMovableObjectIterator("Entity");
    while(it.hasMoreElements())
    {
        Ogre::Entity *tmp = static_cast<Ogre::Entity*>(it.peekNextValue());
        int subEntitiesCount = tmp->getNumSubEntities();
        for(int i = 0; i < subEntitiesCount; i++)
        {
            // Material is "BaseWhite" -> the "real" material was not found at model loading time
            // -> look its material name up in the ModelMaterialMap
            if(tmp->getSubEntity(i)->getMaterialName() == "BaseWhite")
            {
                Ogre::NameValuePairList::iterator mapIt = modelMaterialMap->find(tmp->getMesh()->getName());

                if(mapIt != modelMaterialMap->end())
                    if(mapIt->second == materialName)
                        tmp->getSubEntity(i)->setMaterialName(materialName);
            }
            // Material name is not "BaseWhite" 
            //      -> either "real" material exists and we just want to update the already used material, 
            //         (e.g. due to a MaterialTextEditor change with a reload command) 
            //      -> or it is an internal material in which case we will just reload as the default behavior
            else
            {
                if(tmp->getSubEntity(i)->getMaterialName() == materialName)
                    tmp->getSubEntity(i)->setMaterialName(materialName);
            }
        }
        it.moveNext();
    }

    // Update particle systems
    it = GetSceneManager()->getMovableObjectIterator("ParticleSystem");
    while(it.hasMoreElements())
    {
        Ogre::ParticleSystem *tmp = static_cast<Ogre::ParticleSystem*>(it.peekNextValue());
        if(tmp->getMaterialName() == materialName)
            tmp->setMaterialName(materialName);
        it.moveNext();
    }

    // Update simple renderables
    it = GetSceneManager()->getMovableObjectIterator("SimpleRenderable");
    while(it.hasMoreElements())
    {
        Ogre::SimpleRenderable *tmp = static_cast<Ogre::SimpleRenderable*>(it.peekNextValue());
        if(tmp->getMaterial()->getName() == materialName)
            tmp->setMaterial(materialName);
        it.moveNext();
    }

    // Update ribbon trails
    it = GetSceneManager()->getMovableObjectIterator("RibbonTrail");
    while(it.hasMoreElements())
    {
        Ogre::RibbonTrail *tmp = static_cast<Ogre::RibbonTrail*>(it.peekNextValue());
        if(tmp->getMaterialName() == materialName)
            tmp->setMaterialName(materialName);
        it.moveNext();
    }

    // Update billboard sets
    it = GetSceneManager()->getMovableObjectIterator("BillboardSet");
    while(it.hasMoreElements())
    {
        Ogre::BillboardSet *tmp = static_cast<Ogre::BillboardSet*>(it.peekNextValue());
        if(tmp->getMaterialName() == materialName)
            tmp->setMaterialName(materialName);
        it.moveNext();
    }

    // Update billboard chains
    it = GetSceneManager()->getMovableObjectIterator("BillboardChain");
    while(it.hasMoreElements())
    {
        Ogre::BillboardChain *tmp = static_cast<Ogre::BillboardChain*>(it.peekNextValue());
        if(tmp->getMaterialName() == materialName)
            tmp->setMaterialName(materialName);
        it.moveNext();
    }
}
//-------------------------------------------------------------------------------------------
void OgitorsRoot::UpdateMaterialsInScene()
{
    PropertyOptionsVector *MaterialNames = GetMaterialNames();
    for(unsigned int i = 0; i < MaterialNames->size(); i++)
        UpdateMaterialInScene(MaterialNames->at(i).mKey);
}
//-------------------------------------------------------------------------------------------
void OgitorsRoot::ReloadUserResources()
{   
    Ogre::ResourceGroupManager *mngr = Ogre::ResourceGroupManager::getSingletonPtr();
    mngr->clearResourceGroup(PROJECT_RESOURCE_GROUP);
    FillResourceGroup(mngr, mProjectOptions.ResourceDirectories, mProjectOptions.ProjectDir + mProjectOptions.ProjectName + ".ofs", PROJECT_RESOURCE_GROUP);

    try
    {
        mngr->initialiseResourceGroup(PROJECT_RESOURCE_GROUP);
    }
    catch(Ogre::Exception& e)
    {
        OgitorsSystem::getSingletonPtr()->DisplayMessageDialog(e.getFullDescription(), DLGTYPE_OK);
    }

    mModelNames.clear();
    mModelNames.push_back(PropertyOption("",Ogre::Any(Ogre::String(""))));

    HashMap<Ogre::String, int> tmpEntityList;

    Ogre::StringVectorPtr pList = Ogre::ResourceGroupManager::getSingleton().findResourceNames(PROJECT_RESOURCE_GROUP,"*.mesh",false);

    for(unsigned int i = 0;i < pList->size();i++)
    {
        Ogre::String addstr = (*pList)[i];
        addstr.erase(addstr.find(".mesh"), 5);

        if(tmpEntityList.find(addstr) == tmpEntityList.end())
            tmpEntityList.insert(HashMap<Ogre::String, int>::value_type(addstr, 0));
    }

    pList.setNull();

    HashMap<Ogre::String, int>::iterator eit = tmpEntityList.begin();
    while(eit != tmpEntityList.end())
    {
        mModelNames.push_back(PropertyOption(eit->first,Ogre::Any(eit->first)));
        eit++;
    }

    std::sort(++(mModelNames.begin()), mModelNames.end(), PropertyOption::comp_func);

    mScriptNames.clear();
    mScriptNames.push_back(PropertyOption("",Ogre::Any(Ogre::String(""))));

    OFS::FileList scriptnames = (*mProjectFile)->listFiles("/Scripts", OFS::OFS_FILE);
    
    for(unsigned int i = 0;i < scriptnames.size();i++)
    {
        mScriptNames.push_back(PropertyOption(scriptnames[i].name,Ogre::Any(scriptnames[i].name)));
    }

    std::sort(++(mScriptNames.begin()), mScriptNames.end(), PropertyOption::comp_func);


    mMaterialNames.clear();
    mTerrainPlantMaterialNames.clear();
    mTerrainPlantMaterialNames.push_back(PropertyOption("",Ogre::Any(Ogre::String(""))));
    mSkyboxMaterials.clear();
    mSkyboxMaterials.push_back(PropertyOption("",Ogre::Any(Ogre::String(""))));

    Ogre::ResourcePtr mRes;
    Ogre::ResourceManager::ResourceMapIterator it = Ogre::MaterialManager::getSingleton().getResourceIterator();

    while(it.hasMoreElements())
    {
        mRes = it.getNext();
        if(mRes->getGroup() == PROJECT_RESOURCE_GROUP)
        {
            mMaterialNames.push_back(PropertyOption(mRes->getName(), Ogre::Any(mRes->getName())));
            
            Ogre::String matname =     mRes->getName();
            Ogre::StringUtil::toLowerCase(matname);
            if(matname.find("sky") != -1 && matname.find("skyboxplane") == -1)
                mSkyboxMaterials.push_back(PropertyOption(mRes->getName(), Ogre::Any(mRes->getName())));
        }
        else if(mRes->getGroup() == "Plants")
        {
            mTerrainPlantMaterialNames.push_back(PropertyOption(mRes->getName(), Ogre::Any(mRes->getName())));
        }
    }

    std::sort(mTerrainPlantMaterialNames.begin(), mTerrainPlantMaterialNames.end(), PropertyOption::comp_func);
    std::sort(mMaterialNames.begin(), mMaterialNames.end(), PropertyOption::comp_func);
    std::sort(mSkyboxMaterials.begin(), mSkyboxMaterials.end(), PropertyOption::comp_func);

    NameObjectPairList oblist = GetObjectsByTypeName("Entity Object");
    NameObjectPairList::iterator obit = oblist.begin();
    while(obit != oblist.end())
    {
        CEntityEditor *ed = static_cast<CEntityEditor*>(obit->second);
        if(ed->isUsingPlaceHolderMesh() && ed->isLoaded())
        {
            ed->unLoad();
            ed->load();
        }
        obit++;        
    }

    oblist = GetObjectsByTypeName("PGInstance Manager Object");
    obit = oblist.begin();
    while(obit != oblist.end())
    {
        CPGInstanceManager *ed = static_cast<CPGInstanceManager*>(obit->second);
        if(ed->isUsingPlaceHolderMesh() && ed->isLoaded())
        {
            ed->unLoad();
            ed->load();
        }
        obit++;        
    }

    UpdateMaterialsInScene();

    SetSceneModified(true);
}
//-------------------------------------------------------------------------------------------
void OgitorsRoot::PrepareTerrainResources()
{
    mTerrainDiffuseTextureNames = OgitorsUtils::GetResourceFilenames("TerrainGroupDiffuseSpecular", "Ofs");
    mTerrainNormalTextureNames = OgitorsUtils::GetResourceFilenames("TerrainGroupNormalHeight", "Ofs");
    mTerrainPlantMaterialNames = OgitorsUtils::GetResourceFilenames("TerrainGroupPlants", "Ofs", ".material");
}
//-------------------------------------------------------------------------------------------
void OgitorsRoot::PrepareProjectResources()
{
    try {
        Ogre::ResourceGroupManager *mngr = Ogre::ResourceGroupManager::getSingletonPtr();
        mngr->createResourceGroup(PROJECT_RESOURCE_GROUP);

        Ogre::String tempFilesDir = OgitorsUtils::QualifyPath(mProjectOptions.ProjectDir + "/Temp");
        mSystem->MakeDirectory(tempFilesDir);

        Ogre::String tempDir = "/Temp";
        (*mProjectFile)->createDirectory(tempDir.c_str());

        Ogre::String scriptDir = "/Scripts";
        (*mProjectFile)->createDirectory(scriptDir.c_str());

        mngr->createResourceGroup(PROJECT_TEMP_RESOURCE_GROUP);
        mngr->addResourceLocation(mProjectOptions.ProjectDir + mProjectOptions.ProjectName + ".ofs::" + tempDir, "Ofs", PROJECT_TEMP_RESOURCE_GROUP);
        mngr->initialiseResourceGroup(PROJECT_TEMP_RESOURCE_GROUP);

        FillResourceGroup(mngr, mProjectOptions.ResourceDirectories, mProjectOptions.ProjectDir + mProjectOptions.ProjectName + ".ofs", PROJECT_RESOURCE_GROUP);

        try
        {
            mngr->initialiseResourceGroup("ProjectResources");
        }
        catch(Ogre::Exception& e)
        {
            OgitorsSystem::getSingletonPtr()->DisplayMessageDialog(e.getFullDescription(), DLGTYPE_OK);
        }

        Ogre::MaterialPtr planeMaterial = MaterialManager::getSingletonPtr()->createOrRetrieve("DefaultPlaneMaterial", PROJECT_RESOURCE_GROUP).first;
        planeMaterial->getTechnique(0)->getPass(0)->setAmbient(0.4f, 0.4f, 0.4f);
        planeMaterial->getTechnique(0)->getPass(0)->setDiffuse(0.8f, 0.8f, 0.8f, 0.8f);
        planeMaterial->getTechnique(0)->getPass(0)->setSpecular(1.0f, 1.0f, 1.0f, 1.0f);

        mParticleTemplateNames.clear();

        Ogre::ParticleSystemManager::ParticleSystemTemplateIterator pit = Ogre::ParticleSystemManager::getSingletonPtr()->getTemplateIterator();
        Ogre::ParticleSystem *psys;

        while(pit.hasMoreElements())
        {
            psys = pit.getNext();
            mParticleTemplateNames.push_back(PropertyOption(psys->getName(), Ogre::Any((Ogre::String)psys->getName())));
        }

        mScriptNames.clear();
        mScriptNames.push_back(PropertyOption("", Ogre::Any(Ogre::String(""))));

        OFS::FileList scriptnames = (*mProjectFile)->listFiles("/Scripts", OFS::OFS_FILE);
        
        for(unsigned int i = 0;i < scriptnames.size();i++)
        {
            mScriptNames.push_back(PropertyOption(scriptnames[i].name, Ogre::Any(scriptnames[i].name)));
        }

        std::sort(++(mScriptNames.begin()), mScriptNames.end(), PropertyOption::comp_func);

        mModelNames.clear();
        mModelNames.push_back(PropertyOption("", Ogre::Any(Ogre::String(""))));

        HashMap<Ogre::String, int> tmpEntityList;
        
        Ogre::StringVectorPtr pList = Ogre::ResourceGroupManager::getSingleton().findResourceNames(PROJECT_RESOURCE_GROUP, "*.mesh", false);
    
        for(unsigned int i = 0;i < pList->size();i++)
        {
            Ogre::String addstr = (*pList)[i];
            addstr.erase(addstr.find(".mesh"), 5);

            if(tmpEntityList.find(addstr) == tmpEntityList.end())
                tmpEntityList.insert(HashMap<Ogre::String, int>::value_type(addstr, 0));
        }

        pList.setNull();

        HashMap<Ogre::String, int>::iterator eit = tmpEntityList.begin();
        while(eit != tmpEntityList.end())
        {
            mModelNames.push_back(PropertyOption(eit->first, Ogre::Any(eit->first)));
            eit++;
        }

        std::sort(++(mModelNames.begin()), mModelNames.end(), PropertyOption::comp_func);

        mMaterialNames.clear();
        mTerrainPlantMaterialNames.clear();
        mTerrainPlantMaterialNames.push_back(PropertyOption("", Ogre::Any(Ogre::String(""))));
        mSkyboxMaterials.clear();
        mSkyboxMaterials.push_back(PropertyOption("", Ogre::Any(Ogre::String(""))));

        Ogre::ResourcePtr mRes;
        Ogre::ResourceManager::ResourceMapIterator it = Ogre::MaterialManager::getSingleton().getResourceIterator();

        while(it.hasMoreElements())
        {
            mRes = it.getNext();
            if(mRes->getGroup() == PROJECT_RESOURCE_GROUP)
            {
                mMaterialNames.push_back(PropertyOption(mRes->getName(), Ogre::Any(mRes->getName())));
                
                Ogre::String matname =     mRes->getName();
                Ogre::StringUtil::toLowerCase(matname);
                if(matname.find("sky") != -1 && matname.find("skyboxplane") == -1)
                    mSkyboxMaterials.push_back(PropertyOption(mRes->getName(), Ogre::Any(mRes->getName())));
            }
        }

        std::sort(mMaterialNames.begin(), mMaterialNames.end(), PropertyOption::comp_func);
        std::sort(mSkyboxMaterials.begin(), mSkyboxMaterials.end(), PropertyOption::comp_func);

    } catch(...) {
        Ogre::LogManager::getSingleton().getDefaultLog()->logMessage("OGITOR EXCEPTION: Can not prepare project resources!!", Ogre::LML_CRITICAL);
    }
}
//-----------------------------------------------------------------------------------------

bool Ogitors::ResourceLoadingListener::resourceCollision(Ogre::Resource *resource, Ogre::ResourceManager *resourceManager)
{
    // If there is a naming collision, remove the previously loaded resource
    // and replace it with the one that is currently attempted to be read
    Ogre::ResourceManager::ResourceMapIterator it = resourceManager->getResourceIterator();
    while(it.hasMoreElements())
    {
        Ogre::ResourcePtr resPrt = (Ogre::ResourcePtr)it.getNext();
        if(resPrt->getName() == resource->getName())
        {
            resourceManager->remove(resPrt);
            break;
        }
    }

    return true;
}
//-----------------------------------------------------------------------------------------
typedef std::map<Ogre::Archive*, Ogre::Archive*> ArchiveMap;

void OgitorsRoot::DestroyResourceGroup(const Ogre::String& resGrpName)
{
    ArchiveMap arcMap;

    Ogre::ResourceGroupManager *resMgr = Ogre::ResourceGroupManager::getSingletonPtr();
    
    const Ogre::ResourceGroupManager::LocationList& list = resMgr->getResourceLocationList(resGrpName);

    Ogre::ResourceGroupManager::LocationList::const_iterator it = list.begin();

    while(it != list.end())
    {
        arcMap.insert(ArchiveMap::value_type((*it)->archive, (*it)->archive));
        it++;
    }
    
    resMgr->destroyResourceGroup(resGrpName);

    ArchiveMap::iterator ait = arcMap.begin();

    while(ait != arcMap.end())
    {
        Ogre::ArchiveManager::getSingletonPtr()->unload(ait->first);
        ait++;
    }
}
//-----------------------------------------------------------------------------------------
