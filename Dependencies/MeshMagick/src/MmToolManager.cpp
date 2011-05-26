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

#include "MmToolManager.h"
#include "MmToolFactory.h"

#include <stdexcept>

namespace meshmagick
{
    ToolManager::~ToolManager()
    {
        while (!mFactories.empty())
        {
            delete mFactories.begin()->second;
            mFactories.erase(mFactories.begin());
        }
    }

    void ToolManager::invokeTool(const Ogre::String& name, const OptionList& globalOptions,
        int toolArgc, const char** toolArgV,
        const Ogre::StringVector& inFileNames, const Ogre::StringVector& outFileNames)
    {
        FactoryMap::const_iterator it = mFactories.find(name);
        if (it != mFactories.end())
        {
            OptionDefinitionSet optionDefs = it->second->getOptionDefinitions();
            OptionList toolOptions = OptionsParser::parseOptions(toolArgc, toolArgV, optionDefs);
            Tool* tool = it->second->createTool();
            tool->invoke(globalOptions, toolOptions, inFileNames, outFileNames);
            it->second->destroyTool(tool);
        }
        else
        {
            throw std::logic_error("No such tool registered: " + name);
        }
    }

    void ToolManager::printToolList(std::ostream& out) const
    {
        out << std::endl;
        out << "Available Tools" << std::endl;
        out << "===============" << std::endl;
        out << std::endl;
        for (FactoryMap::const_iterator it = mFactories.begin(); it != mFactories.end(); ++it)
        {
            out << (*it).first << " - "  << it->second->getToolDescription() << std::endl;
        }
    }

    void ToolManager::printToolHelp(const Ogre::String& toolName, std::ostream& out) const
    {
        FactoryMap::const_iterator it = mFactories.find(toolName);
        if (it != mFactories.end())
        {
            it->second->printToolHelp(out);
        }
        else
        {
            throw std::logic_error("No such tool registered: " + toolName);
        }
    }

    void ToolManager::registerToolFactory(ToolFactory* factory)
    {
        mFactories.insert(std::make_pair(factory->getToolName(), factory));
    }

    void ToolManager::unregisterToolFactory(ToolFactory* factory)
    {
        mFactories.erase(factory->getToolName());
    }

	Tool* ToolManager::createTool(const Ogre::String& name)
	{
		FactoryMap::iterator it = mFactories.find(name);
		if (it != mFactories.end())
		{
			Tool* tool = it->second->createTool();
			return tool;
		}

		return NULL;
	}

	void ToolManager::destroyTool(Tool* tool)
	{
		FactoryMap::iterator it = mFactories.find(tool->getName());
		if (it != mFactories.end())
		{
			it->second->destroyTool(tool);
		}
		else
		{
			/// \todo throw MeshMagickException, after inventing it properly.
		}
	}
}
