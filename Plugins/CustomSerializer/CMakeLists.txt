cmake_minimum_required(VERSION 2.6)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
cmake_policy(SET CMP0003 NEW)

project(CustomSerializer)

set(HEADER_FILES
	CustomSerializer.h
)

set(SOURCE_FILES
	CustomSerializer.cpp
)

set(CustomSerializer_VERSION 0)
set(CustomSerializer_VERSION_MAJOR 1)

include_directories(include)
# Terrain Scene Manager
include_directories(${DEPENDENCIES_INCLUDES})
include_directories(${OGITOR_INCLUDES})

include_directories(.)
link_directories(${OGITOR_LIBPATH})
link_directories(${DEPENDENCIES_LIBPATH})

link_libraries(${OGRE_LIBRARIES} Ogitor)
ogitor_add_library(CustomSerializer SHARED ${HEADER_FILES} ${SOURCE_FILES})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

if(NOT OGITOR_DIST)
    set_target_properties(CustomSerializer PROPERTIES INSTALL_RPATH ${OGRE_INSTALL_DIR}/lib/OGRE)
endif(NOT OGITOR_DIST)

set_target_properties(CustomSerializer PROPERTIES VERSION ${CustomSerializer_VERSION} SOVERSION ${CustomSerializer_VERSION_MAJOR})
set_target_properties(CustomSerializer PROPERTIES COMPILE_DEFINITIONS PLUGIN_EXPORT)
set_target_properties(CustomSerializer PROPERTIES SOLUTION_FOLDER Plugins)

install(TARGETS CustomSerializer
        LIBRARY DESTINATION ${OGITOR_PLUGIN_INSTALL_PATH}
        ARCHIVE DESTINATION ${OGITOR_PLUGIN_INSTALL_PATH}
        RUNTIME DESTINATION ${OGITOR_PLUGIN_INSTALL_PATH})


# vim: set sw=2 ts=2 noet:
