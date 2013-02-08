/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2012 Andrew Fenn <andrewfenn@gmail.com> and the Ogitor Team
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

#include "OgitorsPrerequisites.h"
#include <QtGui/QtGui>

//----------------------------------------------------------------------------------------

#if defined( __WIN32__ ) || defined( _WIN32 )
   #ifdef IMAGECONVERTER_EXPORT
     #define ICExport __declspec (dllexport)
   #else
     #define ICExport __declspec (dllimport)
   #endif
#else
   #define ICExport
#endif

//----------------------------------------------------------------------------------------

class ICExport ImageConverter 
{
public:
    ImageConverter(const size_t& width=128, const size_t& height=128);

    virtual ~ImageConverter();

    QImage fromOgreImage(const Ogre::Image& image);
    QImage fromOgreImageName(const Ogre::String& name, const Ogre::String& resourceGroup);
    QImage fromOgreMaterialName(const Ogre::String& name, const Ogre::String& resourceGroup);

protected:
    QImage _getRenderTarget(const Ogre::String& matName);
    QImage _imageFromRenderTarget(const Ogre::Image& img);

    Ogre::String mResourceGroup;
    Ogre::ResourceGroupManager *mResourceManager;

    Ogre::RenderTexture *mRttTex;

    Ogre::SceneManager *mSceneMgrPtr;
    Ogre::Entity* mEntityTerrain;

    size_t mHeight;
    size_t mWidth;
};

//----------------------------------------------------------------------------------------
