# Find OFS includes and library
#
# This module defines
#  OFS_INCLUDE_DIR, the location of the OFS include directory
#  OFS_LIBRARIES, the location of the ofs libraries
#  OFS_FOUND, If false, do not try to use OFS
#
# Copyright © 2012, Andrew Fenn
# Released under public domain

set(paths
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local
    /usr
    /sw # Fink
    /opt/local # DarwinPorts
    /opt/csw # Blastwave
    /opt
  )

set(suffixes
    lib64/
    lib/
    libs64/
    libs/
    libs/Win32/
    libs/Win64/
)

find_path(OFS_INCLUDE_DIR ofs.h
    PATH_SUFFIXES OFS include 
    PATHS
    ${paths}
)

find_library(OFS_LIBRARY_REL NAMES OFS libOFS.so libOFS.lib HINTS $ENV{OFSDIR} PATH_SUFFIXES ${suffixes}; lib/Release PATHS ${paths})
find_library(OFS_LIBRARY_DBG NAMES OFS libOFS.so libOFS.lib HINTS $ENV{OFSDIR} PATH_SUFFIXES ${suffixes}; lib/Debug PATHS ${paths})

set(OFS_LIBRARIES optimized ${OFS_LIBRARY_REL} ${OFS_COMPONENTS_LIBRARY_REL} debug ${OFS_LIBRARY_DBG} ${OFS_COMPONENTS_LIBRARY_DBG})

set(OFS_FOUND False)
if (OFS_LIBRARIES AND OFS_INCLUDE_DIR)
    set(OFS_FOUND True)
endif()

