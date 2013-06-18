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

#include "OgitorsPrerequisites.h"
#include "OgitorsRoot.h"
#include "OgitorsSystem.h"
#include "OgitorsScriptInterpreter.h"

namespace Ogitors
{
    float OgitorsScriptInterpreter::mTimeSinceLastFrame = 0.0f;
    
    OgitorsScriptInterpreter::OgitorsScriptInterpreter()
    {
        mOgitorsRoot = OgitorsRoot::getSingletonPtr();
        mSystem = OgitorsSystem::getSingletonPtr();
        mInitialized = false;
    }

    OgitorsScriptInterpreter::~OgitorsScriptInterpreter()
    {
        mInitialized = false;
        mOgitorsRoot = 0;
        mSystem = 0;
    }

    unsigned int OgitorsScriptInterpreter::getOutput(unsigned int start, OutputDataVector& list)
    {
        list.clear();
        
        OGRE_LOCK_AUTO_MUTEX;
        
        for(unsigned int i = start;i < mBuffer.size();i++)
        {
            list.push_back(mBuffer[i]);
        }

        return mBuffer.size();
    }

}
