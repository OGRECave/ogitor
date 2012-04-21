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

#include "MmRenameTool.h"

#include <OgreBone.h>
#include <OgreLog.h>
#include <OgreMesh.h>
#include <OgreSkeleton.h>
#include <OgreSubMesh.h>

#include "MmEditableBone.h"
#include "MmEditableMesh.h"
#include "MmEditableSkeleton.h"
#include "MmOgreEnvironment.h"
#include "MmStatefulMeshSerializer.h"
#include "MmStatefulSkeletonSerializer.h"

using namespace Ogre;

namespace meshmagick
{
	RenameTool::RenameTool()
	: Tool()
	{
	}

	RenameTool::~RenameTool()
	{
	}

    Ogre::String RenameTool::getName() const
    {
        return "rename";
    }

	void RenameTool::doInvoke(
		const OptionList &toolOptions, 
		const Ogre::StringVector &inFileNames, 
		const Ogre::StringVector &outFileNamesArg)
	{
        // Name count has to match, else we have no way to figure out how to apply output
        // names to input files.
        if (!(outFileNamesArg.empty() || inFileNames.size() == outFileNamesArg.size()))
        {
            fail("number of output files must match number of input files.");
        }

        StringVector outFileNames = outFileNamesArg.empty() ? inFileNames : outFileNamesArg;

        // Process the meshes
        for (size_t i = 0, end = inFileNames.size(); i < end; ++i)
        {
            if (StringUtil::endsWith(inFileNames[i], ".mesh", true))
            {
                processMeshFile(toolOptions, inFileNames[i], outFileNames[i]);
            }
            else if (StringUtil::endsWith(inFileNames[i], ".skeleton", true))
            {
                processSkeletonFile(toolOptions, inFileNames[i], outFileNames[i]);
            }
            else
            {
                warn("unrecognised name ending for file " + inFileNames[i]);
                warn("file skipped.");
            }
        }
	}

	void RenameTool::processSkeletonFile(
		const OptionList &toolOptions, Ogre::String inFile, Ogre::String outFile)
    {
        StatefulSkeletonSerializer* skeletonSerializer =
            OgreEnvironment::getSingleton().getSkeletonSerializer();

        print("Loading skeleton " + inFile + "...");
        SkeletonPtr skeleton;
        try
        {
            skeleton = skeletonSerializer->loadSkeleton(inFile);
        }
        catch(std::exception& e)
        {
            warn(e.what());
            warn("Unable to open skeleton file " + inFile);
            warn("file skipped.");
            return;
        }
        print("Processing skeleton...");
		for (OptionList::const_iterator 
			it = toolOptions.begin(); it != toolOptions.end(); ++it)
		{
			if (it->first == "skeleton")
			{
				warn("Skeletons can only be renamed in meshes, skipped skeleton.");
			}
			else if (it->first == "bone")
			{
                // This approach to rename a bone is very brittle.
                // It largely uses the effect, that animations usebone handles instead
                // of their names and that the serializer gets the bones by handle too.
                // The skeleton will only really work again after the serialised version
                // is reloaded, as its lookup-by-name-map is inconsistent as are the entries
                // in this bone's parents' lists.
				StringPair names = split(any_cast<String>(it->second));
				EditableBone* bone = static_cast<EditableBone*>(skeleton->getBone(names.first));
                bone->setName(names.second);
			}
			else if (it->first == "animation")
			{
				StringPair names = split(any_cast<String>(it->second));
				EditableSkeleton* eskel = dynamic_cast<EditableSkeleton*>(skeleton.getPointer());
				Animation* anim = eskel->getAnimation(names.first);
				eskel->removeAnimation(names.first);
				Animation* newAnim = anim->clone(names.second);
				eskel->addAnimation(newAnim);
			}
            else if (it->first == "material")
            {
                warn("Materials can only be renamed in meshes, skipped skeleton.");
            }
		}
        skeletonSerializer->saveSkeleton(outFile, true);
        print("Skeleton saved as " + outFile + ".");
    }

    void RenameTool::processMeshFile(
		const OptionList &toolOptions, Ogre::String inFile, Ogre::String outFile)
    {
        StatefulMeshSerializer* meshSerializer =
            OgreEnvironment::getSingleton().getMeshSerializer();

        print("Loading mesh " + inFile + "...");
        MeshPtr mesh;
        try
        {
            mesh = meshSerializer->loadMesh(inFile);
        }
        catch(std::exception& e)
        {
            warn(e.what());
            warn("Unable to open mesh file " + inFile);
            warn("file skipped.");
            return;
        }
        print("Processing mesh...");

		for (OptionList::const_iterator 
			it = toolOptions.begin(); it != toolOptions.end(); ++it)
		{
			if (it->first == "skeleton")
			{
				mesh->setSkeletonName(any_cast<String>(it->second));
			}
			else if (it->first == "bone")
			{
				warn("Bones can only be renamed in skeletons, skipped mesh.");
			}
			else if (it->first == "animation")
			{
				warn("Animations must be renamed in skeletons, skipped mesh.");
			}
            else if (it->first == "material")
            {
				StringPair names = split(any_cast<String>(it->second));
                String before = names.first;
                String after = names.second;
                for (Mesh::SubMeshIterator it = mesh->getSubMeshIterator();
                    it.hasMoreElements();)
                {
                    SubMesh* submesh = it.getNext();
                    if (submesh->getMaterialName() == before)
                    {
                        submesh->setMaterialName(after);
                    }
                }
            }
            else if (it->first == "submesh")
            {
				StringPair names = split(any_cast<String>(it->second));
                String before = names.first;
                String after = names.second;

                Mesh::SubMeshNameMap smnn = mesh->getSubMeshNameMap();
                Mesh::SubMeshNameMap::iterator it = smnn.find(before);
                EditableMesh* pMesh = dynamic_cast<EditableMesh*>(mesh.getPointer());
                pMesh->renameSubmesh(before, after);
            }
		}
		
		meshSerializer->saveMesh(outFile, true);
        print("Mesh saved as " + outFile + ".");
    }

	RenameTool::StringPair RenameTool::split(const Ogre::String& value) const
	{
        // We expect two string components delimited by the first character like /foo/bar/ or ~foo~bar~
        if (value.size() > 0)
        {            
            String delimiter = value.substr(0, 1);
            StringVector components = StringUtil::split(value, delimiter);
            if (components.size() == 1)
            {
                return make_pair(components[0], components[0]);
            }
            
            return make_pair(components[0], components[1]);
        }
        return make_pair(String(""), String(""));
	}
}
