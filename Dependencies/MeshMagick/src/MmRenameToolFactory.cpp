/*
This file is part of MeshMagick - An Ogre mesh file manipulation tool.
Copyright (C) 2007-2010 Sascha Kolewa, Daniel Wickert

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
