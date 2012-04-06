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

#include "MmTransformTool.h"

#include <OgreAnimation.h>
#include <OgreStringConverter.h>
#include <OgreSubMesh.h>

#include "MmMeshUtils.h"
#include "MmToolUtils.h"
#include "MmOgreEnvironment.h"
#include "MmStatefulSkeletonSerializer.h"
#include "MmStatefulMeshSerializer.h"

using namespace Ogre;

namespace meshmagick
{
    TransformTool::TransformTool()
        : mTransform(Matrix4::IDENTITY),
          mNormaliseNormals(false),
          mUpdateBoundingBox(true),
          mFlipVertexWinding(false),
          mOptions()
    {
    }

    Ogre::String TransformTool::getName() const
    {
        return "transform";
    }

	void TransformTool::doInvoke(const OptionList& toolOptions,
        const StringVector& inFileNames, const StringVector& outFileNamesArg)
    {
        // Name count has to match, else we have no way to figure out how to apply output
        // names to input files.
        if (!(outFileNamesArg.empty() || inFileNames.size() == outFileNamesArg.size()))
        {
            fail("number of output files must match number of input files.");
        }

        setOptions(toolOptions);

        StringVector outFileNames = outFileNamesArg.empty() ? inFileNames : outFileNamesArg;

        // Process the meshes
        for (size_t i = 0, end = inFileNames.size(); i < end; ++i)
        {
            if (StringUtil::endsWith(inFileNames[i], ".mesh", true))
            {
                processMeshFile(inFileNames[i], outFileNames[i]);
            }
            else if (StringUtil::endsWith(inFileNames[i], ".skeleton", true))
            {
                processSkeletonFile(inFileNames[i], outFileNames[i], true);
            }
            else
            {
                warn("unrecognised name ending for file " + inFileNames[i]);
                warn("file skipped.");
            }
        }
    }

    void TransformTool::processSkeletonFile(String inFile, String outFile, bool calcTransform)
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
        if (calcTransform)
        {
            calculateTransform();
        }
        processSkeleton(skeleton);
        skeletonSerializer->saveSkeleton(outFile, true);
        print("Skeleton saved as " + outFile + ".");
    }

    void TransformTool::processMeshFile(Ogre::String inFile, Ogre::String outFile)
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
        calculateTransform(mesh);
        processMesh(mesh);
        meshSerializer->saveMesh(outFile, true);
        print("Mesh saved as " + outFile + ".");

        if (mFollowSkeletonLink && mesh->hasSkeleton())
        {
            // In this case keep file name and also keep already determined transform
            String skeletonFileName = ToolUtils::getSkeletonFileName(mesh, inFile);
            processSkeletonFile(skeletonFileName, skeletonFileName, false);
        }
    }

    void TransformTool::processSkeleton(Ogre::SkeletonPtr skeleton)
    {
        Skeleton::BoneIterator it = skeleton->getBoneIterator();
        while (it.hasMoreElements())
        {
            processBone(it.peekNext());
            it.moveNext();
        }

        for (unsigned short aniIdx = 0; aniIdx < skeleton->getNumAnimations(); ++aniIdx)
        {
            Animation* ani = skeleton->getAnimation(aniIdx);
            processAnimation(ani);
        }
    }

    void TransformTool::processAnimation(Ogre::Animation* ani)
    {
        print("Processing animation " + ani->getName() + "...", V_HIGH);

        // We only need to apply scaling and rotation, no translation.
        Matrix3 m3x3;
        mTransform.extract3x3Matrix(m3x3);
        Vector3 scale(
            m3x3.GetColumn(0).length(),
            m3x3.GetColumn(1).length(),
            m3x3.GetColumn(2).length());
        Animation::NodeTrackIterator trackIt = ani->getNodeTrackIterator();
        while (trackIt.hasMoreElements())
        {
            NodeAnimationTrack* track = trackIt.getNext();
            // We need to apply full transform to root bone translations and only scale to the others.
            if (track->getAssociatedNode()->getParent() == NULL)
            {
                // An animation track for a skeleton is only supposed to have
                // TransformKeyFrames, so just use these here.
                for (unsigned short frameIdx = 0; frameIdx < track->getNumKeyFrames(); ++frameIdx)
                {
                    TransformKeyFrame* keyframe = track->getNodeKeyFrame(frameIdx);
                    keyframe->setTranslate(m3x3 * keyframe->getTranslate());
                }
            }
            else
            {
                for (unsigned short frameIdx = 0; frameIdx < track->getNumKeyFrames(); ++frameIdx)
                {
                    TransformKeyFrame* keyframe = track->getNodeKeyFrame(frameIdx);
                    keyframe->setTranslate(scale * keyframe->getTranslate());
                }
            }
        }
    }

    void TransformTool::processBone(Ogre::Bone* bone)
    {
        print("Processing bone " + bone->getName() + "...", V_HIGH);
        if (bone->getParent() == NULL)
        {
            // Is root bone, we need to apply full transform
            bone->setPosition(mTransform * bone->getPosition());
            Quaternion rot = mTransform.extractQuaternion();
            rot.normalise();
            bone->setOrientation(rot * bone->getOrientation());
        }
        else
        {
            // Non-root-bone, we apply only scale
            Matrix3 m3x3;
            mTransform.extract3x3Matrix(m3x3);
            Vector3 scale(
                m3x3.GetColumn(0).length(),
                m3x3.GetColumn(1).length(),
                m3x3.GetColumn(2).length());
            bone->setPosition(scale * bone->getPosition());
        }
    }

	void TransformTool::transform(Ogre::MeshPtr mesh, Ogre::Matrix4 transformation, bool followSkeleton)
	{
		mTransform = transformation;
		processMesh(mesh);

		if (followSkeleton && mesh->hasSkeleton())
		{
			processSkeleton(mesh->getSkeleton());
		}
	}

	void TransformTool::transform(Ogre::SkeletonPtr skeleton, Ogre::Matrix4 transformation)
	{
		mTransform = transformation;
		processSkeleton(skeleton);
	}

    void TransformTool::processMesh(Ogre::MeshPtr mesh)
    {
        mBoundingBox.setNull();

        if (mesh->sharedVertexData != NULL)
        {
            processVertexData(mesh->sharedVertexData);
        }

        for(int i = 0;i < mesh->getNumSubMeshes();i++)
        {
            SubMesh* submesh = mesh->getSubMesh(i);
            if (submesh->vertexData != NULL)
            {
                processVertexData(submesh->vertexData);
            }
            if (submesh->indexData != NULL)
            {
            	processIndexData(submesh->indexData);
            }
        }

        // Process poses, if there are any
        for (unsigned short i = 0; i < mesh->getPoseCount(); ++i)
        {
            processPose(mesh->getPose(i));
        }

        // If there are vertex animations, process these too.
        if (mesh->hasVertexAnimation())
        {
            // Then process morph targets
            unsigned short count = mesh->getNumAnimations();
            for (unsigned short i = 0; i < count; ++i)
            {
                Animation* anim = mesh->getAnimation(i);
                Animation::VertexTrackIterator it = anim->getVertexTrackIterator();
                while (it.hasMoreElements())
                {
                    VertexAnimationTrack* track = it.getNext();
                    if (track->getAnimationType() == VAT_MORPH)
                    {
                        for (unsigned short i = 0; i < track->getNumKeyFrames(); ++i)
                        {
                            processVertexMorphKeyFrame(track->getVertexMorphKeyFrame(i),
                                track->getAssociatedVertexData()->vertexCount);
                        }
                    }
                }
            }
        }

        if (mUpdateBoundingBox)
        {
            mesh->_setBounds(mBoundingBox, false);
        }
    }

	void TransformTool::processIndexData(IndexData* indexData)
	{
		if (!mFlipVertexWinding)
		{
			// Nothing to do.
			return;
		}

		if (indexData->indexCount % 3 != 0)
		{
            printf("Index number is not a multiple of 3, no vertex winding flipping possible. Skipped.");
            return;
		}

		print("Flipping index order for vertex winding flipping.", V_HIGH);
		Ogre::HardwareIndexBufferSharedPtr buffer = indexData->indexBuffer;
		unsigned char* data =
               static_cast<unsigned char*>(buffer->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

		if(buffer->getType() == Ogre::HardwareIndexBuffer::IT_16BIT)
		{
			// 16 bit
			print("using 16bit indices", V_HIGH);

			for (size_t i = 0; i < indexData->indexCount; i+=3)
			{
				uint16* i0 = (uint16*)(data+0 * buffer->getIndexSize());
				uint16* i2 = (uint16*)(data+2 * buffer->getIndexSize());

				// flip
				uint16 tmp = *i0;
				*i0 = *i2;
				*i2 = tmp;

				data += 3 * buffer->getIndexSize();
			}
		}
		else
		{
			// 32 bit
			print("using 32bit indices", V_HIGH);

			for (size_t i = 0; i < indexData->indexCount; i+=3)
			{
				uint32* i0 = (uint32*)(data+0 * buffer->getIndexSize());
				uint32* i2 = (uint32*)(data+2 * buffer->getIndexSize());

				// flip
				uint32 tmp = *i0;
				*i0 = *i2;
				*i2 = tmp;

				data += 3 * buffer->getIndexSize();
			}
		}

		buffer->unlock();
	}

    void TransformTool::processVertexData(VertexData* vertexData)
    {
        const VertexElement* position =
            vertexData->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
        if (position != NULL)
        {
            processPositionElement(vertexData, position);
        }

        const VertexElement* normal =
            vertexData->vertexDeclaration->findElementBySemantic(Ogre::VES_NORMAL);
        if (normal != NULL)
        {
            processDirectionElement(vertexData, normal);
        }

        const VertexElement* binormal =
            vertexData->vertexDeclaration->findElementBySemantic(Ogre::VES_BINORMAL);
        if (binormal != NULL)
        {
            processDirectionElement(vertexData, binormal);
        }

        const VertexElement* tangent =
            vertexData->vertexDeclaration->findElementBySemantic(Ogre::VES_TANGENT);
        if (tangent != NULL)
        {
            processDirectionElement(vertexData, tangent);
        }
    }

    void TransformTool::processPositionElement(VertexData* vertexData,
        const VertexElement* vertexElem)
    {
        Ogre::HardwareVertexBufferSharedPtr buffer =
            vertexData->vertexBufferBinding->getBuffer(vertexElem->getSource());

        unsigned char* data =
            static_cast<unsigned char*>(buffer->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
        for (size_t i = 0; i < vertexData->vertexCount; ++i)
        {
            Real* ptr;
            vertexElem->baseVertexPointerToElement(data, &ptr);

            Vector3 vertex(ptr);
            vertex = mTransform * vertex;
            ptr[0] = vertex.x;
            ptr[1] = vertex.y;
            ptr[2] = vertex.z;
            mBoundingBox.merge(vertex);

            data += buffer->getVertexSize();
        }
        buffer->unlock();
    }

    void TransformTool::processDirectionElement(VertexData* vertexData,
        const VertexElement* vertexElem)
    {
        // We only want to apply rotation to normal, binormal and tangent, so extract it.
        Quaternion rotation = mTransform.extractQuaternion();
        rotation.normalise();

        Ogre::HardwareVertexBufferSharedPtr buffer =
            vertexData->vertexBufferBinding->getBuffer(vertexElem->getSource());

        unsigned char* data =
            static_cast<unsigned char*>(buffer->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
        for (size_t i = 0; i < vertexData->vertexCount; ++i)
        {
            Real* ptr;
            vertexElem->baseVertexPointerToElement(data, &ptr);

            Vector3 vertex(ptr);
            vertex = rotation * vertex;
            if (mNormaliseNormals)
            {
                vertex.normalise();
            }
            ptr[0] = vertex.x;
            ptr[1] = vertex.y;
            ptr[2] = vertex.z;

            data += buffer->getVertexSize();
        }
        buffer->unlock();
    }

    void TransformTool::processPose(Pose* pose)
    {
        Matrix3 m3x3;
        mTransform.extract3x3Matrix(m3x3);

        Pose::VertexOffsetIterator it = pose->getVertexOffsetIterator();
        while (it.hasMoreElements())
        {
            Vector3 offset = it.peekNextValue();
            Vector3 newOffset = m3x3 * offset;
            *it.peekNextValuePtr() = newOffset;
            it.moveNext();
        }
    }

    void TransformTool::processVertexMorphKeyFrame(VertexMorphKeyFrame* keyframe,
        size_t vertexCount)
    {
        Vector3* positions = static_cast<Vector3*>(
            keyframe->getVertexBuffer()->lock(HardwareBuffer::HBL_READ_ONLY));
        for (size_t i = 0; i < vertexCount; ++i)
        {
            positions[i] = mTransform * positions[i];
        }
        keyframe->getVertexBuffer()->unlock();
    }

    void TransformTool::setOptions(const OptionList& options)
    {
        mOptions = options;

        mNormaliseNormals = !OptionsUtil::isOptionSet(options, "no-normalise-normals");
        if (!mNormaliseNormals)
        {
            print("Don't normalise normals", V_HIGH);
        }
        mUpdateBoundingBox = !OptionsUtil::isOptionSet(options, "no-update-boundingbox");
        if (!mUpdateBoundingBox)
        {
            print("Don't update bounding box", V_HIGH);
        }
        mFlipVertexWinding = OptionsUtil::isOptionSet(options, "flip-vertex-winding");
        if (mFlipVertexWinding)
        {
            print("Flip vertex winding", V_HIGH);
        }
    }

    void TransformTool::calculateTransform(MeshPtr mesh)
    {
        // Calculate transform
        Matrix4 transform = Matrix4::IDENTITY;

        print("Calculating transformation...", V_HIGH);

        for (OptionList::const_iterator it = mOptions.begin(); it != mOptions.end(); ++it)
        {
            if (it->first == "scale")
            {
                Vector3 scale = any_cast<Vector3>(it->second);
                transform = Matrix4::getScale(scale) * transform;
                print("Apply scaling " + StringConverter::toString(scale), V_HIGH);
            }
            else if (it->first == "translate")
            {
                Vector3 translate = any_cast<Vector3>(it->second);
                transform = Matrix4::getTrans(translate) * transform;
                print("Apply translation " + StringConverter::toString(translate), V_HIGH);
            }
            else if (it->first == "rotate")
            {
                Quaternion rotation = any_cast<Quaternion>(it->second);
                transform = Matrix4(rotation) * transform;
                print("Apply rotation (quat.) " + StringConverter::toString(rotation), V_HIGH);
            }
            else if (it->first == "xalign")
            {
                //ignore, if no mesh given. Without we can't do this op.
                if (mesh.isNull())
                {
                    print("Skipped alignment, operation can't be applied to skeletons", V_HIGH);
                    continue;
                }

                String alignment = any_cast<String>(it->second);
                Vector3 translate = Vector3::ZERO;
                // Apply current transform to the mesh, to get the bounding box to
                // base te translation on.
                AxisAlignedBox aabb = MeshUtils::getMeshAabb(mesh, transform);
                if (alignment == "left")
                {
                    translate = Vector3(-aabb.getMinimum().x, 0, 0);
                }
                else if (alignment == "center")
                {
                    translate = Vector3(-aabb.getCenter().x, 0, 0);
                }
                else if (alignment == "right")
                {
                    translate = Vector3(-aabb.getMaximum().x, 0, 0);
                }

                transform = Matrix4::getTrans(translate) * transform;
            }
            else if (it->first == "yalign")
            {
                //ignore, if no mesh given. Without we can't do this op.
                if (mesh.isNull())
                {
                    print("Skipped alignment, operation can't be applied to skeletons", V_HIGH);
                    continue;
                }

                String alignment = any_cast<String>(it->second);
                Vector3 translate = Vector3::ZERO;
                // Apply current transform to the mesh, to get the bounding box to
                // base te translation on.
                AxisAlignedBox aabb = MeshUtils::getMeshAabb(mesh, transform);
                if (alignment == "bottom")
                {
                    translate = Vector3(0, -aabb.getMinimum().y, 0);
                }
                else if (alignment == "center")
                {
                    translate = Vector3(0, -aabb.getCenter().y, 0);
                }
                else if (alignment == "top")
                {
                    translate = Vector3(0, -aabb.getMaximum().y, 0);
                }

                transform = Matrix4::getTrans(translate) * transform;
            }
            else if (it->first == "zalign")
            {
                //ignore, if no mesh given. Without we can't do this op.
                if (mesh.isNull())
                {
                    print("Skipped alignment, operation can't be applied to skeletons", V_HIGH);
                    continue;
                }

                String alignment = any_cast<String>(it->second);
                Vector3 translate = Vector3::ZERO;
                // Apply current transform to the mesh, to get the bounding box to
                // base the translation on.
                AxisAlignedBox aabb = MeshUtils::getMeshAabb(mesh, transform);
                if (alignment == "front")
                {
                    translate = Vector3(0, 0, -aabb.getMinimum().z);
                }
                else if (alignment == "center")
                {
                    translate = Vector3(0, 0, -aabb.getCenter().z);
                }
                else if (alignment == "back")
                {
                    translate = Vector3(0, 0, -aabb.getMaximum().z);
                }

                transform = Matrix4::getTrans(translate) * transform;
                print("Z-Alignment " + alignment + " - "
                    + StringConverter::toString(translate), V_HIGH);
            }
            else if (it->first == "resize")
            {
                //ignore, if no mesh given. Without we can't do this op.
                if (mesh.isNull())
                {
                    print("Skipped resize, operation can't be applied to skeletons", V_HIGH);
                    continue;
                }

                // determine the components of the resize-argument.
                String resizeString = any_cast<String>(it->second);
                StringVector resizeAxes = StringUtil::split(resizeString, "/");
                if (resizeAxes.size() != 3)
                {
                    warn("unrecongnized -resize option value. skipping..");
                    // Skip this, check next option.
                    continue;
                }

                // determine mean scale value in case we meet an 's' on an axis.
                unsigned short numValues = 0;
                Real valueSum = 0;
                Vector3 meshSize = MeshUtils::getMeshAabb(mesh, transform).getSize();
                for (size_t i = 0; i < 3; ++i)
                {
                    if (StringConverter::isNumber(resizeAxes[i]) && meshSize[i] > 0)
                    {
                        Real newSize = StringConverter::parseReal(resizeAxes[i]);
                        valueSum += newSize / meshSize[i];
                        ++numValues;
                    }
                }

				if (numValues == 0) continue;

                Real meanScale = valueSum / numValues;

                // now check each axis component and apply it to the scale vector accordingly
                Vector3 scale = Vector3::UNIT_SCALE;
                for (size_t i = 0; i < 3; ++i)
                {
                    if (StringConverter::isNumber(resizeAxes[i]) && meshSize[i] > 0)
                    {
                        // Scale this axis according to the length / value quotient
                        Real newSize = StringConverter::parseReal(resizeAxes[i]);
                        scale[i] = newSize / meshSize[i];
                    }
                    else if (resizeAxes[i].at(0) == 's' || resizeAxes[i].at(0) == 'S')
                    {
                        scale[i] = meanScale;
                    }
                    else if (resizeAxes[i].at(0) == 'k' || resizeAxes[i].at(0) == 'K')
                    {
                        // Do nothing with this axis.
                    }
                    else
                    {
                        warn("unrecongnized -resize option value. skipping..");
                    }
                }

                transform = Matrix4::getScale(scale) * transform;
            }
			else if (it->first == "axes")
            {
            	Vector3 axes[] = { Vector3::ZERO, Vector3::ZERO, Vector3::ZERO };

                // determine the components of the axes-argument.
                String axesString = any_cast<String>(it->second);
                StringVector components = StringUtil::split(axesString, "/");
                if (components.size() != 3)
                {
                    warn("unrecongnized -axes option value. skipping..");
                    continue;
                }

                // decompose transform matrix to 3x3 matrix and translation.
                // We will recontruct the 4x4 transform later in this code block.
                Matrix3 m3;
                transform.extract3x3Matrix(m3);
                Vector3 translation = transform.getTrans();

                bool malformed = false;
                for (size_t i = 0; i < components.size(); ++i)
                {
                	String component = components[i];
                	char axis;
                	Real factor = 1.0f;
                	if (component.size() > 2 || component.empty())
                	{
                		malformed = true;
                		break;
                	}
                	else if (component.size() == 2)
                	{
                		if (component.at(0) == '-')
                		{
                			factor = -1.0f;
                		}
                		else if (component.at(0) != '+')
                		{
                			malformed = true;
                			break;
                		}

                		axis = component.at(1);
                	}
                	else
                	{
                		axis = component.at(0);
                	}

                	if (axis == 'x' || axis == 'X')
                	{
                		axes[i] = m3.GetColumn(0) * factor;
                	}
                	else if (axis == 'y' || axis == 'Y')
                	{
                		axes[i] = m3.GetColumn(1) * factor;
                	}
                	else if (axis == 'z' || axis == 'Z')
                	{
                		axes[i] = m3.GetColumn(2) * factor;
                	}
                	else
                	{
                		malformed = true;
                		break;
                	}
                }
                if (malformed)
                {
                    warn("unrecongnized -axes option value. skipping..");
                    continue;
                }

                // Apply new axes to transform matrix.
                m3.FromAxes(axes[0], axes[1], axes[2]);
                transform = m3;
                transform.setTrans(translation);
            }
        }

        // Check whether we have to flip vertex winding.
        // We do have to, if we changed our right hand base.
        // We can test it by using the cross product from X and Y and see, if it is a non-negative
        // projection on Z. Actually it should be exactly Z, as we don't do non-uniform scaling yet,
        // but the test is cheap either way.
        Matrix3 m3;
        transform.extract3x3Matrix(m3);
        if (m3.GetColumn(0).crossProduct(m3.GetColumn(1)).dotProduct(m3.GetColumn(2)) < 0)
        {
        	mFlipVertexWinding = true;
        }

        mTransform = transform;
        print("final transform " + StringConverter::toString(mTransform), V_HIGH);
    }
}
