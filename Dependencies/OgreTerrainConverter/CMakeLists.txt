cmake_minimum_required(VERSION 2.8)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
cmake_policy(SET CMP0003 NEW)

project(OgreTerrainConverter)

set(OgreTerrainConverter_SOURCE
        source/OgreTerrainConverter.cpp
)

set(OgreTerrainConverter_HEADERS
        include/OgreTerrainConverter.h
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include
)

include_directories(${Boost_INCLUDE_DIR})

ogitor_add_library(OgreTerrainConverter SHARED ${OgreTerrainConverter_SOURCE} ${OgreTerrainConverter_HEADERS})

target_link_libraries(OgreTerrainConverter ${OGRE_LIBRARY} ${OGRE_Terrain_LIBRARY} ${OGRE_Paging_LIBRARY} ${Boost_LIBRARIES})

set(LIBRARY_OUTPUT_PATH  ../lib)

set_target_properties(OgreTerrainConverter PROPERTIES COMPILE_DEFINITIONS "OgreTerrainConverter_EXPORT")

if(UNIX)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif(UNIX)

if(MSVC_IDE)
	add_custom_command(TARGET OgreTerrainConverter PRE_BUILD
		COMMAND if not exist ..\\..\\RunPath\\bin mkdir ..\\..\\RunPath\\bin)
	add_custom_command(TARGET OgreTerrainConverter POST_BUILD
		COMMAND copy \"$(TargetPath)\" ..\\..\\RunPath\\bin)
endif(MSVC_IDE)
if(WIN32)
	install(TARGETS OgreTerrainConverter RUNTIME DESTINATION bin CONFIGURATIONS Debug Release RelWithDebInfo MinSizeRel)
endif(WIN32)

IF(UNIX AND NOT APPLE)
    set_target_properties( OgreTerrainConverter PROPERTIES OUTPUT_NAME "OgreTerrainConverter" VERSION
    ${OGITOR_VERSION_STRING} SOVERSION ${abi_version} )

    install(TARGETS OgreTerrainConverter
        LIBRARY DESTINATION lib
        CONFIGURATIONS Debug Release RelWithDebInfo MinSizeRel)
ENDIF(UNIX AND NOT APPLE)

IF (APPLE)
        set_target_properties(OgreTerrainConverter PROPERTIES OUTPUT_NAME OgreTerrainConverter)
        # Set the INSTALL_PATH so that Frameworks can be local
        set_target_properties(OgreTerrainConverter PROPERTIES BUILD_WITH_INSTALL_RPATH 1 INSTALL_NAME_DIR "@executable_path/../Plugins/Ogitor")
ENDIF(APPLE)

