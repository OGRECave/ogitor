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
#include "OgitorsRoot.h"
#include "OgitorsSystem.h"
#include "BaseEditor.h"
#include "CameraEditor.h"
#include "TerrainEditor.h"
#include "TerrainPageEditor.h"
#include "TerrainGroupEditor.h"
#include "OgitorsScriptConsole.h"

#include "angelscript.h"
#include "scriptstdstring.h"
#include "scriptmath.h"
#include "as_ScriptOgreQuaternion.h"
#include "as_ScriptOgreVector3.h"
#include "scriptarray.h"

#include "stdvector.h"

#include "Bindings.h"
#include "Bindings_Base.h"
#include "Bindings_Enum.h"
#include "Bindings_MultiSelection.h"
#include "Bindings_Ogre.h"
#include "Bindings_Property.h"
#include "Bindings_Root.h"
#include "Bindings_UTF.h"
#include "Bindings_Utils.h"
#include "Bindings_Viewport.h"
#include "Bindings_System.h"
#include "ScriptStringExtensions.h"

namespace Ogitors
{
    //-----------------------------------------------------------------------------------------
    static void printConsole(const std::string& text)
    {
        OgitorsScriptConsole *console = OgitorsScriptConsole::getSingletonPtr();
        if(console)
            console->addOutput(text);
    }
    //-----------------------------------------------------------------------------------------
    static void printConsole(int number)
    {
        OgitorsScriptConsole *console = OgitorsScriptConsole::getSingletonPtr();
        if(console)
            console->addOutput(Ogre::StringConverter::toString(number));
    }
    //-----------------------------------------------------------------------------------------
    static void printConsole(unsigned int number)
    {
        OgitorsScriptConsole *console = OgitorsScriptConsole::getSingletonPtr();
        if(console)
            console->addOutput(Ogre::StringConverter::toString(number));
    }
    //-----------------------------------------------------------------------------------------
    static void printConsole(double number)
    {
        OgitorsScriptConsole *console = OgitorsScriptConsole::getSingletonPtr();
        if(console)
            console->addOutput(Ogre::StringConverter::toString((Ogre::Real)number));
    }
    //-----------------------------------------------------------------------------------------
    static void printConsole(const Ogre::Vector3& vec)
    {
        OgitorsScriptConsole *console = OgitorsScriptConsole::getSingletonPtr();
        if(console)
        {
            std::string text = "Vector {";
            text += Ogre::StringConverter::toString(vec);
            text += "}";
            console->addOutput(text);
        }
    }
    //-----------------------------------------------------------------------------------------
    static void printConsole(const Ogre::Quaternion& quat)
    {
        OgitorsScriptConsole *console = OgitorsScriptConsole::getSingletonPtr();
        if(console)
        {
            std::string text = "Quaternion {";
            text += Ogre::StringConverter::toString(quat);
            text += "}";
            console->addOutput(text);
        }
    }
    //-----------------------------------------------------------------------------------------
    static void printEditorObjectInfo(CBaseEditor *object)
    {
        std::string text = "OBJECT INFO : {Null Object}";

        if(object != 0)
        {
            text = "OBJECT INFO : {Name:' ";
            text += object->getName().c_str();
            text += " ', Type:' ";
            text += object->getTypeName().c_str();
            text += " '}";
        }

        OgitorsScriptConsole *console = OgitorsScriptConsole::getSingletonPtr();
        if(console)
            console->addOutput(text);
    }
    //-----------------------------------------------------------------------------------------
    static void logMessage(unsigned int level, const std::string& text)
    {
        Ogre::LogManager::getSingletonPtr()->logMessage(text, (Ogre::LogMessageLevel)level);
    }
    //-----------------------------------------------------------------------------------------
    static float UnitRandomImpl()
    {
        return Ogre::Math::UnitRandom();
    }
    //-----------------------------------------------------------------------------------------
    static float RangeRandomImpl(float a1, float a2)
    {
        return Ogre::Math::RangeRandom(a1, a2);
    }
    //-----------------------------------------------------------------------------------------
    bool prepareScriptBindings(OgitorsRoot *root, asIScriptEngine *engine)
    {
        int r;

        RegisterScriptArray(engine, true);
        RegisterStdString(engine);
        RegisterScriptStringExtensions(engine);

        RegisterUTFBindings(engine);
        RegisterScriptMath(engine);
        RegisterScriptOgreVector3(engine);
        RegisterOgreBindings(engine);
        RegisterScriptOgreQuaternion(engine);
        RegisterEnumBindings(engine);

        RegisterVector<Ogre::vector<Ogre::String>::type, Ogre::String>("StringVector","string", engine);
        RegisterVector<Ogre::vector<Ogre::UTFString>::type, Ogre::UTFString>("UTFStringVector","UTFString", engine);

        r = engine->RegisterGlobalFunction("void printConsole(const string &in)", asFUNCTIONPR(printConsole, (const std::string&), void), asCALL_CDECL);assert(r >= 0);
        r = engine->RegisterGlobalFunction("void printConsole(int)", asFUNCTIONPR(printConsole, (int), void), asCALL_CDECL);assert(r >= 0);
        r = engine->RegisterGlobalFunction("void printConsole(uint)", asFUNCTIONPR(printConsole, (unsigned int), void), asCALL_CDECL);assert(r >= 0);
        r = engine->RegisterGlobalFunction("void printConsole(double)", asFUNCTIONPR(printConsole, (double), void), asCALL_CDECL);assert(r >= 0);
        r = engine->RegisterGlobalFunction("void printConsole(Vector3)", asFUNCTIONPR(printConsole, (const Ogre::Vector3&), void), asCALL_CDECL);assert(r >= 0);
        r = engine->RegisterGlobalFunction("void printConsole(Quaternion)", asFUNCTIONPR(printConsole, (const Ogre::Quaternion&), void), asCALL_CDECL);assert(r >= 0);
        r = engine->RegisterGlobalFunction("void logMessage(uint, const string &in)", asFUNCTION(logMessage), asCALL_CDECL);assert(r >= 0);
        r = engine->RegisterGlobalFunction("float rand()", asFUNCTION(UnitRandomImpl), asCALL_CDECL);assert(r >= 0);
        r = engine->RegisterGlobalFunction("float rand(float, float)", asFUNCTION(RangeRandomImpl), asCALL_CDECL);assert(r >= 0);


        r = engine->RegisterObjectType("OgitorsRoot", 0, asOBJ_REF | asOBJ_NOHANDLE);assert(r >= 0);
        r = engine->RegisterObjectType("OgitorsSystem", 0, asOBJ_REF | asOBJ_NOHANDLE);assert(r >= 0);
        r = engine->RegisterObjectType("OgitorsUtils", 0, asOBJ_REF | asOBJ_NOHANDLE);assert(r >= 0);
        // Register the singleton's address that the script will use to access it
        r = engine->RegisterGlobalProperty("OgitorsRoot root", root);assert(r >= 0);
        r = engine->RegisterGlobalProperty("OgitorsSystem system", OgitorsSystem::getSingletonPtr());assert(r >= 0);
        r = engine->RegisterGlobalProperty("OgitorsUtils utils", 0);assert(r >= 0);

        // Register BaseEditor since all others need it
        r = engine->RegisterObjectType("BaseEditor", 0, asOBJ_REF);assert(r >= 0);
        r = engine->RegisterObjectBehaviour("BaseEditor", asBEHAVE_ADDREF, "void f()", asMETHOD(CBaseEditor,_addRef), asCALL_THISCALL); assert( r >= 0 );
        r = engine->RegisterObjectBehaviour("BaseEditor", asBEHAVE_RELEASE, "void f()", asMETHOD(CBaseEditor,_release), asCALL_THISCALL); assert( r >= 0 );

        RegisterVector<Ogre::vector<CBaseEditor*>::type, CBaseEditor*>("EditorVector","BaseEditor@", engine);

        r = engine->RegisterGlobalFunction("void printEditorObjectInfo(BaseEditor@)", asFUNCTION(printEditorObjectInfo), asCALL_CDECL);assert(r >= 0);

        RegisterPropertyBindings(engine);
        RegisterBaseEditorBindings(engine);
        RegisterViewportBindings(engine);
        RegisterMultiSelectionBindings(engine);
        RegisterRootBindings(engine);
        RegisterUtilsBindings(engine);
        RegisterSystemBindings(engine);

        return true;
    }
    //-----------------------------------------------------------------------------------------
    bool unPrepareScriptBindings(asIScriptEngine *engine)
    {
        return true;
    }
    //----------------------------------------------------------------------------------------
}
