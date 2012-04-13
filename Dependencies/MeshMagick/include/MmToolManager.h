/*
This file is part of MeshMagick - An Ogre mesh file manipulation tool.
Copyright (C) 2007-2009 Daniel Wickert

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef __MM_MESHTOOL_MANAGER_H__
#define __MM_MESHTOOL_MANAGER_H__

#include "MeshMagickPrerequisites.h"

#ifdef __APPLE__
#	include <Ogre/OgreStringVector.h>
#else
#	include <OgreStringVector.h>
#endif

#include "MmOptionsParser.h"
#include "MmTool.h"

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
