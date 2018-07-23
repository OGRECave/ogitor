cmake_minimum_required(VERSION 2.6)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
cmake_policy(SET CMP0003 NEW)

project(OgSkyX)

set(HEADER_FILES
	SkyxEditor.h
)

set(SOURCE_FILES
	SkyxEditor.cpp
)

set(OgSkyX_VERSION 0)
set(OgSkyX_VERSION_MAJOR 4)

include_directories(include)
# Terrain Scene Manager
#include_directories(${OGITOR_SM_INCLUDES})
include_directories(${DEPENDENCIES_INCLUDES})
include_directories(${OGITOR_INCLUDES})
message(STATUS "${OGITOR_INCLUDES}")

include_directories(.)
link_directories(${OGITOR_LIBPATH})
link_directories(${DEPENDENCIES_LIBPATH})

link_libraries(${OGRE_LIBRARIES} Ogitor)
add_library(OgSkyX SHARED ${HEADER_FILES} ${SOURCE_FILES})
if(WIN32)
    target_link_libraries(OgSkyX debug ${DEPENDENCIES_LIBPATH}/SkyX_d.lib optimized ${DEPENDENCIES_LIBPATH}/SkyX.lib)
else()
    target_link_libraries(OgSkyX debug ${DEPENDENCIES_LIBPATH}/SkyX_d.so optimized ${DEPENDENCIES_LIBPATH}/SkyX.so)
endif()
#target_link_libraries(OgreMain Ogitor)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

if(NOT OGITOR_DIST)
    set_target_properties(OgSkyX PROPERTIES INSTALL_RPATH ${OGRE_INSTALL_DIR}/lib/OGRE)
endif(NOT OGITOR_DIST)

set_target_properties(OgSkyX PROPERTIES VERSION ${OgSkyX_VERSION} SOVERSION ${OgSkyX_VERSION_MAJOR})
set_target_properties(OgSkyX PROPERTIES COMPILE_DEFINITIONS PLUGIN_EXPORT)
set_target_properties(OgSkyX PROPERTIES SOLUTION_FOLDER Plugins)
install(TARGETS OgSkyX
        LIBRARY DESTINATION ${OGITOR_PLUGIN_PATH}
        ARCHIVE DESTINATION ${OGITOR_PLUGIN_PATH}
        RUNTIME DESTINATION ${OGITOR_PLUGIN_PATH})
# vim: set sw=2 ts=2 noet:
