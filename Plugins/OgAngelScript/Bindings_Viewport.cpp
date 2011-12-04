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
#include "ViewportEditor.h"

#include "angelscript.h"
#include "Bindings_Viewport.h"


namespace Ogitors
{
    static void SetSnapMultiplierImpl(float a1, CViewportEditor& obj)
    {
        return CViewportEditor::SetSnapMultiplier(a1);
    }
    //-----------------------------------------------------------------------------------------
    static int GetEditorToolImpl(CViewportEditor& obj)
    {
        return CViewportEditor::GetEditorTool();
    }
    //-----------------------------------------------------------------------------------------
    static int GetEditorAxisImpl(CViewportEditor& obj)
    {
        return CViewportEditor::GetEditorAxis();
    }
    //-----------------------------------------------------------------------------------------
    void RegisterViewportBindings(asIScriptEngine *engine)
    {
	    int r;

        r = engine->RegisterObjectMethod("ViewportEditor", "void focusSelectedObject()", asMETHOD(CViewportEditor, FocusSelectedObject), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("ViewportEditor", "void deleteSelectedObject(bool, bool)", asMETHOD(CViewportEditor, DeleteSelectedObject), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("ViewportEditor", "CameraEditor@ getCamera()", asMETHOD(CViewportEditor, getCameraEditor), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("ViewportEditor", "void setCamera(CameraEditor@)", asMETHOD(CViewportEditor, setCameraEditor), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("ViewportEditor", "int getViewportRect(Vector4 &out)", asMETHOD(CViewportEditor, getRect), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("ViewportEditor", "void addCompositor(const string &in, int)", asMETHOD(CViewportEditor, addCompositor), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("ViewportEditor", "void removeCompositor(const string &in)", asMETHOD(CViewportEditor, removeCompositor), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("ViewportEditor", "int getCamPolyMode()", asMETHOD(CViewportEditor, getCamPolyMode), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("ViewportEditor", "int getIndex()", asMETHOD(CViewportEditor, getViewportIndex), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("ViewportEditor", "Vector3 getCamPosition()", asMETHOD(CViewportEditor, getCamPosition), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("ViewportEditor", "Quaternion getCamOrientation()", asMETHOD(CViewportEditor, getCamOrientation), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("ViewportEditor", "Vector2 getCamClipDistance()", asMETHOD(CViewportEditor, getCamClipDistance), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("ViewportEditor", "void setSnapMultiplier(float)", asFUNCTION(SetSnapMultiplierImpl), asCALL_CDECL_OBJLAST);assert(r >= 0);
        r = engine->RegisterObjectMethod("ViewportEditor", "void setCameraSpeed(float)", asMETHOD(CViewportEditor, SetCameraSpeed), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("ViewportEditor", "float getCameraSpeed()", asMETHOD(CViewportEditor, GetCameraSpeed), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("ViewportEditor", "void setEditorTool(EditorTools)", asMETHOD(CViewportEditor, SetEditorTool), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("ViewportEditor", "EditorTools getEditorTool()", asFUNCTION(GetEditorToolImpl), asCALL_CDECL_OBJLAST);assert(r >= 0);
        r = engine->RegisterObjectMethod("ViewportEditor", "void setEditorAxis(AxisType)", asMETHOD(CViewportEditor, SetEditorAxis), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("ViewportEditor", "AxisType getEditorAxis()", asFUNCTION(GetEditorAxisImpl), asCALL_CDECL_OBJLAST);assert(r >= 0);
        r = engine->RegisterObjectMethod("ViewportEditor", "bool getMouseRay(Vector2, Ray &out)", asMETHOD(CViewportEditor, GetMouseRay), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("ViewportEditor", "void saveEditorStates(Ray &in)", asMETHOD(CViewportEditor, SaveEditorStates), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("ViewportEditor", "void doSelect(Ray &in)", asMETHOD(CViewportEditor, DoSelect), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("ViewportEditor", "void cloneMove(bool)", asMETHOD(CViewportEditor, CloneMove), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("ViewportEditor", "void highlightObjectAtPosition(Ray &in)", asMETHOD(CViewportEditor, HighlightObjectAtPosition), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("ViewportEditor", "void setGridVisible(bool)", asMETHOD(CViewportEditor, ShowGrid), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("ViewportEditor", "bool getGridVisible(bool)", asMETHOD(CViewportEditor, IsGridVisible), asCALL_THISCALL);assert(r >= 0);
    }

}
