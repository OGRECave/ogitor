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

#pragma once

//#include "OgitorsPrerequisites.h"
namespace MZP
{
	struct PortalInfo
	{
		Ogre::Real mWidth;
		Ogre::Real mHeight;
		Ogre::Vector3 mPosition;
		Ogre::Quaternion mOrientation;

		PortalInfo():
		mWidth(1.0f),
		mHeight(1.0f),
		mPosition(0.0f,0.0f,0.0f),
		mOrientation(0.0f,0.0f,0.0f,1.0f)
		{}


	};

	struct ZoneInfo
	{
		Ogre::String mName; //name of this zone design
		Ogre::String mMesh; //name of the enclosing mesh
		int mPortalCount; //number of portals 
		std::vector<PortalInfo> mPortals; //data for portals
		Ogre::String mShortDesc;//short description
		Ogre::String mLongDesc; //long description

		ZoneInfo():
		mName(""),
		mMesh(""),
		mPortalCount(0),
		mPortals(0),
		mShortDesc(""),
		mLongDesc("")
		{}

	};

	typedef std::map<int,ZoneInfo> ZoneInfoMap;

	typedef std::pair<Ogre::Vector3,Ogre::Quaternion> HoleSnapPoint;

	struct ZoneDesignTools
	{
		std::vector<HoleSnapPoint> mHoles;
		std::vector<Ogre::Vector3> mPerimeter;
	};




}