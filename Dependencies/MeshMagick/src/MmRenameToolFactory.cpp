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

#include "MmRenameToolFactory.h"

#include "MmOptionsParser.h"
#include "MmRenameTool.h"

namespace meshmagick
{

	RenameToolFactory::RenameToolFactory()
	{
	}

	RenameToolFactory::~RenameToolFactory()
	{
	}

	Tool* RenameToolFactory::createTool()
	{
		return new RenameTool();
	}

	void RenameToolFactory::destroyTool(Tool* tool)
	{
		delete tool;
	}

	OptionDefinitionSet RenameToolFactory::getOptionDefinitions() const
	{
        OptionDefinitionSet optionDefs;
		optionDefs.insert(OptionDefinition("animation", OT_STRING, false, true));
		optionDefs.insert(OptionDefinition("bone", OT_STRING, false, true));
		optionDefs.insert(OptionDefinition("skeleton", OT_STRING, false, false));
		optionDefs.insert(OptionDefinition("material", OT_STRING, false, true));
        optionDefs.insert(OptionDefinition("submesh", OT_STRING, false, true));
		return optionDefs;
	}

	Ogre::String RenameToolFactory::getToolName() const
	{
		return "rename";
	}

	Ogre::String RenameToolFactory::getToolDescription() const
	{
		return "Rename different elements of meshes and skeletons.";
	}

	void RenameToolFactory::printToolHelp(std::ostream& out) const
	{
        out << std::endl;
        out << "Rename different elements of meshes and skeletons" << std::endl << std::endl;
        out << "possible renamings:" << std::endl;
        out << "   -animation=/before/after/ - renames animation 'before' to 'after'"
            << std::endl;
        out << "   -bone=/before/after/ - renames bone 'before' to 'after'"
            << std::endl;
        out << "   -material=/before/after/ - change all materials 'before' to 'after'"
            << std::endl;
        out << "   -skeleton=newname - renames mesh's skeleton to 'newname'"
            << std::endl;
        out << "   -submesh=/before/after/ - renames all submeshes 'before' to 'after'"
            << std::endl;
        out << "Any other char can be used instead of '/', just be careful that it is not part of any name."
            << std::endl;
        out << "All options can be used more than once to execute multiple renamings at once."
            << std::endl
            << std::endl;
	}
}
