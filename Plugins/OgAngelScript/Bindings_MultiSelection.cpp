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

#include "OgitorsPrerequisites.h"
#include "BaseEditor.h"
#include "MultiSelEditor.h"

#include "angelscript.h"

namespace Ogitors
{

    void RegisterMultiSelectionBindings(asIScriptEngine *engine)
    {
        int r;
        r = engine->RegisterObjectMethod("MultiSelection", "void get(EditorVector &out)", asMETHODPR(CMultiSelEditor, getSelection, (ObjectVector&),void), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("MultiSelection", "void set(const EditorVector &in)", asMETHODPR(CMultiSelEditor, setSelection, (const ObjectVector&),void), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("MultiSelection", "void add(const EditorVector &in)", asMETHODPR(CMultiSelEditor, add, (const ObjectVector&),void), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("MultiSelection", "void remove(const EditorVector &in)", asMETHODPR(CMultiSelEditor, remove, (const ObjectVector&),void), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("MultiSelection", "void deleteAll()", asMETHOD(CMultiSelEditor, deleteObjects), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("MultiSelection", "bool contains(BaseEditor@)", asMETHOD(CMultiSelEditor, contains), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("MultiSelection", "bool containsOrEqual(BaseEditor@)", asMETHOD(CMultiSelEditor, containsOrEqual), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("MultiSelection", "bool isSingle()", asMETHOD(CMultiSelEditor, isSingle), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("MultiSelection", "bool isEmpty()", asMETHOD(CMultiSelEditor, isEmpty), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("MultiSelection", "BaseEditor@ getFirstObject()", asMETHOD(CMultiSelEditor, getFirstObject), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("MultiSelection", "BaseEditor@ getAsSingle()", asMETHOD(CMultiSelEditor, getAsSingle), asCALL_THISCALL);assert(r >= 0);
    }

};
