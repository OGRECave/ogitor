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

#ifndef __MM_TRANSFORM_TOOL_FACTORY_H__
#define __MM_TRANSFORM_TOOL_FACTORY_H__

#include "MeshMagickPrerequisites.h"

#include "MmToolFactory.h"

namespace meshmagick
{
    class _MeshMagickExport TransformToolFactory : public ToolFactory
    {
    public:
        virtual Tool* createTool();
        virtual void destroyTool(Tool* tool);

        virtual OptionDefinitionSet getOptionDefinitions() const;

        // Returns the name of the tool this factory creates.
        virtual Ogre::String getToolName() const;

        // Returns a short description of the tool this factory creates.
        virtual Ogre::String getToolDescription() const;

        virtual void printToolHelp(std::ostream& out) const;
    };
}
#endif
