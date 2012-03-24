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
#include "SceneManagerEditor.h"
#include "tinyxml.h"
#include "ofs.h"

using namespace Ogitors;

Ogre::String   OgitorsUtils::mExePath = "";
unsigned int   OgitorsUtils::mVertexBufferSize = 0;
unsigned int   OgitorsUtils::mIndexBufferSize = 0;
Ogre::Vector3 *OgitorsUtils::mVertexBuffer = 0;
unsigned long *OgitorsUtils::mIndexBuffer = 0;


#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#include <CoreFoundation/CoreFoundation.h>
#endif

//-----------------------------------------------------------------------------------------
Ogre::String OgitorsUtils::GetMacBundlePath()
{
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
    char path[1024];
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    assert(mainBundle);
    
    CFURLRef mainBundleURL = CFBundleCopyBundleURL(mainBundle);
    assert(mainBundleURL);
    
    CFStringRef cfStringRef = CFURLCopyFileSystemPath(mainBundleURL, kCFURLPOSIXPathStyle);
    assert(cfStringRef);
    
    CFStringGetCString(cfStringRef, path, 1024, kCFStringEncodingASCII);
    
    CFRelease(mainBundleURL);
    CFRelease(cfStringRef);
    
    return Ogre::String(path);
#else
    return "";
#endif
}
//-----------------------------------------------------------------------------------------
Ogre::String OgitorsUtils::GetEditorResourcesPath()
{
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
	  return QualifyPath(GetMacBundlePath() + "/Contents/Resources/Media/EditorResources");
#else
	  return QualifyPath("../Media/EditorResources");
#endif
}
//-----------------------------------------------------------------------------------------
void OgitorsUtils::ParseStringVector(Ogre::String &str, Ogre::StringVector &list)
{
    list.clear();
    Ogre::StringUtil::trim(str,true,true);
    if(str == "") 
        return;

    int pos = str.find(";");
    while(pos != -1)
    {
        list.push_back(str.substr(0,pos));
        str.erase(0,pos + 1);
        pos = str.find(";");
    }
  
    if(str != "") 
        list.push_back(str);
}
//-----------------------------------------------------------------------------------------
void OgitorsUtils::ParseUTFStringVector(Ogre::UTFString &str, UTFStringVector &list)
{
    list.clear();
    if(str == "") 
        return;

    int pos = str.find(";");
    while(pos != -1)
    {
        list.push_back(str.substr(0,pos));
        str.erase(0,pos + 1);
        pos = str.find(";");
    }
  
    if(str != "") 
        list.push_back(str);
}
//-----------------------------------------------------------------------------------------
void OgitorsUtils::CleanPath(Ogre::String &path)
{
    std::replace( path.begin(), path.end(), '\\', '/' );

    int pos = 0;
    pos = path.find("//",0);
    while(pos != -1)
    {
        path.erase(pos,1);
        pos = path.find("//",0);
    }
    
    pos = path.find("/./",0);
    while(pos != -1)
    {
        path.erase(pos,2);
        pos = path.find("/./",0);
    }
}
//-----------------------------------------------------------------------------------------
Ogre::String OgitorsUtils::ExtractFileName(const Ogre::String &path)
{
    int pos1 = -1, pos2 = -1;
    Ogre::String strName = path;
    pos1 = path.find_last_of("\\");
    pos2 = path.find_last_of("/");

    int pos = std::max(pos1,pos2);

    if(pos != -1)
    {
        strName = path.substr(pos + 1,path.size() - (pos + 1));
    }

    return strName;
}
//-----------------------------------------------------------------------------------------
Ogre::String OgitorsUtils::ExtractFilePath(const Ogre::String &path)
{
    int pos1 = -1, pos2 = -1;
    Ogre::String strName = "";
    pos1 = path.find_last_of("\\");
    pos2 = path.find_last_of("/");

    int pos = std::max(pos1,pos2);

    if(pos != -1)
    {
        strName = path.substr(0,pos + 1);
    }

    return strName;
}
//-----------------------------------------------------------------------------------------
Ogre::String OgitorsUtils::QualifyPath(const Ogre::String &dirname)
{
    Ogre::String path = dirname;
    if(path.substr(0,1) == ".") path = mExePath + "/" + path;

    std::replace(path.begin(),path.end(),'\\','/');
    
    // Remember if there is a leading '/'
    bool leadingSlash = false;
    if(path.substr(0,1) == "/")
        leadingSlash = true;

    Ogre::StringVector list;
    int pos = path.find("/");
    while(pos != -1)
    {
        if(pos > 0 && path.substr(0,pos) != ".")  // Ignore zero strings and same directory pointers
            list.push_back(path.substr(0,pos));
        path.erase(0,pos + 1);
        pos = path.find("/");
    }

    if(path != "") 
        list.push_back(path);

    unsigned int pos2 = 0;
    while(list.size() > pos2)
    {
        if(list[pos2] == "..")
        {
              list.erase(list.begin() + pos2 - 1,list.begin() + pos2 + 1);
            pos2--;
        }
        else
            pos2++;
    }

    if(list.size() == 0) 
        return "";

    path = list[0];

    for(unsigned int i = 1;i < list.size();i++)
    { 
        path += "/" + list[i];
    }
    
    if(leadingSlash)
        path = "/" + path;

    return path;
}
//-----------------------------------------------------------------------------------------
Ogre::String OgitorsUtils::GetRelativePath(const Ogre::String pathFrom,const Ogre::String pathTo)
{
    Ogre::String sFrom = QualifyPath(pathFrom);
    Ogre::String sTo = QualifyPath(pathTo);

    Ogre::String sFromFirst = sFrom.substr(0,1);
    Ogre::String sToFirst = sTo.substr(0,1);
    Ogre::StringUtil::toLowerCase(sFromFirst);
    Ogre::StringUtil::toLowerCase(sToFirst);

    if( sFromFirst != sToFirst) return sTo;

    Ogre::StringVector listfrom;
    int pos = sFrom.find("/");
    while(pos != -1)
    {
        listfrom.push_back(sFrom.substr(0,pos));
        sFrom.erase(0,pos + 1);
        pos = sFrom.find("/");
    }
    
    if(sFrom != "") 
        listfrom.push_back(sFrom);

    Ogre::StringVector listto;
    pos = sTo.find("/");
    while(pos != -1)
    {
        listto.push_back(sTo.substr(0,pos));
        sTo.erase(0,pos + 1);
        pos = sTo.find("/");
    }
    
    if(sTo != "") 
        listto.push_back(sTo);

    unsigned int length = std::min(listfrom.size(), listto.size());

    unsigned int i;
    for(i = 0;i < length;i++)
    {
        Ogre::String valFrom = listfrom[i];
        Ogre::String valTo = listto[i];
        Ogre::StringUtil::toLowerCase(valFrom);
        Ogre::StringUtil::toLowerCase(valTo);
        if(valTo != valFrom) 
            break;
    }

    listfrom.erase(listfrom.begin(),listfrom.begin() + i);
    listto.erase(listto.begin(),listto.begin() + i);

    if(listfrom.size() == 0 && listto.size() == 0) 
        return "./";

    Ogre::String strRet = "";
    for(i = 0;i < listfrom.size();i++) 
        strRet += "../";

    if(strRet.length() == 0) 
        strRet = "./";

    for(i = 0;i < listto.size();i++) 
        strRet += listto[i] + "/";

    if(listto.size())
    {
        strRet.erase(strRet.size() - 1,1);
    }
    return strRet;
}
//-----------------------------------------------------------------------------------------
void OgitorsUtils::GetMeshData(const Ogre::MeshPtr mesh, size_t &vertex_count, size_t &index_count,
                               const Ogre::Vector3 &position, const Ogre::Quaternion &orient, const Ogre::Vector3 &scale)
{
    bool added_shared = false;
    size_t current_offset = 0;
    size_t shared_offset = 0;
    size_t next_offset = 0;
    size_t index_offset = 0;

    vertex_count = index_count = 0;

    // Calculate how many vertices and indices we're going to need
    for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
    {
        Ogre::SubMesh* submesh = mesh->getSubMesh( i );

        // We only need to add the shared vertices once
        if(submesh->useSharedVertices)
        {
            if( !added_shared )
            {
                vertex_count += mesh->sharedVertexData->vertexCount;
                added_shared = true;
            }
        }
        else
        {
            vertex_count += submesh->vertexData->vertexCount;
        }

        // Add the indices
        index_count += submesh->indexData->indexCount;
    }


    if(vertex_count > mVertexBufferSize)
    {
        OGRE_FREE(mVertexBuffer, Ogre::MEMCATEGORY_GEOMETRY);
        mVertexBuffer = OGRE_ALLOC_T(Ogre::Vector3,vertex_count, Ogre::MEMCATEGORY_GEOMETRY);
        mVertexBufferSize = vertex_count;
    }

    if(index_count > mIndexBufferSize)
    {
        OGRE_FREE(mIndexBuffer, Ogre::MEMCATEGORY_GEOMETRY);
        mIndexBuffer = OGRE_ALLOC_T(unsigned long,index_count, Ogre::MEMCATEGORY_GEOMETRY);
        mIndexBufferSize = index_count;
    }

    added_shared = false;

    // Run through the submeshes again, adding the data into the arrays
    for ( unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
    {
        Ogre::SubMesh* submesh = mesh->getSubMesh(i);

        Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;

        if((!submesh->useSharedVertices)||(submesh->useSharedVertices && !added_shared))
        {
            if(submesh->useSharedVertices)
            {
              added_shared = true;
              shared_offset = current_offset;
            }

            const Ogre::VertexElement* posElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);

            Ogre::HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());

            unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

            // There is _no_ baseVertexPointerToElement() which takes an Ogre::Real or a double
            //  as second argument. So make it float, to avoid trouble when Ogre::Real will
            //  be comiled/typedefed as double:
            //      Ogre::Real* pReal;
            float* pReal;

            for( size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
            {
                posElem->baseVertexPointerToElement(vertex, &pReal);

                Ogre::Vector3 pt(pReal[0], pReal[1], pReal[2]);

                mVertexBuffer[current_offset + j] = (orient * (pt * scale)) + position;
            }

            vbuf->unlock();
            next_offset += vertex_data->vertexCount;
        }


        Ogre::IndexData* index_data = submesh->indexData;
        size_t numTris = index_data->indexCount / 3;
        Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;

        bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);

        unsigned long*  pLong = static_cast<unsigned long*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
        unsigned short* pShort = reinterpret_cast<unsigned short*>(pLong);

        size_t offset = (submesh->useSharedVertices)? shared_offset : current_offset;

        if ( use32bitindexes )
        {
            for ( size_t k = 0; k < numTris*3; ++k)
            {
                mIndexBuffer[index_offset++] = pLong[k] + static_cast<unsigned long>(offset);
            }
        }
        else
        {
            for ( size_t k = 0; k < numTris*3; ++k)
            {
                mIndexBuffer[index_offset++] = static_cast<unsigned long>(pShort[k]) + static_cast<unsigned long>(offset);
            }
        }

        ibuf->unlock();
        current_offset = next_offset;
    }
    index_count = index_offset;
}
//-----------------------------------------------------------------------------------------
void OgitorsUtils::GetMeshDataEx(const Ogre::Entity *entity, size_t &vertex_count, size_t &index_count,
                               const Ogre::Vector3 &position, const Ogre::Quaternion &orient, const Ogre::Vector3 &scale)
{
    bool added_shared = false;
    size_t current_offset = 0;
    size_t shared_offset = 0;
    size_t next_offset = 0;
    size_t index_offset = 0;

    vertex_count = index_count = 0;

    Ogre::MeshPtr mesh = entity->getMesh();

    // Calculate how many vertices and indices we're going to need
    for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
    {
        Ogre::SubMesh* submesh = mesh->getSubMesh( i );

        // We only need to add the shared vertices once
        if(submesh->useSharedVertices)
        {
            if( !added_shared )
            {
                vertex_count += mesh->sharedVertexData->vertexCount;
                added_shared = true;
            }
        }
        else
        {
            vertex_count += submesh->vertexData->vertexCount;
        }

        // Add the indices
        index_count += submesh->indexData->indexCount;
    }


    if(vertex_count > mVertexBufferSize)
    {
        OGRE_FREE(mVertexBuffer, Ogre::MEMCATEGORY_GEOMETRY);
        mVertexBuffer = OGRE_ALLOC_T(Ogre::Vector3,vertex_count, Ogre::MEMCATEGORY_GEOMETRY);
        mVertexBufferSize = vertex_count;
    }

    if(index_count > mIndexBufferSize)
    {
        OGRE_FREE(mIndexBuffer, Ogre::MEMCATEGORY_GEOMETRY);
        mIndexBuffer = OGRE_ALLOC_T(unsigned long,index_count, Ogre::MEMCATEGORY_GEOMETRY);
        mIndexBufferSize = index_count;
    }

    added_shared = false;

    // Run through the submeshes again, adding the data into the arrays
    for ( unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
    {
        if(!entity->getSubEntity(i)->isVisible())
            continue;

        Ogre::SubMesh* submesh = mesh->getSubMesh(i);

        Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;

        if((!submesh->useSharedVertices)||(submesh->useSharedVertices && !added_shared))
        {
            if(submesh->useSharedVertices)
            {
              added_shared = true;
              shared_offset = current_offset;
            }

            const Ogre::VertexElement* posElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);

            Ogre::HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());

            unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

            // There is _no_ baseVertexPointerToElement() which takes an Ogre::Real or a double
            //  as second argument. So make it float, to avoid trouble when Ogre::Real will
            //  be comiled/typedefed as double:
            //      Ogre::Real* pReal;
            float* pReal;

            for( size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
            {
                posElem->baseVertexPointerToElement(vertex, &pReal);

                Ogre::Vector3 pt(pReal[0], pReal[1], pReal[2]);

                mVertexBuffer[current_offset + j] = (orient * (pt * scale)) + position;
            }

            vbuf->unlock();
            next_offset += vertex_data->vertexCount;
        }


        Ogre::IndexData* index_data = submesh->indexData;
        size_t numTris = index_data->indexCount / 3;
        Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;

        bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);

        unsigned long*  pLong = static_cast<unsigned long*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
        unsigned short* pShort = reinterpret_cast<unsigned short*>(pLong);

        size_t offset = (submesh->useSharedVertices)? shared_offset : current_offset;

        if ( use32bitindexes )
        {
            for ( size_t k = 0; k < numTris*3; ++k)
            {
                mIndexBuffer[index_offset++] = pLong[k] + static_cast<unsigned long>(offset);
            }
        }
        else
        {
            for ( size_t k = 0; k < numTris*3; ++k)
            {
                mIndexBuffer[index_offset++] = static_cast<unsigned long>(pShort[k]) + static_cast<unsigned long>(offset);
            }
        }

        ibuf->unlock();
        current_offset = next_offset;
    }
    index_count = index_offset;
}
//-----------------------------------------------------------------------------------------
bool OgitorsUtils::PickEntity(Ogre::Ray &ray, Ogre::Entity **result, Ogre::Vector3 &hitpoint, const Ogre::String& excludeobject, Ogre::Real max_distance)
{
    Ogre::RaySceneQuery *mRaySceneQuery = OgitorsRoot::getSingletonPtr()->GetSceneManagerEditor()->getRayQuery();
    mRaySceneQuery->setRay(ray);
    mRaySceneQuery->setQueryMask(QUERYFLAG_MOVABLE);
    mRaySceneQuery->setSortByDistance(true);

    unsigned int mVisibilityMask = OgitorsRoot::getSingletonPtr()->GetSceneManager()->getVisibilityMask();
    
    if (mRaySceneQuery->execute().size() <= 0) return (false);

    // at this point we have raycast to a series of different objects bounding boxes.
    // we need to test these different objects to see which is the first polygon hit.
    // there are some minor optimizations (distance based) that mean we wont have to
    // check all of the objects most of the time, but the worst case scenario is that
    // we need to test every triangle of every object.
    Ogre::Real closest_distance = max_distance;
    Ogre::Vector3 closest_result;
    Ogre::RaySceneQueryResult &query_result = mRaySceneQuery->getLastResults();

    for (size_t qr_idx = 0; qr_idx < query_result.size(); qr_idx++)
    {
        // stop checking if we have found a raycast hit that is closer
        // than all remaining entities
        if ((closest_distance >= 0.0f) && (closest_distance < query_result[qr_idx].distance))
            break;
    
        // only check this result if its a hit against an entity
        if ((query_result[qr_idx].movable != NULL) && (query_result[qr_idx].movable->getMovableType().compare("Entity") == 0))
        {
            // get the entity to check
            Ogre::Entity *pentity = static_cast<Ogre::Entity*>(query_result[qr_idx].movable);

            if(!(pentity->getVisibilityFlags() & mVisibilityMask))
                continue;

            if(!pentity->getVisible() || (pentity->getName() == excludeobject)) 
                continue;

			if(pentity->getName() == "SkyXMeshEnt")
				continue;

            // mesh data to retrieve
            size_t vertex_count;
            size_t index_count;

            // get the mesh information
            GetMeshData(pentity->getMesh(), vertex_count, index_count, 
                        pentity->getParentNode()->_getDerivedPosition(),
                        pentity->getParentNode()->_getDerivedOrientation(),
                        pentity->getParentNode()->_getDerivedScale());

            // test for hitting individual triangles on the mesh
            bool new_closest_found = false;
            for (int i = 0; i < static_cast<int>(index_count); i += 3)
            {
                assert(mIndexBuffer[i] < vertex_count);
                assert(mIndexBuffer[i + 1] < vertex_count);
                assert(mIndexBuffer[i + 2] < vertex_count);
                // check for a hit against this triangle
                std::pair<bool, Ogre::Real> hit = Ogre::Math::intersects(ray, mVertexBuffer[mIndexBuffer[i]],
                         mVertexBuffer[mIndexBuffer[i+1]], mVertexBuffer[mIndexBuffer[i+2]], true, false);

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
                (*result) = pentity;
            }
        }
    }

    // return the result
    if (closest_distance != max_distance)
    {
        hitpoint = closest_result;
        return (true);
    }
    else
    {
        // raycast failed
        return (false);
    }
}
//-----------------------------------------------------------------------------------------
bool OgitorsUtils::PickEntity(Ogre::Ray &ray, Ogre::Entity **result, Ogre::Vector3 &hitpoint, const Ogre::StringVector& excludeobjects, Ogre::Real max_distance)
{
    Ogre::RaySceneQuery *mRaySceneQuery = OgitorsRoot::getSingletonPtr()->GetSceneManagerEditor()->getRayQuery();
    mRaySceneQuery->setRay(ray);
    mRaySceneQuery->setQueryMask(QUERYFLAG_MOVABLE);
    mRaySceneQuery->setSortByDistance(true);

    unsigned int mVisibilityMask = OgitorsRoot::getSingletonPtr()->GetSceneManager()->getVisibilityMask();

    if (mRaySceneQuery->execute().size() <= 0) return (false);

    // at this point we have raycast to a series of different objects bounding boxes.
    // we need to test these different objects to see which is the first polygon hit.
    // there are some minor optimizations (distance based) that mean we wont have to
    // check all of the objects most of the time, but the worst case scenario is that
    // we need to test every triangle of every object.
    Ogre::Real closest_distance = max_distance;
    Ogre::Vector3 closest_result;
    Ogre::RaySceneQueryResult &query_result = mRaySceneQuery->getLastResults();
    
    for (size_t qr_idx = 0; qr_idx < query_result.size(); qr_idx++)
    {
        // stop checking if we have found a raycast hit that is closer
        // than all remaining entities
        if ((closest_distance >= 0.0f) && (closest_distance < query_result[qr_idx].distance))
        {
            break;
        }

        // only check this result if its a hit against an entity
        if ((query_result[qr_idx].movable != NULL) && (query_result[qr_idx].movable->getMovableType().compare("Entity") == 0))
        {
            // get the entity to check
            Ogre::Entity *pentity = static_cast<Ogre::Entity*>(query_result[qr_idx].movable);

            if(!(pentity->getVisibilityFlags() & mVisibilityMask))
                continue;

            if(!pentity->getVisible()) 
                continue;

            Ogre::String pname = pentity->getName();

            bool foundinlist = false;
            for(unsigned int lit = 0;lit < excludeobjects.size();lit++)
            {
                if(excludeobjects[lit] == pname)
                {
                    foundinlist = true;
                    break;
                }
            }
            
            if(foundinlist)
                continue;

            // mesh data to retrieve
            size_t vertex_count;
            size_t index_count;

            // get the mesh information
            GetMeshData(pentity->getMesh(), vertex_count, index_count, 
                        pentity->getParentNode()->_getDerivedPosition(),
                        pentity->getParentNode()->_getDerivedOrientation(),
                        pentity->getParentNode()->_getDerivedScale());

            // test for hitting individual triangles on the mesh
            bool new_closest_found = false;
            for (int i = 0; i < static_cast<int>(index_count); i += 3)
            {
                // check for a hit against this triangle
                std::pair<bool, Ogre::Real> hit = Ogre::Math::intersects(ray, mVertexBuffer[mIndexBuffer[i]],
                         mVertexBuffer[mIndexBuffer[i+1]], mVertexBuffer[mIndexBuffer[i+2]], true, false);

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
                (*result) = pentity;
            }
        }
    }

    // return the result
    if (closest_distance != max_distance)
    {
        hitpoint = closest_result;
        return (true);
    }
    else
    {
        // raycast failed
        return (false);
    }
}
//-----------------------------------------------------------------------------------------
int OgitorsUtils::PickSubMesh(Ogre::Ray& ray, Ogre::Entity* pEntity)
{
    // at this point we have raycast to a series of different objects bounding boxes.
    // we need to test these different objects to see which is the first polygon hit.
    // there are some minor optimizations (distance based) that mean we wont have to
    // check all of the objects most of the time, but the worst case scenario is that
    // we need to test every triangle of every object.
    Ogre::Real closest_distance = -1.0f;
    int closest_index = -1;
    Ogre::Vector3 closest_result;
    int closest_submesh = -1;

    // mesh data to retrieve
    size_t vertex_count;
    size_t index_count;

    // get the mesh information
    GetMeshData(pEntity->getMesh(), vertex_count, index_count,
                pEntity->getParentNode()->_getDerivedPosition(),
                pEntity->getParentNode()->_getDerivedOrientation(),
                pEntity->getParentNode()->_getDerivedScale());

    // test for hitting individual triangles on the mesh
    for (int i = 0; i < static_cast<int>(index_count); i += 3)
    {
        // check for a hit against this triangle
        std::pair<bool, Ogre::Real> hit = Ogre::Math::intersects(ray, mVertexBuffer[mIndexBuffer[i]],
                 mVertexBuffer[mIndexBuffer[i+1]], mVertexBuffer[mIndexBuffer[i+2]], true, false);

        // if it was a hit check if its the closest
        if (hit.first)
        {
            if ((closest_distance < 0.0f) || (hit.second < closest_distance))
            {
                // this is the closest so far, save it off
                closest_distance = hit.second;
                closest_index = i;
            }
        }
    }

    if(closest_index > -1)
    {
        int index_pos = 0;
        for (unsigned short sm = 0; sm < pEntity->getMesh()->getNumSubMeshes(); ++sm)
        {
            index_pos += pEntity->getMesh()->getSubMesh( sm )->indexData->indexCount;
            if(closest_index < index_pos)
                return sm;
        }
    }
    return -1;
}
//-----------------------------------------------------------------------------------------
bool OgitorsUtils::WorldIntersect(Ogre::Ray &ray, Ogre::Vector3 &hitposition)
{
    Ogre::RaySceneQuery *mRaySceneQuery = OgitorsRoot::getSingletonPtr()->GetSceneManagerEditor()->getRayQuery();
    mRaySceneQuery->setRay(ray);
    mRaySceneQuery->setQueryTypeMask(Ogre::SceneManager::WORLD_GEOMETRY_TYPE_MASK);
    mRaySceneQuery->setWorldFragmentType(Ogre::SceneQuery::WFT_SINGLE_INTERSECTION);
    Ogre::RaySceneQueryResult& qryResult = mRaySceneQuery->execute();
    Ogre::RaySceneQueryResult::iterator i = qryResult.begin();
    if (i != qryResult.end() && i->worldFragment)
    {
        hitposition = i->worldFragment->singleIntersection;
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------------------
void OgitorsUtils::SphereQuery(CBaseEditor *object, Ogre::Real radius, ObjectVector &results)
{
    results.clear();

    if(!object)
        return;
    
    Ogre::Sphere sphere(object->getDerivedPosition(), radius);
    Ogre::SphereSceneQuery *mSphereQuery = OgitorsRoot::getSingletonPtr()->GetSceneManagerEditor()->getSphereQuery();
    mSphereQuery->setSphere(sphere);
    mSphereQuery->setQueryMask(QUERYFLAG_MOVABLE);

    unsigned int mVisibilityMask = OgitorsRoot::getSingletonPtr()->GetSceneManager()->getVisibilityMask();

    Ogre::SceneQueryResult& qryResult = mSphereQuery->execute();
    Ogre::SceneQueryResultMovableList::iterator it;
    Ogre::SceneQueryResultMovableList::iterator it_end = qryResult.movables.end();
    
    for(it = qryResult.movables.begin();it != it_end;it++)
    {
        Ogre::MovableObject *entity = (*it);

        if (entity->getMovableType().compare("Entity") == 0)
        {
            // get the entity to check
            Ogre::Entity *pentity = static_cast<Ogre::Entity*>(entity);

            if(!pentity->getVisible() || !(pentity->getVisibilityFlags() & mVisibilityMask))
                continue;

            CBaseEditor *fobject = OgitorsRoot::getSingletonPtr()->FindObject(pentity->getName());

            if(fobject != 0 && fobject != object)
                results.push_back(fobject);
        }
    }
}
//----------------------------------------------------------------------------------------
void OgitorsUtils::SphereQuery(const Ogre::Vector3& pos, Ogre::Real radius, ObjectVector &results)
{
    results.clear();

    Ogre::SphereSceneQuery *mSphereQuery = OgitorsRoot::getSingletonPtr()->GetSceneManagerEditor()->getSphereQuery();
    Ogre::Sphere sphere(pos, radius);
    mSphereQuery->setSphere(sphere);
    mSphereQuery->setQueryMask(QUERYFLAG_MOVABLE);

    unsigned int mVisibilityMask = OgitorsRoot::getSingletonPtr()->GetSceneManager()->getVisibilityMask();

    Ogre::SceneQueryResult& qryResult = mSphereQuery->execute();
    Ogre::SceneQueryResultMovableList::iterator it;
    Ogre::SceneQueryResultMovableList::iterator it_end = qryResult.movables.end();
    
    for(it = qryResult.movables.begin();it != it_end;it++)
    {
        Ogre::MovableObject *entity = (*it);

        if (entity->getMovableType().compare("Entity") == 0)
        {
            // get the entity to check
            Ogre::Entity *pentity = static_cast<Ogre::Entity*>(entity);

            if(!pentity->getVisible() || !(pentity->getVisibilityFlags() & mVisibilityMask))
                continue;

            CBaseEditor *fobject = OgitorsRoot::getSingletonPtr()->FindObject(pentity->getName());

            if(fobject)
                results.push_back(fobject);
        }
    }
}
//----------------------------------------------------------------------------------------
Ogre::String OgitorsUtils::GetValueString(OgitorsPropertyValue& value)
{
    switch(value.propType)
    {
    case PROP_SHORT:
        return Ogre::StringConverter::toString(Ogre::any_cast<short>(value.val));
    case PROP_UNSIGNED_SHORT:
        return Ogre::StringConverter::toString(Ogre::any_cast<unsigned short>(value.val));
    case PROP_INT:
        return Ogre::StringConverter::toString(Ogre::any_cast<int>(value.val));
    case PROP_UNSIGNED_INT:
        return Ogre::StringConverter::toString(Ogre::any_cast<unsigned int>(value.val));
    case PROP_LONG:
        return Ogre::StringConverter::toString(Ogre::any_cast<long>(value.val));
    case PROP_UNSIGNED_LONG:
        return Ogre::StringConverter::toString(Ogre::any_cast<unsigned long>(value.val));
    case PROP_REAL:
        return Ogre::StringConverter::toString(Ogre::any_cast<Ogre::Real>(value.val));
    case PROP_STRING:
        return Ogre::any_cast<Ogre::String>(value.val);
    case PROP_VECTOR2:
        return Ogre::StringConverter::toString(Ogre::any_cast<Ogre::Vector2>(value.val));
    case PROP_VECTOR3:
        return Ogre::StringConverter::toString(Ogre::any_cast<Ogre::Vector3>(value.val));
    case PROP_VECTOR4:
        return Ogre::StringConverter::toString(Ogre::any_cast<Ogre::Vector4>(value.val));
    case PROP_COLOUR:
        return Ogre::StringConverter::toString(Ogre::any_cast<Ogre::ColourValue>(value.val));
    case PROP_BOOL:
        return Ogre::StringConverter::toString(Ogre::any_cast<bool>(value.val));
    case PROP_QUATERNION:
        return Ogre::StringConverter::toString(Ogre::any_cast<Ogre::Quaternion>(value.val));
    case PROP_MATRIX3:
        return Ogre::StringConverter::toString(Ogre::any_cast<Ogre::Matrix3>(value.val));
    case PROP_MATRIX4:
        return Ogre::StringConverter::toString(Ogre::any_cast<Ogre::Matrix4>(value.val));
    };
    return "";
}
//----------------------------------------------------------------------------------------
Ogre::String OgitorsUtils::GetCustomPropertySaveString(OgitorsCustomPropertySet *set, int indentation)
{
    Ogre::String buffer = "";

    if(!set)
        return "";

    Ogre::String indentStr = "";

    for(int k = 0; k < indentation;k++)
        indentStr += " ";
    
    buffer += indentStr + "<CUSTOMPROPERTIES>\n";
    OgitorsPropertyVector vec = set->getPropertyVector();

    for(unsigned int i = 0;i < vec.size();i++)
    {
        OgitorsPropertyBase *property = vec[i];
        const OgitorsPropertyDef *def = property->getDefinition();
        OgitorsPropertyValue value;
        value.propType = property->getType();
        value.val = property->getValue();

        buffer += indentStr + "  <PROPERTY id=\"" + property->getName() + "\"";
        buffer += " type=\"" + Ogre::StringConverter::toString(value.propType) + "\"";
        buffer += " value=\"" + GetValueString(value) + "\"";
        
        
        switch(value.propType)
        {
        case PROP_VECTOR2:
            {
                if(def->fieldName(0) != "X" || def->fieldName(1) != "Y")
                    buffer += " fields=\"" + def->fieldName(0) + ";" + def->fieldName(1) + "\"";
                break;
            }
        case PROP_VECTOR3:
            {
                if(def->fieldName(0) != "X" || def->fieldName(1) != "Y" || def->fieldName(2) != "Z")
                    buffer += " fields=\"" + def->fieldName(0) + ";" + def->fieldName(1) + ";" + def->fieldName(2) + "\"";
                break;
            }
        case PROP_VECTOR4:
            {
                if(def->fieldName(0) != "X" || def->fieldName(1) != "Y" || def->fieldName(2) != "Z" || def->fieldName(0) != "W")
                    buffer += " fields=\"" + def->fieldName(0) + ";" + def->fieldName(1) + ";" + def->fieldName(2) + ";" + def->fieldName(3) + "\"";
                break;
            }
        }

        buffer += " autooptiontype=\"" + Ogre::StringConverter::toString(def->getAutoOptionType()) + "\"";

        const PropertyOptionsVector *opt = def->getOptions();

        if(def->getAutoOptionType() == AUTO_OPTIONS_NONE && opt != NULL)
        {
            buffer += " options=\"";
            for(unsigned int o = 0;o < (*opt).size();o++)
            {
                value.val = (*opt)[o].mValue;
                buffer += (*opt)[o].mKey + ";";
                buffer += GetValueString(value) + ";";
            }
            buffer += "\"";
        }

        buffer += "></PROPERTY>\n";
    }
    
    buffer += indentStr + "</CUSTOMPROPERTIES>\n";
    return buffer;
}
//----------------------------------------------------------------------------------------
void OgitorsUtils::ReadCustomPropertySet(TiXmlElement *element, OgitorsCustomPropertySet *set)
{
    Ogre::String elementName;

    TiXmlElement *properties = element->FirstChildElement();
    if(!properties)
        return;

    OgitorsPropertyDef *def;
    do
    {
        elementName = properties->Value();
        if(elementName != "PROPERTY")
            continue;

        Ogre::String attID = ValidAttr(properties->Attribute("id"),"");
        int attType = Ogre::StringConverter::parseInt(ValidAttr(properties->Attribute("type"),""));
        Ogre::String attValue = ValidAttr(properties->Attribute("value"),"");
        Ogre::String attFields = ValidAttr(properties->Attribute("fields"),"");
        Ogre::String attOptions = ValidAttr(properties->Attribute("options"),"");
        AutoOptionType attAutoOptionType = (AutoOptionType)Ogre::StringConverter::parseInt(ValidAttr(properties->Attribute("autooptiontype"),"0"));
        Ogre::StringVector svec;

        OgitorsUtils::ParseStringVector(attFields, svec);

        def = set->addProperty(attID, OgitorsPropertyValue::createFromString((OgitorsPropertyType)attType, attValue));
        for(unsigned int i = 0;i < svec.size();i++)
            def->setFieldName(i, svec[i]);

        def->setAutoOptionType(attAutoOptionType);

        if(attAutoOptionType == AUTO_OPTIONS_NONE)
        {
            OgitorsUtils::ParseStringVector(attOptions, svec);
            if(svec.size() > 1)
            {
                PropertyOptionsVector *options = new PropertyOptionsVector();
                for(unsigned int o = 0;o < (svec.size() - 1);o += 2)
                {
                    OgitorsPropertyValue propVal = OgitorsPropertyValue::createFromString(def->getType(), svec[o + 1]);
                    options->push_back(PropertyOption(svec[o], propVal.val));
                }
                def->setOptions(options);
            }
        }
        else if(attAutoOptionType == AUTO_OPTIONS_MATERIAL)
            def->setOptions(OgitorsRoot::GetMaterialNames());
        else if(attAutoOptionType == AUTO_OPTIONS_MESH)
            def->setOptions(OgitorsRoot::GetModelNames());

    } while(properties = properties->NextSiblingElement());
}
//----------------------------------------------------------------------------------------
Ogre::String OgitorsUtils::GetUserDataSaveString(OgitorsCustomPropertySet *set, int indentation)
{
    Ogre::String buffer = "";

    if(!set)
        return "";

    Ogre::String indentStr = "";
    OgitorsPropertyVector vec = set->getPropertyVector();
    if(vec.size() < 1)
        return "";

    for(int k = 0; k < indentation;k++)
        indentStr += " ";
    
    buffer += indentStr + "<userData>\n";

    for(unsigned int i = 0;i < vec.size();i++)
    {
        OgitorsPropertyBase *property = vec[i];
        const OgitorsPropertyDef *def = property->getDefinition();
        OgitorsPropertyValue value;
        value.propType = property->getType();
        value.val = property->getValue();

        buffer += indentStr + "  <property type=\"" + Ogre::StringConverter::toString(value.propType) + "\"";
        buffer += " name=\"" + property->getName() + "\"";
        buffer += " data=\"" + GetValueString(value) + "\"></property>\n";
    }
    
    buffer += indentStr + "</userData>\n";
    return buffer;
}
//----------------------------------------------------------------------------------------
Ogre::String OgitorsUtils::GetObjectSaveStringV2(CBaseEditor *object, int indentation, bool useobjid, bool addparent)
{
    Ogre::String outStr;
    Ogre::String indentStr = "";

    for(int k = 0; k < indentation;k++)
        indentStr += " ";
    
    outStr = indentStr + "<OBJECT";
    if(useobjid)
    {
        outStr += " object_id=\"" + Ogre::StringConverter::toString(object->getObjectID()) + "\"";
    }
    outStr += " name=\"" + object->getName() + "\"";
    outStr += " typename=\"" + object->getTypeName() + "\"";
    // If Object's parent name is "" then the parent is mRootEditor
    if(object->getParent()->getName() != "" && addparent)
    {
        outStr += " parentnode=\"" + object->getParent()->getName() + "\"";
    }
    outStr += ">\n";
    
    OgitorsPropertyValueMap theList;
    OgitorsPropertyValueMap::iterator ni;

    object->getPropertyMap(theList);

    theList.erase(theList.find("name"));
    theList.erase(theList.find("typename"));
    theList.erase(theList.find("object_id"));

    ni = theList.begin();
    while(ni != theList.end())
    {
        outStr += indentStr + "  <PROPERTY id=\"" + ni->first + "\"";
        outStr += " type=\"" + Ogre::StringConverter::toString(ni->second.propType) + "\"";
        outStr += " value=\"" + GetValueString(ni->second) + "\"></PROPERTY>\n";
        ni++;
    }
    outStr += GetCustomPropertySaveString(object->getCustomProperties(), indentation + 2).c_str();
    outStr += indentStr + "</OBJECT>\n";
    return outStr;
}
//----------------------------------------------------------------------------------------
bool OgitorsUtils::SaveImageOfs(Ogre::Image& image, Ogre::String filename)
{
    int dotpos = filename.find_last_of(".");
    Ogre::String extension = filename.substr(dotpos, filename.length() - dotpos);
    extension.erase(0,1);

    Ogre::DataStreamPtr imgDataPtr = image.encode(extension);

    OFS::OfsPtr& mFile = OgitorsRoot::getSingletonPtr()->GetProjectFile();

    unsigned int file_size = imgDataPtr->size();
    
    char *data = new char[file_size];

    imgDataPtr->seek(0);
    imgDataPtr->read(data, file_size);

    OFS::OFSHANDLE handle;

    try
    {
        mFile->createFile(handle, filename.c_str(), file_size, file_size, data);
        mFile->closeFile(handle);
    }
    catch(...)
    {
        delete [] data;
        return false;
    }

    delete [] data;

    return true;
}
//----------------------------------------------------------------------------------------
bool OgitorsUtils::SaveStreamOfs(std::stringstream& stream, Ogre::String filename)
{
        OFS::OFSHANDLE handle;

        OFS::OfsPtr& mFile = OgitorsRoot::getSingletonPtr()->GetProjectFile();

        unsigned int file_size = stream.str().size();

        OFS::OfsResult ret = mFile->createFile(handle, filename.c_str(), file_size, file_size, stream.str().c_str());

        if(ret != OFS::OFS_OK)
            return false;
        
        mFile->closeFile(handle);

        return true;
}
//----------------------------------------------------------------------------------------
#define MAX_BUFFER_SIZE (16 * 1024 * 1024)

bool OgitorsUtils::CopyDirOfs(Ogre::String dirpath, Ogre::String ofs_path)
{
    OFS::OfsPtr& mFile = OgitorsRoot::getSingletonPtr()->GetProjectFile();

    OFS::OFSHANDLE fhandle;

    dirpath = QualifyPath(dirpath);

    Ogre::StringVector filelist;

    OgitorsSystem::getSingletonPtr()->GetFileList(dirpath + "/*.*", filelist);

    char *tmp_buffer = new char[MAX_BUFFER_SIZE];

    for(unsigned int i = 0;i < filelist.size();i++)
    {
        Ogre::String path = filelist[i];
        Ogre::String file_ofs_path = ofs_path + ExtractFileName(filelist[i]);
            
        std::ifstream stream;
        stream.open(path.c_str(), std::fstream::in | std::fstream::binary);
            
        if(stream.is_open())
        {
            stream.seekg(0, std::fstream::end);
            unsigned int stream_size = stream.tellg();

            stream.seekg(0, std::fstream::beg);
 
            if(stream_size >= MAX_BUFFER_SIZE)
            {
                // CreateFile accepts initial data to be written during allocation
                // It's an optimization, that's why we don't have to call Ofs:write after createFile
                stream.read(tmp_buffer, MAX_BUFFER_SIZE);
                try
                {
                    if(mFile->createFile(fhandle, file_ofs_path.c_str(), MAX_BUFFER_SIZE, MAX_BUFFER_SIZE, tmp_buffer) != OFS::OFS_OK)
                    {
                        stream.close();
                        continue;
                    }

                    stream_size -= MAX_BUFFER_SIZE;
                }
                catch(OFS::Exception&)
                {
                    stream.close();
                    continue;
                }

                try
                {
                    while(stream_size > 0)
                    {
                        if(stream_size >= MAX_BUFFER_SIZE)
                        {
                            stream.read(tmp_buffer, MAX_BUFFER_SIZE);
                            mFile->write(fhandle, tmp_buffer, MAX_BUFFER_SIZE);
                            stream_size -= MAX_BUFFER_SIZE;
                        }
                        else
                        {
                            stream.read(tmp_buffer, stream_size);
                            mFile->write(fhandle, tmp_buffer, stream_size);
                            stream_size = 0;
                        }
                    }
                }
                catch(OFS::Exception&)
                {
                }

                stream.close();
                mFile->closeFile(fhandle);
            }
            else
            {
                stream.read(tmp_buffer, stream_size);

                try
                {
                    // CreateFile accepts initial data to be written during allocation
                    // It's an optimization, that's why we don't have to call Ofs:write after createFile
                    if(mFile->createFile(fhandle, file_ofs_path.c_str(), stream_size, stream_size, tmp_buffer) != OFS::OFS_OK)
                    {
                        stream.close();
                        continue;
                    }
                }
                catch(OFS::Exception&)
                {
                }

                stream.close();
                mFile->closeFile(fhandle);
            }
        }
    }

    delete [] tmp_buffer;

    filelist.clear();

    OgitorsSystem::getSingletonPtr()->GetDirList(dirpath + "/", filelist);

    for(unsigned int i = 0;i < filelist.size();i++)
    {
        Ogre::String dir_name = ofs_path + filelist[i] + "/"; 
        mFile->createDirectory(dir_name.c_str());
        CopyDirOfs(dirpath + "/" + filelist[i], dir_name);
    }

    return true;
}
//----------------------------------------------------------------------------------------
