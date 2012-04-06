######################################################################
# OGITOR BUILD SYSTEM
# Welcome to the CMake build system for OGITOR.
# This is the main file where we prepare the general build environment
# and provide build configuration options.
######################################################################

# - Try to find Ogitor
# Once done, this will define
#
#  OGITOR_FOUND - system has Ogitor
#  OGITOR_INCLUDE_DIRS - the Ogitor include directories
#  OGITOR_LIBRARIES - link these to use Ogitor

include(FindPkgMacros)
findpkg_begin(OGITOR)

# Get path, convert backslashes as ${ENV_${var}}
getenv_path(OGITOR_HOME)

# construct search paths
set(OGITOR_PREFIX_PATH ${OGITOR_HOME}/Ogitor ${ENV_OGITOR_HOME} /home/jacmoe/programming/ogitor/Ogitor /usr/local /usr/local/include /usr/local/lib /usr/include /usr/lib /usr/local/include/ogitor /usr/include/ogitor /usr/lib/ogitor /usr/local/lib/ogitor /home/jacmoe/programming/ogitor/build)
create_search_paths(OGITOR)
# redo search if prefix path changed
clear_if_changed(OGITOR_PREFIX_PATH
  OGITOR_LIBRARY_REL
  OGITOR_LIBRARY_DBG
  OGITOR_INCLUDE_DIR
)

set(OGITOR_LIBRARY_NAMES Ogitor)
get_debug_names(OGITOR_LIBRARY_NAMES)

use_pkgconfig(OGITOR_PKGC OGITOR)

findpkg_framework(OGITOR)

find_path(OGITOR_INCLUDE_DIR NAMES Ogitors.h HINTS ${OGITOR_INC_SEARCH_PATH} ${OGITOR_PKGC_INCLUDE_DIRS} PATH_SUFFIXES Ogitor)
find_library(OGITOR_LIBRARY_REL NAMES ${OGITOR_LIBRARY_NAMES} HINTS ${OGITOR_LIB_SEARCH_PATH} ${OGITOR_PKGC_LIBRARY_DIRS} PATH_SUFFIXES "" release relwithdebinfo minsizerel)
find_library(OGITOR_LIBRARY_DBG NAMES ${OGITOR_LIBRARY_NAMES_DBG} HINTS ${OGITOR_LIB_SEARCH_PATH} ${OGITOR_PKGC_LIBRARY_DIRS} PATH_SUFFIXES "" debug)
make_library_set(OGITOR_LIBRARY)

findpkg_finish(OGITOR)
add_parent_dir(OGITOR_INCLUDE_DIRS OGITOR_INCLUDE_DIR)
