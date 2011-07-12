///////////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////////

#include "Ogitors.h"
#include "OgitorsScriptInterpreter.h"
#include "AngelScriptInterpreter.h"
#include "Bindings.h"

#include "ofs.h"

using namespace Ogitors;
using namespace Ogre;

ContextDef*       AngelScriptInterpreter::mActiveContext = 0;
CBaseEditor*      AngelScriptInterpreter::mActiveObject = 0;

static void getUpdateObjectGeneric(asIScriptGeneric *gen)
{
    gen->SetReturnObject(AngelScriptInterpreter::getUpdateObject());
}
//----------------------------------------------------------------------------
static void getTimeSinceLastFrameGeneric(asIScriptGeneric *gen)
{
    gen->SetReturnFloat(AngelScriptInterpreter::getTimeSinceLastFrame());
}
//----------------------------------------------------------------------------
static void getTimeSinceLastUpdateGeneric(asIScriptGeneric *gen)
{
    gen->SetReturnFloat(AngelScriptInterpreter::getTimeSinceLastUpdate());
}
//----------------------------------------------------------------------------
AngelScriptInterpreter::AngelScriptInterpreter() : OgitorsScriptInterpreter()
{
    mEngine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
    mEngine->SetMessageCallback(asMETHOD(AngelScriptInterpreter,MessageCallback), this, asCALL_THISCALL);
    mBuilder = new CScriptBuilder();

    mObjectHandleCounter = 0;
    mObjectContexts.clear();

    prepareScriptBindings(mOgitorsRoot, mEngine);

    int r;
    r = mEngine->RegisterGlobalFunction("void yield()", asFUNCTION(AngelScriptInterpreter::yield), asCALL_CDECL);assert(r >= 0);
    r = mEngine->RegisterGlobalFunction("void sleep(uint)", asFUNCTION(AngelScriptInterpreter::sleep), asCALL_CDECL);assert(r >= 0);
    r = mEngine->RegisterGlobalFunction("float getTimeSinceLastUpdate()", asFUNCTION(getTimeSinceLastUpdateGeneric), asCALL_GENERIC);assert(r >= 0);
    r = mEngine->RegisterGlobalFunction("float getTimeSinceLastFrame()", asFUNCTION(getTimeSinceLastFrameGeneric), asCALL_GENERIC);assert(r >= 0);
    r = mEngine->RegisterGlobalFunction("BaseEditor@ getUpdateObject()", asFUNCTION(getUpdateObjectGeneric), asCALL_GENERIC);assert(r >= 0);
}
//----------------------------------------------------------------------------
AngelScriptInterpreter::~AngelScriptInterpreter()
{
    ObjectContextMap::iterator it = mObjectContexts.begin();
    while(it != mObjectContexts.end())
    {
        it->second.context->Release();
        it++;
    }

    mObjectContexts.clear();

    delete mBuilder;

    unPrepareScriptBindings(mEngine);

    mEngine->Release();
}
//----------------------------------------------------------------------------
const std::string AngelScriptInterpreter::getInitMessage()
{
    std::string version_string(asGetLibraryVersion());

    return std::string(LI_MESSAGE) + ". Using Angelscript version " + version_string;
}
//----------------------------------------------------------------------------
void AngelScriptInterpreter::yield()
{
    if(mActiveContext)
        mActiveContext->context->Suspend();
}
//----------------------------------------------------------------------------
void AngelScriptInterpreter::sleep(unsigned int miliseconds)
{
    if(mActiveContext)
    {
        mActiveContext->context->Suspend();
        mActiveContext->sleep = (float)miliseconds / 1000.0f;
    }
}
//----------------------------------------------------------------------------
CBaseEditor *AngelScriptInterpreter::getUpdateObject()
{
    return mActiveObject;
}
//----------------------------------------------------------------------------
asIScriptModule *AngelScriptInterpreter::getActiveModule()
{
    if(mActiveContext)
        return mActiveContext->module;
    else
        return 0;
}
//----------------------------------------------------------------------------
asIScriptContext *AngelScriptInterpreter::getActiveContext()
{
    if(mActiveContext)
        return mActiveContext->context;
    else
        return 0;
}
//----------------------------------------------------------------------------
float AngelScriptInterpreter::getTimeSinceLastUpdate()
{
    if(mActiveContext)
        return mActiveContext->delay;
    else
        return 0.0f;
}//----------------------------------------------------------------------------
Ogre::StringVector AngelScriptInterpreter::buildString(std::string &section, std::string &arg)
{
    Ogre::StringVector ret;

    mBuilder->StartNewModule(mEngine, section.c_str());
    mBuilder->AddSectionFromMemory(arg.c_str());
    int r = mBuilder->BuildModule();
    if( r < 0 )
    {
        // An error occurred. Instruct the script writer to fix the
        // compilation errors that were listed in the output stream.
        ret.push_back("Please correct the errors in the script and try again.");
        return ret;
    }

    return ret;
}
//----------------------------------------------------------------------------
Ogre::StringVector AngelScriptInterpreter::runScript(std::string &section, std::string &file)
{
    Ogre::StringVector ret;

    int r = mBuilder->StartNewModule(mEngine, section.c_str());
    if( r < 0 )
    {
        // If the code fails here it is usually because there
        // is no more memory to allocate the module
        return ret;
    }

    if(file.substr(0,5) == "proj:")
    {
        file.erase(0,5);

        OFS::OFSHANDLE fHandle;
        OFS::OfsPtr& ofsFile = OgitorsRoot::getSingletonPtr()->GetProjectFile();

        if(ofsFile->openFile(fHandle, file.c_str()) == OFS::OFS_OK)
        {
            char *contents;
            unsigned int file_size = 0;
            ofsFile->getFileSize(fHandle, file_size);

            if(file_size > 0)
            {
                contents = new char[file_size + 1];
                ofsFile->read(fHandle, contents, file_size);
                contents[file_size] = 0;
                r = mBuilder->AddSectionFromMemory(contents, file.c_str());
                delete [] contents;
            }
            else
                r = -1;

            ofsFile->closeFile(fHandle);
        }
        else
            r = -1;
    }
    else
       r = mBuilder->AddSectionFromFile(file.c_str());

    if( r < 0 )
    {
        // The builder wasn't able to load the file. Maybe the file
        // has been removed, or the wrong name was given, or some
        // preprocessing commands are incorrectly written.
        ret.push_back("The script file can not be found.");
        return ret;
    }
    int errpos;

    {
        OGRE_LOCK_AUTO_MUTEX
        errpos = mBuffer.size();
    }
    r = mBuilder->BuildModule();
    if( r < 0 )
    {
        // An error occurred. Instruct the script writer to fix the
        // compilation errors that were listed in the output stream.

        OGRE_LOCK_AUTO_MUTEX
        for(unsigned int i = errpos;i < mBuffer.size();i++)
        {
            char buf[1000];
            sprintf(buf,"Row: %d, Col: %d :: %s", mBuffer[i].mRow, mBuffer[i].mCol, mBuffer[i].mMessage.c_str());

            ret.push_back(Ogre::String(buf));
        }

        ret.push_back("Please correct the errors in the script and try again.");
        return ret;
    }

    // Find the function that is to be called.
    asIScriptModule *mod = mEngine->GetModule(section.c_str());
    int funcId = mod->GetFunctionIdByDecl("void main()");
    if( funcId < 0 )
    {
        // The function couldn't be found. Instruct the script writer
        // to include the expected function in the script.
        ret.push_back("The script must have the function 'void main()'. Please add it and try again.");
        return ret;
    }

    // Create our context, prepare it, and then execute
    asIScriptContext *ctx = mEngine->CreateContext();
    ctx->SetExceptionCallback(asFUNCTION(ExceptionCallback), this, asCALL_CDECL);
    ctx->Prepare(funcId);

    ContextDef def;
    def.context = ctx;
    def.module = mod;
    def.funcID = funcId;
    def.sleep = 0.0f;
    def.delay = 0.0f;
    mActiveContext = &def;

    r = ctx->Execute();
    if( r != asEXECUTION_FINISHED )
    {
        // The execution didn't complete as expected. Determine what happened.
        if( r == asEXECUTION_EXCEPTION )
        {
            // An exception occurred, let the script writer know what happened so it can be corrected.
            ret.push_back("An exception '" + std::string(ctx->GetExceptionString()) + "' occurred. Please correct the code and try again.");
        }
    }

    mActiveContext = 0;

    ctx->Release();

    return ret;
}
//----------------------------------------------------------------------------
Ogre::StringVector AngelScriptInterpreter::compileModule(std::string &section, std::string &file)
{
    Ogre::StringVector ret;

    int r = mBuilder->StartNewModule(mEngine, section.c_str());
    if( r < 0 )
    {
        // If the code fails here it is usually because there
        // is no more memory to allocate the module
        return ret;
    }

    if(file.substr(0,5) == "proj:")
    {
        file.erase(0,5);

        OFS::OFSHANDLE fHandle;
        OFS::OfsPtr& ofsFile = OgitorsRoot::getSingletonPtr()->GetProjectFile();

        if(ofsFile->openFile(fHandle, file.c_str()) == OFS::OFS_OK)
        {
            char *contents;
            unsigned int file_size = 0;
            ofsFile->getFileSize(fHandle, file_size);

            if(file_size > 0)
            {
                contents = new char[file_size + 1];
                ofsFile->read(fHandle, contents, file_size);
                contents[file_size] = 0;
                r = mBuilder->AddSectionFromMemory(contents, file.c_str());
                delete [] contents;
            }
            else
                r = -1;

            ofsFile->closeFile(fHandle);
        }
        else
            r = -1;
    }
    else
       r = mBuilder->AddSectionFromFile(file.c_str());

    if( r < 0 )
    {
        // The builder wasn't able to load the file. Maybe the file
        // has been removed, or the wrong name was given, or some
        // preprocessing commands are incorrectly written.
        ret.push_back("The script file can not be found.");
        return ret;
    }

    int errpos;

    {
        OGRE_LOCK_AUTO_MUTEX
        errpos = mBuffer.size();
    }

    r = mBuilder->BuildModule();
    if( r < 0 )
    {
        // An error occurred. Instruct the script writer to fix the
        // compilation errors that were listed in the output stream.
        ret.push_back("Please correct the errors in the script and try again.");
        ret.push_back("Following errors found in script:");

        OGRE_LOCK_AUTO_MUTEX
        for(unsigned int i = errpos;i < mBuffer.size();i++)
        {
            char buf[1000];
            sprintf(buf,"Row: %d, Col: %d :: %s", mBuffer[i].mRow, mBuffer[i].mCol, mBuffer[i].mMessage.c_str());

            ret.push_back(Ogre::String(buf));
        }

        return ret;
    }

    return ret;
}
//----------------------------------------------------------------------------
Ogre::StringVector AngelScriptInterpreter::compileModule(std::string &section, const char *source)
{
    Ogre::StringVector ret;

    int r = mBuilder->StartNewModule(mEngine, section.c_str());
    if( r < 0 )
    {
        return ret;
    }
    r = mBuilder->AddSectionFromMemory(source, "memory");
    if( r < 0 )
    {
        return ret;
    }

    int errpos;

    {
        OGRE_LOCK_AUTO_MUTEX
        errpos = mBuffer.size();
    }

    r = mBuilder->BuildModule();
    if( r < 0 )
    {
        // An error occurred. Instruct the script writer to fix the
        // compilation errors that were listed in the output stream.
        ret.push_back("Please correct the errors in the script and try again.");
        ret.push_back("Following errors found in script:");

        OGRE_LOCK_AUTO_MUTEX
        for(unsigned int i = errpos;i < mBuffer.size();i++)
        {
            char buf[1000];
            sprintf(buf,"Row: %d, Col: %d :: %s", mBuffer[i].mRow, mBuffer[i].mCol, mBuffer[i].mMessage.c_str());

            ret.push_back(Ogre::String(buf));
        }

        return ret;
    }

    ret.push_back("Compilation successful!!");

    return ret;
}
//----------------------------------------------------------------------------
Ogre::StringVector AngelScriptInterpreter::runUpdateFunction(std::string &section, CBaseEditor *object, Ogre::Real time)
{
    Ogre::StringVector ret;
    ContextDef *def;

    unsigned int objectresourcehandle = object->getScriptResourceHandle();

    asIScriptContext* ctx = 0;

    if(objectresourcehandle != 0)
    {
        ObjectContextMap::iterator it = mObjectContexts.find(objectresourcehandle);
        if(it != mObjectContexts.end())
        {
            def = &(it->second);
            ctx = def->context;
            float tsince = mTimeSinceLastFrame;
            def->sleep -= mTimeSinceLastFrame;
            def->delay += mTimeSinceLastFrame;
            if(def->sleep > 0.0f)
                return ret;
        }
    }

    if(ctx == 0)
    {
        objectresourcehandle = ++mObjectHandleCounter;
        object->setScriptResourceHandle(objectresourcehandle);

        Ogre::String script = object->getUpdateScript();

        assert(!script.empty());

        Ogre::String module = Ogre::StringConverter::toString(object->getObjectID());

        script = "proj:/Scripts/" + script;

        compileModule(module, script);

        ContextDef newdef;
        newdef.sleep = 0.0f;
        newdef.delay = 0.0f;
        newdef.module = mEngine->GetModule(section.c_str());
        newdef.funcID = newdef.module->GetFunctionIdByDecl("void update()");
        newdef.context = mEngine->CreateContext();
        newdef.context->SetExceptionCallback(asFUNCTION(ExceptionCallback), this, asCALL_CDECL);
        mObjectContexts[objectresourcehandle] = newdef;
        def = &(mObjectContexts[objectresourcehandle]);
        ctx = newdef.context;
    }

    if( def->funcID < 0 )
    {
        // The function couldn't be found. Instruct the script writer
        // to include the expected function in the script.
        ret.push_back("The script must have the function 'void update()'. Please add it and try again.");
        return ret;
    }

    mActiveContext = def;
    mActiveObject = object;

    if(ctx->GetState() != asEXECUTION_SUSPENDED)
        ctx->Prepare(def->funcID);

    int r = ctx->Execute();
    def->delay = 0.0f;

    switch(r)
    {
    case asEXECUTION_SUSPENDED:
    case asEXECUTION_FINISHED:break;
    default:{
                ctx->Release();
                ctx = mEngine->CreateContext();
                ctx->SetExceptionCallback(asFUNCTION(ExceptionCallback), this, asCALL_CDECL);
                mObjectContexts[objectresourcehandle].context = ctx;
                // The execution didn't complete as expected. Determine what happened.
                if( r == asEXECUTION_EXCEPTION )
                {
                    // An exception occurred, let the script writer know what happened so it can be corrected.
                    ret.push_back("An exception '" + std::string(ctx->GetExceptionString()) + "' occurred. Please correct the code and try again.");
                }
            }
    }

    mActiveContext = 0;
    mActiveObject = 0;

    return ret;
}
//----------------------------------------------------------------------------
Ogre::StringVector AngelScriptInterpreter::addFunction(std::string &section, std::string &arg)
{
    Ogre::StringVector ret;

    asIScriptModule *mod = mEngine->GetModule(section.c_str(), asGM_CREATE_IF_NOT_EXISTS);

    asIScriptFunction *func = 0;
    int r = mod->CompileFunction("addfunc", arg.c_str(), 0, asCOMP_ADD_TO_MODULE, &func);
    if( r < 0 )
    {
        // TODO: Add better description of error (invalid declaration, name conflict, etc)
        ret.push_back("Failed to add function.");
    }
    else
    {
        // The script engine supports function overloads, but to simplify the
        // console we'll disallow multiple functions with the same name
        if( mod->GetFunctionIdByName(func->GetName()) == asMULTIPLE_FUNCTIONS )
        {
            mod->RemoveFunction(func->GetId());
            ret.push_back("Another function with that name already exists.");
        }
        else
            ret.push_back("Function added.");
    }

    // We must release the function object
    if( func )
        func->Release();

    return ret;
}
//----------------------------------------------------------------------------
Ogre::StringVector AngelScriptInterpreter::listFunctions(std::string &section)
{
    Ogre::StringVector ret;

    asUINT n;

    // List the application registered functions
    for( n = 0; n < mEngine->GetGlobalFunctionCount(); n++ )
    {
        int id = mEngine->GetGlobalFunctionIdByIndex(n);
        asIScriptFunction *func = mEngine->GetFunctionDescriptorById(id);

        // Skip the functions that start with _ as these are not meant to be called explicitly by the user
        if( func->GetName()[0] != '_' )
            ret.push_back(" " + std::string(func->GetDeclaration()));
    }

    ret.push_back("I am listing functions:");
    // List the user functions in the module
    asIScriptModule *mod = mEngine->GetModule(section.c_str());
    if( mod )
    {
        ret.push_back("");
        ret.push_back("User functions:");
        for( n = 0; n < mod->GetFunctionCount(); n++ )
        {
            asIScriptFunction *func = mod->GetFunctionDescriptorByIndex(n);
            ret.push_back(func->GetDeclaration());
        }
    }

    return ret;
}
//----------------------------------------------------------------------------
Ogre::StringVector AngelScriptInterpreter::execString(std::string &section, std::string &arg)
{
    Ogre::StringVector ret;

    // Wrap the expression in with a call to _grab, which allow us to print the resulting value
    std::string script = arg;//"_grab(" + arg + ")";

    // TODO: Add a time out to the script, so that never ending scripts doesn't freeze the application

    asIScriptContext *ctx = mEngine->CreateContext();
    ctx->SetExceptionCallback(asFUNCTION(ExceptionCallback), this, asCALL_CDECL);
    int r = ExecuteString(mEngine, script.c_str(), mEngine->GetModule(section.c_str()), ctx);
    if( r < 0 )
    {
        ret.push_back("Invalid script statement.");
    }
    else if( r == asEXECUTION_EXCEPTION )
    {
        ret.push_back("A script exception was raised.");
    }

    return ret;
}
//----------------------------------------------------------------------------
void AngelScriptInterpreter::releaseHandle(unsigned int handle)
{
    ObjectContextMap::iterator it = mObjectContexts.find(handle);

    if(it != mObjectContexts.end())
    {
        it->second.context->Release();
        mObjectContexts.erase(it);
    }
}
//----------------------------------------------------------------------------

static OutputData data;

//----------------------------------------------------------------------------
void AngelScriptInterpreter::MessageCallback(const asSMessageInfo *msg)
{
    OGRE_LOCK_AUTO_MUTEX

    data.mType = (int)msg->type;
    data.mSection = msg->section;
    data.mRow = msg->row;
    data.mCol = msg->col;
    data.mMessage = msg->message;

    printf("AS ERROR: %s\n", msg->message);

    mBuffer.push_back(data);
}
//----------------------------------------------------------------------------
void LineCallback(asIScriptContext *ctx, void *param)
{
	char tmp[1024]="";
	asIScriptEngine *engine = ctx->GetEngine();
    int funcID = ctx->GetFunction(0)->GetId();
	int col;
    const char *section;
	int line = ctx->GetLineNumber(0,&col,&section);
	int indent = ctx->GetCallstackSize();
	for( int n = 0; n < indent; n++ )
		sprintf(tmp+n," ");
	const asIScriptFunction *function = engine->GetFunctionDescriptorById(funcID);
	sprintf(tmp+indent,"%s:%s:%d,%d", function->GetModuleName(),
	                    function->GetDeclaration(),
	                    line, col);

	LogManager::getSingleton().logMessage(tmp);

	PrintVariables(ctx, -1);
}
//----------------------------------------------------------------------------
void PrintVariables(asIScriptContext *ctx, int stackLevel)
{
	char tmp[1024]="";
	asIScriptEngine *engine = ctx->GetEngine();

	int typeId = ctx->GetThisTypeId(stackLevel);
	void *varPointer = ctx->GetThisPointer(stackLevel);
	if( typeId )
	{
		sprintf(tmp," this = %p", varPointer);
		LogManager::getSingleton().logMessage(tmp);
	}

	int numVars = ctx->GetVarCount(stackLevel);
	for( int n = 0; n < numVars; n++ )
	{
		int typeId = ctx->GetVarTypeId(n, stackLevel);
		void *varPointer = ctx->GetAddressOfVar(n, stackLevel);
		if( typeId == engine->GetTypeIdByDecl("int") )
		{
			sprintf(tmp, " %s = %d", ctx->GetVarDeclaration(n, stackLevel), *(int*)varPointer);
			LogManager::getSingleton().logMessage(tmp);
		}
		else if( typeId == engine->GetTypeIdByDecl("string") )
		{
			std::string *str = (std::string*)varPointer;
			if( str )
			{
				sprintf(tmp, " %s = '%s'", ctx->GetVarDeclaration(n, stackLevel), str->c_str());
				LogManager::getSingleton().logMessage(tmp);
			} else
			{
				sprintf(tmp, " %s = <null>", ctx->GetVarDeclaration(n, stackLevel));
				LogManager::getSingleton().logMessage(tmp);
			}
		LogManager::getSingleton().logMessage(tmp);
		}
	}
};
//----------------------------------------------------------------------------
void ExceptionCallback(asIScriptContext *ctx, void *param)
{
	asIScriptEngine *engine = ctx->GetEngine();
	int funcID = ctx->GetExceptionFunction();
	const asIScriptFunction *function = engine->GetFunctionDescriptorById(funcID);
	LogManager::getSingleton().logMessage("--- exception ---");
	LogManager::getSingleton().logMessage("desc: " + String(ctx->GetExceptionString()));
	LogManager::getSingleton().logMessage("func: " + String(function->GetDeclaration()));
	LogManager::getSingleton().logMessage("modl: " + String(function->GetModuleName()));
	LogManager::getSingleton().logMessage("sect: " + String(function->GetScriptSectionName()));
	int col, line = ctx->GetExceptionLineNumber(&col);
	LogManager::getSingleton().logMessage("line: "+StringConverter::toString(line)+","+StringConverter::toString(col));

	// Print the variables in the current function
	PrintVariables(ctx, -1);

    const char *section;
	// Show the call stack with the variables
	LogManager::getSingleton().logMessage("--- call stack ---");
	for(unsigned int n = 0; n < ctx->GetCallstackSize(); n++ )
	{
		const asIScriptFunction *func = ctx->GetFunction(n);
		line = ctx->GetLineNumber(n,&col,&section);
		LogManager::getSingleton().logMessage(String(func->GetModuleName()) + ":" + func->GetDeclaration() + ":" + StringConverter::toString(line)+ "," + StringConverter::toString(col));

		PrintVariables(ctx, n);
	}
}
//----------------------------------------------------------------------------

static AngelScriptInterpreter *interpreter = 0;

//----------------------------------------------------------------------------
bool dllStartPlugin(void *identifier, Ogre::String& name)
{
    interpreter = OGRE_NEW AngelScriptInterpreter();
    name = "AngelScript Interpreter Plugin";
    OgitorsRoot::getSingletonPtr()->RegisterScriptInterpreter(identifier, interpreter);
    return true;
}
//----------------------------------------------------------------------------
bool dllGetPluginName(Ogre::String& name)
{
    name = "AngelScript Interpreter Plugin";
    return true;
}
//----------------------------------------------------------------------------
bool dllStopPlugin(void)
{
    OGRE_DELETE interpreter;
    interpreter = 0;
    return true;
}
//----------------------------------------------------------------------------