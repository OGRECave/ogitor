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

#ifndef _SkyX_MeshManager_H_
#define _SkyX_MeshManager_H_

#include "Prerequisites.h"

namespace SkyX
{
	class SkyX;

    class DllExport MeshManager 
	{
	public:
		/** Vertex struct for position and tex coords data.
		 */
		struct POS_UV_VERTEX
		{
			      // Position
			float x, y, z,
			      // Normalized position
			      nx, ny, nz,
				  // Texture coords
			      u, v,
				  // Opacity
				  o;
		};

		/** Constructor
		    @param s Parent SkyX pointer
		 */
		MeshManager(SkyX *s);

		/** Destructor 
		 */
		~MeshManager();

		/** Create our water mesh, geometry, entity, etc...
            @remarks Call it after setMaterialName()
         */
        void create();

		/** Remove all resources
		 */
		void remove();

		/** Update geometry
		 */
		void _updateGeometry();

		/** Get mesh
            @return Mesh
         */
        inline Ogre::MeshPtr getMesh()
        {
            return mMesh;
        }

        /** Get sub mesh
            @return Sub mesh
         */
        inline Ogre::SubMesh* getSubMesh()
        {
            return mSubMesh;
        }

        /** Get entity
            @return Entity
         */
        inline Ogre::Entity* getEntity()
        {
            return mEntity;
        }

		/** Set skydome fading parameters
		    @param SmoothSkydomeFading true for smooth fading, false for non-smooth fading
			@param SkydomeFadingPercent Fading region (0 = none, 1 = Skydome radius, default = 0.05)
		 */
		inline void setSkydomeFadingParameters(const bool& SmoothSkydomeFading, const Ogre::Real& SkydomeFadingPercent = 0.05f)
		{
			mSmoothSkydomeFading = SmoothSkydomeFading;
			mSkydomeFadingPercent = SkydomeFadingPercent;
		}

		/** Get smooth skydome fading
		    @return true means smooth fading, false non-smooth fading
		 */
		inline const bool& getSmoothSkydomeFading() const
		{
			return mSmoothSkydomeFading;
		}

		/** Get skydome fading percent
		    @return Fading region (0 = none, 1 = Skydome radius, default = 0.05)
		 */
		inline const Ogre::Real& getSkydomeFadingPercent() const
		{
			return mSkydomeFadingPercent;
		}

        /** Get material name
            @return Material name
         */
        inline const Ogre::String& getMaterialName() const
        {
            return mMaterialName;
        }

		/** Set mesh material
            @param MaterialName The material name
         */
        void setMaterialName(const Ogre::String &MaterialName);

		/** Get hardware vertex buffer reference
            @return Ogre::HardwareVertexBufferSharedPtr reference
         */
        inline Ogre::HardwareVertexBufferSharedPtr &getHardwareVertexBuffer()
        {
            return mVertexBuffer;
        }

		/** Get hardware index buffer reference
		    @return Ogre::HardwareIndexBufferSharedPtr reference
		 */
		inline Ogre::HardwareIndexBufferSharedPtr &getHardwareIndexBuffer()
		{
			return mIndexBuffer;
		}

		/** Get the Ogre::SceneNode pointer where Hydrax mesh is attached
		    @return Ogre::SceneNode*
		 */
		inline Ogre::SceneNode* getSceneNode()
		{
			return mSceneNode;
		}

		/** Is _createGeometry() called?
		    @return true if created() have been already called
		 */
		inline const bool& isCreated() const
		{
			return mCreated;
		}

		/** Set geometry parameters
		    @param Steps Sphere number of steps
			@param Circles Spehere number of circes
		 */
		void _setGeometryParameters(const int& Steps, const int& Circles);

		/** Get number of steps
		    @return Number of steps
		 */
		inline const int& getSteps() const
		{
			return mSteps;
		}

		/** Get number of circles
		    @return Number of circles
		 */
		inline const int& getCircles() const
		{
			return mCircles;
		}

		/** Get skydome radius
		    @return Skydome radius
		 */
		const float getSkydomeRadius() const;

	private:
		/** Create geometry
		 */
		void _createGeometry();

		/// Has been create() already called?
		bool mCreated;

		/// Ogre::MeshPtr
        Ogre::MeshPtr mMesh;
        /// Ogre::Submesh pointer
        Ogre::SubMesh *mSubMesh;
        /// Ogre::Entity pointer
        Ogre::Entity *mEntity;

        /// Vertex buffer
        Ogre::HardwareVertexBufferSharedPtr mVertexBuffer;
        /// Index buffer
        Ogre::HardwareIndexBufferSharedPtr  mIndexBuffer;

		/// Vertices
		POS_UV_VERTEX* mVertices;

		/// Circles
		int mCircles;
		/// Steps
		int mSteps;

		/// Smooth skydome fading
		bool mSmoothSkydomeFading;
		/// Skydome fading perfcent
		Ogre::Real mSkydomeFadingPercent;

		/// Ogre::SceneNode pointer
		Ogre::SceneNode* mSceneNode;

        /// Material name
        Ogre::String mMaterialName;

		/// Main SkyX pointer
		SkyX* mSkyX;
	};
}

#endif