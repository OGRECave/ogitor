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
set(LIBRARY_OUTPUT_PATH ../lib)

if(NOT OGITOR_DIST)
    set_target_properties(CustomSerializer PROPERTIES INSTALL_RPATH ${OGRE_INSTALL_DIR}/lib/OGRE)
endif(NOT OGITOR_DIST)

set_target_properties(CustomSerializer PROPERTIES VERSION ${CustomSerializer_VERSION} SOVERSION ${CustomSerializer_VERSION_MAJOR})
set_target_properties(CustomSerializer PROPERTIES COMPILE_DEFINITIONS PLUGIN_EXPORT)
set_target_properties(CustomSerializer PROPERTIES SOLUTION_FOLDER Plugins)

if(UNIX AND NOT APPLE)
	add_custom_command(TARGET CustomSerializer PRE_BUILD
		COMMAND mkdir -p ../../RunPath/Plugins)
	add_custom_command(TARGET CustomSerializer POST_BUILD
		COMMAND cp -r ../lib/libCustomSerializer.* ../../RunPath/Plugins/)
endif(UNIX AND NOT APPLE)

if(WIN32)
	add_custom_command(TARGET CustomSerializer PRE_BUILD
		COMMAND if not exist ..\\..\\RunPath\\Plugins mkdir ..\\..\\RunPath\\Plugins)
	add_custom_command(TARGET CustomSerializer POST_BUILD
		COMMAND copy \"$(TargetPath)\" ..\\..\\RunPath\\Plugins)
	add_custom_command(TARGET CustomSerializer POST_BUILD
		COMMAND if exist ..\\..\\RunPath\\Plugins\\CustomSerializer_d.dll move ..\\..\\RunPath\\Plugins\\CustomSerializer_d.dll ..\\..\\RunPath\\Plugins\\debug)

	install(TARGETS CustomSerializer
		RUNTIME DESTINATION Plugins
		CONFIGURATIONS Debug Release RelWithDebInfo MinSizeRel)
endif(WIN32)

IF(UNIX AND NOT APPLE)
    if(OGITOR_DIST)
        install(TARGETS CustomSerializer
            LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/ogitor
            CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
    endif(OGITOR_DIST)
ENDIF(UNIX AND NOT APPLE)

if (APPLE)
	install(TARGETS CustomSerializer
            LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/ogitor
            CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
endif(APPLE)


# vim: set sw=2 ts=2 noet:
