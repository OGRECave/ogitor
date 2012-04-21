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

#ifndef __MM__RENAMETOOL_H__
#define __MM__RENAMETOOL_H__

#include "MeshMagickPrerequisites.h"

#include "MmTool.h"

namespace meshmagick
{

	class _MeshMagickExport RenameTool : public Tool
	{
	public:
		RenameTool();
		~RenameTool();

		Ogre::String getName() const;

	protected:
		virtual void doInvoke(const OptionList& toolOptions,
				const Ogre::StringVector& inFileNames,
				const Ogre::StringVector& outFileNames);

	private:
		typedef std::pair<Ogre::String, Ogre::String> StringPair;

		void processMeshFile(
			const OptionList &toolOptions, Ogre::String inFile, Ogre::String outFile);
		void processSkeletonFile(
			const OptionList &toolOptions, Ogre::String inFile, Ogre::String outFile);
		StringPair split(const Ogre::String& value) const;
	};

}

#endif // __MM__RENAMETOOL_H__
