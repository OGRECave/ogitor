/*
This file is part of MeshMagick - An Ogre mesh file manipulation tool.
Copyright (C) 2007-2010 Henrik Hinrichs, Sascha Kolewa, Daniel Wickert

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
