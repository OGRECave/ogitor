cmake_minimum_required(VERSION 2.8.11)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
cmake_policy(SET CMP0003 NEW)

project(OgAngelDialogManager)

find_package(Qt5Svg REQUIRED)

if(NOT Qt5Svg_FOUND)
	message(SEND_ERROR "Failed to find Qt 5.0.0 or greater.")
else(NOT Qt5Svg_FOUND)
	set(CMAKE_PACKAGE_QTGUI TRUE)

	file(GLOB UI_FILES *.ui)

	set(HEADER_FILES
		dialog.hxx
		inputwidgets.hxx
		AngelDialogManagerPlugin.h
	)

	set(SOURCE_FILES
		dialog.cpp
		inputwidgets.cpp
		AngelDialogManagerPlugin.cpp
		)

	set(MOC_SRCS2
		dialog.hxx
		inputwidgets.hxx
	)

	qt5_wrap_cpp(MOC_SRCS ${MOC_SRCS2})
	qt5_wrap_ui(UI_SRCS ${UI_FILES})

	source_group("Moc Files" FILES ${MOC_SRCS})
	source_group("Ui Files" FILES ${UI_FILES})

	qt5_add_resources(RESOURCE_SRCS ./res/resource.qrc)
	source_group("Qrc Files" FILES ${RESOURCE_SRCS})

	# if we don't include this CMake will not include ui headers properly:
	include_directories(${CMAKE_CURRENT_BINARY_DIR})

	#qt5_add_resources(RESOURCE_SRCS ./res/qtOgitor.qrc)
	#SOURCE_GROUP("Qrc Files" FILES ${RESOURCE_SRCS})

	set(SRCS ${HEADER_FILES} ${SOURCE_FILES} ${MOC_SRCS} ${UI_FILES} ${RESOURCE_SRCS})

	set(OgAngelDialogManager_VERSION 0)
	set(OgAngelDialogManager_VERSION_MAJOR 1)

	include_directories (include)
	include_directories(${DEPENDENCIES_INCLUDES})
	include_directories(${OGITOR_INCLUDES})
	include_directories("../OgAngelScript/")
	message(STATUS "${OGITOR_INCLUDES}")

	include_directories(.)
	LINK_DIRECTORIES(${OGITOR_LIBPATH})
	LINK_DIRECTORIES(${DEPENDENCIES_LIBPATH})

	link_libraries(${QT_QTMAIN_LIBRARY} ${QT_LIBRARIES} ${OGRE_LIBRARIES} Ogitor Angelscript OgAngelScript)
	add_library(OgAngelDialogManager SHARED ${SRCS})
	set(LIBRARY_OUTPUT_PATH ../lib)

    if(NOT OGITOR_DIST)
        set_target_properties(OgAngelDialogManager PROPERTIES INSTALL_RPATH ${OGRE_INSTALL_DIR}/lib/OGRE)
    endif(NOT OGITOR_DIST)

    set_target_properties(OgAngelDialogManager PROPERTIES VERSION ${OgAngelDialogManager_VERSION} SOVERSION ${OgAngelDialogManager_VERSION_MAJOR})
	set_target_properties(OgAngelDialogManager PROPERTIES COMPILE_DEFINITIONS PLUGIN_EXPORT)
	set_target_properties(OgAngelDialogManager PROPERTIES SOLUTION_FOLDER Plugins)

	if(UNIX AND NOT APPLE)
		add_custom_command(TARGET OgAngelDialogManager PRE_BUILD
			COMMAND mkdir -p ../../RunPath/Plugins)
		add_custom_command(TARGET OgAngelDialogManager POST_BUILD
			COMMAND cp -r ../lib/libOgAngelDialogManager.* ../../RunPath/Plugins/)
	endif(UNIX AND NOT APPLE)

	if(WIN32)
		add_custom_command(TARGET OgAngelDialogManager PRE_BUILD
			COMMAND if not exist ..\\..\\RunPath\\Plugins mkdir ..\\..\\RunPath\\Plugins)
		add_custom_command(TARGET OgAngelDialogManager POST_BUILD
			COMMAND copy \"$(TargetPath)\" ..\\..\\RunPath\\Plugins)
		add_custom_command(TARGET OgAngelDialogManager POST_BUILD
			COMMAND if exist ..\\..\\RunPath\\Plugins\\OgAngelDialogManager_d.dll move ..\\..\\RunPath\\Plugins\\OgAngelDialogManager_d.dll ..\\..\\RunPath\\Plugins\\debug)

		install(TARGETS OgAngelDialogManager
			RUNTIME DESTINATION Plugins
			CONFIGURATIONS Debug Release RelWithDebInfo MinSizeRel)
	endif(WIN32)

	IF(UNIX AND NOT APPLE)
		if(OGITOR_DIST)
			install(TARGETS OgAngelDialogManager
				LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/ogitor
				CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
		endif(OGITOR_DIST)
	ENDIF(UNIX AND NOT APPLE)

endif(NOT Qt5Svg_FOUND)

# vim: set sw=2 ts=2 noet:
