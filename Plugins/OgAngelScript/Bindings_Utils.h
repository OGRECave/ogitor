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

/*////////////////////////////////////////////////////////////////////////////////
// The main script binding functions
////////////////////////////////////////////////////////////////////////////////*/

#ifndef BINDINGS_UTILS_H
#define BINDINGS_UTILS_H

class asIScriptEngine;

namespace Ogitors
{
    
    //static void GetMeshData(const Ogre::MeshPtr mesh, size_t &vertex_count, size_t &index_count,
    //                        const Ogre::Vector3 &position, const Ogre::Quaternion &orient, const Ogre::Vector3 &scale);
    //static void GetMeshDataEx(const Ogre::Entity *entity, size_t &vertex_count, size_t &index_count,
    //                        const Ogre::Vector3 &position, const Ogre::Quaternion &orient, const Ogre::Vector3 &scale);
    //static int PickSubMesh(Ogre::Ray& ray, Ogre::Entity* pEntity);
    //static bool WorldIntersect(Ogre::Ray &ray, Ogre::Vector3 &hitposition);

    //static Ogre::String GetValueString(OgitorsPropertyValue& value);
    //static void ReadCustomPropertySet(TiXmlElement *element, OgitorsCustomPropertySet *set);
    //-----------------------------------------------------------------------------------------
    static Ogre::String GetExePathImpl(OgitorsUtils& obj)
    {
        return OgitorsUtils::GetExePath();
    }
    //-----------------------------------------------------------------------------------------
    static Ogre::String QualifyPathImpl(const Ogre::String &a1, OgitorsUtils& obj)
    {
        return OgitorsUtils::QualifyPath(a1);
    }
    //-----------------------------------------------------------------------------------------
    static Ogre::String GetRelativePathImpl(const Ogre::String a1, const Ogre::String a2, OgitorsUtils& obj)
    {
        return OgitorsUtils::GetRelativePath(a1, a2);
    }
    //-----------------------------------------------------------------------------------------
    static void ParseStringVectorImpl(Ogre::String a1, Ogre::StringVector &a2, OgitorsUtils& obj)
    {
        OgitorsUtils::ParseStringVector(a1, a2);
    }
    //-----------------------------------------------------------------------------------------   
    static void ParseUTFStringVectorImpl(Ogre::UTFString a1, UTFStringVector &a2, OgitorsUtils& obj)
    {
        OgitorsUtils::ParseUTFStringVector(a1, a2);
    }
    //-----------------------------------------------------------------------------------------
    static void CleanPathImpl(Ogre::String &a1, OgitorsUtils& obj)
    {
        OgitorsUtils::CleanPath(a1);
    }
    //-----------------------------------------------------------------------------------------
    static Ogre::String ExtractFileNameImpl(const Ogre::String &a1, OgitorsUtils& obj)
    {
        return OgitorsUtils::ExtractFileName(a1);
    }
    //-----------------------------------------------------------------------------------------
    static Ogre::String ExtractFilePathImpl(const Ogre::String &a1, OgitorsUtils& obj)
    {
        return OgitorsUtils::ExtractFilePath(a1);
    }
    //-----------------------------------------------------------------------------------------
    static void GetBuffersImpl(Ogre::Vector3 **a1, unsigned long **a2, OgitorsUtils& obj)
    {
        OgitorsUtils::GetBuffers(a1, a2);
    }
    //-----------------------------------------------------------------------------------------
    static void SphereQueryImpl1(CBaseEditor *a1, float a2, ObjectVector &a3, OgitorsUtils& obj)
    {
        OgitorsUtils::SphereQuery(a1, a2, a3);
    }
    //-----------------------------------------------------------------------------------------
    static void SphereQueryImpl2(const Ogre::Vector3 &a1, float a2, ObjectVector &a3, OgitorsUtils& obj)
    {
        OgitorsUtils::SphereQuery(a1, a2, a3);
    }
    //-----------------------------------------------------------------------------------------
    static Ogre::String GetCustomPropertySaveStringImpl(OgitorsCustomPropertySet *a1, int a2, OgitorsUtils& obj)
    {
        return OgitorsUtils::GetCustomPropertySaveString(a1, a2);
    }
    //-----------------------------------------------------------------------------------------
    static Ogre::String GetObjectSaveStringImpl(CBaseEditor *a1, int a2, bool a3, bool a4, OgitorsUtils& obj)
    {
        return OgitorsUtils::GetObjectSaveStringV2(a1, a2, a3, a4);
    }
    //-----------------------------------------------------------------------------------------
    static Ogre::String GetUserDataSaveStringImpl(OgitorsCustomPropertySet *a1, int a2, OgitorsUtils& obj)
    {
        return OgitorsUtils::GetUserDataSaveString(a1, a2);
    }
    //-----------------------------------------------------------------------------------------
    static CBaseEditor *PickImplementation1(Ogre::Ray &ray, Ogre::Vector3 &hitposition, const Ogre::String& exclude, OgitorsUtils &dummy)
    {
        CBaseEditor *result = 0;
        Ogre::Entity *pentity = 0;
        if(OgitorsUtils::PickEntity(ray, &pentity, hitposition, exclude))
        {
            result = OgitorsRoot::getSingletonPtr()->FindObject(pentity->getName());
        }

        return result;
    }
    //-----------------------------------------------------------------------------------------
    static CBaseEditor *PickImplementation2(Ogre::Ray &ray, Ogre::Vector3 &hitposition, const Ogre::StringVector& exclude, OgitorsUtils &dummy)
    {
        CBaseEditor *result = 0;
        Ogre::Entity *pentity = 0;
        if(OgitorsUtils::PickEntity(ray, &pentity, hitposition, exclude))
        {
            result = OgitorsRoot::getSingletonPtr()->FindObject(pentity->getName());
        }

        return result;
    }
    //-----------------------------------------------------------------------------------------
    void RegisterUtilsBindings(asIScriptEngine *engine)
    {
        int r;
        r = engine->RegisterObjectMethod("OgitorsUtils", "string getExePath()", asFUNCTION(GetExePathImpl), asCALL_CDECL_OBJLAST);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsUtils", "string qualifyPath(const string &in)", asFUNCTION(QualifyPathImpl), asCALL_CDECL_OBJLAST);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsUtils", "string getRelativePath(const string, const string)", asFUNCTION(GetRelativePathImpl), asCALL_CDECL_OBJLAST);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsUtils", "void parseStringVector(string &in, StringVector &out)", asFUNCTION(ParseStringVectorImpl), asCALL_CDECL_OBJLAST);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsUtils", "void parseUTFStringVector(UTFString &in, UTFStringVector &out)", asFUNCTION(ParseUTFStringVectorImpl), asCALL_CDECL_OBJLAST);assert(r >= 0);
        //r = engine->RegisterObjectMethod("OgitorsUtils", "void cleanPath(string &inout)", asFUNCTION(CleanPathImpl), asCALL_CDECL_OBJLAST);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsUtils", "string extractFileName(const string &in)", asFUNCTION(ExtractFileNameImpl), asCALL_CDECL_OBJLAST);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsUtils", "string extractFilePath(const string &in)", asFUNCTION(ExtractFilePathImpl), asCALL_CDECL_OBJLAST);assert(r >= 0);

        //r = engine->RegisterObjectMethod("OgitorsUtils", "void getBuffers(Vector3@@, ulong@@)", asFUNCTION(GetBuffersImpl), asCALL_CDECL_OBJLAST);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsUtils", "string getCustomPropertySaveString(CustomPropertySet@, int)", asFUNCTION(GetCustomPropertySaveStringImpl), asCALL_CDECL_OBJLAST);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsUtils", "string getObjectSaveString(BaseEditor@, int, bool, bool)", asFUNCTION(GetObjectSaveStringImpl), asCALL_CDECL_OBJLAST);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsUtils", "string getUserDataSaveString(CustomPropertySet@, int)", asFUNCTION(GetUserDataSaveStringImpl), asCALL_CDECL_OBJLAST);assert(r >= 0);

        r = engine->RegisterObjectMethod("OgitorsUtils", "BaseEditor@ pick(Ray &in, Vector3 &out, const string &in)", asFUNCTION(PickImplementation1), asCALL_CDECL_OBJLAST);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsUtils", "BaseEditor@ pick(Ray &in, Vector3 &out, const StringVector &in)", asFUNCTION(PickImplementation2), asCALL_CDECL_OBJLAST);assert(r >= 0);

        r = engine->RegisterObjectMethod("OgitorsUtils", "void sphereQuery(BaseEditor@, float, EditorVector &out)", asFUNCTION(SphereQueryImpl1), asCALL_CDECL_OBJLAST);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsUtils", "void sphereQuery(const Vector3 &in, float, EditorVector &out)", asFUNCTION(SphereQueryImpl2), asCALL_CDECL_OBJLAST);assert(r >= 0);
    }

};

#endif