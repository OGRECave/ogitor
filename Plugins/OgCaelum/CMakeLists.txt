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

link_libraries(${OGRE_LIBRARIES} Ogitor Caelum)
add_library(OgCaelum SHARED ${HEADER_FILES} ${SOURCE_FILES})
#target_link_libraries(OgreMain Ogitor ETL)
set(LIBRARY_OUTPUT_PATH ../lib)

#set_target_properties(OgCaelum PROPERTIES INSTALL_RPATH ${OGRE_INSTALL_DIR}/lib/OGRE)
set_target_properties(OgCaelum PROPERTIES SOLUTION_FOLDER Plugins)
set_target_properties(OgCaelum PROPERTIES VERSION ${OgCaelum_VERSION} SOVERSION ${OgCaelum_VERSION_MAJOR})
set_target_properties(OgCaelum PROPERTIES COMPILE_DEFINITIONS PLUGIN_EXPORT)

if(UNIX AND NOT APPLE)
	add_custom_command(TARGET OgCaelum PRE_BUILD
		COMMAND mkdir -p ../../RunPath/Plugins)
	add_custom_command(TARGET OgCaelum POST_BUILD
		COMMAND cp -r ../lib/libOgCaelum.* ../../RunPath/Plugins/)
endif(UNIX AND NOT APPLE)

if(WIN32)
	add_custom_command(TARGET OgCaelum PRE_BUILD
		COMMAND if not exist ..\\..\\RunPath\\Plugins mkdir ..\\..\\RunPath\\Plugins)
	add_custom_command(TARGET OgCaelum POST_BUILD
		COMMAND copy \"$(TargetPath)\" ..\\..\\RunPath\\Plugins & if exist ..\\..\\RunPath\\Plugins\\OgCaelum_d.dll move ..\\..\\RunPath\\Plugins\\OgCaelum_d.dll ..\\..\\RunPath\\Plugins\\debug)

	install(TARGETS OgCaelum
		RUNTIME DESTINATION Plugins
		CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
endif(WIN32)

IF(UNIX AND NOT APPLE)
    if(OGITOR_DIST)
        install(TARGETS OgCaelum
            LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/share/qtOgitor/Plugins
            CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
    endif(OGITOR_DIST)
ENDIF(UNIX AND NOT APPLE)

# vim: set sw=2 ts=2 noet:
