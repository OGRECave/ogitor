/*
--------------------------------------------------------------------------------
This source file is part of SkyX.
Visit ---

Copyright (C) 2009 Xavier Verguín González <xavierverguin@hotmail.com>
                                           <xavyiy@gmail.com>

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
--------------------------------------------------------------------------------
*/

#include "MeshManager.h"

#include "SkyX.h"

namespace SkyX
{
	MeshManager::MeshManager(SkyX *s)
		: mSkyX(s)
		, mCreated(false)
		, mMesh(0)
        , mSubMesh(0)
        , mEntity(0)
        , mVertexBuffer(0)
		, mVertices(0)
        , mIndexBuffer(0)
	    , mSceneNode(0)
		, mSteps(70)
		, mCircles(80)
		, mSmoothSkydomeFading(true)
		, mSkydomeFadingPercent(0.05f)
        , mMaterialName("_NULL_")
	{
	}

	MeshManager::~MeshManager()
	{
		remove();
	}

	void MeshManager::remove()
	{
		if (!mCreated)
		{
			return;
		}

		mSceneNode->detachAllObjects();
		mSceneNode->getParentSceneNode()->removeAndDestroyChild(mSceneNode->getName());
		mSceneNode = 0;

		Ogre::MeshManager::getSingleton().remove("SkyXMesh");
		mSkyX->getSceneManager()->destroyEntity(mEntity);

		mMesh.setNull();
		mSubMesh = 0;
		mEntity = 0;
		mVertexBuffer.setNull();
		mIndexBuffer.setNull();
		mMaterialName = "_NULL_";

		delete [] mVertices;

		mCreated = false;
	}

	void MeshManager::create()
	{
		if (mCreated)
		{
			return;
		}

		// Create mesh and submesh
        mMesh = Ogre::MeshManager::getSingleton().createManual("SkyXMesh",
                    SKYX_RESOURCE_GROUP);
        mSubMesh = mMesh->createSubMesh();
        mSubMesh->useSharedVertices = false;

		// Create mesh geometry
		_createGeometry();

		// End mesh creation
        mMesh->load();
        mMesh->touch();

        mEntity = mSkyX->getSceneManager()->createEntity("SkyXMeshEnt", "SkyXMesh");
        mEntity->setMaterialName(mMaterialName);
		mEntity->setCastShadows(false);
		mEntity->setRenderQueueGroup(Ogre::RENDER_QUEUE_SKIES_EARLY);

		mSceneNode = mSkyX->getSceneManager()->getRootSceneNode()->createChildSceneNode();
		mSceneNode->showBoundingBox(false);
        mSceneNode->attachObject(mEntity);
        mSceneNode->setPosition(mSkyX->getCamera()->getDerivedPosition());

		mCreated = true;

		_updateGeometry();
	}

	void MeshManager::_updateGeometry()
	{
		if (!mCreated)
		{
			return;
		}

		// 95% of camera far clip distance
		float TODO_Radius = mSkyX->getCamera()->getFarClipDistance()*0.95f;

		mVertices[0].x = 0; mVertices[0].z = 0;	mVertices[0].y = TODO_Radius;
		mVertices[0].nx = 0; mVertices[0].nz = 0; mVertices[0].ny = 1; 
		mVertices[0].u = 4; mVertices[0].v = 4;
		mVertices[0].o = 1;

		float AngleStep = (Ogre::Math::PI/2) / (mCircles-1);

		float r, uvr, c, s, sc;
		int x, y;

		for(y=0;y<mCircles-1;y++) 
		{
			r = Ogre::Math::Cos(Ogre::Math::PI/2 - AngleStep*(y+1));
			uvr = static_cast<float>(y+1)/(mCircles-1);

			for(x=0;x<mSteps;x++) 
			{
				c = Ogre::Math::Cos(Ogre::Math::TWO_PI * x / mSteps) * r;
				s = Ogre::Math::Sin(Ogre::Math::TWO_PI * x / mSteps) * r;
				sc = Ogre::Math::Sin(Ogre::Math::ACos(r));

				mVertices[1+y*mSteps + x].x = c * TODO_Radius;
				mVertices[1+y*mSteps + x].z = s * TODO_Radius;
				mVertices[1+y*mSteps + x].y = sc * TODO_Radius;

				mVertices[1+y*mSteps + x].nx = c;
				mVertices[1+y*mSteps + x].nz = s;
				mVertices[1+y*mSteps + x].ny = sc;

				mVertices[1+y*mSteps + x].u = (1 + c*uvr/r)*4;
				mVertices[1+y*mSteps + x].v = (1 + s*uvr/r)*4;

				mVertices[1+y*mSteps + x].o = 1;
			}
		}

		r = Ogre::Math::Cos(AngleStep);
		uvr = static_cast<float>(mCircles+1)/(mCircles-1);

		for(x=0;x<mSteps;x++) 
		{
			c = Ogre::Math::Cos(Ogre::Math::TWO_PI * x / mSteps) * r;
			s = Ogre::Math::Sin(Ogre::Math::TWO_PI * x / mSteps) * r;

			mVertices[1+(mCircles-1)*mSteps + x].x = mVertices[1+(mCircles-2)*mSteps + x].x;
			mVertices[1+(mCircles-1)*mSteps + x].z = mVertices[1+(mCircles-2)*mSteps + x].z;
			mVertices[1+(mCircles-1)*mSteps + x].y = mVertices[1+(mCircles-2)*mSteps + x].y - TODO_Radius*mSkydomeFadingPercent;

			mVertices[1+(mCircles-1)*mSteps + x].nx = mVertices[1+(mCircles-2)*mSteps + x].nx;
			mVertices[1+(mCircles-1)*mSteps + x].nz = mVertices[1+(mCircles-2)*mSteps + x].nz;
			mVertices[1+(mCircles-1)*mSteps + x].ny = mVertices[1+(mCircles-2)*mSteps + x].ny;

			mVertices[1+(mCircles-1)*mSteps + x].u = (1 + c*uvr/r)*4;
			mVertices[1+(mCircles-1)*mSteps + x].v = (1 + s*uvr/r)*4;

			mVertices[1+(mCircles-1)*mSteps + x].o = mSmoothSkydomeFading ? 0 : 1;
		}

		// Update data
		mVertexBuffer->
				writeData(0,
		                  mVertexBuffer->getSizeInBytes(),
	                      mVertices,
		                  true);
	
		// Update bounds
	    Ogre::AxisAlignedBox meshBounds =
			Ogre::AxisAlignedBox(-TODO_Radius, 0,          -TODO_Radius,
			                      TODO_Radius, TODO_Radius, TODO_Radius);

		mMesh->_setBounds(meshBounds);
		mSceneNode->_updateBounds();
	}

	void MeshManager::_createGeometry()
	{
		int numVertices = mSteps * mCircles + 1;
		int numEle = 6 * mSteps * (mCircles-1) + 3 * mSteps;

		// Vertex buffers
		mSubMesh->vertexData = new Ogre::VertexData();
		mSubMesh->vertexData->vertexStart = 0;
		mSubMesh->vertexData->vertexCount = numVertices;

		Ogre::VertexDeclaration* vdecl = mSubMesh->vertexData->vertexDeclaration;
		Ogre::VertexBufferBinding* vbind = mSubMesh->vertexData->vertexBufferBinding;

		size_t offset = 0;
		vdecl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
		offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);
		vdecl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_TEXTURE_COORDINATES, 0);
		offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);
		vdecl->addElement(0, offset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES, 1);
		offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT2);
		vdecl->addElement(0, offset, Ogre::VET_FLOAT1, Ogre::VES_TEXTURE_COORDINATES, 2);

		mVertexBuffer = Ogre::HardwareBufferManager::getSingleton().
			createVertexBuffer(sizeof(POS_UV_VERTEX),
			                   numVertices,
			                   Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY);

		vbind->setBinding(0, mVertexBuffer);

		unsigned int *indexbuffer = new unsigned int[numEle];

		for (int k = 0; k < mSteps; k++)
		{
			indexbuffer[k*3] = 0;
			indexbuffer[k*3+1] = k+1;

			if (k != mSteps-1)
			{
			    indexbuffer[k*3+2] = k+2;
			}
			else
			{
				indexbuffer[k*3+2] = 1;
			}
		}

		for(int y=0; y<mCircles-1; y++) 
		{
		    for(int x=0; x<mSteps; x++) 
			{
			    unsigned int *twoface = indexbuffer + (y*mSteps+x)*6 + 3 * mSteps;

			    int p0 = 1+y * mSteps + x ;
			    int p1 = 1+y * mSteps + x + 1 ;
			    int p2 = 1+(y+1)* mSteps + x ;
			    int p3 = 1+(y+1)* mSteps + x + 1 ;

				if (x == mSteps-1)
				{
					p1 -= x+1;
					p3 -= x+1;
				}

				// First triangle
			    twoface[2]=p0;
			    twoface[1]=p1;
			    twoface[0]=p2;

				// Second triangle
			    twoface[5]=p1;
			    twoface[4]=p3;
			    twoface[3]=p2;
		    }
	    }

		// Prepare buffer for indices
		mIndexBuffer =
			Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
			Ogre::HardwareIndexBuffer::IT_32BIT,
			numEle,
			Ogre::HardwareBuffer::HBU_STATIC, true);

		mIndexBuffer->
			writeData(0,
			          mIndexBuffer->getSizeInBytes(),
			          indexbuffer,
			          true);

		delete []indexbuffer;

		// Set index buffer for this submesh
		mSubMesh->indexData->indexBuffer = mIndexBuffer;
		mSubMesh->indexData->indexStart = 0;
		mSubMesh->indexData->indexCount = numEle;

	    // Create our internal buffer for manipulations
		mVertices = new POS_UV_VERTEX[1+mSteps * mCircles];
	}

	void MeshManager::_setGeometryParameters(const int &Steps, const int &Circles)
	{
		mSteps = Steps;
		mCircles = Circles;

		if (mCreated)
		{
		    remove();
		    create();
		}
	}

	void MeshManager::setMaterialName(const Ogre::String& MaterialName)
	{
		mMaterialName = MaterialName;

		if (mCreated)
		{
			mEntity->setMaterialName(MaterialName);
		}
	}

	const float MeshManager::getSkydomeRadius() const
	{
		return mSkyX->getCamera()->getFarClipDistance()*0.95f;
	}
}