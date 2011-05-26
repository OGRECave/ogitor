###############################################################
# Install media
###############################################################

# Set media target directory
if(WIN32)
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
		DESTINATION ${PREFIX}
		CONFIGURATIONS Release RelWithDebInfo MinSizeRel
		PATTERN ".svn" EXCLUDE
		PATTERN "CMakeLists.txt" EXCLUDE
		PATTERN "Debug" EXCLUDE
		PATTERN "languages" EXCLUDE
	)

	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
		DESTINATION ${PREFIX}
		CONFIGURATIONS Debug
		PATTERN ".svn" EXCLUDE
		PATTERN "CMakeLists.txt" EXCLUDE
		PATTERN "bin" EXCLUDE
		PATTERN "Debug" EXCLUDE
		PATTERN "languages" EXCLUDE
		PATTERN "Plugins/icon_backup" EXCLUDE
	)

	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/bin/plugins_debug.cfg
		${CMAKE_CURRENT_SOURCE_DIR}/bin/resources.cfg
		${CMAKE_CURRENT_SOURCE_DIR}/bin/SampleAppConfig.xml
		${CMAKE_CURRENT_SOURCE_DIR}/bin/SampleAppConfig_dist.xml
		DESTINATION bin
		CONFIGURATIONS Debug
	)
endif(WIN32)

if(UNIX)
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
		DESTINATION ${PREFIX}
		PATTERN ".svn" EXCLUDE
		PATTERN "CMakeLists.txt" EXCLUDE
		PATTERN "Debug" EXCLUDE
		PATTERN "languages" EXCLUDE
		PATTERN "bin" EXCLUDE
		PATTERN "Plugins/icon_backup" EXCLUDE
	)

	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/bin/resources.cfg
		${CMAKE_CURRENT_SOURCE_DIR}/bin/SampleAppConfig.xml
		${CMAKE_CURRENT_SOURCE_DIR}/bin/SampleAppConfig_dist.xml
		DESTINATION bin
  )
endif(UNIX)

# vim: set sw=2 ts=2 noet:
