cmake_minimum_required(VERSION 2.6)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
cmake_policy(SET CMP0003 NEW)

project(DotSceneSerializer)

set(HEADER_FILES
	DotSceneSerializer.h
)

set(SOURCE_FILES
	DotSceneSerializer.cpp
	DotSceneSerializerExport.cpp
)

set(DotSceneSerializer_VERSION 0)
set(DotSceneSerializer_VERSION_MAJOR 1)

include_directories(include)
# Terrain Scene Manager
#include_directories (${OGITOR_SM_INCLUDES})
include_directories(${DEPENDENCIES_INCLUDES})
include_directories(${OGITOR_INCLUDES})
message(STATUS "${OGITOR_INCLUDES}")

include_directories(.)
link_directories(${OGITOR_LIBPATH})
link_directories(${DEPENDENCIES_LIBPATH})

link_libraries(${OGRE_LIBRARIES} Ogitor)
ogitor_add_library(DotSceneSerializer SHARED ${HEADER_FILES} ${SOURCE_FILES})
#target_link_libraries(OgreMain Ogitor)
set(LIBRARY_OUTPUT_PATH ../lib)

if(NOT OGITOR_DIST)
    set_target_properties(DotSceneSerializer PROPERTIES INSTALL_RPATH ${OGRE_INSTALL_DIR}/lib/OGRE)
endif(NOT OGITOR_DIST)

set_target_properties(DotSceneSerializer PROPERTIES VERSION ${DotSceneSerializer_VERSION} SOVERSION ${DotSceneSerializer_VERSION_MAJOR})
set_target_properties(DotSceneSerializer PROPERTIES COMPILE_DEFINITIONS PLUGIN_EXPORT)
set_target_properties(DotSceneSerializer PROPERTIES SOLUTION_FOLDER Plugins)

install(TARGETS DotSceneSerializer LIBRARY DESTINATION ${OGITOR_PLUGIN_PATH})


# vim: set sw=2 ts=2 noet:
