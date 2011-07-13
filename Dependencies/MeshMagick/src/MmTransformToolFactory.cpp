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

#include "MmTransformToolFactory.h"
#include "MmTransformTool.h"

#include <stdexcept>

using namespace Ogre;

namespace meshmagick
{
    //------------------------------------------------------------------------
    Tool* TransformToolFactory::createTool()
    {
        Tool* tool = new TransformTool();
        return tool;
    }
    //------------------------------------------------------------------------

    void TransformToolFactory::destroyTool(Tool* tool)
    {
        delete tool;
    }
    //------------------------------------------------------------------------

    OptionDefinitionSet TransformToolFactory::getOptionDefinitions() const
    {
        OptionDefinitionSet optionDefs;
        optionDefs.insert(OptionDefinition("scale", OT_VECTOR3, false, true));
        optionDefs.insert(OptionDefinition("rotate", OT_QUATERNION, false, true));
        optionDefs.insert(OptionDefinition("translate", OT_VECTOR3, false, true));
        optionDefs.insert(OptionDefinition("xalign", OT_SELECTION, false, true, Any(),
            ";left;center;right"));
        optionDefs.insert(OptionDefinition("yalign", OT_SELECTION, false, true, Any(),
            ";top;center;bottom"));
        optionDefs.insert(OptionDefinition("zalign", OT_SELECTION, false, true, Any(),
            ";back;center;front"));

        optionDefs.insert(OptionDefinition("resize", OT_STRING, false, true));
        optionDefs.insert(OptionDefinition("axes", OT_STRING, false, true));

        optionDefs.insert(OptionDefinition("no-normalise-normals"));
        optionDefs.insert(OptionDefinition("no-update-boundingbox"));
        optionDefs.insert(OptionDefinition("flip-vertex-winding"));
        return optionDefs;
    }
    //------------------------------------------------------------------------

    void TransformToolFactory::printToolHelp(std::ostream& out) const
    {
        out << std::endl;
        out << "Scales, rotates or otherwise transforms a mesh" << std::endl << std::endl;
        out << "possible transformations:" << std::endl;
        out << "   -scale=x/y/z - scale the mesh by this scale vector"
            << std::endl;
        out << "   -rotate=angle/x/y/z - rotate the mesh <angle> degrees on the axis x/y/z"
            << std::endl;
        out << "   -translate=x/y/z - translate the mesh by this vector"
            << std::endl;
        out << "   -xalign=right|left|center : align the mesh on x axis"
            << std::endl;
        out << "   -yalign=top|bottom|center : align the mesh on y axis"
            << std::endl;
        out << "   -zalign=front|back|center : align the mesh on z axis"
            << std::endl;
        out << "   -resize=x/y/z : scale the mesh so, that its size is x/y/z"
            << std::endl;
        out << "       axes components can be a non-negative value or 's' or 'k'"
            << std::endl;
        out << "       k: keep scale for this axis, s: scale uniformely"
            << std::endl;
        out << "   -axes=x/y/z : remaps main axes components to new ones."
            << std::endl;
        out << "       e.g.: -axes=-z/y/z will change x -> -z, y stays y, z -> x"
            << std::endl;
        out << "(All transform options are applied in their relative order.)" << std::endl
            << std::endl;
        out << "other options:" << std::endl;
        out << "   -no-normalise-normals: prevents normalisation of normals" << std::endl;
        out << "   -flip-normals: flip normals by reordering triangle indices" << std::endl;
        out << "   -no-update-boundingbox: keeps bounding box as defined in the file"
            << std::endl
            << std::endl;
    }
    //------------------------------------------------------------------------

    Ogre::String TransformToolFactory::getToolName() const
    {
        return "transform";
    }
    //------------------------------------------------------------------------

    Ogre::String TransformToolFactory::getToolDescription() const
    {
        return "Scale, rotate or otherwise transform a mesh.";
    }
    //------------------------------------------------------------------------
}
