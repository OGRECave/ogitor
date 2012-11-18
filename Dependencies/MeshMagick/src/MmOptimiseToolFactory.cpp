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
