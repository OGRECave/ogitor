/*
This file is part of MeshMagick - An Ogre mesh file manipulation tool.
Copyright (C) 2007-2010 Daniel Wickert

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef __MM_MESHTOOL_MANAGER_H__
#define __MM_MESHTOOL_MANAGER_H__

#include "MeshMagickPrerequisites.h"
#include "MmOptionsParser.h"
#include "MmTool.h"

#ifdef __APPLE__
#	include <Ogre/OgreStringVector.h>
#else
#	include <OgreStringVector.h>
#endif

namespace meshmagick
{
    class _MeshMagickExport ToolManager
    {
    public:
        ~ToolManager();

        void invokeTool(const Ogre::String& name, const OptionList& globalOptions,
            int toolArgc, const char** toolArgV,
            const Ogre::StringVector& inFileNames, const Ogre::StringVector& outFileNames);

		Tool* createTool(const Ogre::String& name);
        void destroyTool(Tool*);

        void printToolList(std::ostream& out) const;
        void printToolHelp(const Ogre::String& toolName, std::ostream& out) const;

        void registerToolFactory(ToolFactory* factory);
        void unregisterToolFactory(ToolFactory* factory);

    private:
        typedef std::map<Ogre::String, ToolFactory*> FactoryMap;
        FactoryMap mFactories;
    };
}
#endif
