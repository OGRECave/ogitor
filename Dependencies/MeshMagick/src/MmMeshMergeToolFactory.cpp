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

#include "MmMeshMergeToolFactory.h"
#include "MmMeshMergeTool.h"

#include <stdexcept>

using namespace Ogre;

namespace meshmagick
{
    //------------------------------------------------------------------------
    Tool* MeshMergeToolFactory::createTool()
    {
        Tool* tool = new MeshMergeTool();
        return tool;
    }
    //------------------------------------------------------------------------

    void MeshMergeToolFactory::destroyTool(Tool* tool)
    {
        delete tool;
    }
    //------------------------------------------------------------------------

    OptionDefinitionSet MeshMergeToolFactory::getOptionDefinitions() const
    {
        OptionDefinitionSet optionDefs;
        return optionDefs;
    }
    //------------------------------------------------------------------------

    void MeshMergeToolFactory::printToolHelp(std::ostream& out) const
    {
    }
    //------------------------------------------------------------------------

    Ogre::String MeshMergeToolFactory::getToolName() const
    {
        return "meshmerge";
    }
    //------------------------------------------------------------------------

    Ogre::String MeshMergeToolFactory::getToolDescription() const
    {
        return "Merge multiple submeshes into a single mesh.";
    }
    //------------------------------------------------------------------------
}
