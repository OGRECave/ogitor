cmake_minimum_required(VERSION 2.8.10)

# CMake policy introduced in relation to Qt 5.1
if("${CMAKE_VERSION}" VERSION_GREATER 2.8.11)   
	cmake_policy(SET CMP0020 OLD)
endif()

project(QtOfs)

find_package(Qt5Widgets REQUIRED)

if(NOT Qt5Widgets_FOUND)
	message(SEND_ERROR "Failed to find Qt 5.0.0 or greater.")
else(NOT Qt5Widgets_FOUND)
	set(CMAKE_PACKAGE_QTGUI TRUE)
	
	file(GLOB UI_FILES *.ui)

	set(SRCS
		./include/filelistwidget.hxx
		./src/filelistwidget.cpp
		./src/main.cpp
		./include/mainwindow.hxx
		./src/mainwindow.cpp
		./src/mainwindowactions.cpp
	)

	SET(MOC_SRCS2 
		./include/filelistwidget.hxx
		./include/mainwindow.hxx
	)

    set(OFS_QT_PREPROCESSOR "")

	qt5_wrap_cpp(MOC_SRCS ${MOC_SRCS2} OPTIONS ${OFS_QT_PREPROCESSOR})
	qt5_wrap_ui(UI_SRCS ${UI_FILES})

	source_group("Moc Files" FILES ${MOC_SRCS})
	source_group("Ui Files" FILES ${UI_FILES})

	# if we don't include this CMake will not include ui headers properly:
	include_directories(${CMAKE_CURRENT_BINARY_DIR})
	include_directories(./include)
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/OFS/include")

	qt5_add_resources(RESOURCE_SRCS ./res/qtOfs.qrc)
	source_group("Qrc Files" FILES ${RESOURCE_SRCS})

	set(SRCS ${SRCS} ${MOC_SRCS} ${UI_FILES})

	if(WIN32)
		file(GLOB qtOfs_res ./res/*.rc)
	endif(WIN32)

    ogitor_add_executable(qtOfs WIN32 ${SRCS} ${RESOURCE_SRCS} ${qtOfs_res})
	
	qt5_use_modules(qtOfs Widgets)

	if(WIN32)
		set_target_properties(qtOfs PROPERTIES DEBUG_POSTFIX _d)
		set_property(TARGET qtOfs PROPERTY WIN32_EXECUTABLE 0) 
	endif(WIN32)

	target_link_libraries(qtOfs ${QT_QTMAIN_LIBRARY} ${QT_LIBRARIES} OFS)

	if(UNIX)
		target_link_libraries(qtOfs optimized
		${Boost_FILESYSTEM_LIBRARY}
		${Boost_SYSTEM_LIBRARY}
		${Boost_REGEX_LIBRARY}
		)
	endif()

	if(WIN32)
		add_custom_command(TARGET qtOfs PRE_BUILD
			COMMAND if not exist ..\\RunPath\\bin mkdir ..\\RunPath\\bin)
		add_custom_command(TARGET qtOfs POST_BUILD
			COMMAND copy \"$(TargetPath)\" ..\\RunPath\\bin)
	endif(WIN32)

	if(UNIX)
		get_target_property(OFSFILE qtOfs LOCATION)
		add_custom_command(TARGET qtOfs POST_BUILD
			COMMAND cp -r ${OFSFILE} ../RunPath/bin/${TARGET})
	endif(UNIX)

	if(WIN32)
		set(QT_DLL_DIR "$ENV{QTDIR}/bin")
        set(QT_PLUGINS_DIR "$ENV{QTDIR}/plugins")

		install(FILES
            ${QT_DLL_DIR}/Qt5Core.dll
			${QT_DLL_DIR}/Qt5Widgets.dll
            ${QT_DLL_DIR}/Qt5Gui.dll
            ${QT_DLL_DIR}/Qt5Svg.dll
            ${QT_DLL_DIR}/assistant.exe
            ${QT_DLL_DIR}/Qt5Help.dll
            ${QT_DLL_DIR}/Qt5Network.dll
            ${QT_DLL_DIR}/Qt5Sql.dll
            ${QT_DLL_DIR}/Qt5WebKit.dll
            ${QT_DLL_DIR}/Qt5CLucene.dll
            ${QT_DLL_DIR}/Qt5Xml.dll
            ${QT_DLL_DIR}/Qt5XmlPatterns.dll
            ${QT_DLL_DIR}/icudt51.dll
            ${QT_DLL_DIR}/icuin51.dll
            ${QT_DLL_DIR}/icuuc51.dll
            DESTINATION bin
            CONFIGURATIONS Release)

        install(FILES
            ${QT_DLL_DIR}/Qt5Cored.dll
			${QT_DLL_DIR}/Qt5Widgetsd.dll
            ${QT_DLL_DIR}/Qt5Guid.dll
            ${QT_DLL_DIR}/Qt5Svgd.dll
            ${QT_DLL_DIR}/assistant.exe
            ${QT_DLL_DIR}/Qt5Helpd.dll
            ${QT_DLL_DIR}/Qt5Networkd.dll
            ${QT_DLL_DIR}/Qt5Sqld.dll
            ${QT_DLL_DIR}/Qt5WebKitd.dll
            ${QT_DLL_DIR}/Qt5CLucened.dll
            ${QT_DLL_DIR}/Qt5Xmld.dll
            ${QT_DLL_DIR}/Qt5XmlPatternsd.dll
            ${QT_DLL_DIR}/icudt51.dll
            ${QT_DLL_DIR}/icuin51.dll
            ${QT_DLL_DIR}/icuuc51.dll
            DESTINATION bin
            CONFIGURATIONS Debug)

		install(FILES ${QT_PLUGINS_DIR}/iconengines/qsvgicon.dll 
			DESTINATION bin/iconengines
			CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
		install(FILES ${QT_PLUGINS_DIR}/iconengines/qsvgicond.dll
			DESTINATION bin/iconengines
			CONFIGURATIONS Debug)
		install(FILES ${QT_PLUGINS_DIR}/sqldrivers/qsqlite.dll
			DESTINATION bin/sqldrivers
			CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
		install(FILES ${QT_PLUGINS_DIR}/sqldrivers/qsqlited.dll
			DESTINATION bin/sqldrivers
			CONFIGURATIONS Debug)
		install(TARGETS qtOfs
			RUNTIME DESTINATION bin
			CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
    elseif(UNIX AND NOT APPLE)
        install(TARGETS qtOfs
            RUNTIME DESTINATION bin)
	endif(WIN32)

endif(NOT Qt5Widgets_FOUND)

# vim: set sw=2 ts=2 noet:
