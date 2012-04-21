/*
This file is part of MeshMagick - An Ogre mesh file manipulation tool.
Copyright (C) 2010 Steve Streeting

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

#include "MmTootleToolFactory.h"

#include "MmOptionsParser.h"
#include "MmTootleTool.h"

namespace meshmagick
{

	TootleToolFactory::TootleToolFactory()
	{
	}

	TootleToolFactory::~TootleToolFactory()
	{
	}

	Tool* TootleToolFactory::createTool()
	{
		return new TootleTool();
	}

	void TootleToolFactory::destroyTool(Tool* tool)
	{
		delete tool;
	}

	OptionDefinitionSet TootleToolFactory::getOptionDefinitions() const
	{
		OptionDefinitionSet optionDefs;
		optionDefs.insert(OptionDefinition("vcachesize", OT_INT, false, false));
		optionDefs.insert(OptionDefinition("clockwise", OT_BOOL, false, false, Ogre::Any(false)));
		optionDefs.insert(OptionDefinition("clusters", OT_INT, false, false));
		optionDefs.insert(OptionDefinition("viewpoint", OT_VECTOR3, false, true));
		
		return optionDefs;
	}

	Ogre::String TootleToolFactory::getToolName() const
	{
		return "tootle";
	}

	Ogre::String TootleToolFactory::getToolDescription() const
	{
		return "Use AMD Tootle to optimise mesh.";
	}

	void TootleToolFactory::printToolHelp(std::ostream& out) const
	{
		out << std::endl;
		out << "Use AMD Tootle to optimise mesh." << std::endl << std::endl;
		out << "Optimisation parameters:" << std::endl;
		out << " -vcachesize=N    - specify the vertex cache size (omit to use default)"
			<< std::endl;
		out << " -clockwise       - treat clockwise faces as front-facing (default is CCW)"
			<< std::endl;
		out << " -clusters=N      - manually specify the number of clusters (default auto)"
			<< std::endl;
		out << " -viewpoint=x/y/z - specify one or more viewpoints to judge overdraw"
			<< std::endl;
		out << "                    Default is to generate viewpoints automatically"
			<< std::endl;
	}
}
