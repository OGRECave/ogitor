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
