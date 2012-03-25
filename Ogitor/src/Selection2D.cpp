/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2012 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
//
/// The MIT License
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////*/

#include "OgreStableHeaders.h"
#include "Selection2D.h"

#include "OgreSimpleRenderable.h"
#include "OgreHardwareBufferManager.h"
#include "OgreCamera.h"

namespace Ogitors 
{

    Selection2D::Selection2D(bool includeTextureCoords) 
    {
        // use identity projection and view matrices
        mUseIdentityProjection = true;
        mUseIdentityView = true;

        mRenderOp.vertexData = OGRE_NEW Ogre::VertexData();
        mRenderOp.indexData = 0;
        mRenderOp.vertexData->vertexCount = 5;
        mRenderOp.vertexData->vertexStart = 0;
        mRenderOp.operationType = Ogre::RenderOperation::OT_LINE_STRIP;
        mRenderOp.useIndexes = false;
        Ogre::VertexDeclaration* decl = mRenderOp.vertexData->vertexDeclaration;
        Ogre::VertexBufferBinding* bind = mRenderOp.vertexData->vertexBufferBinding;
        decl->addElement(0, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
        Ogre::HardwareVertexBufferSharedPtr vbuf = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
            decl->getVertexSize(0), mRenderOp.vertexData->vertexCount, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
        // Bind buffer
        bind->setBinding(0, vbuf);

        this->setCastShadows(false);
        this->setQueryFlags(0); // set a query flag to exlude from queries (if necessary).

        // set basic white material
        this->setMaterial("BaseWhiteNoLighting");
    }

    Selection2D::~Selection2D() 
    {
        OGRE_DELETE mRenderOp.vertexData;
    }

    void Selection2D::setCorners(Ogre::Real left, Ogre::Real top, Ogre::Real right, Ogre::Real bottom, bool updateAABB) 
    {
        Ogre::HardwareVertexBufferSharedPtr vbuf = 
            mRenderOp.vertexData->vertexBufferBinding->getBuffer(0);
        float* pFloat = static_cast<float*>(vbuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));

        *pFloat++ = left;
        *pFloat++ = top;
        *pFloat++ = -1;

        *pFloat++ = left;
        *pFloat++ = bottom;
        *pFloat++ = -1;

        *pFloat++ = right;
        *pFloat++ = bottom;
        *pFloat++ = -1;

        *pFloat++ = right;
        *pFloat++ = top;
        *pFloat++ = -1;

        *pFloat++ = left;
        *pFloat++ = top;
        *pFloat++ = -1;

        vbuf->unlock();

        if(updateAABB)
        {
            mBox.setExtents(
                std::min(left, right), std::min(top, bottom), 0,
                std::max(left, right), std::max(top, bottom), 0);
        }
    }

    // Override this method to prevent parent transforms (rotation,translation,scale)
    void Selection2D::getWorldTransforms( Ogre::Matrix4* xform ) const
    {
        // return identity matrix to prevent parent transforms
        *xform = Ogre::Matrix4::IDENTITY;
    }

}

