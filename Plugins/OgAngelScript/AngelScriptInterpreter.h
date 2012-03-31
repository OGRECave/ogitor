///////////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////////

#ifndef ANGEL_SCRIPT_INTERPRETER_H
#define ANGEL_SCRIPT_INTERPRETER_H

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
   #ifdef PLUGIN_EXPORT
     #define PluginExport __declspec (dllexport)
   #else
     #define PluginExport __declspec (dllimport)
   #endif
#else
   #define PluginExport
#endif

#include <string>
#include <angelscript.h>
#include <scriptbuilder.h>
#include "scripthelper.h"
#include "debugger.h"

namespace Ogitors
{
    #define LI_MESSAGE "Angelscript Interpreter. Version 0.5. 2010"

    struct ContextDef
    {
        asIScriptModule*  module;
        int               funcID;
        asIScriptContext* context;
        float             sleep;
        float             delay;
    };

    typedef Ogre::map<unsigned int, ContextDef>::type ObjectContextMap;

    class PluginExport AngelScriptInterpreter : public OgitorsScriptInterpreter
    {
    public:
        AngelScriptInterpreter();
        virtual ~AngelScriptInterpreter();

        // return Interpreter Type String
        const std::string getTypeString() { return "AngelScript"; };
        // return Interpreter Init Message
        const std::string getInitMessage();
        //Initialization
        void Init() {};
        //Script Specific Engine Handle
        void *getHandle() { return mEngine; };
        //Create a new instance of the interpreter
        OgitorsScriptInterpreter* createNewInstance() { return OGRE_NEW AngelScriptInterpreter(); };
        // build a string
        Ogre::StringVector buildString(std::string &section, std::string &arg);
        // execute a string as script
        Ogre::StringVector execString(std::string &section, std::string &arg);
        // execute a script from file
        Ogre::StringVector runScript(std::string &section, std::string &file);
        // add function
        Ogre::StringVector addFunction(std::string &section, std::string &arg);
        // list functions
        Ogre::StringVector listFunctions(std::string &section);
        // compile a module from a script file
        Ogre::StringVector compileModule(std::string &section, std::string &file);
        // compile a module from memory
        Ogre::StringVector compileModule(std::string &section, const char *source);
        // run a previously compiled update function
        Ogre::StringVector runUpdateFunction(std::string &section, CBaseEditor *object, Ogre::Real time);
        // release resources associated with handle
        void releaseHandle(unsigned int handle);
        // Causes active context to sleep for a period
        static void sleep(unsigned int miliseconds);
        // Suspends active context
        static void yield();
        // Get active update object
        static CBaseEditor *getUpdateObject();
        // Get time since last update
        static float getTimeSinceLastUpdate();

        // Get active module
        static asIScriptModule *getActiveModule();
        // Get active context
        static asIScriptContext *getActiveContext();

    private:
        asIScriptEngine*  mEngine;
        CScriptBuilder*   mBuilder;
        unsigned int      mObjectHandleCounter;
        ObjectContextMap  mObjectContexts;
        static ContextDef*   mActiveContext;
        static CBaseEditor*  mActiveObject;

        // Callback for Angelscript to provide output.
        void MessageCallback(const asSMessageInfo *msg);
    };
}

extern void ExceptionCallback(asIScriptContext *ctx, void *param);
extern void PrintVariables(asIScriptContext *ctx, int stackLevel);
extern void LineCallback(asIScriptContext *ctx, void *param);

extern "C" bool PluginExport dllStartPlugin(void *identifier, Ogre::String& name);

extern "C" bool PluginExport dllGetPluginName(Ogre::String& name);

extern "C" bool PluginExport dllStopPlugin(void);

#endif