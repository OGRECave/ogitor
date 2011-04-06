///Modular Zone Plugin
///
/// Copyright (c) 2009 Gary Mclean
//
//This program is free software; you can redistribute it and/or modify it under
//the terms of the GNU Lesser General Public License as published by the Free Software
//Foundation; either version 2 of the License, or (at your option) any later
//version.
//
//This program is distributed in the hope that it will be useful, but WITHOUT
//ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
//FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
//
//You should have received a copy of the GNU Lesser General Public License along with
//this program; if not, write to the Free Software Foundation, Inc., 59 Temple
//Place - Suite 330, Boston, MA 02111-1307, USA, or go to
//http://www.gnu.org/copyleft/lesser.txt.
////////////////////////////////////////////////////////////////////////////////*/
#include "Ogre.h"
#include "PortalOutlineRenderable.h"


PortalOutlineRenderable::PortalOutlineRenderable(Ogre::String matname, Ogre::ColourValue colour)
{

	mRenderOp.vertexData = OGRE_NEW Ogre::VertexData();
	mRenderOp.indexData = 0;
	mRenderOp.vertexData->vertexCount = 10;//8
	mRenderOp.vertexData->vertexStart = 0;
	mRenderOp.operationType = Ogre::RenderOperation::OT_LINE_LIST;
	mRenderOp.useIndexes = false;
	Ogre::VertexDeclaration* decl = mRenderOp.vertexData->vertexDeclaration;
	Ogre::VertexBufferBinding* bind = mRenderOp.vertexData->vertexBufferBinding;
	decl->addElement(0, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
	Ogre::HardwareVertexBufferSharedPtr vbuf = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
		decl->getVertexSize(0), mRenderOp.vertexData->vertexCount, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
	// Bind buffer
	bind->setBinding(0, vbuf);
	// setup material
	createPortalMaterials();

	this->setCastShadows(false);
    this->setQueryFlags(0); // set a query flag to exlude from queries (if necessary).
	this->setMaterial("PortalOutlineMaterial");
}
//----------------------------------------------------------------------------------------
PortalOutlineRenderable::~PortalOutlineRenderable()
{
	OGRE_DELETE mRenderOp.vertexData;
}
//----------------------------------------------------------------------------------------
void PortalOutlineRenderable::setupVertices(Ogre::Real width, Ogre::Real height)
{

	Ogre::Real maxx = width/2;
	Ogre::Real maxy = height/2;
	Ogre::Real maxz = 0.2f;
	Ogre::Real minx = -(width/2);
	Ogre::Real miny = -(height/2);
	Ogre::Real minz = -0.2f;

	// fill in the Vertex buffer
	Ogre::HardwareVertexBufferSharedPtr vbuf = mRenderOp.vertexData->vertexBufferBinding->getBuffer(0);
	float* pPos = static_cast<float*>(vbuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));
	//outline
	// line 0
	*pPos++ = minx;
	*pPos++ = miny;
	*pPos++ = 0.0;

    *pPos++ = maxx;
	*pPos++ = miny;
	*pPos++ = 0.0;

	// line 1
	*pPos++ = maxx;
	*pPos++ = miny;
	*pPos++ = 0.0;

	*pPos++ = maxx;
	*pPos++ = maxy;
	*pPos++ = 0.0;

	// line 2
	*pPos++ = maxx;
	*pPos++ = maxy;
	*pPos++ = 0.0;

    *pPos++ = minx;
	*pPos++ = maxy;
	*pPos++ = 0.0;

	// line 3
	*pPos++ = minx;
	*pPos++ = maxy;
	*pPos++ = 0.0;

	*pPos++ = minx;
	*pPos++ = miny;
	*pPos++ = 0.0;

	//line 4 (direction pointer)
	*pPos++ = 0.0;
	*pPos++ = 0.0;
	*pPos++ = 0.0;

	*pPos++ = 0.0;
	*pPos++ = 0.0;
	*pPos++ = 0.5;

	vbuf->unlock();
	// setup the bounding box of this SimpleRenderable
	Ogre::AxisAlignedBox aab(Ogre::Vector3(minx,miny,minz),Ogre::Vector3(maxx,maxy,maxz));
	setBoundingBox(aab);
}
//----------------------------------------------------------------------------------------
Ogre::Real PortalOutlineRenderable::getBoundingRadius()const
{
	return 0;
}
//----------------------------------------------------------------------------------------
Ogre::Real PortalOutlineRenderable::getSquaredViewDepth(const Ogre::Camera* cam)const
{
	Ogre::Vector3 min, max, mid, dist;
	min = mBox.getMinimum();
	max = mBox.getMaximum();
	mid = ((max - min) * 0.5) + min;
	dist = cam->getDerivedPosition() - mid;
	return dist.squaredLength();
}
//----------------------------------------------------------------------------------------
 void PortalOutlineRenderable::createPortalMaterials(void)
 {
	Ogre::ColourValue colour = Ogre::ColourValue(0,1,0);//
	Ogre::String matname = "PortalOutlineMaterial";

	Ogre::ResourceManager::ResourceCreateOrRetrieveResult result = Ogre::MaterialManager::getSingleton().createOrRetrieve(matname, "General");
	if(result.second)
	{
		Ogre::MaterialPtr freePortalMaterial = result.first;
	    freePortalMaterial->setReceiveShadows(false);
	    freePortalMaterial->getTechnique(0)->setLightingEnabled(true);
	    freePortalMaterial->getTechnique(0)->getPass(0)->setDiffuse(colour);
	    freePortalMaterial->getTechnique(0)->getPass(0)->setAmbient(colour);
	    freePortalMaterial->getTechnique(0)->getPass(0)->setSelfIllumination(colour);
	}

	colour = Ogre::ColourValue(1,1,0);//Yellow
	matname = "PortalOutlineMaterialConnected";
	result = Ogre::MaterialManager::getSingleton().createOrRetrieve(matname, "General");
	if(result.second)
	{
		Ogre::MaterialPtr connectedPortalMaterial = result.first;
	    connectedPortalMaterial->setReceiveShadows(false);
	    connectedPortalMaterial->getTechnique(0)->setLightingEnabled(true);
	    connectedPortalMaterial->getTechnique(0)->getPass(0)->setDiffuse(colour);
	    connectedPortalMaterial->getTechnique(0)->getPass(0)->setAmbient(colour);
	    connectedPortalMaterial->getTechnique(0)->getPass(0)->setSelfIllumination(colour);
	}

	colour = Ogre::ColourValue(1,0,0);//Red
	matname = "PortalOutlineMaterialLinked";
	result = Ogre::MaterialManager::getSingleton().createOrRetrieve(matname, "General");
	if(result.second)
	{
		Ogre::MaterialPtr linkedPortalMaterial = result.first;
	    linkedPortalMaterial->setReceiveShadows(false);
	    linkedPortalMaterial->getTechnique(0)->setLightingEnabled(true);
	    linkedPortalMaterial->getTechnique(0)->getPass(0)->setDiffuse(colour);
	    linkedPortalMaterial->getTechnique(0)->getPass(0)->setAmbient(colour);
	    linkedPortalMaterial->getTechnique(0)->getPass(0)->setSelfIllumination(colour);
	}

 }

//----------------------------------------------------------------------------------------
 void PortalOutlineRenderable::setPortalState(PortalState state)
 {
	 switch(state)
	 {
	 case PS_FREE:this->setMaterial("PortalOutlineMaterial");break;
	 case PS_CONNECTED:this->setMaterial("PortalOutlineMaterialConnected");break;
	 case PS_LINKED:this->setMaterial("PortalOutlineMaterialLinked");break;
	 }

 }