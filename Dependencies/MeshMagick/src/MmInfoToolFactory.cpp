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

#include "MmInfoToolFactory.h"
#include "MmInfoTool.h"

#include <stdexcept>

using namespace Ogre;

namespace meshmagick
{
    Tool* InfoToolFactory::createTool()
    {
        Tool* tool = new InfoTool();
        return tool;
    }

    void InfoToolFactory::destroyTool(Tool* tool)
    {
        delete tool;
    }

    OptionDefinitionSet InfoToolFactory::getOptionDefinitions() const
    {
        OptionDefinitionSet optionDefs;
        optionDefs.insert(OptionDefinition("list", OT_STRING));
        optionDefs.insert(OptionDefinition("delim", OT_STRING));
        return optionDefs;
    }

    void InfoToolFactory::printToolHelp(std::ostream& out) const
    {
        out << "Print information about the mesh" << std::endl
            << "without further options, info tool prints informations in report style" << std::endl
			<< "-delim=<delimiter> : delimiter character used by the -list option. Default is tab." << std::endl
			<< "-list=<field-key1>/<field-key2>/.. : print delim separated fields" << std::endl
			<< "    The following field-keys are available:" << std::endl
			<< std::endl
			<< "         name" << std::endl
			<< "         version" << std::endl
			<< "         endian" << std::endl
			<< std::endl
			<< "         stored_bounding_box" << std::endl
			<< "         actual_bounding_box" << std::endl
			<< "         stored_mesh_extent" << std::endl
			<< "         actual_mesh_extent" << std::endl
			<< std::endl
			<< "         shared_vertices" << std::endl
			<< "         shared_vertex_count" << std::endl
			<< "         shared_bone_assignment_count" << std::endl
			<< "         shared_bone_references_count" << std::endl
			<< "         shared_vertex_layout" << std::endl
			<< std::endl
			<< "         submesh_count" << std::endl
			<< "         submesh_index" << std::endl
			<< "         submesh_name" << std::endl
			<< "         submesh_material" << std::endl
			<< "         submesh_use_shared_vertices" << std::endl
			<< "         submesh_vertex_count" << std::endl
			<< "         submesh_bone_assignment_count" << std::endl
			<< "         submesh_bone_references_count" << std::endl
			<< "         submesh_vertex_layout" << std::endl
			<< "         submesh_operation_type" << std::endl
			<< "         submesh_element_count" << std::endl
			<< "         submesh_triangle_count" << std::endl
			<< "         submesh_line_count" << std::endl
			<< "         submesh_point_count" << std::endl
			<< "         submesh_index_width" << std::endl
			<< std::endl
			<< "         max_bone_assignments" << std::endl
			<< "         max_bone_references" << std::endl
			<< "         total_vertex_count" << std::endl
			<< "         total_element_count" << std::endl
			<< "         total_triangle_count" << std::endl
			<< "         total_line_count" << std::endl
			<< "         total_point_count" << std::endl
			<< std::endl
			<< "         morph_animation_count" << std::endl
			<< "         pose_count" << std::endl
			<< std::endl
			<< "         edge_list" << std::endl
			<< "         lod_level_count" << std::endl
			<< std::endl
			<< "         skeleton" << std::endl
			<< "         skeleton_name" << std::endl
			<< "         skeleton_bone_count" << std::endl
			<< "         skeleton_animation_count" << std::endl
			<< std::endl;
    }

    Ogre::String InfoToolFactory::getToolName() const
    {
        return "info";
    }

    Ogre::String InfoToolFactory::getToolDescription() const
    {
        return "print information about the mesh.";
    }
}
