/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2015 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
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

namespace Ogitors
{
    class TerrainColourUndo : public OgitorsUndoBase
    {
    public:
        TerrainColourUndo(unsigned int objectID, Ogre::Rect area, Ogre::ColourValue *data);
        virtual ~TerrainColourUndo();
        virtual bool apply();

    protected:
        unsigned int  mObjectID;
        Ogre::Rect    mAreaOfEffect;
        Ogre::String  mFileName;
    };
//-----------------------------------------------------------------------------------------    
    class TerrainHeightUndo : public OgitorsUndoBase
    {
    public:
        TerrainHeightUndo(unsigned int objectID, Ogre::Rect area, float *data);
        virtual ~TerrainHeightUndo();
        virtual bool apply();

    protected:
        unsigned int  mObjectID;
        Ogre::Rect    mAreaOfEffect;
        Ogre::String  mFileName;
    };
//-----------------------------------------------------------------------------------------    
    class TerrainBlendUndo : public OgitorsUndoBase
    {
    public:
        TerrainBlendUndo(unsigned int objectID, Ogre::Rect area, int blendStart, int blendCount, float *data);
        virtual ~TerrainBlendUndo();
        virtual bool apply();

    protected:
        unsigned int  mObjectID;
        Ogre::Rect    mAreaOfEffect;
        Ogre::String  mFileName;
        int           mBlendStart;
        int           mBlendCount;
    };
//-----------------------------------------------------------------------------------------       
    class TerrainLayerUndo : public OgitorsUndoBase
    {
    public:
        enum LayerUndoType
        {
            LU_CREATE = 0,
            LU_DELETE = 1,
            LU_MODIFY = 2
        };

        TerrainLayerUndo(unsigned int objectID, int layerID, LayerUndoType type, const Ogre::String& diffuse = "", const Ogre::String& normal = "", Ogre::Real worldSize = 20.0f);
        virtual bool apply();

    protected:
        unsigned int  mObjectID;
        int           mLayerID;
        LayerUndoType mType;
        Ogre::String  mDiffuse;
        Ogre::String  mNormal;
        Ogre::Real    mWorldSize;
    };
//-----------------------------------------------------------------------------------------    
    class TerrainGrassUndo : public OgitorsUndoBase
    {
    public:
        TerrainGrassUndo(unsigned int objectID, Ogre::Rect area, float *data);
        virtual ~TerrainGrassUndo();
        virtual bool apply();

    protected:
        unsigned int  mObjectID;
        Ogre::Rect    mAreaOfEffect;
        Ogre::String  mFileName;
    };
//-----------------------------------------------------------------------------------------

}
