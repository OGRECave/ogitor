/*
This file is part of MeshMagick - An Ogre mesh file manipulation tool.
Copyright (C) 2007-2009 Daniel Wickert

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef __MM_EDITABLE_BONE_H__
#define __MM_EDITABLE_BONE_H__

#ifdef __APPLE__
#	include <Ogre/OgreBone.h>
#else
#	include <OgreBone.h>
#endif
#include "MeshMagickPrerequisites.h"

namespace meshmagick
{
    /// Very hacky class used by the rename tool to rename a bone.
	class _MeshMagickExport EditableBone : public Ogre::Bone
	{
	public:
		void setName(const Ogre::String& name);
	};
}

#endif // __MM_EDITABLE_BONE_H__
