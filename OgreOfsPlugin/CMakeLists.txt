project(OgreOfsPlugin)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/OFS/include")
include_directories(include)

set(OgreOfsPlugin_hdrs
	./include/OfsFileSystem.h
)

set(OgreOfsPlugin_srcs
	./src/OfsFileSystem.cpp
	./src/Plugin.cpp
)

add_library(OgreOfsPlugin SHARED ${OgreOfsPlugin_hdrs} ${OgreOfsPlugin_srcs})
set(LIBRARY_OUTPUT_PATH ../lib)


set_target_properties(OgreOfsPlugin PROPERTIES COMPILE_DEFINITIONS "PLUGIN_EXPORT")

target_link_libraries(OgreOfsPlugin ${OGRE_LIBRARY} OFS ${Boost_LIBRARIES})

if(APPLE)

	# Setup to be a framework
	set_target_properties(OgreOfsPlugin PROPERTIES FRAMEWORK TRUE)
	set_target_properties(OgreOfsPlugin PROPERTIES PUBLIC_HEADER "${HEADER_FILES}")

	# Framework is called 'OgreOfsPlugin'
	set_target_properties(OgreOfsPlugin PROPERTIES OUTPUT_NAME OgreOfsPlugin)
	# Set the INSTALL_PATH so that Frameworks can be local
	set_target_properties(OgreOfsPlugin PROPERTIES BUILD_WITH_INSTALL_RPATH 1 INSTALL_NAME_DIR "@executable_path/../Frameworks")
endif(APPLE)

if(WIN32)
	add_custom_command(TARGET OgreOfsPlugin PRE_BUILD
		COMMAND if not exist ..\\RunPath\\bin mkdir ..\\RunPath\\bin)
	add_custom_command(TARGET OgreOfsPlugin POST_BUILD
		COMMAND copy \"$(TargetPath)\" ..\\RunPath\\bin)

	install(TARGETS OgreOfsPlugin RUNTIME DESTINATION bin CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
endif(WIN32)

IF(UNIX AND NOT APPLE)

    set_target_properties( OgreOfsPlugin PROPERTIES OUTPUT_NAME "OgreOfsPlugin" VERSION
    ${OGITOR_VERSION_STRING} SOVERSION ${abi_version} )

    if(OGITOR_DIST)
        install(TARGETS OgreOfsPlugin
            LIBRARY DESTINATION ${OGRE_PLUGIN_DIR_LINUX}
            CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
    else(OGITOR_DIST)
        install(TARGETS OgreOfsPlugin
            LIBRARY DESTINATION lib
            CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
    endif(OGITOR_DIST)

ENDIF(UNIX AND NOT APPLE)

if (APPLE)
	install(TARGETS OgreOfsPlugin
            LIBRARY DESTINATION lib
            FRAMEWORK DESTINATION "~/Library/Frameworks"
            CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
endif(APPLE)

# vim: set sw=2 ts=2 noet:
