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

#ifndef __MM_TOOL_H__
#define __MM_TOOL_H__

#include "MeshMagickPrerequisites.h"

#ifdef __APPLE__
#	include <Ogre/OgreStringVector.h>
#else
#	include <OgreStringVector.h>
#endif

#include "MmOptionsParser.h"

namespace meshmagick
{
    class _MeshMagickExport Tool
    {
    public:
        Tool();
        virtual ~Tool();

		virtual Ogre::String getName() const = 0;

        void invoke(const OptionList& globalOptions, const OptionList& toolOptions,
            const Ogre::StringVector& inFileNames, const Ogre::StringVector& outFileNames);

    protected:
        typedef enum {V_QUIET, V_NORMAL, V_HIGH} Verbosity;
        Verbosity mVerbosity;
        bool mFollowSkeletonLink;

        void print(const Ogre::String& msg, Verbosity verbosity=V_NORMAL,
			std::ostream& out = std::cout) const;
        void warn(const Ogre::String& msg) const;
        void fail(const Ogre::String& msg) const;

        virtual void doInvoke(const OptionList& toolOptions,
            const Ogre::StringVector& inFileNames,
            const Ogre::StringVector& outFileNames) = 0;

    private:
        void setGlobalOptions(const OptionList& globalOptions);
    };
}
#endif
