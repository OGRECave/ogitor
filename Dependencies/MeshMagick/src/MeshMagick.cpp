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

#include "MeshMagick.h"

#include "MmInfoToolFactory.h"
#include "MmMeshMergeToolFactory.h"
#include "MmOptimiseToolFactory.h"
#include "MmRenameToolFactory.h"
#include "MmTransformToolFactory.h"

template<> meshmagick::MeshMagick* Ogre::Singleton<meshmagick::MeshMagick>::ms_Singleton = NULL;

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
