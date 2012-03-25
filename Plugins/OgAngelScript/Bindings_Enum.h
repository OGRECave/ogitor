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

#ifndef BINDINGS_ENUM_H
#define BINDINGS_ENUM_H

class asIScriptEngine;

namespace Ogitors
{

#define REG_ENUM_VALUE(type, value) r = engine->RegisterEnumValue(type, #value, value);assert(r >= 0);

    void RegisterEnumBindings(asIScriptEngine *engine)
    {
        int r;

        r = engine->RegisterEnum("PropertyType");assert(r >= 0);
        REG_ENUM_VALUE("PropertyType", PROP_SHORT);
        REG_ENUM_VALUE("PropertyType", PROP_UNSIGNED_SHORT);
        REG_ENUM_VALUE("PropertyType", PROP_INT);
        REG_ENUM_VALUE("PropertyType", PROP_UNSIGNED_INT);
        REG_ENUM_VALUE("PropertyType", PROP_LONG);
        REG_ENUM_VALUE("PropertyType", PROP_UNSIGNED_LONG);
        REG_ENUM_VALUE("PropertyType", PROP_REAL);
        REG_ENUM_VALUE("PropertyType", PROP_STRING);
        REG_ENUM_VALUE("PropertyType", PROP_VECTOR2);
        REG_ENUM_VALUE("PropertyType", PROP_VECTOR3);
        REG_ENUM_VALUE("PropertyType", PROP_VECTOR4);
        REG_ENUM_VALUE("PropertyType", PROP_COLOUR);
        REG_ENUM_VALUE("PropertyType", PROP_BOOL);
        REG_ENUM_VALUE("PropertyType", PROP_QUATERNION);
        REG_ENUM_VALUE("PropertyType", PROP_MATRIX3);
        REG_ENUM_VALUE("PropertyType", PROP_MATRIX4);
        REG_ENUM_VALUE("PropertyType", PROP_UNKNOWN);

        r = engine->RegisterEnum("EditorType");assert(r >= 0);
        REG_ENUM_VALUE("EditorType", ETYPE_BASE);
        REG_ENUM_VALUE("EditorType", ETYPE_SCENEMANAGER);
        REG_ENUM_VALUE("EditorType", ETYPE_VIEWPORT);
        REG_ENUM_VALUE("EditorType", ETYPE_PAGINGMANAGER);
        REG_ENUM_VALUE("EditorType", ETYPE_SKY_MANAGER);
        REG_ENUM_VALUE("EditorType", ETYPE_LIGHT);
        REG_ENUM_VALUE("EditorType", ETYPE_TERRAIN_MANAGER);
        REG_ENUM_VALUE("EditorType", ETYPE_WATER_MANAGER);
        REG_ENUM_VALUE("EditorType", ETYPE_CUSTOM_MANAGER);
        REG_ENUM_VALUE("EditorType", ETYPE_NODE);
        REG_ENUM_VALUE("EditorType", ETYPE_CAMERA);
        REG_ENUM_VALUE("EditorType", ETYPE_ENTITY);
        REG_ENUM_VALUE("EditorType", ETYPE_PLANE);
        REG_ENUM_VALUE("EditorType", ETYPE_PARTICLE);
        REG_ENUM_VALUE("EditorType", ETYPE_BILLBOARDSET);
        REG_ENUM_VALUE("EditorType", ETYPE_MOVABLE);
        REG_ENUM_VALUE("EditorType", ETYPE_GENERALPURPOSE);
        REG_ENUM_VALUE("EditorType", ETYPE_MULTISEL);

        r = engine->RegisterEnum("LoadState");assert(r >= 0);
        REG_ENUM_VALUE("LoadState", LS_UNLOADED);
        REG_ENUM_VALUE("LoadState", LS_LOADING);
        REG_ENUM_VALUE("LoadState", LS_LOADED);

        r = engine->RegisterEnum("RunState");assert(r >= 0);
        REG_ENUM_VALUE("RunState", RS_STOPPED);
        REG_ENUM_VALUE("RunState", RS_PAUSED);
        REG_ENUM_VALUE("RunState", RS_RUNNING);

        r = engine->RegisterEnum("AxisType");assert(r >= 0);
        REG_ENUM_VALUE("AxisType", AXIS_X);
        REG_ENUM_VALUE("AxisType", AXIS_Y);
        REG_ENUM_VALUE("AxisType", AXIS_XY);
        REG_ENUM_VALUE("AxisType", AXIS_Z);
        REG_ENUM_VALUE("AxisType", AXIS_XZ);
        REG_ENUM_VALUE("AxisType", AXIS_YZ);
        REG_ENUM_VALUE("AxisType", AXIS_ALL);

        r = engine->RegisterEnum("EditFlags");assert(r >= 0);
        REG_ENUM_VALUE("EditFlags", CAN_MOVE);
        REG_ENUM_VALUE("EditFlags", CAN_SCALE);
        REG_ENUM_VALUE("EditFlags", CAN_ROTATEX);
        REG_ENUM_VALUE("EditFlags", CAN_ROTATEY);
        REG_ENUM_VALUE("EditFlags", CAN_ROTATEZ);
        REG_ENUM_VALUE("EditFlags", CAN_ROTATE);
        REG_ENUM_VALUE("EditFlags", CAN_DELETE);
        REG_ENUM_VALUE("EditFlags", CAN_UNDO);
        REG_ENUM_VALUE("EditFlags", CAN_CLONE);
        REG_ENUM_VALUE("EditFlags", CAN_FOCUS);
        REG_ENUM_VALUE("EditFlags", CAN_DRAG);
        REG_ENUM_VALUE("EditFlags", CAN_DROP);
        REG_ENUM_VALUE("EditFlags", CAN_USEMATERIAL);
        REG_ENUM_VALUE("EditFlags", CAN_ACCEPTCOPY);
        REG_ENUM_VALUE("EditFlags", CAN_ACCEPTPASTE);
        REG_ENUM_VALUE("EditFlags", CAN_PAGE);

        r = engine->RegisterEnum("EditorTools");assert(r >= 0);
        REG_ENUM_VALUE("EditorTools", TOOL_SELECT);
        REG_ENUM_VALUE("EditorTools", TOOL_MOVE);
        REG_ENUM_VALUE("EditorTools", TOOL_ROTATE);
        REG_ENUM_VALUE("EditorTools", TOOL_SCALE);
        REG_ENUM_VALUE("EditorTools", TOOL_DEFORM);
        REG_ENUM_VALUE("EditorTools", TOOL_SPLAT);
        REG_ENUM_VALUE("EditorTools", TOOL_SPLATGRASS);
        REG_ENUM_VALUE("EditorTools", TOOL_PAINT);
        REG_ENUM_VALUE("EditorTools", TOOL_SMOOTH);

        r = engine->RegisterEnum("EditorToolsEx");assert(r >= 0);
        REG_ENUM_VALUE("EditorToolsEx", TOOL_EX_NONE);
        REG_ENUM_VALUE("EditorToolsEx", TOOL_EX_CLONE);
        REG_ENUM_VALUE("EditorToolsEx", TOOL_EX_INSTANCECLONE);

        r = engine->RegisterEnum("QueryFlags");assert(r >= 0);
        REG_ENUM_VALUE("QueryFlags", QUERYFLAG_WIDGET);
        REG_ENUM_VALUE("QueryFlags", QUERYFLAG_MOVABLE);

        r = engine->RegisterEnum("DialogReturnType");assert(r >= 0);
        REG_ENUM_VALUE("DialogReturnType", DLGRET_YES);
        REG_ENUM_VALUE("DialogReturnType", DLGRET_NO);
        REG_ENUM_VALUE("DialogReturnType", DLGRET_CANCEL);

        r = engine->RegisterEnum("DialogType");assert(r >= 0);
        REG_ENUM_VALUE("DialogType", DLGTYPE_OK);
        REG_ENUM_VALUE("DialogType", DLGTYPE_YESNO);
        REG_ENUM_VALUE("DialogType", DLGTYPE_YESNOCANCEL);

        r = engine->RegisterEnum("MouseCursorType");assert(r >= 0);
        REG_ENUM_VALUE("MouseCursorType", MOUSE_ARROW);
        REG_ENUM_VALUE("MouseCursorType", MOUSE_CROSS);
    
        r = engine->RegisterEnum("MouseButtonType");assert(r >= 0);
        REG_ENUM_VALUE("MouseButtonType", OMB_LEFT);
        REG_ENUM_VALUE("MouseButtonType", OMB_RIGHT);
        REG_ENUM_VALUE("MouseButtonType", OMB_MIDDLE);

        r = engine->RegisterEnum("CameraViewMode");assert(r >= 0);
        REG_ENUM_VALUE("CameraViewMode", CVM_FREE);
        REG_ENUM_VALUE("CameraViewMode", CVM_GLOBAL_LEFT);
        REG_ENUM_VALUE("CameraViewMode", CVM_GLOBAL_RIGHT);
        REG_ENUM_VALUE("CameraViewMode", CVM_GLOBAL_FRONT);
        REG_ENUM_VALUE("CameraViewMode", CVM_GLOBAL_BACK);
        REG_ENUM_VALUE("CameraViewMode", CVM_GLOBAL_TOP);
        REG_ENUM_VALUE("CameraViewMode", CVM_GLOBAL_BOTTOM);
        REG_ENUM_VALUE("CameraViewMode", CVM_LOCAL_LEFT);
        REG_ENUM_VALUE("CameraViewMode", CVM_LOCAL_RIGHT);
        REG_ENUM_VALUE("CameraViewMode", CVM_LOCAL_FRONT);
        REG_ENUM_VALUE("CameraViewMode", CVM_LOCAL_BACK);
        REG_ENUM_VALUE("CameraViewMode", CVM_LOCAL_TOP);
        REG_ENUM_VALUE("CameraViewMode", CVM_LOCAL_BOTTOM);

        r = engine->RegisterEnum("PolygonMode");assert(r >= 0);
        engine->RegisterEnumValue("PolygonMode", "PM_SOLID", Ogre::PM_SOLID);assert(r >= 0);
        engine->RegisterEnumValue("PolygonMode", "PM_POINTS", Ogre::PM_POINTS);assert(r >= 0);
        engine->RegisterEnumValue("PolygonMode", "PM_WIREFRAME", Ogre::PM_WIREFRAME);assert(r >= 0);

        r = engine->RegisterEnum("LightType");assert(r >= 0);
        engine->RegisterEnumValue("LightType", "LT_POINT", Ogre::Light::LT_POINT);assert(r >= 0);
        engine->RegisterEnumValue("LightType", "LT_DIRECTIONAL", Ogre::Light::LT_DIRECTIONAL);assert(r >= 0);
        engine->RegisterEnumValue("LightType", "LT_SPOTLIGHT", Ogre::Light::LT_SPOTLIGHT);assert(r >= 0);
    }

#undef REG_ENUM_VALUE
}

#endif