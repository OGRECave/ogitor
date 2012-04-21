/*
This file is part of MeshMagick - An Ogre mesh file manipulation tool.
Copyright (C) 2010 Steve Streeting

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

#ifndef __MM__TOOTLETOOL_H__
#define __MM__TOOTLETOOL_H__

#include "MeshMagickPrerequisites.h"

#include "MmTool.h"

#include <OgreCommon.h>

namespace meshmagick
{

	class _MeshMagickExport TootleTool : public Tool
	{
	public:
		TootleTool();
		~TootleTool();

		Ogre::String getName() const;

		void processMeshFile(Ogre::String file, Ogre::String outFile);
		void processMesh(Ogre::MeshPtr mesh);
		void processMesh(Ogre::Mesh* mesh);

		unsigned int getVCacheSize() const { return mVCacheSize; }
		void setVCacheSize(unsigned int sz) { mVCacheSize = sz; }

		Ogre::CullingMode getCullingMode() const { return mClockwise ? Ogre::CULL_ANTICLOCKWISE : Ogre::CULL_CLOCKWISE; }
		void setCullingMode(Ogre::CullingMode md) { mClockwise = (md == Ogre::CULL_ANTICLOCKWISE); }

		unsigned int getClusters() const { return mClusters; }
		void setClusters(unsigned int sz) { mClusters = sz; }
	protected:
		virtual void doInvoke(const OptionList& toolOptions,
			const Ogre::StringVector& inFileNames,
			const Ogre::StringVector& outFileNames);

	private:
		unsigned int mVCacheSize;
		bool mClockwise;
		unsigned int mClusters;
		typedef std::vector<Ogre::Vector3> ViewpointList;
		ViewpointList mViewpointList;

		void setOptions(const OptionList& options);
	};

}

#endif // __MM__TOOTLETOOL_H__
