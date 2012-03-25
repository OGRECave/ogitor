cmake_minimum_required(VERSION 2.6)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
cmake_policy(SET CMP0003 NEW)

project(OgScriptEditor)

set(QT_MIN_VERSION "4.5.0")
find_package(Qt4 COMPONENTS QtSvg REQUIRED)

if(NOT QT4_FOUND)
	message(SEND_ERROR "Failed to find Qt 4.5 or greater.")
else(NOT QT4_FOUND)
	include(${QT_USE_FILE})
	set(CMAKE_PACKAGE_QTGUI TRUE)

	file(GLOB UI_FILES *.ui)

	set(HEADER_FILES
		scriptview.hxx
		scripthighlighter.hxx
		Plugin.h
		scripttexteditorcodec.hxx
	)

	set(SOURCE_FILES
		scriptview.cpp
		scripthighlighter.cpp
		Plugin.cpp
		scripttexteditorcodec.cpp
	)

	SET(MOC_SRCS2
		scriptview.hxx
		scripthighlighter.hxx
		scripttexteditorcodec.hxx
	)

	qt4_wrap_cpp(MOC_SRCS ${MOC_SRCS2})
	qt4_wrap_ui(UI_SRCS ${UI_FILES})

	source_group("Moc Files" FILES ${MOC_SRCS})
	source_group("Ui Files" FILES ${UI_FILES})

	qt4_add_resources(RESOURCE_SRCS ./res/resource.qrc)
	source_group("Qrc Files" FILES ${RESOURCE_SRCS})

	# if we don't include this CMake will not include ui headers properly:
	include_directories(${CMAKE_CURRENT_BINARY_DIR})

	#qt4_add_resources(RESOURCE_SRCS ./res/qtOgitor.qrc)
	#SOURCE_GROUP("Qrc Files" FILES ${RESOURCE_SRCS})

	set(SRCS ${HEADER_FILES} ${SOURCE_FILES} ${MOC_SRCS} ${UI_FILES} ${RESOURCE_SRCS})

	set(OgScriptEditor_VERSION 0)
	set(OgScriptEditor_VERSION_MAJOR 1)

	include_directories (include)
	include_directories(${DEPENDENCIES_INCLUDES})
	include_directories(${OGITOR_INCLUDES})
	message(STATUS "${OGITOR_INCLUDES}")

	include_directories(.)
	link_directories(${OGITOR_LIBPATH})
	link_directories(${DEPENDENCIES_LIBPATH})

	add_library(OgScriptEditor SHARED ${SRCS})
	target_link_libraries(OgScriptEditor ${QT_QTMAIN_LIBRARY} ${QT_LIBRARIES} ${OGRE_LIBRARIES} Ogitor GenericTextEditor)
	set(LIBRARY_OUTPUT_PATH ../lib)

    if(NOT OGITOR_DIST)
        set_target_properties(OgScriptEditor PROPERTIES INSTALL_RPATH ${OGRE_INSTALL_DIR}/lib/OGRE)
    endif(NOT OGITOR_DIST)
	
	set_target_properties(OgScriptEditor PROPERTIES VERSION ${OgScriptEditor_VERSION} SOVERSION ${OgScriptEditor_VERSION_MAJOR})
	set_target_properties(OgScriptEditor PROPERTIES COMPILE_DEFINITIONS PLUGIN_EXPORT)
	set_target_properties(OgScriptEditor PROPERTIES SOLUTION_FOLDER Plugins)

	IF(UNIX AND NOT APPLE)
		add_custom_command(TARGET OgScriptEditor PRE_BUILD
			COMMAND mkdir -p ../../RunPath/Plugins)
		add_custom_command(TARGET OgScriptEditor POST_BUILD
			COMMAND cp -r ../lib/libOgScriptEditor.* ../../RunPath/Plugins/)
	ENDIF(UNIX AND NOT APPLE)

	if(WIN32)
		add_custom_command(TARGET OgScriptEditor PRE_BUILD
			COMMAND if not exist ..\\..\\RunPath\\Plugins mkdir ..\\..\\RunPath\\Plugins)
		add_custom_command(TARGET OgScriptEditor POST_BUILD
			COMMAND copy \"$(TargetPath)\" ..\\..\\RunPath\\Plugins)
		add_custom_command(TARGET OgScriptEditor POST_BUILD
			COMMAND if exist ..\\..\\RunPath\\Plugins\\OgScriptEditor_d.dll move ..\\..\\RunPath\\Plugins\\OgScriptEditor_d.dll ..\\..\\RunPath\\Plugins\\debug)

		install(TARGETS OgScriptEditor
			RUNTIME DESTINATION Plugins
			CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
	endif(WIN32)

    IF(UNIX AND NOT APPLE)
        if(OGITOR_DIST)
            install(TARGETS OgScriptEditor
                LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/share/qtOgitor/Plugins
                CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
        endif(OGITOR_DIST)
    ENDIF(UNIX AND NOT APPLE)
endif(NOT QT4_FOUND)

# vim: set sw=2 ts=2 noet:
