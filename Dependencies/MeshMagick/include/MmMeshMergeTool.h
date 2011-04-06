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

#ifndef __MM_MERGE_TOOL_H__
#define __MM_MERGE_TOOL_H__

#include "MeshMagickPrerequisites.h"

#ifdef __APPLE__
#	include <Ogre/OgreMesh.h>
#	include <Ogre/OgreResourceGroupManager.h>
#	include <Ogre/OgreSkeleton.h>
#else
#	include <OgreMesh.h>
#	include <OgreResourceGroupManager.h>
#	include <OgreSkeleton.h>
#endif

#include "MmOptionsParser.h"
#include "MmTool.h"

namespace meshmagick
{
	/** This tool merges multiple input Meshes into a single one.
	@par
		In order to merge meshes, you add meshes to the tool using
		MeshMergeTool#addMesh. To finish merge, call MeshMergeTool#merge.
		The tool is then reset to be able to merge the next batch of meshes.
	@par
		The merge tool creates a new SubMesh for each SubMesh of the Meshes you add.
		Its functionality is pretty basic still. No submesh merging based on material.
		Only one Mesh can have shared vertex data.
	 */
	class _MeshMagickExport MeshMergeTool : public Tool
    {
    public:
        MeshMergeTool();
		~MeshMergeTool();

		Ogre::String getName() const;

		/// Add a Mesh to the batch that creates a new Mesh. Call MeshMergeTool#bake, when done.
		void addMesh(Ogre::MeshPtr mesh);

		/** Merges all added Meshes into a new one. Internal state is reset afterwards.
		@param name The name of the merged mesh.
		@param resourceGroupName The resource group the merged mesh is created in.
		@return the newly created merged mesh
		*/
		Ogre::MeshPtr merge(const Ogre::String& name,
			const Ogre::String& resourceGroupName = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

		/// Clears the list of Meshes to be baked.
		void reset();

	private: 
		Ogre::SkeletonPtr mBaseSkeleton;
		std::vector<Ogre::MeshPtr> mMeshes;

		const Ogre::String findSubmeshName(Ogre::MeshPtr m, Ogre::ushort sid) const;

		void doInvoke(const OptionList& toolOptions,
			const Ogre::StringVector& inFileNames,
			const Ogre::StringVector& outFileNames);
    };
}
#endif
