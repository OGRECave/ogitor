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

#include "OgitorsPrerequisites.h"
#include "BaseEditor.h"
#include "OgitorsRoot.h"
#include "OgitorsSystem.h"
#include "OgitorsScriptConsole.h"
#include "OgitorsScriptInterpreter.h"
#include <OgreStringConverter.h>
#include <vector>

namespace Ogitors
{
    static std::string console_str("console");

    //-----------------------------------------------------------------------------------------
    // Functions for running, evaluating scripts and single lines
    //-----------------------------------------------------------------------------------------
    template<> OgitorsScriptConsole* Singleton<OgitorsScriptConsole>::ms_Singleton = 0;

    //-------------------------------------------------------------------------------
    OgitorsScriptConsole::OgitorsScriptConsole(OgitorsRoot *ogRoot, OgitorsScriptInterpreter *interpreter) : mFirstLine(true)
    {
        mOgitorsRoot = ogRoot;
        mSystem = OgitorsSystem::getSingletonPtr();
        mInterpreter = interpreter;

        mOutput.push_back(interpreter->getInitMessage());

        mPrompt = LI_PROMPT;
    }

    OgitorsScriptConsole::~OgitorsScriptConsole()
    {
    }

    // Retrieves the current output from the interpreter.
    unsigned int OgitorsScriptConsole::getOutput(unsigned int start, Ogre::StringVector& list)
    {
        list.clear();

        OGRE_LOCK_AUTO_MUTEX
        
        for(unsigned int i = start;i < mOutput.size();i++)
        {
            list.push_back(mOutput[i]);
        }

        return mOutput.size();
    }

    // Adds a line to output
    void OgitorsScriptConsole::addOutput(const std::string& text)
    {
        OGRE_LOCK_AUTO_MUTEX
        
        mOutput.push_back(text);
    }

    
    // Insert (another) line of text into the interpreter.
    OgitorsScriptConsole::State OgitorsScriptConsole::insertLine( std::string& line, bool fInsertInOutput )
    {
        OGRE_LOCK_AUTO_MUTEX
        if( fInsertInOutput == true )
        {
            mOutput.push_back(line);
        }

        if( mFirstLine && line.substr(0,1) == "=" )
        {
            line = "return " + line.substr(1, line.length()-1 );
        }

        mCurrentStatement += line;
        mFirstLine = false;

        mState = LI_READY;

        if(mCurrentStatement == "list")
        {
            listFunctions();
        }
        else
        {
            std::string sought = "(";
            int pos = mCurrentStatement.find(sought);
            if(pos == std::string::npos)
            {
                mCurrentStatement += std::string("()");
            }
            execString(mCurrentStatement);
        }

        mCurrentStatement.clear();
        mFirstLine = true;

        mPrompt = LI_PROMPT;

        return mState;
    }

    void OgitorsScriptConsole::buildString(std::string &arg)
    {
        Ogre::StringVector result = mInterpreter->buildString(console_str, arg);
        
        OGRE_LOCK_AUTO_MUTEX
        
        for(unsigned int i = 0;i < result.size();i++)
            mOutput.push_back(result[i]);
    }

    void OgitorsScriptConsole::runScript(std::string &file)
    {
        Ogre::StringVector result = mInterpreter->runScript(console_str, file);
        OGRE_LOCK_AUTO_MUTEX
        mOutput.push_back("Running Script : " + file);
        for(unsigned int i = 0;i < result.size();i++)
            mOutput.push_back(result[i]);
    }

    void OgitorsScriptConsole::testScript(std::string &file)
    {
        Ogre::StringVector result = mInterpreter->compileModule(console_str, file);
        OGRE_LOCK_AUTO_MUTEX
        mOutput.push_back("Testing Script : " + file);
        for(unsigned int i = 0;i < result.size();i++)
            mOutput.push_back(result[i]);
    }

    void OgitorsScriptConsole::testScript(const char *source)
    {
        Ogre::StringVector result = mInterpreter->compileModule(console_str, source);
        OGRE_LOCK_AUTO_MUTEX
        mOutput.push_back("Compiling Script...");
        for(unsigned int i = 0;i < result.size();i++)
            mOutput.push_back(result[i]);
    }

    void OgitorsScriptConsole::addFunction(std::string &arg)
    {
        Ogre::StringVector result = mInterpreter->addFunction(console_str, arg);
        OGRE_LOCK_AUTO_MUTEX
        for(unsigned int i = 0;i < result.size();i++)
            mOutput.push_back(result[i]);
    }

    void OgitorsScriptConsole::listFunctions()
    {
        Ogre::StringVector result = mInterpreter->listFunctions(console_str);
        OGRE_LOCK_AUTO_MUTEX
        for(unsigned int i = 0;i < result.size();i++)
            mOutput.push_back(result[i]);
    }

    void OgitorsScriptConsole::execString(std::string &arg)
    {
        Ogre::StringVector result = mInterpreter->execString(console_str, arg);
        OGRE_LOCK_AUTO_MUTEX
        mOutput.push_back("Executing : " + arg);
        for(unsigned int i = 0;i < result.size();i++)
            mOutput.push_back(result[i]);
    }

    std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
        std::stringstream ss(s);
        std::string item;
        while(std::getline(ss, item, delim)) {
            elems.push_back(item);
        }
        return elems;
    }

    std::vector<std::string> split(const std::string &s, char delim) {
        std::vector<std::string> elems;
        return split(s, delim, elems);
    }
}
