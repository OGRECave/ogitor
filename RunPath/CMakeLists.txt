###############################################################
# Install media
###############################################################

# Set media target directory
if(WIN32)
	install(DIRECTORY ${CMAKE_SOURCE_DIR}/RunPath/
		DESTINATION ${PREFIX}
		CONFIGURATIONS Release RelWithDebInfo MinSizeRel
		PATTERN ".svn" EXCLUDE
		PATTERN "CMakeLists.txt" EXCLUDE
		PATTERN "Debug" EXCLUDE
	)

	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
		DESTINATION ${PREFIX}
		CONFIGURATIONS Debug
		PATTERN ".svn" EXCLUDE
		PATTERN "CMakeLists.txt" EXCLUDE
		PATTERN "bin" EXCLUDE
		PATTERN "Debug" EXCLUDE
	)

	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/bin/SampleAppConfig.xml
		${CMAKE_CURRENT_SOURCE_DIR}/bin/SampleAppConfig_dist.xml
		DESTINATION bin
		CONFIGURATIONS Debug
	)
endif(WIN32)

if(UNIX)
	install(DIRECTORY ${CMAKE_SOURCE_DIR}/RunPath/
		DESTINATION ${CMAKE_INSTALL_PREFIX}/share/ogitor
		PATTERN "CMakeLists.txt" EXCLUDE
		PATTERN "Debug" EXCLUDE
        PATTERN "Projects" EXCLUDE
		PATTERN "bin" EXCLUDE
		PATTERN ".xml" EXCLUDE
		PATTERN "Plugins/icon_backup" EXCLUDE
	)

    if(OGITOR_DIST)
        install(FILES
            ${CMAKE_BINARY_DIR}/RunPath/bin/resources.cfg
            # Sample app only installed locally ?
            #${CMAKE_CURRENT_SOURCE_DIR}/bin/SampleAppConfig.xml
            #${CMAKE_CURRENT_SOURCE_DIR}/bin/SampleAppConfig_dist.xml
            DESTINATION ${CMAKE_INSTALL_PREFIX}/share/ogitor
        )
    else(OGITOR_DIST)
        install(FILES ${CMAKE_SOURCE_DIR}/RunPath/bin/SampleAppConfig.xml
            #${CMAKE_CURRENT_SOURCE_DIR}/bin/SampleAppConfig_dist.xml
            DESTINATION bin
        )
    endif(OGITOR_DIST)
endif(UNIX)

if (APPLE)
	file(COPY Plugins/Icons DESTINATION ${CMAKE_BINARY_DIR}/Plugins)
endif(APPLE)
# vim: set sw=2 ts=2 noet:
