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

#include "Ogitors.h"
#include "PagingEditor.h"
#include "OgitorsPaging.h"
#include "PGInstanceManager.h"
#include "PGInstanceEditor.h"
#include "TerrainPageEditor.h"
#include "TerrainGroupEditor.h"

#include "angelscript.h"
#include "Bindings_Base.h"

#include "aswrappedcall.h"

namespace Ogitors
{
    #define REGISTER_REFERENCE_BASEOBJECT( name, classname )\
    {\
        r = engine->RegisterObjectType(name, 0, asOBJ_REF);assert(r >= 0);\
        r = engine->RegisterObjectBehaviour(name, asBEHAVE_ADDREF, "void f()", asMETHOD(classname,_addRef), asCALL_THISCALL); assert( r >= 0 );\
        r = engine->RegisterObjectBehaviour(name, asBEHAVE_RELEASE, "void f()", asMETHOD(classname,_release), asCALL_THISCALL); assert( r >= 0 );\
        r = engine->RegisterObjectBehaviour("BaseEditor", asBEHAVE_REF_CAST, name "@ f()", asFUNCTION((refCast<CBaseEditor,classname>)), asCALL_CDECL_OBJLAST); assert( r >= 0 );\
        r = engine->RegisterObjectBehaviour(name, asBEHAVE_IMPLICIT_REF_CAST, "BaseEditor@ f()", asFUNCTION((refCast<classname,CBaseEditor>)), asCALL_CDECL_OBJLAST); assert( r >= 0 );\
    }\
    //-----------------------------------------------------------------------------------------
    #define REGISTER_BASEOBJECT_WRAPPERS( name, classname )\
    asDECLARE_METHOD_WRAPPER(classname##_getDerivedPosition_Wrapper, classname, getDerivedPosition);\

    #define REGISTER_BASEOBJECT_FUNCTIONS( name, classname )\
    {\
        r = engine->RegisterObjectMethod(name, "Vector3 getDerivedPosition()", asFUNCTION(classname##_getDerivedPosition_Wrapper), asCALL_GENERIC);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "void setDerivedPosition(Vector3)", asMETHOD(classname, setDerivedPosition), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "Quaternion getDerivedOrientation()", asMETHOD(classname, getDerivedOrientation), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "void setDerivedOrientation(Quaternion)", asMETHOD(classname, setDerivedOrientation), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "Vector3 getDerivedScale()", asMETHOD(classname, getDerivedScale), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "void setDerivedScale(Vector3)", asMETHOD(classname, setDerivedScale), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "PropertySet@ getProperties()", asMETHOD(classname, getProperties), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "CustomPropertySet@ getCustomProperties()", asMETHOD(classname, getCustomProperties), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "Property@ getProperty(string &in)", asMETHOD(classname, getProperty), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "bool hasProperty(string &in)", asMETHOD(classname, hasProperty), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "string getName()", asMETHOD(classname, getName), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "string getTypeName()", asMETHOD(classname, getTypeName), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "int getType()", asMETHOD(classname, getEditorType), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "uint getID()", asMETHOD(classname, getObjectID), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "bool getModified()", asMETHOD(classname, isModified), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "bool getLoaded()", asMETHOD(classname, isLoaded), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "BaseEditor@ getParent()", asMETHOD(classname, getParent), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "AxisAlignedBox getAABB()", asMETHOD(classname, getAABB), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "AxisAlignedBox getWorldAABB()", asMETHOD(classname, getWorldAABB), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "int getLayer()", asMETHOD(classname, getLayer), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "bool getLocked()", asMETHOD(classname, getLocked), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "bool getSelected()", asMETHOD(classname, getSelected), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "void setSelected(bool)", asMETHOD(classname, setSelected), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "void setHighlighted(bool)", asMETHOD(classname, setHighlighted), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "void setModified(bool)", asMETHOD(classname, setModified), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "void setLoaded(bool)", asMETHOD(classname, setLoaded), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "void setLocked(bool)", asMETHOD(classname, setLocked), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "void setName(const string &in)", asMETHOD(classname, setName), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "void setParent(BaseEditor@)", asMETHOD(classname, setParent), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "void setLayer(int)", asMETHOD(classname, setLayer), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "void setUpdateScript(const string &in)", asMETHOD(classname, setUpdateScript), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "bool supports(EditFlags)", asMETHOD(classname, supports), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "bool usesGizmos()", asMETHOD(classname, usesGizmos), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "bool usesHelper()", asMETHOD(classname, usesHelper), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "void destroy(bool)", asMETHOD(classname, destroy), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "void addChild(BaseEditor@)", asMETHOD(classname, _addChild), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "void removeChild(string)", asMETHOD(classname, _removeChild), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "BaseEditor@ findChild(string, bool)", asMETHOD(classname, findChild), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "void destroyAllChildren()", asMETHOD(classname, destroyAllChildren), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "void loadAllChildren(bool)", asMETHOD(classname, loadAllChildren), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "void unLoadAllChildren()", asMETHOD(classname, unLoadAllChildren), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "void getNodeList(EditorVector &out)", asMETHOD(classname, getNodeList), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "void load(bool)", asMETHOD(classname, load), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "void unLoad()", asMETHOD(classname, unLoad), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "void onSave(bool)", asMETHOD(classname, onSave), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "void showBoundingBox(bool)", asMETHOD(classname, showBoundingBox), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "void showHelper(bool)", asMETHOD(classname, showHelper), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "bool isSerializable()", asMETHOD(classname, isSerializable), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "bool isNodeType()", asMETHOD(classname, isNodeType), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "bool isAutoTrackTarget()", asMETHOD(classname, isAutoTrackTarget), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "bool isTerrainType()", asMETHOD(classname, isTerrainType), asCALL_THISCALL);assert(r >= 0);\
    }\
    //-----------------------------------------------------------------------------------------
    #define REGISTER_NODE_WRAPPERS( name, classname )\
    asDECLARE_METHOD_WRAPPER(classname##_getPosition_Wrapper, classname, getPosition);\

    #define REGISTER_NODE_FUNCTIONS( name, classname )\
    {\
        r = engine->RegisterObjectMethod(name, "void setPosition(const Vector3 &in)", asMETHOD(classname, setPosition), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "Vector3 getPosition()", asFUNCTION(classname##_getPosition_Wrapper), asCALL_GENERIC);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "void setScale(const Vector3 &in)", asMETHOD(classname, setScale), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "Vector3 getScale()", asMETHOD(classname, getScale), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "void setOrientation(const Quaternion &in)", asMETHOD(classname, setOrientation), asCALL_THISCALL);assert(r >= 0);\
        r = engine->RegisterObjectMethod(name, "Quaternion getOrientation()", asMETHOD(classname, getOrientation), asCALL_THISCALL);assert(r >= 0);\
    }\
    //-----------------------------------------------------------------------------------------
    // Example REF_CAST behaviour
    template<class A, class B>
    B* refCast(A* a)
    {
        // If the handle already is a null handle, then just return the null handle
        if( !a ) return 0;

        // Now try to dynamically cast the pointer to the wanted type
        B* b = static_cast<B*>(a);
        if( b != 0 )
        {
            // Since the cast was made, we need to increase the ref counter for the returned handle
            b->_addRef();
        }
        return b;
    }

    REGISTER_BASEOBJECT_WRAPPERS("BaseEditor", CBaseEditor);
    REGISTER_BASEOBJECT_WRAPPERS("BillboardSetEditor", CBillboardSetEditor);
    REGISTER_BASEOBJECT_WRAPPERS("CameraEditor", CCameraEditor);
    REGISTER_BASEOBJECT_WRAPPERS("EntityEditor", CEntityEditor);
    REGISTER_BASEOBJECT_WRAPPERS("LightEditor", CLightEditor);
    REGISTER_BASEOBJECT_WRAPPERS("MarkerEditor", CMarkerEditor);
    REGISTER_BASEOBJECT_WRAPPERS("MultiSelection", CMultiSelEditor);
    REGISTER_BASEOBJECT_WRAPPERS("NodeEditor", CNodeEditor);
    REGISTER_BASEOBJECT_WRAPPERS("PagingManager", CPagingManager);
    REGISTER_BASEOBJECT_WRAPPERS("ParticleEditor", CParticleEditor);
    REGISTER_BASEOBJECT_WRAPPERS("PGInstanceEditor", CPGInstanceEditor);
    REGISTER_BASEOBJECT_WRAPPERS("PGInstanceManager", CPGInstanceManager);
    REGISTER_BASEOBJECT_WRAPPERS("PlaneEditor", CPlaneEditor);
    REGISTER_BASEOBJECT_WRAPPERS("SceneManagerEditor", CSceneManagerEditor);
    REGISTER_BASEOBJECT_WRAPPERS("TerrainGroupEditor", CTerrainGroupEditor);
    REGISTER_BASEOBJECT_WRAPPERS("TerrainPageEditor" , CTerrainPageEditor);
    REGISTER_BASEOBJECT_WRAPPERS("ViewportEditor"    , CViewportEditor);

    REGISTER_NODE_WRAPPERS("NodeEditor", CNodeEditor);
    REGISTER_NODE_WRAPPERS("EntityEditor", CEntityEditor);
    REGISTER_NODE_WRAPPERS("PlaneEditor", CPlaneEditor);
    REGISTER_NODE_WRAPPERS("ParticleEditor", CParticleEditor);
    REGISTER_NODE_WRAPPERS("MarkerEditor", CMarkerEditor);

    //-----------------------------------------------------------------------------------------
    void RegisterBaseEditorBindings(asIScriptEngine *engine)
    {
	    int r;

        REGISTER_BASEOBJECT_FUNCTIONS("BaseEditor", CBaseEditor);


        REGISTER_REFERENCE_BASEOBJECT("BillboardSetEditor", CBillboardSetEditor);
        REGISTER_BASEOBJECT_FUNCTIONS("BillboardSetEditor", CBillboardSetEditor);

        REGISTER_REFERENCE_BASEOBJECT("CameraEditor"      , CCameraEditor);
        REGISTER_BASEOBJECT_FUNCTIONS("CameraEditor"      , CCameraEditor);

        REGISTER_REFERENCE_BASEOBJECT("EntityEditor"      , CEntityEditor);
        REGISTER_BASEOBJECT_FUNCTIONS("EntityEditor"      , CEntityEditor);

        REGISTER_REFERENCE_BASEOBJECT("LightEditor"       , CLightEditor);
        REGISTER_BASEOBJECT_FUNCTIONS("LightEditor"       , CLightEditor);

        REGISTER_REFERENCE_BASEOBJECT("MarkerEditor"      , CMarkerEditor);
        REGISTER_BASEOBJECT_FUNCTIONS("MarkerEditor"      , CMarkerEditor);

        REGISTER_REFERENCE_BASEOBJECT("MultiSelection"    , CMultiSelEditor);
        REGISTER_BASEOBJECT_FUNCTIONS("MultiSelection"    , CMultiSelEditor);

        REGISTER_REFERENCE_BASEOBJECT("NodeEditor"        , CNodeEditor);
        REGISTER_BASEOBJECT_FUNCTIONS("NodeEditor"        , CNodeEditor);

        REGISTER_REFERENCE_BASEOBJECT("PagingManager"     , CPagingManager);
        REGISTER_BASEOBJECT_FUNCTIONS("PagingManager"     , CPagingManager);

        REGISTER_REFERENCE_BASEOBJECT("ParticleEditor"    , CParticleEditor);
        REGISTER_BASEOBJECT_FUNCTIONS("ParticleEditor"    , CParticleEditor);

        REGISTER_REFERENCE_BASEOBJECT("PGInstanceEditor"  , CPGInstanceEditor);
        REGISTER_BASEOBJECT_FUNCTIONS("PGInstanceEditor"  , CPGInstanceEditor);

        REGISTER_REFERENCE_BASEOBJECT("PGInstanceManager" , CPGInstanceManager);
        REGISTER_BASEOBJECT_FUNCTIONS("PGInstanceManager" , CPGInstanceManager);

        REGISTER_REFERENCE_BASEOBJECT("PlaneEditor"       , CPlaneEditor);
        REGISTER_BASEOBJECT_FUNCTIONS("PlaneEditor"       , CPlaneEditor);

        REGISTER_REFERENCE_BASEOBJECT("SceneManagerEditor", CSceneManagerEditor);
        REGISTER_BASEOBJECT_FUNCTIONS("SceneManagerEditor", CSceneManagerEditor);

        REGISTER_REFERENCE_BASEOBJECT("TerrainGroupEditor", CTerrainGroupEditor);
        REGISTER_BASEOBJECT_FUNCTIONS("TerrainGroupEditor", CTerrainGroupEditor);

        REGISTER_REFERENCE_BASEOBJECT("TerrainPageEditor" , CTerrainPageEditor);
        REGISTER_BASEOBJECT_FUNCTIONS("TerrainPageEditor" , CTerrainPageEditor);

        REGISTER_REFERENCE_BASEOBJECT("ViewportEditor"    , CViewportEditor);
        REGISTER_BASEOBJECT_FUNCTIONS("ViewportEditor"    , CViewportEditor);

         //---------------- WILL BE MOVED TO OWN FILE WHEN READY -----------------------------
        r = engine->RegisterObjectMethod("CameraEditor", "void setPosition(const Vector3 &in)", asMETHOD(CCameraEditor, setPosition), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("CameraEditor", "Vector3 getPosition()", asMETHOD(CCameraEditor, getPosition), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("CameraEditor", "void setOrientation(const Quaternion &in)", asMETHOD(CCameraEditor, setOrientation), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("CameraEditor", "Quaternion getOrientation()", asMETHOD(CCameraEditor, getOrientation), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("CameraEditor", "void setDirection(const Vector3 &in)", asMETHOD(CCameraEditor, setDirection), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("CameraEditor", "Vector3 getDirection()", asMETHOD(CCameraEditor, getDirection), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("CameraEditor", "void setClipDistance(const Vector2 &in)", asMETHOD(CCameraEditor, setClipDistance), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("CameraEditor", "Vector2 getClipDistance()", asMETHOD(CCameraEditor, getClipDistance), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("CameraEditor", "void setFOV(float)", asMETHOD(CCameraEditor, setFOV), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("CameraEditor", "float getFOV()", asMETHOD(CCameraEditor, getFOV), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("CameraEditor", "void setPolygonMode(PolygonMode)", asMETHOD(CCameraEditor, setPolygonMode), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("CameraEditor", "PolygonMode getPolygonMode()", asMETHOD(CCameraEditor, getPolygonMode), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("CameraEditor", "void setViewMode(CameraViewMode)", asMETHOD(CCameraEditor, setViewMode), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("CameraEditor", "CameraViewMode getViewMode()", asMETHOD(CCameraEditor, getViewMode), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("CameraEditor", "void yaw(float)", asMETHOD(CCameraEditor, yaw), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("CameraEditor", "void pitch(float)", asMETHOD(CCameraEditor, pitch), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("CameraEditor", "void roll(float)", asMETHOD(CCameraEditor, roll), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("CameraEditor", "void lookAt(const Vector2 &in)", asMETHOD(CCameraEditor, lookAt), asCALL_THISCALL);assert(r >= 0);

        r = engine->RegisterObjectMethod("LightEditor", "void setPosition(const Vector3 &in)", asMETHOD(CLightEditor, setPosition), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("LightEditor", "Vector3 getPosition()", asMETHOD(CLightEditor, getPosition), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("LightEditor", "void setOrientation(const Quaternion &in)", asMETHOD(CLightEditor, setOrientation), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("LightEditor", "Quaternion getOrientation()", asMETHOD(CLightEditor, getOrientation), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("LightEditor", "void setDirection(const Vector3 &in)", asMETHOD(CLightEditor, setDirection), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("LightEditor", "Vector3 getDirection()", asMETHOD(CLightEditor, getDirection), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("LightEditor", "void setRange(const Vector3 &in)", asMETHOD(CLightEditor, setRange), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("LightEditor", "Vector3 getRange()", asMETHOD(CLightEditor, getRange), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("LightEditor", "void setAttenuation(const Vector3 &in)", asMETHOD(CLightEditor, setAttenuation), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("LightEditor", "Vector3 getAttenuation()", asMETHOD(CLightEditor, getAttenuation), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("LightEditor", "void setLightType(LightType)", asMETHOD(CLightEditor, setLightType), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("LightEditor", "LightType getLightType()", asMETHOD(CLightEditor, getLightType), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("LightEditor", "void setPower(float)", asMETHOD(CLightEditor, setPower), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("LightEditor", "float getPower()", asMETHOD(CLightEditor, getPower), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("LightEditor", "void setCastShadows(bool)", asMETHOD(CLightEditor, setCastShadows), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("LightEditor", "bool getCastShadows()", asMETHOD(CLightEditor, getCastShadows), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("LightEditor", "void setDiffuse(const ColourValue &in)", asMETHOD(CLightEditor, setDiffuse), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("LightEditor", "ColourValue getDiffuse()", asMETHOD(CLightEditor, getDiffuse), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("LightEditor", "void setSpecular(const ColourValue &in)", asMETHOD(CLightEditor, setSpecular), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("LightEditor", "ColourValue getSpecular()", asMETHOD(CLightEditor, getSpecular), asCALL_THISCALL);assert(r >= 0);

        REGISTER_NODE_FUNCTIONS("NodeEditor", CNodeEditor);
        REGISTER_NODE_FUNCTIONS("EntityEditor", CEntityEditor);
        REGISTER_NODE_FUNCTIONS("PlaneEditor", CPlaneEditor);
        REGISTER_NODE_FUNCTIONS("ParticleEditor", CParticleEditor);
        REGISTER_NODE_FUNCTIONS("MarkerEditor", CMarkerEditor);

        r = engine->RegisterObjectMethod("EntityEditor", "void setCastShadows(bool)", asMETHOD(CEntityEditor, setCastShadows), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("EntityEditor", "bool getCastShadows()", asMETHOD(CEntityEditor, getCastShadows), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("PlaneEditor", "void setCastShadows(bool)", asMETHOD(CPlaneEditor, setCastShadows), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("PlaneEditor", "bool getCastShadows()", asMETHOD(CPlaneEditor, getCastShadows), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("ParticleEditor", "void setCastShadows(bool)", asMETHOD(CParticleEditor, setCastShadows), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("ParticleEditor", "bool getCastShadows()", asMETHOD(CParticleEditor, getCastShadows), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("PGInstanceManager", "void setCastShadows(bool)", asMETHOD(CPGInstanceManager, setCastShadows), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("PGInstanceManager", "bool getCastShadows()", asMETHOD(CPGInstanceManager, getCastShadows), asCALL_THISCALL);assert(r >= 0);


        r = engine->RegisterObjectMethod("TerrainGroupEditor", "bool hitTest(Ray, Vector3 &out)", asMETHOD(CTerrainGroupEditor, hitTest), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("TerrainGroupEditor", "float getHeightAt(float, float)", asMETHOD(CTerrainGroupEditor, getHeightAt), asCALL_THISCALL);assert(r >= 0);

        r = engine->RegisterObjectMethod("TerrainPageEditor", "int addLayer(string &in, string &in, float, bool)", asMETHOD(CTerrainPageEditor, _createNewLayer), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("TerrainPageEditor", "void changeLayer(int, string &in, string &in, float)", asMETHOD(CTerrainPageEditor, _changeLayer), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("TerrainPageEditor", "void deleteLayer(int)", asMETHOD(CTerrainPageEditor, _deleteLayer), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("TerrainPageEditor", "bool importHeightMap(string, float, float)", asMETHODPR(CTerrainPageEditor, importHeightMap, (Ogre::String, Ogre::Real, Ogre::Real), bool), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("TerrainPageEditor", "bool importBlendMap(int, string)", asMETHODPR(CTerrainPageEditor, importBlendMap, (int, Ogre::String), bool), asCALL_THISCALL);assert(r >= 0);
    }

}
