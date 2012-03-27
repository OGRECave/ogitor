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

#include "Ogitors.h"
#include "OgitorsScriptInterpreter.h"
#include "PythonqtInterpreter.h"

using namespace Ogitors;
using namespace Ogre;

PythonqtInterpreter::PythonqtInterpreter() : OgitorsScriptInterpreter()
{
}
//----------------------------------------------------------------------------
PythonqtInterpreter::~PythonqtInterpreter()
{
}
//----------------------------------------------------------------------------
const std::string PythonqtInterpreter::getInitMessage()
{
    return std::string("PythonQt");
}
//----------------------------------------------------------------------------
Ogre::StringVector PythonqtInterpreter::buildString(std::string &section, std::string &arg)
{
    Ogre::StringVector ret;

    return ret;
}
//----------------------------------------------------------------------------
Ogre::StringVector PythonqtInterpreter::runScript(std::string &section, std::string &file)
{
    Ogre::StringVector ret;

    return ret;
}
//----------------------------------------------------------------------------
Ogre::StringVector PythonqtInterpreter::compileModule(std::string &section, std::string &file)
{
    Ogre::StringVector ret;

    return ret;
}
//----------------------------------------------------------------------------
Ogre::StringVector PythonqtInterpreter::compileModule(std::string &section, const char *source)
{
    Ogre::StringVector ret;

    return ret;
}
//----------------------------------------------------------------------------
Ogre::StringVector PythonqtInterpreter::runUpdateFunction(std::string &section, CBaseEditor *object, Ogre::Real time)
{
    Ogre::StringVector ret;
    return ret;
}
//----------------------------------------------------------------------------
Ogre::StringVector PythonqtInterpreter::addFunction(std::string &section, std::string &arg)
{
    Ogre::StringVector ret;

    return ret;
}
//----------------------------------------------------------------------------
Ogre::StringVector PythonqtInterpreter::listFunctions(std::string &section)
{
    Ogre::StringVector ret;

    return ret;
}
//----------------------------------------------------------------------------
Ogre::StringVector PythonqtInterpreter::execString(std::string &section, std::string &arg)
{
    Ogre::StringVector ret;

    return ret;
}
//----------------------------------------------------------------------------
void PythonqtInterpreter::releaseHandle(unsigned int handle)
{
}
//----------------------------------------------------------------------------

static PythonqtInterpreter *interpreter = 0;

//----------------------------------------------------------------------------
bool dllStartPlugin(void *identifier, Ogre::String& name)
{
    interpreter = OGRE_NEW PythonqtInterpreter();
    name = "PythonQt Interpreter Plugin";
    OgitorsRoot::getSingletonPtr()->RegisterScriptInterpreter(identifier, interpreter);
    return true;
}
//----------------------------------------------------------------------------
bool dllGetPluginName(Ogre::String& name)
{
    name = "PythonQt Interpreter Plugin";
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
