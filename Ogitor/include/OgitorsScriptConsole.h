/*/////////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////*/

/*////////////////////////////////////////////////////////////////////////////////
// Interface to the script console
////////////////////////////////////////////////////////////////////////////////*/

#pragma once

#include "OgitorsPrerequisites.h"

#include <string>

#define LI_PROMPT  ">"
#define LI_PROMPT2 ">>"

namespace Ogitors
{
    class OgitorsScriptInterpreter;

    //! Ogitor Script Console class
    /*!  
    A class that is responsible for preparing, registering, executing and displaying Scripts
    */

    class OgitorExport OgitorsScriptConsole: public Ogre::GeneralAllocatedObject, public Singleton<OgitorsScriptConsole>
    {
    public:
        enum State
        {
            LI_READY = 0,
            LI_NEED_MORE_INPUT,
            LI_ERROR
        };

        OgitorsScriptConsole(OgitorsRoot *ogRoot, OgitorsScriptInterpreter *interpreter);
        virtual ~OgitorsScriptConsole();

        // Retrieves the current output from the interpreter.
        unsigned int getOutput(unsigned int start, Ogre::StringVector& list);
        // Clears output
        void clearOutput() { mOutput.clear(); }
        // Adds a line to output
        void addOutput(const std::string& text);

        std::string getPrompt() { return mPrompt; }

        // Insert (another) line of text into the interpreter.
        // If fInsertInOutput is true, the line will also go into the
        // output.
        State insertLine( std::string& line, bool fInsertInOutput = false );

        void addFunction(std::string &arg);
        // build a string
        void buildString(std::string &arg);
        // execute a string as script
        void execString(std::string &arg);
        // execute a script from file
        void runScript(std::string &file);
        // List all functions
        void listFunctions();
        // Retrieve the current state of affairs.
        State getState() { return mState; }
        // test a script from file
        void testScript(std::string &file);
        // test a script from memory
        void testScript(const char *source);

    protected:
        OgitorsRoot        *mOgitorsRoot;
        OgitorsSystem      *mSystem;
        OgitorsScriptInterpreter *mInterpreter;
        std::string         mCurrentStatement;
        Ogre::StringVector  mOutput;
        std::string         mPrompt;
        State               mState;
        bool                mFirstLine;
        OGRE_AUTO_MUTEX;
    };
}
