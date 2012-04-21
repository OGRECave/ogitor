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

#include "MmMeshMergeTool.h"

#include <stdexcept>
#include <OgreAnimation.h>
#include <OgreAxisAlignedBox.h>
#include <OgreHardwareBufferManager.h>
#include <OgreMeshManager.h>
#include <OgreSkeletonManager.h>
#include <OgreSubMesh.h>

#include "MmOgreEnvironment.h"

using namespace Ogre;

namespace meshmagick
{
	MeshMergeTool::MeshMergeTool()
		: mBaseSkeleton(), mMeshes()
	{
	}

	MeshMergeTool::~MeshMergeTool()
	{
		mMeshes.clear();
		mBaseSkeleton.setNull();
	}

    Ogre::String MeshMergeTool::getName() const
    {
        return "meshmerge";
    }

	void MeshMergeTool::doInvoke(const OptionList& toolOptions,
			const Ogre::StringVector& inFileNames,
			const Ogre::StringVector& outFileNames)
	{
		if (outFileNames.size() != 1)
		{
			fail("Exactly one output file must be specified.");
			return;
		}
		else if (inFileNames.size() == 0)
		{
			fail("No input files specified.");
			return;
		}

		StatefulMeshSerializer* meshSer = OgreEnvironment::getSingleton().getMeshSerializer();
		StatefulSkeletonSerializer* skelSer =
			OgreEnvironment::getSingleton().getSkeletonSerializer();
		for (Ogre::StringVector::const_iterator it = inFileNames.begin();
			it != inFileNames.end(); ++it)
		{
			MeshPtr curMesh = meshSer->loadMesh(*it);
			if (!curMesh.isNull())
			{
				if (curMesh->hasSkeleton() && SkeletonManager::getSingleton().getByName(
					curMesh->getSkeletonName()).isNull())
				{
					skelSer->loadSkeleton(curMesh->getSkeletonName());
				}
				addMesh(curMesh);
			}
			else
			{
				warn("Skipped: Mesh " + *it + " cannnot be loaded.");
			}
		}
		Ogre::String outputfile = *outFileNames.begin();
		meshSer->exportMesh(merge(outputfile).getPointer(), outputfile);
	}


	void MeshMergeTool::addMesh(Ogre::MeshPtr mesh)
	{
		SkeletonPtr meshSkel = mesh->getSkeleton();
		if (meshSkel.isNull() && mesh->hasSkeleton())
		{
			meshSkel = SkeletonManager::getSingleton().getByName(mesh->getSkeletonName());
		}

		if (meshSkel.isNull() && !mBaseSkeleton.isNull())
		{
			throw std::logic_error(
					"Some meshes have a skeleton, but others have none, cannot merge.");
		}

		if (!meshSkel.isNull() && mBaseSkeleton.isNull() && !mMeshes.empty())
		{
			throw std::logic_error(
					"Some meshes have a skeleton, but others have none, cannot merge.");
		}

		if (!meshSkel.isNull() && mBaseSkeleton.isNull() && mMeshes.empty())
		{
			mBaseSkeleton = meshSkel;
			print("Set: base skeleton (" + mBaseSkeleton->getName()+")", V_HIGH);
		}

		if (meshSkel != mBaseSkeleton)
		{
			throw std::logic_error(
					"Some meshes have a skeleton, but others have none, cannot merge.");
		}

		mMeshes.push_back(mesh);
	}

	const String MeshMergeTool::findSubmeshName(MeshPtr m, Ogre::ushort sid) const
	{
		Mesh::SubMeshNameMap map = m->getSubMeshNameMap();
		for (Mesh::SubMeshNameMap::const_iterator it = map.begin();
				it != map.end(); ++it)
		{
			if (it->second == sid)
				return it->first;
		}

		return "";
	}

	MeshPtr MeshMergeTool::merge(const Ogre::String& name, const Ogre::String& resourceGroupName)
	{
		print("Baking: New Mesh started", V_HIGH);

		MeshPtr mp = MeshManager::getSingleton().createManual(name, resourceGroupName);

		if (!mBaseSkeleton.isNull())
		{
			mp->setSkeletonName(mBaseSkeleton->getName());
		}

		AxisAlignedBox totalBounds = AxisAlignedBox();
		for (std::vector<Ogre::MeshPtr>::iterator it = mMeshes.begin(); it != mMeshes.end(); ++it)
		{
			print("Baking: adding submeshes for " + (*it)->getName(), V_HIGH);

			// insert all submeshes
			for (Ogre::ushort sid = 0; sid < (*it)->getNumSubMeshes(); ++sid)
			{
				SubMesh* sub = (*it)->getSubMesh(sid);
				const String name = findSubmeshName((*it), sid);

				// create submesh with correct name
				SubMesh* newsub;
				if (name.length() == 0)
				{
					newsub = mp->createSubMesh();
				}
				else
				{
					/// @todo check if a submesh with this name has been created before
					newsub = mp->createSubMesh(name);
				}

				newsub->useSharedVertices = sub->useSharedVertices;

				// add index
				newsub->indexData = sub->indexData->clone();

				// add geometry
				if (!newsub->useSharedVertices)
				{
					newsub->vertexData = sub->vertexData->clone();

					if (!mBaseSkeleton.isNull())
					{
						// build bone assignments
						SubMesh::BoneAssignmentIterator bit = sub->getBoneAssignmentIterator();
						while (bit.hasMoreElements())
						{
							VertexBoneAssignment vba = bit.getNext();
							newsub->addBoneAssignment(vba);
						}
					}
				}

				newsub->setMaterialName(sub->getMaterialName());

				// Add vertex animations for this submesh
				Animation *anim = 0;
				for (unsigned short i = 0; i < (*it)->getNumAnimations(); ++i)
				{
					anim = (*it)->getAnimation(i);

					// get or create the animation for the new mesh
					Animation *newanim;
					if (mp->hasAnimation(anim->getName()))
					{
						newanim = mp->getAnimation(anim->getName());
					}
					else
					{
						newanim = mp->createAnimation(anim->getName(), anim->getLength());
					}

					print("Baking: adding vertex animation "
						+ anim->getName() + " for " + (*it)->getName(), V_HIGH);

					Animation::VertexTrackIterator vti=anim->getVertexTrackIterator();
					while (vti.hasMoreElements())
					{
						VertexAnimationTrack *vt = vti.getNext();

						// handle=0 targets the main mesh, handle i (where i>0) targets submesh i-1.
						// In this case there are only submeshes so index 0 will not be used.
						unsigned short handle = mp->getNumSubMeshes();
						VertexAnimationTrack* newvt = newanim->createVertexTrack(
								handle,
								vt->getAssociatedVertexData()->clone(),
								vt->getAnimationType());
						for (int keyFrameIndex = 0; keyFrameIndex < vt->getNumKeyFrames();
							++keyFrameIndex)
						{
							switch (vt->getAnimationType())
							{
								case VAT_MORPH:
								{
									// copy the keyframe vertex buffer
									VertexMorphKeyFrame *kf =
										vt->getVertexMorphKeyFrame(keyFrameIndex);
									VertexMorphKeyFrame *newkf =
										newvt->createVertexMorphKeyFrame(kf->getTime());
									// This creates a ref to the buffer in the original model
									// so don't delete it until the export is completed.
									newkf->setVertexBuffer(kf->getVertexBuffer());
									break;
								}
								case VAT_POSE:
								{
									/// @todo implement pose amination merge
									break;
								}
								case VAT_NONE:
								default:
								{
									break;
								}
							}
						}
					}
				}

				print("Baking: adding submesh '" +
					name + "'  with material " + sub->getMaterialName(), V_HIGH);
			}

			// sharedvertices
			if ((*it)->sharedVertexData)
			{
				/// @todo merge with existing sharedVertexData
				if (!mp->sharedVertexData)
				{
					mp->sharedVertexData = (*it)->sharedVertexData->clone();
				}

				if (!mBaseSkeleton.isNull())
				{
					Mesh::BoneAssignmentIterator bit = (*it)->getBoneAssignmentIterator();
					while (bit.hasMoreElements())
					{
						VertexBoneAssignment vba = bit.getNext();
						mp->addBoneAssignment(vba);
					}
				}
			}

			print("Baking: adding bounds for " + (*it)->getName(), V_HIGH);

			// add bounds
			totalBounds.merge((*it)->getBounds());
		}
		mp->_setBounds(totalBounds);

		/// @todo merge submeshes with same material

		/// @todo add parameters
		mp->buildEdgeList();

		print("Baking: Finished", V_HIGH);

		reset();

		return mp;
	}

	void MeshMergeTool::reset()
	{
		mMeshes.clear();
	}
}
