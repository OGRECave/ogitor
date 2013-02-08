/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2013 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
///
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

#pragma once

#include "OgrePrerequisites.h"
#include "OgreSimpleRenderable.h"

namespace Ogitors
{
    class Selection2D : public Ogre::SimpleRenderable
    {
    protected:
        /** Override this method to prevent parent transforms (rotation,translation,scale)
        */
        void getWorldTransforms( Ogre::Matrix4* xform ) const;

    public:

        Selection2D(bool includeTextureCoordinates = false);
        ~Selection2D();

        /** Sets the corners of the rectangle, in relative coordinates.
        @param
        left Left position in screen relative coordinates, -1 = left edge, 1.0 = right edge
        @param top Top position in screen relative coordinates, 1 = top edge, -1 = bottom edge
        @param right Right position in screen relative coordinates
        @param bottom Bottom position in screen relative coordinates
        @param updateAABB Tells if you want to recalculate the AABB according to 
        the new corners. If false, the axis aligned bounding box will remain identical.
        */
        void setCorners(Ogre::Real left, Ogre::Real top, Ogre::Real right, Ogre::Real bottom, bool updateAABB = true);

        Ogre::Real getSquaredViewDepth(const Ogre::Camera* cam) const
        { (void)cam; return 0; }

        Ogre::Real getBoundingRadius(void) const { return 0; }

    };

}// namespace


