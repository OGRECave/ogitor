cmake_minimum_required(VERSION 2.6)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
cmake_policy(SET CMP0003 NEW)

project(OgSkyx)

set(HEADER_FILES
	SkyxEditor.h
)

set(SOURCE_FILES
	SkyxEditor.cpp
)

set(OgSkyx_VERSION 0)
set(OgSkyx_VERSION_MAJOR 1)

include_directories(include)
# Terrain Scene Manager
#include_directories(${OGITOR_SM_INCLUDES})
include_directories(${DEPENDENCIES_INCLUDES})
include_directories(${OGITOR_INCLUDES})
message(STATUS "${OGITOR_INCLUDES}")

include_directories(.)
link_directories(${OGITOR_LIBPATH})
link_directories(${DEPENDENCIES_LIBPATH})

link_libraries(${OGRE_LIBRARIES} Ogitor Skyx)
add_library(OgSkyx SHARED ${HEADER_FILES} ${SOURCE_FILES})
#target_link_libraries(OgreMain Ogitor)
set(LIBRARY_OUTPUT_PATH ../lib)

if(NOT OGITOR_DIST)
    set_target_properties(OgSkyx PROPERTIES INSTALL_RPATH ${OGRE_INSTALL_DIR}/lib/OGRE)
endif(NOT OGITOR_DIST)

set_target_properties(OgSkyx PROPERTIES VERSION ${OgSkyx_VERSION} SOVERSION ${OgSkyx_VERSION_MAJOR})
set_target_properties(OgSkyx PROPERTIES	COMPILE_DEFINITIONS PLUGIN_EXPORT)
set_target_properties(OgSkyx PROPERTIES SOLUTION_FOLDER Plugins)
set_target_properties(OgSkyx PROPERTIES FOLDER Plugins) 

if(UNIX AND NOT APPLE)
	add_custom_command(TARGET OgSkyx PRE_BUILD
		COMMAND mkdir -p ../../RunPath/Plugins)
	add_custom_command(TARGET OgSkyx POST_BUILD
		COMMAND cp -r ../lib/libOgSkyx.* ../../RunPath/Plugins/)
endif(UNIX AND NOT APPLE)

if(WIN32)
	add_custom_command(TARGET OgSkyx PRE_BUILD
		COMMAND if not exist ..\\..\\RunPath\\Plugins mkdir ..\\..\\RunPath\\Plugins)
	add_custom_command(TARGET OgSkyx POST_BUILD
		COMMAND copy \"$(TargetPath)\" ..\\..\\RunPath\\Plugins & if exist ..\\..\\RunPath\\Plugins\\OgSkyx_d.dll move ..\\..\\RunPath\\Plugins\\OgSkyx_d.dll ..\\..\\RunPath\\Plugins\\debug)

	install(TARGETS OgSkyx
		RUNTIME DESTINATION Plugins
		CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
endif(WIN32)

IF(UNIX AND NOT APPLE)
    if(OGITOR_DIST)
        install(TARGETS OgSkyx
            LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/ogitor
            CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
    endif(OGITOR_DIST)
ENDIF(UNIX AND NOT APPLE)

# vim: set sw=2 ts=2 noet:
