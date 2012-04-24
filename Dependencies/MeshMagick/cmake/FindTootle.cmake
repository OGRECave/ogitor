# - Try to find AMD Tootle
# Once done, this will define
#
#  Tootle_FOUND - system has Tootle
#  Tootle_INCLUDE_DIRS - the include directories 
#  Tootle_LIBRARY_DIRS - link these 
# 
#  To assist, define TOOTLE_HOME in CMake or in environment
#

# Only written for windows at this time
if (NOT WIN32)
  return()
endif()

include(FindPkgMacros)

findpkg_begin(Tootle)

# Try to find DirectX; using occlusion queries for overdraw detection instead of software raytrace is vastly faster
if (NOT DirectX_FOUND)
	find_package(DirectX)
endif()

if (DirectX_FOUND)
	set(Tootle_USE_DIRECTX TRUE)
else()
	set(Tootle_USE_DIRECTX FALSE)
endif()

# Get path, convert backslashes as ${ENV_${var}}
getenv_path(TOOTLE_HOME)

# construct search paths
set(Tootle_PREFIX_PATH ${TOOTLE_HOME} ${ENV_TOOTLE_HOME} "./")
create_search_paths(Tootle)
# redo search if prefix path changed
clear_if_changed(Tootle_PREFIX_PATH
  Tootle_LIBRARY_FWK
  Tootle_LIBRARY_REL
  Tootle_LIBRARY_DBG
  Tootle_INCLUDE_DIR
)

if (Tootle_USE_DIRECTX)
	if (${MSVC_VERSION} EQUAL 1500)
		set(Tootle_LIBRARY_NAMES "TootleStatic_2k8_MTDLL")
	else()
		set(Tootle_LIBRARY_NAMES "TootleStatic_MTDLL")
	endif()
else()
	if (${MSVC_VERSION} EQUAL 1500)
		set(Tootle_LIBRARY_NAMES "TootleSoftwareOnlyStatic_2k8_MTDLL")
	else()
		set(Tootle_LIBRARY_NAMES "TootleSoftwareOnlyStatic_MTDLL")
	endif()
endif()

get_debug_names(Tootle_LIBRARY_NAMES)

use_pkgconfig(Tootle_PKGC Tootle)

find_path(Tootle_INCLUDE_DIR NAMES tootlelib.h HINTS ${Tootle_INC_SEARCH_PATH} ${Tootle_PKGC_INCLUDE_DIRS} PATH_SUFFIXES include)
find_library(Tootle_LIBRARY_REL NAMES ${Tootle_LIBRARY_NAMES} HINTS ${Tootle_LIB_SEARCH_PATH} ${Tootle_PKGC_LIBRARY_DIRS} PATH_SUFFIXES lib)
find_library(Tootle_LIBRARY_DBG NAMES ${Tootle_LIBRARY_NAMES_DBG} HINTS ${Tootle_LIB_SEARCH_PATH} ${Tootle_PKGC_LIBRARY_DIRS} PATH_SUFFIXES ${Tootle_LIB_PATH_SUFFIX} lib)
make_library_set(Tootle_LIBRARY)

findpkg_finish(Tootle)

if (Tootle_USE_DIRECTX)
	set(Tootle_LIBRARIES ${Tootle_LIBRARIES} ${DirectX_LIBRARIES})
endif()
message(STATUS "tt: ${Tootle_LIBRARIES}")

