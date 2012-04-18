cmake_minimum_required(VERSION 2.6)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
cmake_policy(SET CMP0003 NEW)

project(Hydrax)


set(HEADER_FILES
    CfgFileManager.h
    GPUNormalMapManager.h
    MaterialManager.h  TextureManager.h
    DecalsManager.h
    Help.h
    Mesh.h
    Enums.h
    Hydrax.h
    Prerequisites.h
    GodRaysManager.h
    Image.h
    RttManager.h
    Noise/Noise.h
    Noise/Perlin/Perlin.h
    Noise/FFT/FFT.h
    Modules/Module.h
    Modules/ProjectedGrid/ProjectedGrid.h
    Modules/RadialGrid/RadialGrid.h
    Modules/SimpleGrid/SimpleGrid.h

)
set(SOURCE_FILES
    CfgFileManager.cpp
    Help.cpp
    Prerequisites.cpp
    DecalsManager.cpp
    Hydrax.cpp
    RttManager.cpp
    Enums.cpp
    Image.cpp
    TextureManager.cpp
    GodRaysManager.cpp
    MaterialManager.cpp
    GPUNormalMapManager.cpp
    Mesh.cpp
    Noise/Noise.cpp
    Noise/Perlin/Perlin.cpp
    Noise/FFT/FFT.cpp
    Modules/Module.cpp
    Modules/ProjectedGrid/ProjectedGrid.cpp
    Modules/RadialGrid/RadialGrid.cpp
    Modules/SimpleGrid/SimpleGrid.cpp

)


SET(HYDRAX_VERSION 0)
SET(HYDRAX_VERSION_MAJOR 1)

include_directories (.)
# Terrain Scene Manager
#include_directories ( ${OGITOR_SM_INCLUDES} )

ogitor_add_library(Hydrax SHARED ${HEADER_FILES} ${SOURCE_FILES}  )
SET(LIBRARY_OUTPUT_PATH  ../../../../lib)

set_target_properties(PROPERTIES
        INSTALL_RPATH ${OGRE_INSTALL_DIR}/lib/OGRE  )

set_target_properties(Hydrax PROPERTIES VERSION ${HYDRAX_VERSION} SOVERSION ${HYDRAX_VERSION_MAJOR})

set_target_properties(Hydrax PROPERTIES
COMPILE_DEFINITIONS HYDRAX_LIB
)

IF(WIN32)
	add_custom_command( TARGET Hydrax PRE_BUILD
		COMMAND if not exist ..\\..\\..\\..\\..\\RunPath\\bin mkdir ..\\..\\..\\..\\..\\RunPath\\bin )
	add_custom_command( TARGET Hydrax POST_BUILD
		COMMAND copy \"$(TargetPath)\" ..\\..\\..\\..\\..\\RunPath\\bin )
ENDIF(WIN32)

set_target_properties(Hydrax PROPERTIES SOLUTION_FOLDER Dependencies)

target_link_libraries(Hydrax  ${OGRE_LIBRARIES} ${Boost_LIBRARIES})

IF(WIN32)
install(TARGETS Hydrax
	RUNTIME DESTINATION bin
	CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
ENDIF(WIN32)

IF(UNIX AND NOT APPLE)

    set_target_properties( Hydrax PROPERTIES OUTPUT_NAME "Hydrax_Ogitor" VERSION
    ${OGITOR_VERSION_STRING} SOVERSION ${abi_version} )

    if(OGITOR_DIST)
        install(TARGETS Hydrax
            LIBRARY DESTINATION ${OGRE_PLUGIN_DIR_LINUX}
            CONFIGURATIONS Release RelWithDebInfo MinSizeRel)

        install(TARGETS Hydrax
            LIBRARY DESTINATION lib
            CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
    endif(OGITOR_DIST)

ENDIF(UNIX AND NOT APPLE)
