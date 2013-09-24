cmake_minimum_required(VERSION 2.8.10)

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
	endif(WIN32)

	TARGET_LINK_LIBRARIES(qtOfs ${QT_QTMAIN_LIBRARY} ${QT_LIBRARIES} OFS)

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
		set(QT_DLL_DIR ${QT_BINARY_DIR})

		install(FILES 
			${QT_DLL_DIR}/QtCore4.dll
			${QT_DLL_DIR}/QtGui4.dll
			${QT_DLL_DIR}/QtSvg4.dll
			${QT_BINARY_DIR}/assistant.exe
			${QT_DLL_DIR}/QtHelp4.dll
			${QT_DLL_DIR}/QtNetwork4.dll
			${QT_DLL_DIR}/QtSql4.dll
			${QT_DLL_DIR}/QtWebKit4.dll
			${QT_DLL_DIR}/QtCLucene4.dll
			${QT_DLL_DIR}/QtXml4.dll
			${QT_DLL_DIR}/QtXmlPatterns4.dll
			${QT_DLL_DIR}/phonon4.dll
			DESTINATION bin
			CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
		install(FILES 
			${QT_DLL_DIR}/QtCored4.dll
			${QT_DLL_DIR}/QtGuid4.dll
			${QT_DLL_DIR}/QtSvgd4.dll
			${QT_BINARY_DIR}/assistant.exe
			${QT_DLL_DIR}/QtHelpd4.dll
			${QT_DLL_DIR}/QtNetworkd4.dll
			${QT_DLL_DIR}/QtSqld4.dll
			${QT_DLL_DIR}/QtWebKitd4.dll
			${QT_DLL_DIR}/QtCLucened4.dll
			${QT_DLL_DIR}/QtXmld4.dll
			${QT_DLL_DIR}/QtXmlPatternsd4.dll
			${QT_DLL_DIR}/phonond4.dll
			DESTINATION bin
			CONFIGURATIONS Debug)
		install(FILES ${QT_PLUGINS_DIR}/iconengines/qsvgicon4.dll 
			DESTINATION bin/iconengines
			CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
		install(FILES ${QT_PLUGINS_DIR}/iconengines/qsvgicond4.dll
			DESTINATION bin/iconengines
			CONFIGURATIONS Debug)
		install(FILES ${QT_PLUGINS_DIR}/sqldrivers/qsqlite4.dll
			DESTINATION bin/sqldrivers
			CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
		install(FILES ${QT_PLUGINS_DIR}/sqldrivers/qsqlited4.dll
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
