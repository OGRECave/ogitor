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

#ifndef _SkyX_MoonManager_H_
#define _SkyX_MoonManager_H_

#include "Prerequisites.h"

namespace SkyX
{
	class SkyX;

    class DllExport MoonManager 
	{
	public:
	    /** Constructor
		    @param s Parent SkyX pointer
		 */
		MoonManager(SkyX *s);

		/** Destructor 
		 */
		~MoonManager();

		/** Create all resources
		 */
		void create();

		/** Update
		 */
		void update();

		/** Remove all resources
		 */
		void remove();
	
		/** Get moon billboard
		 */
		inline Ogre::BillboardSet* getMoonBillboard()
		{
			return mMoonBillboard;
		}

		/** Get moon scene node
		 */
		inline Ogre::SceneNode* getMoonSceneNode()
		{
			return mMoonSceneNode;
		}

		/** Set moon size
		    @param MoonSize Moon size
		 */
		void setMoonSize(const Ogre::Real& MoonSize);

		/** Get moon size
		    @return Moon size
		 */
		inline const Ogre::Real& getMoonSize() const
		{
			return mMoonSize;
		}

		/** Is moon manager created?
		    @return true if yes, false if not
		 */
		inline const bool& isCreated() const
		{
			return mCreated;
		}

	private:
		/** Update moon bounds
	     */
		void _updateMoonBounds();

		/// Moon billboard
		Ogre::BillboardSet* mMoonBillboard;
		/// Moon scene node
		Ogre::SceneNode* mMoonSceneNode;

		/// Is moon manager created?
		bool mCreated;

		// Moon size
		Ogre::Real mMoonSize;

		/// SkyX parent pointer
		SkyX *mSkyX;
	};
}

#endif