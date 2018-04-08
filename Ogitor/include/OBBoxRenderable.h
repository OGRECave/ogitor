// This Class is taken from OGRE Wiki

#pragma once

#include <OgreSimpleRenderable.h>
#include <OgreAxisAlignedBox.h>

namespace Ogitors
{
    //! Object-oriented box class
    /*!  
    An object-oriented box class implementation that is not present in OGRE
    */
    class OBBoxRenderable : public Ogre::SimpleRenderable
    {
    private:
        Ogre::VertexData vertexes;  /** Box' vertices */

    public:
        /**
        * Constructor
        * @param Material name for the box
        * @param colour colour of the box
        */
        OBBoxRenderable();
        /**
        * Destructor
        */
        virtual ~OBBoxRenderable();
        /**
        * Sets up object-oriented box using standard AABB
        * @param aab AAB box
        */
        void setupVertices(const Ogre::AxisAlignedBox& aab);
        /**
        * Fetches squared view depth
        * @param camera handle to current camera
        * @return squared view depth
        */    
        Ogre::Real getSquaredViewDepth(const Ogre::Camera* camera)const;
        /**
        * Fetches calculated bounding sphere radius
        * @return bounding sphere radius
        */
        Ogre::Real getBoundingRadius()const;
        /**
        * Fetches world transformation matrix
        * @param xform handle to matrix into which to put world transformation
        */
        virtual void getWorldTransforms (Ogre::Matrix4 *xform)const;

        using Ogre::SimpleRenderable::setMaterial;

        void setMaterial(const Ogre::String& mat)
        {
            setMaterial(Ogre::MaterialManager::getSingleton().getByName(mat));
        }
    };
}
