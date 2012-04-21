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

#ifndef __MM_TOOL_H__
#define __MM_TOOL_H__

#include "MeshMagickPrerequisites.h"
#include "MmOptionsParser.h"

#ifdef __APPLE__
#	include <Ogre/OgreStringVector.h>
#else
#	include <OgreStringVector.h>
#endif

namespace meshmagick
{
    class Tool
    {
    public:
        Tool();
        virtual ~Tool();

		virtual Ogre::String getName() const = 0;

        void invoke(const OptionList& globalOptions, const OptionList& toolOptions,
            const Ogre::StringVector& inFileNames, const Ogre::StringVector& outFileNames);

		typedef enum {V_QUIET, V_NORMAL, V_HIGH} Verbosity;
		void setVerbosity(Verbosity v) { mVerbosity = v; }
		Verbosity getVerbosity() const { return mVerbosity; }

		void setFollowSkeletonLink(bool f) { mFollowSkeletonLink = f; }
		bool getFollowSkeletonLink() const { return mFollowSkeletonLink; }

    protected:
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
