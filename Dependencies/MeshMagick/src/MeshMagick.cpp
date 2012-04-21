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

#include "MeshMagick.h"

#include "MmInfoToolFactory.h"
#include "MmMeshMergeToolFactory.h"
#include "MmOptimiseToolFactory.h"
#include "MmRenameToolFactory.h"
#include "MmTransformToolFactory.h"

template<> meshmagick::MeshMagick* Ogre::Singleton<meshmagick::MeshMagick>::msSingleton = NULL;

using namespace Ogre;

namespace meshmagick
{
	MeshMagick::MeshMagick(Log* log)
		: mToolManager(NULL),
		mOgreEnvironment(NULL),
		mInfoTool(NULL),
		mMeshMergeTool(NULL),
		mTransformTool(NULL)
	{
		mOgreEnvironment = new OgreEnvironment();
		mOgreEnvironment->initialize(false, log);

		mToolManager = new ToolManager();
		mToolManager->registerToolFactory(new InfoToolFactory());
		mToolManager->registerToolFactory(new MeshMergeToolFactory());
		mToolManager->registerToolFactory(new TransformToolFactory());
	}
    //------------------------------------------------------------------------

	MeshMagick::~MeshMagick()
	{
		if (mInfoTool != NULL) mToolManager->destroyTool(mInfoTool);
		if (mMeshMergeTool != NULL) mToolManager->destroyTool(mMeshMergeTool);
		if (mTransformTool != NULL) mToolManager->destroyTool(mTransformTool);

		delete mToolManager;
		delete mOgreEnvironment;
	}
    //------------------------------------------------------------------------

	InfoTool* MeshMagick::getInfoTool()
	{
		if (mInfoTool == NULL)
		{
			mInfoTool = static_cast<InfoTool*>(mToolManager->createTool("info"));
		}
		return mInfoTool;
	}
    //------------------------------------------------------------------------
	MeshMergeTool* MeshMagick::getMeshMergeTool()
	{
		if (mMeshMergeTool == NULL)
		{
			mMeshMergeTool = static_cast<MeshMergeTool*>(mToolManager->createTool("meshmerge"));
		}
		return mMeshMergeTool;
	}
    //------------------------------------------------------------------------
	TransformTool* MeshMagick::getTransformTool()
	{
		if (mTransformTool == NULL)
		{
			mTransformTool = static_cast<TransformTool*>(mToolManager->createTool("transform"));
		}
		return mTransformTool;
	}
    //------------------------------------------------------------------------
}
