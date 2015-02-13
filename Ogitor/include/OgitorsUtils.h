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

/*////////////////////////////////////////////////////////////////////////////////
//This Header is used to define any library wide structs and enums and defines
////////////////////////////////////////////////////////////////////////////////*/

#pragma once

namespace Ogitors
{
    class OgitorExport OgitorsUtils
    {
    public:
        /**
        * Sets full path to the Ogitor' executable
        * @param path full path to the Ogitor' executable
        */
        static void SetExePath(const Ogre::String &path) { mExePath = path; } 
        /**
        * Fetches full path to the Ogitor' executable
        * @return full path to the Ogitor' executable
        */
        static Ogre::String GetExePath() { return mExePath; }
        /**
        * Fetches mac bundle path
        * @return full path to the mac bundle
        */
        static Ogre::String GetMacBundlePath();
        /**
        * Fetches full path to Editor Resources Directory
        * @return full path to Editor Resources Directory
        */
        static Ogre::String GetEditorResourcesPath();
        /**
        * Fetches full path to specified directory
        * @param dirname directory name
        * @return full path to specified directory
        * @todo Recommend renaming to GetFullPath
        */
        static Ogre::String QualifyPath(const Ogre::String &dirname);
        /**
        * Fetches relative path from one directory to another
        * @param pathFrom originating path
        * @param pathTo destination path
        * @return relative path from origin to destination directories
        */
        static Ogre::String GetRelativePath(const Ogre::String pathFrom,const Ogre::String pathTo);
        /**
        * Parses a string of strings into the list
        * @param str string containing multiple entries 
        * @param list a list to put strings into
        */
        static void ParseStringVector(Ogre::String &str, Ogre::StringVector &list);
        /**
        * Parses a string of strings into the list (UTF based)
        * @param str string containing multiple entries 
        * @param list a list to put strings into
        */
        static void ParseUTFStringVector(Ogre::UTFString &str, UTFStringVector &list);
        /**
        * Cleans a path of unneeded characters/symbols
        * @param path to be cleaned
        */
        static void CleanPath(Ogre::String &path);
        /**
        * Extracts file name from given path
        * @param path path containing the filename
        * @return file name
        * @todo Recommend renaming to GetFileName
        */
        static Ogre::String ExtractFileName(const Ogre::String &path);
        /**
        * Extracts file path from given path
        * @param path path containing the the file path
        * @return file path
        * @todo Recommend renaming to GetFilePath
        */
        static Ogre::String ExtractFilePath(const Ogre::String &path);
        /**
        * Fetches information about OGRE-based mesh
        * @param mesh mesh to retrieve information about
        * @param vertex_count number of vertices to retrieve
        * @param vertices raw Ogre::Vector3 array pointer to mesh' vertices
        * @param index_count number of indices to retrieve
        * @param indices raw ulong array pointer to mesh' indices
        * @param position mesh position to retrieve
        * @param orient mesh orientation to retrieve
        * @param scale mesh scale to retrieve
        */
        static void GetMeshData(const Ogre::MeshPtr mesh, size_t &vertex_count, size_t &index_count,
                                const Ogre::Vector3 &position, const Ogre::Quaternion &orient, const Ogre::Vector3 &scale);
        /**
        * Fetches information about OGRE-based mesh
        * @param entity entity to retrieve information about
        * @param vertex_count number of vertices to retrieve
        * @param vertices raw Ogre::Vector3 array pointer to mesh' vertices
        * @param index_count number of indices to retrieve
        * @param indices raw ulong array pointer to mesh' indices
        * @param position mesh position to retrieve
        * @param orient mesh orientation to retrieve
        * @param scale mesh scale to retrieve
        */
        static void GetMeshDataEx(const Ogre::Entity *entity, size_t &vertex_count, size_t &index_count,
                                const Ogre::Vector3 &position, const Ogre::Quaternion &orient, const Ogre::Vector3 &scale);
        /**
        * Makes a sphere query at object's origin with a given radius
        * @param object the object around which to make sphere query
        * @param radius radius of the query
        * @param results the list of objects returned by query
        */
        static void SphereQuery(CBaseEditor *object, Ogre::Real radius, ObjectVector &results);
        /**
        * Makes a sphere query with a given sphere
        * @param sphere the sphere to check against scene
        * @param results the list of objects returned by query
        */
        static void SphereQuery(const Ogre::Vector3& pos, Ogre::Real radius, ObjectVector &results);
        /**
        * Attempts to pick an entity within scene using ray cast from the mouse
        * @see Ogre::RaySceneQuery
        * @param ray ray cast from the mouse  
        * @see COgitorsRoot::GetMouseRay
        * @param result a result of entities that were intersected with the ray
        * @param hitpoint a closest point of the ray/entity intersection to the camera
        * @param max_distance check only objects closer than this value
        * @param excludeobject the name of the object to exclude from hit test
        * @return true if any entity was intersected with the ray, otherwise false
        */
        static bool PickEntity(Ogre::Ray &ray, Ogre::Entity **result, Ogre::Vector3 &hitpoint, const Ogre::String& excludeobject = "", Ogre::Real max_distance = -1.0f);
        /**
        * Attempts to pick an entity within scene using ray cast from the mouse
        * @see Ogre::RaySceneQuery
        * @param ray ray cast from the mouse  
        * @see COgitorsRoot::GetMouseRay
        * @param result a result of entities that were intersected with the ray
        * @param hitpoint a closest point of the ray/entity intersection to the camera
        * @param max_distance check only objects closer than this value
        * @param excludeobjects the list of names of the objects to exclude from hit test
        * @return true if any entity was intersected with the ray, otherwise false
        */
        static bool PickEntity(Ogre::Ray &ray, Ogre::Entity **result, Ogre::Vector3 &hitpoint, const Ogre::StringVector& excludeobjects, Ogre::Real max_distance = -1.0f);
        /**
        * Attempts to find the index of the submesh containing the point hitpoint
        * @param pEntity the parent Entity Object  
        * @see Ogre::Entity
         * @param hitpoint the location on the Entity
        * @return the index of the submesh containing the hitpoint
        */
        static int PickSubMesh(Ogre::Ray& ray, Ogre::Entity* pEntity);
        /**
        * Test if specified ray has intersected with anything on the scene
        * @param mRaySceneQuery ray scene query object helper 
        * @param ray a ray that is to be tested
        * @param hitposition location of an intersect (if any)
        * @return true if ray had intersected with anything, otherwise false
        */
        static bool WorldIntersect(Ogre::Ray &ray, Ogre::Vector3 &hitposition);
        /**
        * Fetches internal vertex and index buffers
        * @param VertexBuffer address of pointer to vertex buffers to be set
        * @param IndexBuffer address of pointer to index buffers to be set
        */
        static void GetBuffers(Ogre::Vector3 **VertexBuffer, unsigned long **IndexBuffer)
        {
            *VertexBuffer = mVertexBuffer;
            *IndexBuffer = mIndexBuffer;
        }
        /**
        * Get's a list of files in an OGRE resource
        * @param resourceName the name of the OGRE resource to get the name list from
        * @param resourceType optional parameter, whether the resource is from Ofs or FileSystem
        * @param searchName optional parameter, will only return files that match this string
        */
        static Ogitors::PropertyOptionsVector GetResourceFilenames(const Ogre::String &resourceName, const Ogre::String resourceType="", const Ogre::String match="");
        /**
        * Frees internal vertex and index buffers
        */
        static void FreeBuffers()
        {
            OGRE_FREE(mVertexBuffer, Ogre::MEMCATEGORY_GEOMETRY);
            mVertexBuffer = 0;
            mVertexBufferSize = 0;
            OGRE_FREE(mIndexBuffer, Ogre::MEMCATEGORY_GEOMETRY);
            mIndexBuffer = 0;
            mIndexBufferSize = 0;
        }
        /**
        * Returns the value of a property as string
        * @param value the PropertyValue to be converted 
        * @return the string value
        */
        static Ogre::String GetValueString(OgitorsPropertyValue& value);
        /**
        * Returns a string containing XML structure of a custom property set
        * @param set the set that will be used to create XML structure
        * @param indentation space to be left at the beginning of each line
        */
        static Ogre::String GetCustomPropertySaveString(OgitorsCustomPropertySet *set, int indentation);
        /**
        * Returns a string containing XML structure of a custom property set
        * @param set the set that will be used to create XML structure
        * @param indentation space to be left at the beginning of each line
        */
        static void ReadCustomPropertySet(TiXmlElement *element, OgitorsCustomPropertySet *set);
        /**
        * Returns a string containing XML structure of an object
        * @param object the object that will be used to create XML structure
        * @param useobjid will there be a object_id parameter?
        * @param addparent will there be a parent node parameter?
        * @return returns a string containing XML syntax created from the object
        */
        static Ogre::String GetObjectSaveStringV2(CBaseEditor *object, int indentation, bool useobjid, bool addparent);
        /**
        * Returns a string containing XML structure of a custom property set for DotScene Format
        * @param set the set that will be used to create XML structure
        * @param indentation space to be left at the beginning of each line
        */
        static Ogre::String GetUserDataSaveString(OgitorsCustomPropertySet *set, int indentation);
        /**
        * Saves given image to OFS
        * @param image the image file to be saved
        * @param filename the filename to be saved as
        */
        static bool SaveImageOfs(Ogre::Image& image, Ogre::String filename);
        /**
        * Saves given stream to OFS
        * @param stream the stream to be saved
        * @param filename the filename to be saved as
        */
        static bool SaveStreamOfs(std::stringstream& stream, Ogre::String filename);
        /**
        * Copies a file system directory contents into OFS file system
        * @param dirpath path of the file system directory
        * @param ofs_path path of the OFS directory to copy files into
        */
        static bool CopyDirOfs(Ogre::String dirpath, Ogre::String ofs_path);
        /**
        * Copies a file into OFS file system
        * @param filepath path of the file in a system directory
        * @param ofs_dest path of the OFS file to write to
        */
        static bool CopyFileOfs(Ogre::String filepath, Ogre::String ofs_dest);
        
    protected:
        static Ogre::String   mExePath;
        static unsigned int   mVertexBufferSize;
        static unsigned int   mIndexBufferSize;
        static Ogre::Vector3 *mVertexBuffer;
        static unsigned long *mIndexBuffer;

    private:
        OgitorsUtils();
        ~OgitorsUtils();
    };
};
