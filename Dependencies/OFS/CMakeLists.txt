cmake_minimum_required(VERSION 2.8)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
cmake_policy(SET CMP0003 NEW)

project(OFS)

set(OFS_SOURCE
	source/ofs.cpp
	source/ofs_converter.cpp
)

set(OFS_HEADERS
	include/ofs.h
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include
)

include_directories(${Boost_INCLUDE_DIR})

ogitor_add_library(OFS SHARED ${OFS_SOURCE} ${OFS_HEADERS})

target_link_libraries(OFS ${Boost_LIBRARIES})

set(LIBRARY_OUTPUT_PATH  ../lib)

set_target_properties(OFS PROPERTIES COMPILE_DEFINITIONS "OFS_EXPORT")

if(WIN32)
	set_target_properties(OFS PROPERTIES COMPILE_FLAGS "/MP")
endif(WIN32)


if(UNIX)
        set(CMAKE_CXX_FLAGS -fPIC)
endif(UNIX)

if(WIN32)
	add_custom_command(TARGET OFS PRE_BUILD
		COMMAND if not exist ..\\..\\RunPath\\bin mkdir ..\\..\\RunPath\\bin)
	add_custom_command(TARGET OFS POST_BUILD
		COMMAND copy \"$(TargetPath)\" ..\\..\\RunPath\\bin)

	install(TARGETS OFS RUNTIME DESTINATION bin CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
endif(WIN32)

IF(UNIX AND NOT APPLE)

    set_target_properties( OFS PROPERTIES OUTPUT_NAME "OFS" VERSION
    ${OGITOR_VERSION_STRING} SOVERSION ${abi_version} )

    install(TARGETS OFS
        LIBRARY DESTINATION lib
        CONFIGURATIONS Debug Release RelWithDebInfo MinSizeRel)

ENDIF(UNIX AND NOT APPLE)

IF (APPLE)
        set_target_properties(OFS PROPERTIES OUTPUT_NAME OFS)
        # Set the INSTALL_PATH so that Frameworks can be local
        set_target_properties(OFS PROPERTIES BUILD_WITH_INSTALL_RPATH 1 INSTALL_NAME_DIR "@executable_path/../Plugins/Ogitor")
   ENDIF(APPLE)


