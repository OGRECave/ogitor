/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2013 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
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

#include "Ogitors.h"
#include "PagingEditor.h"
#include "OgitorsPaging.h"
#include "PGInstanceManager.h"
#include "PGInstanceEditor.h"
#include "TerrainPageEditor.h"
#include "TerrainGroupEditor.h"
#include "BaseEditor.h"

#include "angelscript.h"
#include "Bindings_Base.h"

#include "aswrappedcall.h"

namespace Ogitors
{
#define REGISTER_REFERENCE_BASEOBJECT( name, classname )\
{\
    r = engine->RegisterObjectType(name, sizeof(classname), asOBJ_REF | asOBJ_NOCOUNT); assert(r >= 0);\
}\
//-----------------------------------------------------------------------------------------


#define REGISTER_BASEOBJECT_FUNCTIONS( name, classname )\
{\
    r = engine->RegisterObjectMethod(name, "Vector3 getDerivedPosition()", asMETHODPR(classname, getDerivedPosition, (), Ogre::Vector3), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "void setDerivedPosition(Vector3)", asMETHODPR(classname, setDerivedPosition, (Ogre::Vector3), void), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "Quaternion getDerivedOrientation()", asMETHODPR(classname, getDerivedOrientation, (), Ogre::Quaternion), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "void setDerivedOrientation(Quaternion)", asMETHODPR(classname, setDerivedOrientation, (Ogre::Quaternion), void), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "Vector3 getDerivedScale()", asMETHODPR(classname, getDerivedScale, (), Ogre::Vector3), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "void setDerivedScale(Vector3)", asMETHODPR(classname, setDerivedScale, (Ogre::Vector3), void), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "bool hasProperty(const string &in)", asMETHODPR(classname, hasProperty, (const std::string&), bool), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "string getName()", asMETHODPR(classname, getName, (), std::string), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "string getTypeName()", asMETHODPR(classname, getTypeName, (), std::string), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "int getType()", asMETHODPR(classname, getEditorType, (), Ogitors::EDITORTYPE), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "int getID()", asMETHODPR(classname, getObjectID, (), unsigned int), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "PropertySet@ getProperties()", asMETHOD(classname, getProperties), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "CustomPropertySet@ getCustomProperties()", asMETHOD(classname, getCustomProperties), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "Property@ getProperty(string &in)", asMETHOD(classname, getProperty), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "bool hasProperty(string &in)", asMETHOD(classname, hasProperty), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "bool getModified()", asMETHODPR(classname, isModified, (), bool), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "bool getLoaded()", asMETHODPR(classname, isLoaded, (), bool), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "BaseEditor@ getParent()", asMETHODPR(classname, getParent, (), CBaseEditor*), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "AxisAlignedBox getAABB()", asMETHODPR(classname, getAABB, (), Ogre::AxisAlignedBox), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "AxisAlignedBox getWorldAABB()", asMETHODPR(classname, getWorldAABB, (), Ogre::AxisAlignedBox), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "int getLayer()", asMETHODPR(classname, getLayer, (), int), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "bool getLocked()", asMETHODPR(classname, getLocked, (), bool), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "bool getSelected()", asMETHODPR(classname, getSelected, (), bool), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "void setSelected(bool)", asMETHODPR(classname, setSelected, (bool), void), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "void setHighlighted(bool)", asMETHODPR(classname, setHighlighted, (bool), void), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "void setModified(bool)", asMETHODPR(classname, setModified, (bool), void), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "void setLoaded(bool)", asMETHODPR(classname, setLoaded, (bool), void), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "void setLocked(bool)", asMETHODPR(classname, setLocked, (bool), void), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "void setName(const string &in)", asMETHODPR(classname, setName, (const std::string&), void), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "void setParent(BaseEditor@)", asMETHODPR(classname, setParent, (CBaseEditor*), void), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "void setLayer(int)", asMETHODPR(classname, setLayer, (int), void), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "void setUpdateScript(const string &in)", asMETHODPR(classname, setUpdateScript, (const std::string&), void), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "bool supports(int)", asMETHODPR(classname, supports, (unsigned int), bool), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "bool usesGizmos()", asMETHODPR(classname, usesGizmos, (), bool), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "bool usesHelper()", asMETHODPR(classname, usesHelper, (), bool), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "void destroy(bool)", asMETHODPR(classname, destroy, (bool), void), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "void addChild(BaseEditor@)", asMETHODPR(classname, _addChild, (CBaseEditor*), void), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "void removeChild(string)", asMETHODPR(classname, _removeChild, (std::string), void), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "BaseEditor@ findChild(string, bool)", asMETHODPR(classname, findChild, (std::string, bool), CBaseEditor*), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "void destroyAllChildren()", asMETHODPR(classname, destroyAllChildren, (), void), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "void loadAllChildren(bool)", asMETHODPR(classname, loadAllChildren, (bool), void), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "void unLoadAllChildren()", asMETHODPR(classname, unLoadAllChildren, (), void), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "void getNodeList(EditorVector &out)", asMETHODPR(classname, getNodeList, (ObjectVector&), void), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "bool load(bool)", asMETHODPR(classname, load, (bool), bool), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "bool unLoad()", asMETHODPR(classname, unLoad, (), bool), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "void onSave(bool)", asMETHODPR(classname, onSave, (bool), void), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "void showBoundingBox(bool)", asMETHODPR(classname, showBoundingBox, (bool), void), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "void showHelper(bool)", asMETHODPR(classname, showHelper, (bool), void), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "bool isSerializable()", asMETHODPR(classname, isSerializable, (), bool), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "bool isNodeType()", asMETHODPR(classname, isNodeType, (), bool), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "bool isAutoTrackTarget()", asMETHODPR(classname, isAutoTrackTarget, (), bool), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "bool isTerrainType()", asMETHODPR(classname, isTerrainType, (), bool), asCALL_THISCALL);assert(r >= 0);\
}\
    //-----------------------------------------------------------------------------------------

    #define REGISTER_NODE_FUNCTIONS( name, classname )\
    {\
    r = engine->RegisterObjectMethod(name, "void setPosition(const Vector3 &in)", asMETHODPR(classname, setPosition, (const Ogre::Vector3&), void), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "Vector3 getPosition()", asMETHODPR(classname, getPosition, (), Ogre::Vector3), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "void setScale(const Vector3 &in)", asMETHODPR(classname, setScale, (const Ogre::Vector3&), void), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "Vector3 getScale()", asMETHODPR(classname, getScale, (), Ogre::Vector3), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "void setOrientation(const Quaternion &in)", asMETHODPR(classname, setOrientation, (const Ogre::Quaternion&), void), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, "Quaternion getOrientation()", asMETHODPR(classname, getOrientation, (), Ogre::Quaternion), asCALL_THISCALL);assert(r >= 0);\
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
        r = engine->RegisterObjectMethod("CameraEditor", "void setPosition(const Vector3 &in)", asMETHODPR(CCameraEditor, setPosition, (const Ogre::Vector3&), void), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("CameraEditor", "Vector3 getPosition()", asMETHODPR(CCameraEditor, getPosition, (), Ogre::Vector3), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("CameraEditor", "void setOrientation(const Quaternion &in)", asMETHODPR(CCameraEditor, setOrientation, (const Ogre::Quaternion&), void), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("CameraEditor", "Quaternion getOrientation()", asMETHODPR(CCameraEditor, getOrientation, (), Ogre::Quaternion), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("CameraEditor", "void setDirection(const Vector3 &in)", asMETHODPR(CCameraEditor, setDirection, (const Ogre::Vector3&), void), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("CameraEditor", "Vector3 getDirection()", asMETHODPR(CCameraEditor, getDirection, (), Ogre::Vector3), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("CameraEditor", "void setClipDistance(const Vector2 &in)", asMETHODPR(CCameraEditor, setClipDistance, (const Ogre::Vector2&), void), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("CameraEditor", "Vector2 getClipDistance()", asMETHODPR(CCameraEditor, getClipDistance, (), Ogre::Vector2), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("CameraEditor", "void setFOV(float)", asMETHODPR(CCameraEditor, setFOV, (float), void), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("CameraEditor", "float getFOV()", asMETHODPR(CCameraEditor, getFOV, (), float), asCALL_THISCALL);assert(r >= 0);

        r = engine->RegisterObjectMethod("CameraEditor", "void setPolygonMode(PolygonMode)", asMETHOD(CCameraEditor, setPolygonMode), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("CameraEditor", "PolygonMode getPolygonMode()", asMETHOD(CCameraEditor, getPolygonMode), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("CameraEditor", "void setViewMode(CameraViewMode)", asMETHOD(CCameraEditor, setViewMode), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("CameraEditor", "CameraViewMode getViewMode()", asMETHOD(CCameraEditor, getViewMode), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("CameraEditor", "void yaw(float)", asMETHOD(CCameraEditor, yaw), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("CameraEditor", "void pitch(float)", asMETHOD(CCameraEditor, pitch), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("CameraEditor", "void roll(float)", asMETHOD(CCameraEditor, roll), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("CameraEditor", "void lookAt(const Vector2 &in)", asMETHOD(CCameraEditor, lookAt), asCALL_THISCALL);assert(r >= 0);

        r = engine->RegisterObjectMethod("LightEditor", "void setPosition(const Vector3 &in)", asMETHODPR(CLightEditor, setPosition, (const Ogre::Vector3&), void), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("LightEditor", "Vector3 getPosition()", asMETHODPR(CLightEditor, getPosition, (), Ogre::Vector3), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("LightEditor", "void setOrientation(const Quaternion &in)", asMETHODPR(CLightEditor, setOrientation, (const Ogre::Quaternion&), void), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("LightEditor", "Quaternion getOrientation()", asMETHODPR(CLightEditor, getOrientation, (), Ogre::Quaternion), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("LightEditor", "void setDirection(const Vector3 &in)", asMETHODPR(CLightEditor, setDirection, (const Ogre::Vector3&), void), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("LightEditor", "Vector3 getDirection()", asMETHODPR(CLightEditor, getDirection, (), Ogre::Vector3), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("LightEditor", "void setRange(const Vector3 &in)", asMETHODPR(CLightEditor, setRange, (const Ogre::Vector3&), void), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("LightEditor", "Vector3 getRange()", asMETHODPR(CLightEditor, getRange, (), Ogre::Vector3), asCALL_THISCALL);assert(r >= 0);

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
