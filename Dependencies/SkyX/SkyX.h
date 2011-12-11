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

#ifndef _SkyX_SkyX_H_
#define _SkyX_SkyX_H_

#include "Prerequisites.h"

#include "MeshManager.h"
#include "AtmosphereManager.h"
#include "GPUManager.h"
#include "MoonManager.h"
#include "CloudsManager.h"
#include "VCloudsManager.h"
#include "VClouds/VClouds.h"
#include "ColorGradient.h"

namespace SkyX
{
	/** Main SkyX class
	    Create simple and beautiful skies!
     */
    class DllExport SkyX 
	{
	public:
		/** Lighting mode enumeration
		    SkyX is designed for true HDR rendering, but there're a lot of applications
			that doesn't use HDR rendering, due to this a little exponential tone-mapping 
			algoritm is applied to SkyX materials if LM_LDR is selected. (See: AtmosphereManager::Options::Exposure)
			Select LM_HDR if your app is designed for true HDR rendering.
		 */
		enum LightingMode
		{
			/// Low dynamic range
			LM_LDR = 0,
			/// High dynamic range
			LM_HDR = 1
		};

		/** Contructor 
		    @param sm Ogre Scene manager pointer
		    @param c Ogre Camera pointer
		 */
		SkyX(Ogre::SceneManager* sm, Ogre::Camera *c);

		/** Destructor 
		 */
		~SkyX();

		/** Create SkyX
         */
        void create();

		/** Remove SkyX, you can call this method to remove SkyX from the scene
		    or release (secondary) SkyX memory, call create() to return SkyX to the scene.
		 */
		void remove();

        /** Call every frame
		    @param timeSinceLastFrame Time elapsed since last frame
            @todo Add listener interface
         */
        void update(const Ogre::Real &timeSinceLastFrame);

		/** Is SkyX created?
		    @return true if yes, false if not
		 */
		inline const bool isCreated() const
		{
			return mCreated;
		}

		/** Set time multiplier
		    @param TimeMultiplier Time multiplier
			@remarks The time multiplier can be a negative number, 0 will disable auto-updating
			         For setting a custom time of day, check: AtmosphereManager::Options::Time
		 */
		inline void setTimeMultiplier(const Ogre::Real& TimeMultiplier)
		{
			mTimeMultiplier = TimeMultiplier;
			mVCloudsManager->_updateWindSpeedConfig();
		}

		/** Get time multiplier
		    @return Time multiplier
		 */
		inline const Ogre::Real& getTimeMultiplier() const
		{
			return mTimeMultiplier;
		}

		/** Get time offset
		    @return Time offset
			@remarks Only for internal use
		 */
		inline const Ogre::Real& _getTimeOffset() const
		{
			return mTimeOffset;
		}

		/** Get mesh manager
		    @return Mesh manager pointer
		 */
		inline MeshManager* getMeshManager()
		{
			return mMeshManager;
		}

		/** Get atmosphere manager
		    @return Atmosphere manager pointer
		 */
		inline AtmosphereManager* getAtmosphereManager()
		{
			return mAtmosphereManager;
		}

		/** Get GPU manager
		    @return Atmosphere manager pointer
		 */
		inline GPUManager* getGPUManager()
		{
			return mGPUManager;
		}

		/** Get moon manager
		    @return Moon manager
		 */
		inline MoonManager* getMoonManager()
		{
			return mMoonManager;
		}

		/** Get clouds manager
		    @return Clouds manager
		 */
		inline CloudsManager* getCloudsManager()
		{
			return mCloudsManager;
		}

		/** Get volumetric clouds manager
		    @return Volumetric clouds manager
		 */
		inline VCloudsManager* getVCloudsManager()
		{
			return mVCloudsManager;
		}

		/** Set lighting mode
		    @param lm Lighting mode
			@remarks SkyX is designed for true HDR rendering, but there're a lot of applications
			that doesn't use HDR rendering, due to this a little exponential tone-mapping 
			algoritm is applied to SkyX materials if LM_LDR is selected. (See: AtmosphereManager::Options::Exposure)
			Select LM_HDR if your app is designed for true HDR rendering.
		 */
		void setLightingMode(const LightingMode& lm);

		/** Get lighting mode
		    @return Lighting mode
		 */
		inline const LightingMode& getLightingMode() const
		{
			return mLightingMode;
		}

		/** Set the starfield enabled/disabled
		    @param Enabled true for starfield, false for not
		 */
		void setStarfieldEnabled(const bool& Enabled);

		/** Is the starfield enable?
		    @return true if the starfield is enabled, false if it isn't
		 */
		inline const bool& isStarfieldEnabled() const
		{
			return mStarfield;
		}

		/** Get scene manager
		    @return Ogre::SceneManager pointer
		 */
		inline Ogre::SceneManager* getSceneManager()
		{
			return mSceneManager;
		}

		/** Get camera
		    @return SkyX camera
		 */
		inline Ogre::Camera* getCamera()
		{
            return mCamera;
		}

	private:
		/// Is SkyX created?
		bool mCreated;

		/// Mesh manager
		MeshManager* mMeshManager;
		/// Atmosphere manager
		AtmosphereManager* mAtmosphereManager;
		/// GPU manager
		GPUManager* mGPUManager;
		/// Moon manager
		MoonManager* mMoonManager;
		/// Clouds manager
		CloudsManager* mCloudsManager;
		/// Volumetric clouds manager
		VCloudsManager* mVCloudsManager;

		/// Pointer to Ogre::SceneManager
        Ogre::SceneManager *mSceneManager;
		/// Main ogre camera
		Ogre::Camera* mCamera;

		/// Last camera position
		Ogre::Vector3 mLastCameraPosition;
		/// Last camera far clip distance
		Ogre::Real mLastCameraFarClipDistance;

		/// Lighting mode
		LightingMode mLightingMode;
		/// Enable starfield?
		bool mStarfield;

		/// Time multiplier
		Ogre::Real mTimeMultiplier;
		/// Time offset
		Ogre::Real mTimeOffset;
	};
}

#endif