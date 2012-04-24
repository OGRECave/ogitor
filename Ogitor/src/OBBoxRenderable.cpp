// This Class is taken from OGRE Wiki
#include "OgitorsPrerequisites.h"
#include "Ogre.h"
#include "OBBoxRenderable.h"

using namespace Ogre;
using namespace Ogitors;


OBBoxRenderable::OBBoxRenderable()
{
    mRenderOp.vertexData = OGRE_NEW Ogre::VertexData();
    mRenderOp.indexData = 0;
    mRenderOp.vertexData->vertexCount = 48;
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
    this->setCastShadows(false);
    this->setQueryFlags(0); // set a query flag to exlude from queries (if necessary).
}
    
OBBoxRenderable::~OBBoxRenderable()
{
    OGRE_DELETE mRenderOp.vertexData;
}
    
void OBBoxRenderable::setupVertices(const Ogre::AxisAlignedBox& aab)
{
    Ogre::Vector3 vmax = aab.getMaximum();
    Ogre::Vector3 vmin = aab.getMinimum();
    Ogre::Real maxdistground = vmax.y;
    Ogre::Real mindistground = vmin.y;
    Ogre::Real maxx = vmax.x;
    Ogre::Real maxy = vmax.y;
    Ogre::Real maxz = vmax.z;
    Ogre::Real minx = vmin.x;
    Ogre::Real miny = vmin.y;
    Ogre::Real minz = vmin.z;

    Ogre::Real xdiff = (vmax.x - vmin.x) / 4.0f;
    Ogre::Real ydiff = (vmax.y - vmin.y) / 4.0f;
    Ogre::Real zdiff = (vmax.z - vmin.z) / 4.0f;
  

    // fill in the Vertex buffer: 12 lines with 2 endpoints each make up a box
    Ogre::HardwareVertexBufferSharedPtr vbuf = mRenderOp.vertexData->vertexBufferBinding->getBuffer(0);
    float* pPos = static_cast<float*>(vbuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));
    // line 0
    *pPos++ = minx;
    *pPos++ = miny;
    *pPos++ = minz;

    *pPos++ = minx + xdiff;
    *pPos++ = miny;
    *pPos++ = minz;

    *pPos++ = maxx - xdiff;
    *pPos++ = miny;
    *pPos++ = minz;

    *pPos++ = maxx;
    *pPos++ = miny;
    *pPos++ = minz;
    // line 1
    *pPos++ = minx;
    *pPos++ = miny;
    *pPos++ = minz;

    *pPos++ = minx;
    *pPos++ = miny;
    *pPos++ = minz + zdiff;

    *pPos++ = minx;
    *pPos++ = miny;
    *pPos++ = maxz - zdiff;

    *pPos++ = minx;
    *pPos++ = miny;
    *pPos++ = maxz;
    // line 2
    *pPos++ = minx;
    *pPos++ = miny;
    *pPos++ = minz;

    *pPos++ = minx;
    *pPos++ = miny + ydiff;
    *pPos++ = minz;

    *pPos++ = minx;
    *pPos++ = maxy - ydiff;
    *pPos++ = minz;

    *pPos++ = minx;
    *pPos++ = maxy;
    *pPos++ = minz;
    // line 3
    *pPos++ = minx;
    *pPos++ = maxy;
    *pPos++ = minz;

    *pPos++ = minx;
    *pPos++ = maxy;
    *pPos++ = minz + zdiff;

    *pPos++ = minx;
    *pPos++ = maxy;
    *pPos++ = maxz - zdiff;

    *pPos++ = minx;
    *pPos++ = maxy;
    *pPos++ = maxz;
    // line 4
    *pPos++ = minx;
    *pPos++ = maxy;
    *pPos++ = minz;

    *pPos++ = minx + xdiff;
    *pPos++ = maxy;
    *pPos++ = minz;

    *pPos++ = maxx - zdiff;
    *pPos++ = maxy;
    *pPos++ = minz;

    *pPos++ = maxx;
    *pPos++ = maxy;
    *pPos++ = minz;
    // line 5
    *pPos++ = maxx;
    *pPos++ = miny;
    *pPos++ = minz;

    *pPos++ = maxx;
    *pPos++ = miny;
    *pPos++ = minz + zdiff;

    *pPos++ = maxx;
    *pPos++ = miny;
    *pPos++ = maxz - zdiff;

    *pPos++ = maxx;
    *pPos++ = miny;
    *pPos++ = maxz;
    // line 6
    *pPos++ = maxx;
    *pPos++ = miny;
    *pPos++ = minz;

    *pPos++ = maxx;
    *pPos++ = miny + ydiff;
    *pPos++ = minz;

    *pPos++ = maxx;
    *pPos++ = maxy - ydiff;
    *pPos++ = minz;

    *pPos++ = maxx;
    *pPos++ = maxy;
    *pPos++ = minz;
    // line 7
    *pPos++ = minx;
    *pPos++ = maxy;
    *pPos++ = maxz;

    *pPos++ = minx + xdiff;
    *pPos++ = maxy;
    *pPos++ = maxz;

    *pPos++ = maxx - xdiff;
    *pPos++ = maxy;
    *pPos++ = maxz;

    *pPos++ = maxx;
    *pPos++ = maxy;
    *pPos++ = maxz;
    // line 8
    *pPos++ = minx;
    *pPos++ = miny;
    *pPos++ = maxz;

    *pPos++ = minx;
    *pPos++ = miny + ydiff;
    *pPos++ = maxz;

    *pPos++ = minx;
    *pPos++ = maxy - ydiff;
    *pPos++ = maxz;

    *pPos++ = minx;
    *pPos++ = maxy;
    *pPos++ = maxz;

    // line 9
    *pPos++ = maxx;
    *pPos++ = maxy;
    *pPos++ = minz;

    *pPos++ = maxx;
    *pPos++ = maxy;
    *pPos++ = minz + zdiff;

    *pPos++ = maxx;
    *pPos++ = maxy;
    *pPos++ = maxz - zdiff;

    *pPos++ = maxx;
    *pPos++ = maxy;
    *pPos++ = maxz;
    // line 10
    *pPos++ = maxx;
    *pPos++ = miny;
    *pPos++ = maxz;

    *pPos++ = maxx;
    *pPos++ = miny + ydiff;
    *pPos++ = maxz;

    *pPos++ = maxx;
    *pPos++ = maxy - ydiff;
    *pPos++ = maxz;

    *pPos++ = maxx;
    *pPos++ = maxy;
    *pPos++ = maxz;
    // line 11
    *pPos++ = minx;
    *pPos++ = miny;
    *pPos++ = maxz;

    *pPos++ = minx + xdiff;
    *pPos++ = miny;
    *pPos++ = maxz;

    *pPos++ = maxx - xdiff;
    *pPos++ = miny;
    *pPos++ = maxz;

    *pPos++ = maxx;
    *pPos++ = miny;
    *pPos++ = maxz;
    vbuf->unlock();
    // setup the bounding box of this SimpleRenderable
    setBoundingBox(aab);
}
    
Ogre::Real OBBoxRenderable::getSquaredViewDepth(const Ogre::Camera* cam)const
{
    Ogre::Vector3 min, max, mid, dist;
    min = mBox.getMinimum();
    max = mBox.getMaximum();
    mid = ((max - min) * 0.5) + min;
    dist = cam->getDerivedPosition() - mid;
    return dist.squaredLength();
}
    
Ogre::Real OBBoxRenderable::getBoundingRadius()const
{
    return 0;
}
    
void OBBoxRenderable::getWorldTransforms (Ogre::Matrix4 *xform)const
{
    SimpleRenderable::getWorldTransforms (xform);
}