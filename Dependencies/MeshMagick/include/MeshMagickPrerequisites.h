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

#ifndef __MM_MESHMAGICK_PREREQUISITES_H__
#define __MM_MESHMAGICK_PREREQUISITES_H__

namespace meshmagick
{
    class Tool;
    class ToolFactory;

    class TransformTool;
    class TransformToolFactory;
}

#ifdef __APPLE__
#	include <Ogre/OgrePlatform.h>
#else
#	include <OgrePlatform.h>
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 && !MESHMAGICK_STANDALONE
// Export control
#   if defined( MESHMAGICK_EXPORTS )
#       define _MeshMagickExport __declspec( dllexport )
#   else
#       define _MeshMagickExport __declspec( dllimport )
#   endif
#else // Linux / Mac OSX etc
#   define _MeshMagickExport
#endif

#define MESHMAGICK_VERSION_MAJOR 0
#define MESHMAGICK_VERSION_MINOR 6
#define MESHMAGICK_VERSION_PATCH 0

#endif
