///////////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////////

#include "OgitorsPrerequisites.h"
#include "OgitorsRoot.h"
#include "OgitorsScriptInterpreter.h"

#include "angelscript.h"

#include "AngelDialogManagerPlugin.h"
#include "dialog.hxx"
#include "inputwidgets.hxx"

#include <AngelScriptInterpreter.h>

asIScriptEngine *DialogManagerScriptEngine = 0;
Ogitors::AngelScriptInterpreter *DialogManagerInterpreter;
//----------------------------------------------------------------------------
asIScriptModule *getDialogManagerActiveModule()
{
    return DialogManagerInterpreter->getActiveModule();
}
//----------------------------------------------------------------------------
asIScriptContext *getDialogManagerActiveContext()
{
    return DialogManagerInterpreter->getActiveContext();
}
//----------------------------------------------------------------------------
bool dllStartPlugin(void *identifier, Ogre::String& name)
{
    name = "Angel Dialog Manager Plugin";

    DialogManagerInterpreter = static_cast<Ogitors::AngelScriptInterpreter*>(Ogitors::OgitorsRoot::getSingletonPtr()->GetScriptInterpreter("AngelScript"));

    if(!DialogManagerInterpreter)
        return false;

    DialogManagerScriptEngine = static_cast<asIScriptEngine *>(DialogManagerInterpreter->getHandle());

    if(!DialogManagerScriptEngine)
        return false;

    RegisterInputWidgetBindings(DialogManagerScriptEngine);
    RegisterDialogBindings(DialogManagerScriptEngine);

    return true;
}
//----------------------------------------------------------------------------
bool dllGetPluginName(Ogre::String& name)
{
    name = "Angel Dialog Manager Plugin";
    return true;
}
//----------------------------------------------------------------------------
bool dllStopPlugin(void)
{
    return true;
}
//----------------------------------------------------------------------------