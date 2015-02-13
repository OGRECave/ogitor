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
// The main script binding functions
////////////////////////////////////////////////////////////////////////////////*/

#ifndef BINDINGS_ROOT_H
#define BINDINGS_ROOT_H

class asIScriptEngine;

namespace Ogitors
{
    static CBaseEditor *CreateEditorObjectImpl(CBaseEditor *parent, const Ogre::String& objecttypestring, OgitorsRoot* root)
    {
        OgitorsPropertyValueMap params;
        params["init"] = EMPTY_PROPERTY_VALUE;
        return root->CreateEditorObject(parent, objecttypestring, params, true, false); 
    }
    //-----------------------------------------------------------------------------------------
    static CBaseEditor *CreateEntityObjectImpl(CBaseEditor *parent, const Ogre::String& meshfile, OgitorsRoot* root)
    {
        OgitorsPropertyValueMap params;
        params["init"] = EMPTY_PROPERTY_VALUE;
        params["meshfile"] = Ogitors::OgitorsPropertyValue(PROP_STRING , Ogre::Any(meshfile));
        return root->CreateEditorObject(parent, "Entity", params, true, false); 
    }
    //-----------------------------------------------------------------------------------------
    static CTerrainPageEditor *CreateTerrainPageImpl(int X, int Y, const Ogre::String& diffuse, const Ogre::String& normal, Ogre::Real worldSize, OgitorsRoot* root)
    {
        CTerrainGroupEditor *terGroup = static_cast<CTerrainGroupEditor *>(OgitorsRoot::getSingletonPtr()->FindObject("Terrain Group"));
        if(!terGroup)
            return 0;

        OgitorsPropertyValueMap params;

        Ogre::String pagename = terGroup->getPageNamePrefix();
        pagename += Ogre::StringConverter::toString(X);
        pagename += "x";
        pagename += Ogre::StringConverter::toString(Y);

        params["init"] = EMPTY_PROPERTY_VALUE;
        params["name"] = OgitorsPropertyValue(PROP_STRING, Ogre::Any(pagename));
        params["position"] = OgitorsPropertyValue(PROP_VECTOR3, Ogre::Any(terGroup->getPagePosition(X, Y)));
        params["pagex"] = OgitorsPropertyValue(PROP_INT, Ogre::Any(X));
        params["pagey"] = OgitorsPropertyValue(PROP_INT, Ogre::Any(Y));
        if(diffuse.empty())
            params["layer0::diffusespecular"] = OgitorsPropertyValue(PROP_STRING, Ogre::Any(Ogre::String("dirt_grayrocky_diffusespecular.dds")));
        else
            params["layer0::diffusespecular"] = OgitorsPropertyValue(PROP_STRING, Ogre::Any(diffuse));
        if(normal.empty())
            params["layer0::normalheight"] = OgitorsPropertyValue(PROP_STRING, Ogre::Any(Ogre::String("dirt_grayrocky_normalheight.dds")));
        else
            params["layer0::normalheight"] = OgitorsPropertyValue(PROP_STRING, Ogre::Any(normal));
        if(worldSize == 0.0f)
            params["layer0::worldsize"] = OgitorsPropertyValue(PROP_REAL, Ogre::Any((Ogre::Real)25.0f));
        else
            params["layer0::worldsize"] = OgitorsPropertyValue(PROP_REAL, Ogre::Any(worldSize));

        return (CTerrainPageEditor*)OgitorsRoot::getSingletonPtr()->CreateEditorObject(terGroup,"Terrain Page", params, true, true);
    }
    //-----------------------------------------------------------------------------------------
    static void GetTerrainDiffuseTextureNames(Ogre::StringVector& list, OgitorsRoot* root)
    {
        list.clear();
        PropertyOptionsVector *source = OgitorsRoot::GetTerrainDiffuseTextureNames();
        for(unsigned int i = 1;i < source->size();i++)
            list.push_back((*source)[i].mKey);
    }
    //-----------------------------------------------------------------------------------------
    static void GetTerrainNormalTextureNames(Ogre::StringVector& list, OgitorsRoot* root)
    {
        list.clear();
        PropertyOptionsVector *source = OgitorsRoot::GetTerrainNormalTextureNames();
        for(unsigned int i = 1;i < source->size();i++)
            list.push_back((*source)[i].mKey);
    }
    //-----------------------------------------------------------------------------------------
    static void GetTerrainPlantMaterialNames(Ogre::StringVector& list, OgitorsRoot* root)
    {
        list.clear();
        PropertyOptionsVector *source = OgitorsRoot::GetTerrainPlantMaterialNames();
        for(unsigned int i = 0;i < source->size();i++)
            list.push_back((*source)[i].mKey);
    }
    //-----------------------------------------------------------------------------------------
    static void GetScriptNames(Ogre::StringVector& list, OgitorsRoot* root)
    {
        list.clear();
        PropertyOptionsVector *source = OgitorsRoot::GetScriptNames();
        for(unsigned int i = 0;i < source->size();i++)
            list.push_back((*source)[i].mKey);
    }
    //-----------------------------------------------------------------------------------------
    static void GetMaterialNames(Ogre::StringVector& list, OgitorsRoot* root)
    {
        list.clear();
        PropertyOptionsVector *source = OgitorsRoot::GetMaterialNames();
        for(unsigned int i = 0;i < source->size();i++)
            list.push_back((*source)[i].mKey);
    }
    //-----------------------------------------------------------------------------------------
    static void GetModelNames(Ogre::StringVector& list, OgitorsRoot* root)
    {
        list.clear();
        PropertyOptionsVector *source = OgitorsRoot::GetModelNames();
        for(unsigned int i = 1;i < source->size();i++)
            list.push_back((*source)[i].mKey);
    }
    //-----------------------------------------------------------------------------------------
    static void GetSkyboxMaterials(Ogre::StringVector& list, OgitorsRoot* root)
    {
        list.clear();
        PropertyOptionsVector *source = OgitorsRoot::GetSkyboxMaterials();
        for(unsigned int i = 1;i < source->size();i++)
            list.push_back((*source)[i].mKey);
    }
    //-----------------------------------------------------------------------------------------
    static void GetLayerNames(Ogre::StringVector& list, OgitorsRoot* root)
    {
        list.clear();
        PropertyOptionsVector *source = OgitorsRoot::GetLayerNames();
        for(unsigned int i = 0;i < source->size();i++)
            list.push_back((*source)[i].mKey);
    }
    //-----------------------------------------------------------------------------------------
    void RegisterRootBindings(asIScriptEngine *engine)
    {
        int r;
        r = engine->RegisterObjectMethod("OgitorsRoot", "void getObjectList(const string &in, EditorVector &out)", asMETHODPR(OgitorsRoot, GetObjectList, (const Ogre::String&, ObjectVector&),void), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsRoot", "void getObjectList(EditorType, EditorVector &out)", asMETHODPR(OgitorsRoot, GetObjectList, (unsigned int, ObjectVector&),void), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsRoot", "void getObjectListByName(EditorType, const string &in, bool, EditorVector &out)", asMETHOD(OgitorsRoot, GetObjectListByName), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsRoot", "void getObjectListByProperty(EditorType, const string &in, bool, EditorVector &out)", asMETHOD(OgitorsRoot, GetObjectListByProperty), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsRoot", "void getObjectListByCustomProperty(EditorType, const string &in, bool, EditorVector &out)", asMETHOD(OgitorsRoot, GetObjectListByCustomProperty), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsRoot", "void regexpByName(const string &in, bool, EditorVector &in, EditorVector &out)", asMETHOD(OgitorsRoot, RegExpByName), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsRoot", "void regexpByProperty(const string &in, bool, EditorVector &in, EditorVector &out)", asMETHOD(OgitorsRoot, RegExpByProperty), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsRoot", "void regexpByCustomProperty(const string &in, bool, EditorVector &in, EditorVector &out)", asMETHOD(OgitorsRoot, RegExpByCustomProperty), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsRoot", "bool terminateScene()", asMETHOD(OgitorsRoot, TerminateScene), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsRoot", "bool isSceneLoaded()", asMETHOD(OgitorsRoot, IsSceneLoaded), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsRoot", "bool isSceneModified()", asMETHOD(OgitorsRoot, IsSceneModified), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsRoot", "bool setSceneModified(bool)", asMETHOD(OgitorsRoot, SetSceneModified), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsRoot", "bool changeSceneModified(bool)", asMETHOD(OgitorsRoot, ChangeSceneModified), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsRoot", "void update(float)", asMETHOD(OgitorsRoot, Update), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsRoot", "bool saveScene(bool)", asMETHOD(OgitorsRoot, SaveScene), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsRoot", "int loadScene(string)", asMETHOD(OgitorsRoot, LoadScene), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsRoot", "bool getWorldSpaceGizmoOrientation(bool)", asMETHOD(OgitorsRoot, GetWorldSpaceGizmoOrientation), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsRoot", "void setWorldSpaceGizmoOrientation(bool)", asMETHOD(OgitorsRoot, SetWorldSpaceGizmoOrientation), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsRoot", "BaseEditor@ findObject(string, uint)", asMETHODPR(OgitorsRoot, FindObject, (Ogre::String, unsigned int), CBaseEditor*), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsRoot", "ViewportEditor@ getViewport()", asMETHOD(OgitorsRoot, GetViewport), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsRoot", "MultiSelection@ getSelection()", asMETHOD(OgitorsRoot, GetSelection), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsRoot", "TerrainGroupEditor@ getTerrainEditor()", asMETHOD(OgitorsRoot, GetTerrainEditorObject), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsRoot", "void destroyObject(BaseEditor@, bool, bool)", asMETHOD(OgitorsRoot, DestroyEditorObject), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsRoot", "BaseEditor@ createObject(BaseEditor@, const string &in)", asFUNCTION(CreateEditorObjectImpl), asCALL_CDECL_OBJLAST);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsRoot", "EntityEditor@ createEntityObject(BaseEditor@, const string &in)", asFUNCTION(CreateEntityObjectImpl), asCALL_CDECL_OBJLAST);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsRoot", "TerrainPageEditor@ createTerrainPage(int, int, const string &in, const string &in, float)", asFUNCTION(CreateTerrainPageImpl), asCALL_CDECL_OBJLAST);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsRoot", "void getMaterialNames(StringVector &out)", asFUNCTION(GetMaterialNames), asCALL_CDECL_OBJLAST);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsRoot", "void getSkyboxMaterialNames(StringVector &out)", asFUNCTION(GetSkyboxMaterials), asCALL_CDECL_OBJLAST);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsRoot", "void getModelNames(StringVector &out)", asFUNCTION(GetModelNames), asCALL_CDECL_OBJLAST);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsRoot", "void getLayerNames(StringVector &out)", asFUNCTION(GetLayerNames), asCALL_CDECL_OBJLAST);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsRoot", "void getScriptNames(StringVector &out)", asFUNCTION(GetScriptNames), asCALL_CDECL_OBJLAST);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsRoot", "void getTerrainDiffuseTextureNames(StringVector &out)", asFUNCTION(GetTerrainDiffuseTextureNames), asCALL_CDECL_OBJLAST);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsRoot", "void getTerrainNormalTextureNames(StringVector &out)", asFUNCTION(GetTerrainNormalTextureNames), asCALL_CDECL_OBJLAST);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsRoot", "void getTerrainPlantMaterialNames(StringVector &out)", asFUNCTION(GetTerrainPlantMaterialNames), asCALL_CDECL_OBJLAST);assert(r >= 0);
    }

};

#endif