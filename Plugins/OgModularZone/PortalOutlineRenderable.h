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

#ifndef PORTAL_OUTLINE_RENDERABLE_H
#define PORTAL_OUTLINE_RENDERABLE_H

#include "OgreSimpleRenderable.h"
#include "OgreTechnique.h"

class PortalOutlineRenderable : public Ogre::SimpleRenderable
{
public:
    enum PortalState
    {
        PS_FREE,
        PS_CONNECTED,
        PS_LINKED
    };

    /**
    * Constructor
    * @param Material name for the box
    * @param colour colour of the box
    */
    PortalOutlineRenderable(Ogre::String matname = "PortalOutlineMaterial", Ogre::ColourValue colour = Ogre::ColourValue(1,1,0));//Yellow. Oh Yeahhhh
    /**
    * Destructor
    */
    virtual ~PortalOutlineRenderable();
    /**
    * Sets up object-oriented box using standard AABB
    * @param aab AAB box
    */
    void setupVertices(Ogre::Real width = 2.0, Ogre::Real height = 2.0);

    /**
    * Fetches calculated bounding sphere radius
    * @return bounding sphere radius
    */
    Ogre::Real getBoundingRadius()const;

    /**
    * Fetches squared view depth
    * @param camera handle to current camera
    * @return squared view depth
    */    
    Ogre::Real getSquaredViewDepth(const Ogre::Camera* cam)const ;
    
    void setPortalState(PortalState state);

protected:
    virtual void createPortalMaterials(void);

    Ogre::VertexData vertexes;  /** portal outline's vertices */
};

#endif