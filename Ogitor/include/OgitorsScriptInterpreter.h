/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2013 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
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

#include <string>

namespace Ogitors
{
    struct OutputData
    {
        int          mType;
        Ogre::String mSection;
        int          mRow;
        int          mCol;
        Ogre::String mMessage;
    };

    typedef Ogre::vector<OutputData>::type OutputDataVector;

    //! Ogitor Script Interpreter class
    /*!  
    A class that is responsible for preparing, registering and executing scripts
    */

    class OgitorExport OgitorsScriptInterpreter : public Ogre::GeneralAllocatedObject
    {
    public:

        OgitorsScriptInterpreter();
        virtual ~OgitorsScriptInterpreter();

        // return Interpreter Type String
        virtual const std::string getTypeString() = 0;
        // return Interpreter Init Message
        virtual const std::string getInitMessage() = 0;
        //Initialization
        virtual void Init() = 0;
        //Create a new instance of the interpreter
        virtual OgitorsScriptInterpreter* createNewInstance() = 0;
        //Return engine specific handle
        virtual void * getHandle() = 0;
        // build a string
        virtual Ogre::StringVector buildString(std::string &section, std::string &arg) = 0;
        // execute a string as script
        virtual Ogre::StringVector execString(std::string &section, std::string &arg) = 0;
        // execute a script from file
        virtual Ogre::StringVector runScript(std::string &section, std::string &file) = 0;
        // add function
        virtual Ogre::StringVector addFunction(std::string &section, std::string &arg) = 0;
        // list functions
        virtual Ogre::StringVector listFunctions(std::string &section) = 0;
        // compile a module from a script file
        virtual Ogre::StringVector compileModule(std::string &section, std::string &file) = 0;
        // compile a module from memory
        virtual Ogre::StringVector compileModule(std::string &section, const char *source) = 0;
        // run a previously compiled update function
        virtual Ogre::StringVector runUpdateFunction(std::string &section, CBaseEditor *object, Ogre::Real time) = 0;
        // release resources associated with handle
        virtual void releaseHandle(unsigned int handle) = 0;
        // set time since last frame
        static void setTimeSinceLastFrame(float timePassed) { mTimeSinceLastFrame = timePassed; };
        // Get timePassed since last frame
        static float getTimeSinceLastFrame() { return mTimeSinceLastFrame; };
        // Retrieves the current output from the interpreter.
        unsigned int getOutput(unsigned int start, OutputDataVector& list);

    protected:
        OgitorsRoot        *mOgitorsRoot;
        OgitorsSystem      *mSystem;
        bool                mInitialized;
        OGRE_AUTO_MUTEX;
        OutputDataVector    mBuffer;
        static float        mTimeSinceLastFrame;
    };

    class OgitorExport OgitorsDummyScriptInterpreter : public OgitorsScriptInterpreter
    {
    public:

        OgitorsDummyScriptInterpreter()
        {
        }

        virtual ~OgitorsDummyScriptInterpreter()
        {
        }

        // return Interpreter Type String
        const std::string getTypeString() { return "Dummy"; };
        // return Interpreter Init Message
        const std::string getInitMessage() { return "WARNING: Dummy Interpreter Active!"; };
        //Initialization
        void Init() {};
        //Return engine specific handle
        void * getHandle() { return 0; };
        //Create a new instance of the interpreter
        OgitorsScriptInterpreter* createNewInstance() { return OGRE_NEW OgitorsDummyScriptInterpreter(); };
        // build a string
        Ogre::StringVector buildString(std::string &section, std::string &arg) 
        { 
            Ogre::StringVector out;
            out.push_back("Cannot interpret : " + arg);
            return out;
        }
        // execute a string as script
        Ogre::StringVector execString(std::string &section, std::string &arg)
        {
            Ogre::StringVector out;
            out.push_back("Cannot interpret : " + arg);
            return out;
        }
        // execute a script from file
        Ogre::StringVector runScript(std::string &section, std::string &file)
        {
            Ogre::StringVector out;
            out.push_back("Cannot run : " + file);
            return out;
        }

        // add function
        Ogre::StringVector addFunction(std::string &section, std::string &arg)
        {
            Ogre::StringVector out;
            out.push_back("Cannot add function : " + arg);
            return out;
        }

        // list functions
        Ogre::StringVector listFunctions(std::string &section)
        {
            Ogre::StringVector out;
            out.push_back("No functions defined...");
            return out;
        }

        // compile a module from a script file
        Ogre::StringVector compileModule(std::string &section, std::string &file)
        {
            Ogre::StringVector out;
            out.push_back("Cannot compile : " + file);
            return out;
        }

        // compile a module from memory
        Ogre::StringVector compileModule(std::string &section, const char *source)
        {
            Ogre::StringVector out;
            out.push_back("Cannot compile...");
            return out;
        }

        // run a previously compiled update function
        Ogre::StringVector runUpdateFunction(std::string &section, CBaseEditor *object, Ogre::Real time)
        {
            Ogre::StringVector out;
            out.push_back("Cannot run : " + section);
            return out;
        }

        // release resources associated with handle
        void releaseHandle(unsigned int handle) {};
    };
}
