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

#ifndef __MM__MESHMAGICK_H__
#define __MM__MESHMAGICK_H__

#include "MeshMagickPrerequisites.h"

#include <OgreLog.h>
#include <OgreSingleton.h>

#include "MmOgreEnvironment.h"
#include "MmTool.h"
#include "MmToolManager.h"

#include "MmInfoTool.h"
#include "MmMeshMergeTool.h"
#include "MmOptimiseTool.h"
#include "MmRenameTool.h"
#include "MmTransformTool.h"

namespace meshmagick
{
	/** This class provides the entry point for MeshMagick usage.
	@par
		In order to use MeshMagick's tools from your program,
		create a MeshMagick instance. Call the getXxxTool() member function
		in order to retrieve a tool. Use the tool as desired.
		Delete this class when done with the tools. Don't delete a Tool,
		it gets destroyed when MeshMagick gets destroyed.
	*/
	class _MeshMagickExport MeshMagick : public Ogre::Singleton<MeshMagick>
	{
	public:
		MeshMagick(Ogre::Log* log = NULL);
		~MeshMagick();

		InfoTool* getInfoTool();
		MeshMergeTool* getMeshMergeTool();
		TransformTool* getTransformTool();

	private:
		InfoTool* mInfoTool;
		MeshMergeTool* mMeshMergeTool;
		TransformTool* mTransformTool;

		ToolManager* mToolManager;
		OgreEnvironment* mOgreEnvironment;
	};
}
#endif
