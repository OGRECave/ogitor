cmake_minimum_required(VERSION 2.6)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
cmake_policy(SET CMP0003 NEW)

project(OgCaelum)

set(HEADER_FILES
  CaelumEditor.h
)

set(SOURCE_FILES
	CaelumEditor.cpp
)

set(OgCaelum_VERSION 0)
set(OgCaelum_VERSION_MAJOR 1)

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
add_library(OgCaelum SHARED ${HEADER_FILES} ${SOURCE_FILES})
if(WIN32)
    target_link_libraries(OgCaelum debug ${DEPENDENCIES_LIBPATH}/Caelum_d.lib optimized ${DEPENDENCIES_LIBPATH}/Caelum.lib)
else()
    target_link_libraries(OgCaelum debug ${DEPENDENCIES_LIBPATH}/Caelum_d.so optimized ${DEPENDENCIES_LIBPATH}/Caelum.so)
endif()
#target_link_libraries(OgreMain Ogitor ETL)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

if(NOT OGITOR_DIST)
    set_target_properties(OgCaelum PROPERTIES INSTALL_RPATH ${OGRE_INSTALL_DIR}/lib/OGRE)
endif(NOT OGITOR_DIST)

set_target_properties(OgCaelum PROPERTIES SOLUTION_FOLDER Plugins)
set_target_properties(OgCaelum PROPERTIES VERSION ${OgCaelum_VERSION} SOVERSION ${OgCaelum_VERSION_MAJOR})
set_target_properties(OgCaelum PROPERTIES COMPILE_DEFINITIONS PLUGIN_EXPORT)
install(TARGETS OgCaelum
        LIBRARY DESTINATION ${OGITOR_PLUGIN_PATH}
        ARCHIVE DESTINATION ${OGITOR_PLUGIN_PATH}
        RUNTIME DESTINATION ${OGITOR_PLUGIN_PATH})
# vim: set sw=2 ts=2 noet:
