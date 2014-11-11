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

#include "MmTool.h"

#include <iostream>
#include <stdexcept>
#include <OgreLog.h>

#include "MmOgreEnvironment.h"

using namespace Ogre;

namespace meshmagick
{
    Tool::Tool() : mVerbosity(V_NORMAL), mFollowSkeletonLink(true)
    {
    }

    Tool::~Tool()
    {
    }

    void Tool::invoke(const OptionList& globalOptions, const OptionList& toolOptions,
        const StringVector& inFileNames, const StringVector& outFileNames)
    {
        setGlobalOptions(globalOptions);
        doInvoke(toolOptions, inFileNames, outFileNames);
    }

    void Tool::setGlobalOptions(const OptionList& globalOptions)
    {
        // Reset to defaults..
        mVerbosity = V_NORMAL;
        mFollowSkeletonLink = true;

        for (OptionList::const_iterator it = globalOptions.begin(); it != globalOptions.end(); ++it)
        {
            if (it->first == "no-follow-skeleton")
            {
                mFollowSkeletonLink = false;
            }
            else if (it->first == "quiet")
            {
                mVerbosity = V_QUIET;
            }
            else if (it->first == "verbose")
            {
                mVerbosity = V_HIGH;
            }
        }
    }

    void Tool::print(const Ogre::String& msg, Verbosity verbosity, std::ostream& out) const
    {
		if (OgreEnvironment::getSingleton().isStandalone())
		{
			if (verbosity <= mVerbosity)
			{
				out << msg << std::endl;
			}
		}

		OgreEnvironment::getSingleton().getLog()
			->logMessage(msg);
    }

    void Tool::warn(const Ogre::String& msg) const
    {
        print("warning: " + msg, V_NORMAL, std::cerr);
    }

    void Tool::fail(const Ogre::String& msg) const
    {
        print("fatal error: " + msg, V_QUIET, std::cerr);
        throw std::logic_error(msg);
    }
}
