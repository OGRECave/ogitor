find_package(OIS REQUIRED)

if(NOT OIS_FOUND)
	message(SEND_ERROR "Failed to find OIS.")
else(NOT OIS_FOUND)

	set(SRCS
		./src/BaseApplication.cpp
		./src/SampleApp.cpp
		./src/DotSceneLoader.cpp
		./include/BaseApplication.h
		./include/SampleApp.h
		./include/DotSceneLoader.h
		./include/rapidxml.hpp
	)

	set(SRCS ${SRCS})

	if(WIN32)
		getenv_path(OGRE_HOME)
		set(OGRE_SAMPLES_INCLUDEPATH ${ENV_OGRE_HOME}/Samples/include)
	endif(WIN32)

 	if(UNIX)
		if(NOT OGRE_ROOT)
			get_filename_component(OGRE_ROOT "${OGRE_INCLUDE_DIRS}/.." ABSOLUTE)
		endif()
                set(OGRE_SAMPLES_INCLUDEPATH
                        ${ENV_OGRE_HOME}/Samples/Common/include
                )
		find_path(OGRE_SAMPLES_INCLUDEPATH SdkTrays.h
			${OGRE_ROOT}/share/OGRE/Samples/Common/include/
 		)
 	endif(UNIX)
 
	if(NOT OGRE_SAMPLES_INCLUDEPATH)
		message(SEND_ERROR "Can't find SdkTrays.h header!"
			"Reinstall Ogre with OGRE_INSTALL_SAMPLES_SOURCE=1")
	endif()

	include_directories(
		./include 
		${OIS_INCLUDE_DIRS}
		${OGRE_INCLUDE_DIRS}
		${OGRE_TERRAIN_INCLUDE_DIR}
		${OGRE_OVERLAY_INCLUDE_DIR}
		${Boost_INCLUDE_DIR}
		${OGRE_SAMPLES_INCLUDEPATH}
		${OGRE_INCLUDE_OVERLAY_DIR}
        	${DEPENDENCIES_INCLUDES}
	)

	if(WIN32)
		file(GLOB SampleLoadApp_res ./res/*.rc)
	endif(WIN32)

	if(NOT APPLE)
		ogitor_add_executable(SampleLoadApp WIN32 ${SRCS} ${SampleLoadApp_res})
	else()
		ogitor_add_executable(SampleLoadApp MACOSX_BUNDLE 
			${SRCS} 
			${SampleLoadApp_res})
	endif(NOT APPLE)

	if ((${OGRE_VERSION} VERSION_EQUAL "1.9.0") OR (${OGRE_VERSION} VERSION_GREATER "1.9.0"))
		include_directories(${OGRE_Overlay_INCLUDE_DIR})
		target_link_libraries(SampleLoadApp ${OGRE_Overlay_LIBRARIES})
	endif()

	if(WIN32)
		set_target_properties(SampleLoadApp PROPERTIES DEBUG_POSTFIX _d)
	endif(WIN32)

	if(APPLE)
		find_package(IOKIT REQUIRED)
		find_package(Cocoa REQUIRED)
        	TARGET_LINK_LIBRARIES(SampleLoadApp ${OGRE_LIBRARIES} ${OIS_LIBRARIES} ${OGRE_Terrain_LIBRARY} ${OGRE_Overlay_LIBRARY} PagedGeometry ${Cocoa_LIBRARIES} ${IOKit_LIBRARIES})
    	else()
        	TARGET_LINK_LIBRARIES(SampleLoadApp ${OGRE_LIBRARIES} ${OIS_LIBRARIES} ${OGRE_Terrain_LIBRARY} ${OGRE_Overlay_LIBRARY} PagedGeometry ${Boost_LIBRARIES})
	endif(APPLE)

	if(APPLE)
		install(TARGETS SampleLoadApp BUNDLE DESTINATION ${PREFIX}bin CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
		install(TARGETS SampleLoadApp BUNDLE DESTINATION ${PREFIX}Debug CONFIGURATIONS Debug)
	endif()

	if(WIN32)
		add_custom_command(TARGET SampleLoadApp PRE_BUILD 
			COMMAND if not exist ..\\RunPath\\bin mkdir ..\\RunPath\\bin)
		add_custom_command(TARGET SampleLoadApp POST_BUILD 
			COMMAND copy \"$(TargetPath)\" ..\\RunPath\\bin)

		install(FILES ${OGRE_PLUGIN_DIR_REL}/OIS.dll
			DESTINATION bin
			CONFIGURATIONS Release)
		install(FILES ${OGRE_PLUGIN_DIR_DBG}/OIS_d.dll
			DESTINATION bin
			CONFIGURATIONS Debug)
		install(TARGETS SampleLoadApp
			RUNTIME DESTINATION bin
			CONFIGURATIONS Release)
	endif(WIN32)
	
	if(UNIX AND NOT OGITOR_DIST AND NOT APPLE)
        	install(TARGETS SampleLoadApp
            	RUNTIME DESTINATION bin
            	CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
    endif(UNIX AND NOT OGITOR_DIST AND NOT APPLE)

endif(NOT OIS_FOUND)

# vim: set sw=2 ts=2 noet:
