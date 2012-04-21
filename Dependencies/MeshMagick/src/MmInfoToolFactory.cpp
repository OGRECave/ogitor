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
