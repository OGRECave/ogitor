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

#ifndef _SkyX_VClouds_VClouds_H_
#define _SkyX_VClouds_VClouds_H_

#include "../Prerequisites.h"

#include "DataManager.h"
#include "GeometryManager.h"

namespace SkyX { namespace VClouds{

	class DllExport VClouds
	{
	public:
		/** Constructor
			@param sm Scene manager 
			@param c Camera
			@param Height x = Cloud field y-coord start, y: Field height (both in world coordinates)
			@param Radius Radius
			@param Alpha Alpha angle
			@param Beta Beta angle
			@param NumberOfBlocks Number of geometry blocks
			@param Na Number of slices in A zone
			@param Nb Number of slices in B zone
			@param Nc Number of slices in C zone
		 */
		VClouds(Ogre::SceneManager *sm, Ogre::Camera *c,
			const Ogre::Vector2& Height, const float& Radius,
			const Ogre::Radian& Alpha, const Ogre::Radian& Beta, 
            const int& NumberOfBlocks, const int& Na, const int& Nb, const int& Nc);

		/** Simple constructor
			@param sm Scene manager 
			@param c Camera
			@param Height x = Cloud field y-coord start, y: Field height (both in world coordinates)
			@param Radius Radius
		 */
		VClouds(Ogre::SceneManager *sm, Ogre::Camera *c,
			const Ogre::Vector2& Height, const float& Radius);

		/** Destructor
		 */
		~VClouds();

		/** Create
		 */
		void create();

		/** Update
		    @param timeSinceLastFrame Time since last frame
         */
        void update(const Ogre::Real& timeSinceLastFrame);

		/** Remove
		 */
		void remove();

		/** Has been create() already called?
		    @return true if created() have been already called, false if not
		 */
		inline const bool& isCreated() const
		{
			return mCreated;
		}

		/** Get height data
		    @return Height data: x = Altitude over the camera, y: Field height (both in world coordinates)
		 */
		inline const Ogre::Vector2& getHeight() const
		{
			return mHeight;
		}

		/** Get radius
		    @return Radius
		 */
		inline const float& getRadius() const
		{
			return mRadius;
		}

		/** Set wind direction
		    @param WindDirection Wind direction
		 */
		inline void setWindDirection(const Ogre::Radian& WindDirection)
		{
			mWindDirection = WindDirection;
		}

		/** Get wind direction
		    @return Wind direction
		 */
		inline const Ogre::Radian& getWindDirection() const
		{
			return mWindDirection;
		}

		/** Get wind direction as a Vector2
		    @return Wind direction
		 */
		inline const Ogre::Vector2 getWindDirectionV2() const
		{
			return Ogre::Vector2(Ogre::Math::Cos(mWindDirection), Ogre::Math::Sin(mWindDirection));
		}

		/** Set wind speed
		    @param WindSpeed Wind speed
		 */
		inline void setWindSpeed(const float& WindSpeed)
		{
			mWindSpeed = WindSpeed;
		}

		/** Get wind speed
		    @return Wind speed
		 */
		inline const float& getWindSpeed() const
		{
			return mWindSpeed;
		}

		/** Set sun direction
		    @param SunDirection Sun direction
		 */
		inline void setSunDirection(const Ogre::Vector3& SunDirection)
		{
			mSunDirection = SunDirection;
		}

		/** Get sun direction
		    @return Sun direction
		 */
		inline const Ogre::Vector3& getSunDirection() const
		{
			return mSunDirection;
		}

		/** Set sun color
		    @param SunColor Sun color
		 */
		void setSunColor(const Ogre::Vector3& SunColor);

		/** Get sun color
		    @return Sun color
		 */
		inline const Ogre::Vector3& getSunColor() const
		{
			return mSunColor;
		}

		/** Set ambient color
		    @param AmbientColor Ambient color
		 */
		void setAmbientColor(const Ogre::Vector3& AmbientColor);

		/** Get Ambient color
		    @return Ambient color
		 */
		inline const Ogre::Vector3& getAmbientColor() const
		{
			return mAmbientColor;
		}

		/** Set light response
		    @param LightResponse
				   x - Sun light power
				   y - Sun beta multiplier
				   z - Ambient color multiplier
				   w - Distance attenuation
	     */
		void setLightResponse(const Ogre::Vector4& LightResponse);

		/** Get light response
		    @return Light response
		 */
		inline const Ogre::Vector4& getLightResponse() const
		{
			return mLightResponse;
		}

		/** Set ambient factors
		    @param AmbientFactors x - constant, y - linear, z - cuadratic, w - cubic
	     */
		void setAmbientFactors(const Ogre::Vector4& AmbientFactors);

		/** Get ambient factors
		    @return Ambient factors
		 */
		inline const Ogre::Vector4& getAmbientFactors() const
		{
			return mAmbientFactors;
		}

		/** Set global opacity
		    @param GlobalOpacity Global opacity: [0,1] range 0->Transparent cloud field
		 */
		inline void setGlobalOpacity(const Ogre::Real& GlobalOpacity)
		{
			mGlobalOpacity = GlobalOpacity;
		}

		/** Get global opacity
		    @return Global opacity
		 */
		inline const Ogre::Real& getGlobalOpacity() const
		{
			return mGlobalOpacity;
		}

		/** Set cloud field scale
		    @param CloudFieldScale Cloud field scale
		 */
		inline void setCloudFieldScale(const Ogre::Real& CloudFieldScale)
		{
			mCloudFieldScale = CloudFieldScale;
		}

		/** Get cloud field scale
		    @return Cloud field scale
		 */
		inline const Ogre::Real& getCloudFieldScale() const
		{
			return mCloudFieldScale;
		}

		/** Set noise scale
		    @param NoiseScale Noise scale
		 */
		inline void setNoiseScale(const Ogre::Real& NoiseScale)
		{
			mNoiseScale = NoiseScale;
		}

		/** Get noise scale
		    @return Noise scale
		 */
		inline const Ogre::Real& getNoiseScale() const
		{
			return mNoiseScale;
		}

		/** Set wheater parameters
		    Use this funtion to update the cloud field parameters, you'll get a smart and smooth transition from your old 
			setting to your new ones.
			@param Humidity Humidity, in other words: the percentage of clouds in [0,1] range.
			@param AverageCloudsSize Average clouds size, for example: if previous wheater clouds size parameter was very different from new one(i.e: more little)
			       only the old biggest clouds are going to be keept and the little ones are going to be replaced
		    @param NumberOfForcedUpdates Number of times the data simulation are going to be re-calculated for the next frame.
			       This parameters is useful if you want to avoid a delayed response or, in other words, 0 means that you're going to get a smooth transition
				   between old and news wheater parameters(delayed response) and a positive number(2 might be sufficient) is going to change the clouds for the next frame
		 */
		void setWheater(const float& Humidity, const float& AverageCloudsSize, const int& NumberOfForcedUpdates = 0);

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

		/** Get data manager
		    @return Data manager
		 */
		inline DataManager* getDataManager()
		{
			return mDataManager;
		}

		/** Get geometry manager
		    @return Geometry manager
		 */
		inline GeometryManager* getGeometryManager()
		{
			return mGeometryManager;
		}

	private:

		/// Has been create() already called?
		bool mCreated;

		/// Height: x = Altitude over the camera, y: Field height (both in world coordinates)
		Ogre::Vector2 mHeight;
		/// Angles
		Ogre::Radian mAlpha, mBeta;
		/// Radius
		float mRadius;
		/// Number of blocks
		int mNumberOfBlocks;
		/// Number of slices per geometry zone
		int mNa, mNb, mNc;

		/// Wind direction
		Ogre::Radian mWindDirection;
		/// Wind speed
		float mWindSpeed;

		/// Wheater parameters: x = Humidity, y = Average clouds size, both un [0,1] range
		Ogre::Vector2 mWheater;
		/// Number of forced updates (This param is stored because of the user can call setWheater(...) before create() )
		int mNumberOfForcedUpdates;

		/// Sun direction
		Ogre::Vector3 mSunDirection;

		/// Sun color
		Ogre::Vector3 mSunColor;
		/// Ambient color
		Ogre::Vector3 mAmbientColor;

		/** Light response:
		    x - Sun light power
			y - Sun beta multiplier
		    z - Ambient color multiplier
		    w - Distance attenuation
	     */
		Ogre::Vector4 mLightResponse;
		/** Ambient factors
		    x - constant, y - linear, z - cuadratic, w - cubic
		 */
		Ogre::Vector4 mAmbientFactors;

		/// Global opacity
		float mGlobalOpacity;

		/// Cloud field scale
		float mCloudFieldScale;
		/// Noise scale
		float mNoiseScale;

		/// Data manager
		DataManager *mDataManager;
		/// Geometry manager
		GeometryManager *mGeometryManager;

		/// Ogre::SceneManager pointer
        Ogre::SceneManager *mSceneManager;
		/// Main Ogre camera
		Ogre::Camera* mCamera;
	};

}}

#endif