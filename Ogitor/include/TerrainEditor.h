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
    /** Edit mode for terrain */
    enum EditMode
    {
        EM_NONE = 0,    /** No modification will occur */
        EM_DEFORM,      /** Terrain area will be deformed */
        EM_SPLAT,       /** Terrain area will receive splatting decal */
        EM_PAINT,       /** Terrain area will be painted on with color */
        EM_STAMP,       /** Terrain area will receive "stamp-out" mesh/entity */
        EM_SMOOTH,      /** Terrain area will be smoothed */
        EM_SPLATGRASS   /** Terrain area will be painted with grass */
    };

    typedef Ogre::Real (PGHeightFunction)(Ogre::Real x, Ogre::Real z, void *userData);

    //! Terrain editor class
    /*!  
    A class that handles terrain(s) editing
    */
    class OgitorExport ITerrainEditor
    {
    public:
        /**
        * Constructor
        */
        ITerrainEditor() {};
        /**
        * Destructor
        */
        virtual ~ITerrainEditor() {};
        /**
        * Fetches "selection" flag
        * @return true if terrain is selected, otherwise false
        */
        virtual bool isSelected() = 0;
        /**
        * Is terrain doing any background calculation at the moment?
        */
        virtual bool isBackgroundProcessActive() = 0;
        /**
        * Tests if terrain mesh was hit with a ray, and if so, calculates the location and distance to the point of intersection
        * @param camRay ray to test intersection against
        * @param result point of intersection
        * @return true if intersection occured, otherwise false
        */
        virtual bool hitTest(Ogre::Ray camRay, Ogre::Vector3 *result = NULL) = 0;
        /**
        * Exports the parameters to a dotscene file
        * @param output the stream to output
        * @param indent the indentation at the beginning
        */    
        virtual TiXmlElement* exportDotScene(TiXmlElement *pParent) { return NULL; };
        /**
        * Fetches currently assigned height localization function pointer
        * @return assigned height localization function pointer
        */
        virtual PGHeightFunction *getHeightFunction() = 0;
        /**
        * Fetches AAB box for the terrain
        * @return AAB box for the terrain
        */
        virtual Ogre::AxisAlignedBox getSize() = 0;
        /**
        * Fetches material name(s) used for terrain/splatting/etc
        * @return material name(s) used for terrain/splatting/etc
        */
        virtual Ogre::StringVector   getMaterialNames() = 0;
        /**
        * Sets mouse position using camera ray
        * @param camRay camera view direction
        */
        virtual void setMousePosition(Ogre::Ray camRay) = 0;
        /**
        * Sets current brush name
        * @param brush new brush name
        */
        virtual void setBrushName(const std::string& brush) = 0;
        virtual const Ogre::String& getBrushName() = 0;
        /**
        * Sets current brush size
        * @param size new brush size
        */
        virtual void setBrushSize(unsigned int size) = 0;
        virtual int  getBrushSize() = 0;
        /**
        * Sets current brush orientation
        * @param orient new brush orientation
        */
        virtual void setBrushOrientation(Ogre::Radian orient) = 0;
        /**
        * Sets current brush intensity
        * @param intensity new brush intensity
        */
        virtual void setBrushIntensity(float intensity) = 0;
        /**
        * Sets current editing mode
        * @param mode new editing mode
        */
        virtual void setEditMode(unsigned int mode) = 0;
        /**
        * Sets current splatting texture name
        * @param texture new splatting texture name
        */
        virtual void setTexture(const Ogre::String& texture) = 0;
        /**
        * Sets current grass texture name
        * @param texture new splatting texture name
        */
        virtual void setGrassTexture(const Ogre::String& texture) = 0;
        /**
        * Sets current painting colour
        * @param colour new painting colour
        */
        virtual void setColour(const Ogre::ColourValue& colour) = 0;
        /**
        * Sets current editing direction (up/down)
        * @param colour new editing direction (up/down)
        */
        virtual void setEditDirection(bool inverse) = 0;
        /**
        * Begins editing operation
        */
        virtual void startEdit() = 0;
        /**
        * Commits editing operation
        */
        virtual void stopEdit() = 0;
        /**
        * Tests if terrain area can be painted on
        * @return true if terrain area can be painted on, otherwise false
        */
        virtual bool canPaint() = 0;
        /**
        * Tests if terrain area can be splatted on
        * @return true if terrain area can be splatted on, otherwise false
        */
        virtual bool canSplat() = 0;
        /**
        * Tests if terrain area can be deformed
        * @return true if terrain area can be deformed, otherwise false
        */
        virtual bool canDeform() = 0;
        /**
        * Fetches current editing mode
        * @return current editing mode
        */
        virtual unsigned int getEditMode() {return (unsigned int)mEditMode;};
        /**
        * Re-Calculates Lighting
        */
        virtual void recalculateLighting() = 0;
    protected:
        Ogre::Real           *mBrush;               /** Brush index handle */
        unsigned int          mBrushSize;           /** Brush size */
        float                 mBrushIntensity;      /** Brush intensity */
        Ogre::Radian          mBrushOrient;         /** Brush orientation */
        Ogre::String          mBrushName;           /** Brush name */
        Ogre::Vector3         mEditPos;             /** Editing position */
        EditMode              mEditMode;            /** Editing mode */
        bool                  mEditActive;          /** Is editing active flag */
        Ogre::String          mTextureGrass;        /** Grass texture(material) name */
        Ogre::String          mTextureDiffuse;      /** Diffuse texture name */
        Ogre::String          mTextureNormal;       /** Normal texture name */
        Ogre::ColourValue     mColour;              /** Painting colour */
        bool                  mEditDirection;       /** Editing direction (up/down) */
    };
};
