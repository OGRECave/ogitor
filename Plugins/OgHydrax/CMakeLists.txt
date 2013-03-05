cmake_minimum_required(VERSION 2.6)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
cmake_policy(SET CMP0003 NEW)

project(OgHydrax)

set(HEADER_FILES
	HydraxWaterEditor.h
)

set(SOURCE_FILES
	HydraxWaterEditor.cpp
)

set(OgHydrax_VERSION 0)
set(OgHydrax_VERSION_MAJOR 1)

include_directories(include)
# Terrain Scene Manager
#include_directories(${OGITOR_SM_INCLUDES})
include_directories(${DEPENDENCIES_INCLUDES})
include_directories(${OGITOR_INCLUDES})
message(STATUS "${OGITOR_INCLUDES}")

include_directories(.)
link_directories(${OGITOR_LIBPATH})
link_directories(${DEPENDENCIES_LIBPATH})

link_libraries(${OGRE_LIBRARIES} Ogitor Hydrax)
ogitor_add_library(OgHydrax SHARED ${HEADER_FILES} ${SOURCE_FILES})
#target_link_libraries(OgreMain Ogitor)
set(LIBRARY_OUTPUT_PATH ../lib)

if(NOT OGITOR_DIST)
    set_target_properties(OgHydrax PROPERTIES INSTALL_RPATH ${OGRE_INSTALL_DIR}/lib/OGRE)
endif(NOT OGITOR_DIST)

set_target_properties(OgHydrax PROPERTIES VERSION ${OgHydrax_VERSION} SOVERSION ${OgHydrax_VERSION_MAJOR})
set_target_properties(OgHydrax PROPERTIES COMPILE_DEFINITIONS PLUGIN_EXPORT)
set_target_properties(OgHydrax PROPERTIES SOLUTION_FOLDER Plugins)

if(UNIX AND NOT APPLE)
	add_custom_command(TARGET OgHydrax PRE_BUILD
		COMMAND mkdir -p ../../RunPath/Plugins)
	add_custom_command(TARGET OgHydrax POST_BUILD
		COMMAND cp -r ../lib/libOgHydrax.* ../../RunPath/Plugins/)
endif(UNIX AND NOT APPLE)

if(WIN32)
	add_custom_command(TARGET OgHydrax PRE_BUILD
		COMMAND if not exist ..\\..\\RunPath\\Plugins mkdir ..\\..\\RunPath\\Plugins)
	add_custom_command(TARGET OgHydrax POST_BUILD
		COMMAND copy \"$(TargetPath)\" ..\\..\\RunPath\\Plugins & if exist ..\\..\\RunPath\\Plugins\\OgHydrax_d.dll move ..\\..\\RunPath\\Plugins\\OgHydrax_d.dll ..\\..\\RunPath\\Plugins\\debug)

	install(TARGETS OgHydrax
		RUNTIME DESTINATION Plugins
		CONFIGURATIONS Debug Release RelWithDebInfo MinSizeRel)
endif(WIN32)

IF(UNIX AND NOT APPLE)
    if(OGITOR_DIST)
        install(TARGETS OgHydrax
            LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/ogitor
            CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
    endif(OGITOR_DIST)
ENDIF(UNIX AND NOT APPLE)

# vim: set sw=2 ts=2 noet:
