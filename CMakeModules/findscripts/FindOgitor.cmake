# Find Ogitor includes and library
#
# This module defines
#  Ogitor_INCLUDE_DIR, the location of JUST the Ogitor include directory
#  Ogitor_INCLUDE_DIRS, the location of the Ogitor include directory plus plugins
#  Ogitor_LIBRARIES, the location of the libraries
#  Ogitor_***_LIBRARY, the location of the plugin library, replace ***
#  Ogitor_FOUND, If false, do not try to use Ogitor
#
# Copyright © 2012, Andrew Fenn
# Released under public domain

set(PLUGIN_NAME
    Null
    AngelScript
    Caelum
    ModularZone
    Hydrax
    SkyX
)

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
    lib64/ogitor
    lib/ogitor
    libs64/ogitor
    libs/ogitor
    libs/Win32/ogitor
    libs/Win64/ogitor
)

find_path(Ogitor_INCLUDE_DIR Ogitors.h
    PATH_SUFFIXES ogitor include 
    PATHS
    ${paths}
)

foreach(COMPONENT ${Ogitor_FIND_COMPONENTS})
    HELPER_GET_CASE_FROM_LIST( ${COMPONENT} PLUGIN_NAME COMPONENT_CASE)
    message(STATUS "Looking for Ogitor plugin: ${COMPONENT_CASE}")

    find_path(Ogitor_${COMPONENT_CASE}_INCLUDE_DIR ${COMPONENT_CASE}/${COMPONENT_CASE}.h
        PATH_SUFFIXES ogitor/dependencies include
        PATHS
        ${paths}
    )

    set(Ogitor_INCLUDE_DIRS ${Ogitor_INCLUDE_DIRS} ${Ogitor_${COMPONENT_CASE}_INCLUDE_DIR})
    find_library(Ogitor_${COMPONENT_CASE}_LIBRARY_REL NAMES ${COMPONENT_CASE} libOg${COMPONENT_CASE} libOg${COMPONENT_CASE}.so ${COMPONENT_CASE}.lib HINTS $ENV{OGITORDIR} PATH_SUFFIXES ${suffixes}; lib/Release PATHS ${paths})
    find_library(Ogitor_${COMPONENT_CASE}_LIBRARY_DBG NAMES ${COMPONENT_CASE} libOg${COMPONENT_CASE} libOg${COMPONENT_CASE}.so ${COMPONENT_CASE}.lib HINTS $ENV{OGITORDIR} PATH_SUFFIXES ${suffixes}; lib/Debug PATHS ${paths})

    set(Ogitor_${COMPONENT_CASE}_LIBRARY optimized ${Ogitor_${COMPONENT_CASE}_LIBRARY_REL} debug ${Ogitor_${COMPONENT_CASE}_LIBRARY_DBG})

    if (Ogitor_${COMPONENT_CASE}_LIBRARY_REL OR Ogitor_${COMPONENT_CASE}_LIBRARY_DBG AND Ogitor_${COMPONENT_CASE}_INCLUDE_DIR)
        set(Ogitor_${COMPONENT_CASE}_FOUND True)
        message(STATUS "Found plugin: ${COMPONENT_CASE}")

        set(Ogitor_INCLUDE_DIRS ${Ogitor_INCLUDE_DIRS} ${Ogitor_${COMPONENT_CASE}_INCLUDE_DIR})
    else()
        message(STATUS "Could not find plugin: ${COMPONENT_CASE}")
    endif()

endforeach(COMPONENT)

find_library(Ogitor_LIBRARY_REL NAMES DotSceneSerializer libDotSceneSerializer.so libDotSceneSerializer.lib HINTS $ENV{OGITORDIR} PATH_SUFFIXES ${suffixes}; lib/Release PATHS ${paths})
find_library(Ogitor_LIBRARY_DBG NAMES DotSceneSerializer libDotSceneSerializer.so libDotSceneSerializer.lib HINTS $ENV{OGITORDIR} PATH_SUFFIXES ${suffixes}; lib/Debug PATHS ${paths})

set(Ogitor_LIBRARIES optimized ${Ogitor_LIBRARY_REL} ${Ogitor_COMPONENTS_LIBRARY_REL} debug ${Ogitor_LIBRARY_DBG} ${Ogitor_COMPONENTS_LIBRARY_DBG})

set(Ogitor_FOUND False)
if (Ogitor_LIBRARIES AND Ogitor_INCLUDE_DIR)
    set(Ogitor_FOUND True)
    set(Ogitor_INCLUDE_DIRS ${Ogitor_INCLUDE_DIR} ${Ogitor_INCLUDE_DIRS})
endif()

