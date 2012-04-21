/*
This file is part of MeshMagick - An Ogre mesh file manipulation tool.
Copyright (C) 2007-2010 Steve Streeting

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

#include "MmOptimiseToolFactory.h"
#include "MmOptimiseTool.h"

#include <stdexcept>

using namespace Ogre;

namespace meshmagick
{
	Tool* OptimiseToolFactory::createTool()
	{
		Tool* tool = new OptimiseTool();
		return tool;
	}

	void OptimiseToolFactory::destroyTool(Tool* tool)
	{
		delete tool;
	}

	OptionDefinitionSet OptimiseToolFactory::getOptionDefinitions() const
	{
		OptionDefinitionSet optionDefs;

		optionDefs.insert(OptionDefinition("tolerance", OT_REAL, false, false, Ogre::Any(1e-06)));
		optionDefs.insert(OptionDefinition("pos_tolerance", OT_REAL, false, false, Ogre::Any(1e-06)));
		optionDefs.insert(OptionDefinition("norm_tolerance", OT_REAL, false, false, Ogre::Any(1e-06)));
		optionDefs.insert(OptionDefinition("uv_tolerance", OT_REAL, false, false, Ogre::Any(1e-06)));
		optionDefs.insert(OptionDefinition("keep-identity-tracks", OT_BOOL, false, false));

		return optionDefs;
	}

	void OptimiseToolFactory::printToolHelp(std::ostream& out) const
	{
		out << std::endl;
		out << "Allows you to optimise meshes and skeletons" << std::endl << std::endl;
		out << "Options:" << std::endl;
		out << "   -tolerance=val - Tolerance value for treating vertices as equal (all components)"
			<< std::endl;
		out << "   -pos_tolerance=val - Tolerance value for treating positions as equal"
			<< std::endl;
		out << "   -norm_tolerance=val - Tolerance value for treating normals as equal"
			<< std::endl;
		out << "   -uv_tolerance=val - Tolerance value for treating uvs as equal"
			<< std::endl;
		out << "   -keep-identity-tracks - When optimising skeletons, keep tracks which do nothing"
			<< std::endl;

	}

	Ogre::String OptimiseToolFactory::getToolName() const
	{
		return "optimise";
	}

	Ogre::String OptimiseToolFactory::getToolDescription() const
	{
		return "Optimise meshes and skeletons.";
	}
}
