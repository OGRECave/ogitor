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

#ifndef _SkyX_VClouds_GeometryManager_H_
#define _SkyX_VClouds_GeometryManager_H_

#include "../Prerequisites.h"

#include "GeometryBlock.h"

namespace SkyX { namespace VClouds{

	class VClouds;

	class DllExport GeometryManager
	{
	public:
		/** Constructor
			@param vc VClouds pointer
			@param Height x = Altitude over the camera, y: Field height (both in world coordinates)
			@param Radius Radius
			@param Alpha Alpha angle
			@param Beta Beta angle
			@param NumberOfBlocks Number of geometry blocks
			@param Na Number of slices in A zone
			@param Nb Number of slices in B zone
			@param Nc Number of slices in C zone
		 */
		GeometryManager(VClouds* vc,
			const Ogre::Vector2& Height, const float& Radius,
			const Ogre::Radian& Alpha, const Ogre::Radian& Beta, 
            const int& NumberOfBlocks, const int& Na, const int& Nb, const int& Nc);

		/** Destructor
		 */
		~GeometryManager();

		/** Create
		 */
		void create();

		/** Remove
		 */
		void remove();

		/** Call every frame
		    @param timeSinceLastFrame Time since last frame
         */
        void update(const Ogre::Real& timeSinceLastFrame);

		/** Has been create() already called?
		    @return true if created() have been already called, false if not
		 */
		inline const bool& isCreated() const
		{
			return mCreated;
		}

	private:
		/** Create geometry
		 */
		void _createGeometry();

		/** Update geometry
		    @param timeSinceLastFrame Time since last frame
		 */
		void _updateGeometry(const Ogre::Real& timeSinceLastFrame);

		/// Has been create() already called?
		bool mCreated;

		/// Height: x = Altitude over the camera, y: Field height (both in world coordinates)
		Ogre::Vector2 mHeight;
		/// Angles
		Ogre::Radian mAlpha, mBeta;
		/// Radius
		float mRadius;
		/// Azimutal angle per block
		Ogre::Radian mPhi;
		/// Number of blocks
		int mNumberOfBlocks;
		/// Number of slices per geometry zone
		int mNa, mNb, mNc;

		/// World coords offset
		Ogre::Vector2 mWorldOffset;

		/// Geometry blocks
		std::vector<GeometryBlock*> mGeometryBlocks;

		/// Scene node
		Ogre::SceneNode *mSceneNode;

		/// Last camera position
		Ogre::Vector3 mLastCameraPosition;

		/// VClouds pointer
		VClouds *mVClouds;
	};


}}

#endif