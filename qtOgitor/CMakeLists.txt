cmake_minimum_required(VERSION 2.8.10)

# CMake policy introduced in relation to Qt 5
if("${CMAKE_VERSION}" VERSION_GREATER 2.8.11)   
	cmake_policy(SET CMP0020 OLD)
endif()

project(QtOgitor)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5LinguistTools)

if(UNIX AND NOT APPLE)
    if(EXISTS /usr/share/kde4/apps/cmake/modules/KDE4Macros.cmake)
        include(/usr/share/kde4/apps/cmake/modules/KDE4Macros.cmake)
        set(HAS_KDE TRUE)
    endif()
    if(EXISTS /usr/share/apps/cmake/modules/KDE4Macros.cmake)
        include(/usr/share/apps/cmake/modules/KDE4Macros.cmake)
        set(HAS_KDE TRUE)
    endif()
endif(UNIX AND NOT APPLE)

if(NOT Qt5Widgets_FOUND)
    message(SEND_ERROR "Failed to find Qt 5.0.0 or greater.")
else(NOT Qt5Widgets_FOUND)
    set(CMAKE_PACKAGE_QTGUI TRUE)
	
	file(GLOB UI_FILES *.ui)

    set(SRCS
        ./include/application.hxx
        ./src/application.cpp
        ./include/aboutdialog.hxx
        ./src/aboutdialog.cpp
        ./include/actiontoolbar.hxx
        ./src/actiontoolbar.cpp
        ./include/addscriptactiondialog.hxx
        ./src/addscriptactiondialog.cpp
        ./include/addtemplatedialog.hxx
        ./src/addtemplatedialog.cpp
        ./include/manageTerrainGraphicsView.hxx
        ./src/manageTerrainGraphicsView.cpp
        ./include/manageTerrainDialog.hxx
        ./src/manageTerrainDialog.cpp
        ./include/calculateblendmapdialog.hxx
        ./src/calculateblendmapdialog.cpp
        ./include/colourpicker.hxx
        ./src/colourpicker.cpp
        ./include/complexproperties.hxx
        ./src/complexproperties.cpp
        ./include/createterraindialog.hxx
        ./src/createterraindialog.cpp
        ./include/entityview.hxx
        ./src/entityview.cpp
        ./include/eucliddialog.hxx
        ./src/eucliddialog.cpp
	    ./include/extendedTreeView.hxx
	    ./src/extendedTreeView.cpp
        ./include/importheightmapdialog.hxx
        ./src/importheightmapdialog.cpp
        ./include/layerview.hxx
        ./src/layerview.cpp
        ./include/lineeditwithhistory.hxx
        ./src/lineeditwithhistory.cpp
        ./src/main.cpp
        ./include/mainwindow.hxx
        ./src/mainwindow.cpp
        ./src/mainwindowactions.cpp
        ./include/magickwidget.hxx
        ./src/magickwidget.cpp
        ./include/objectsview.hxx
        ./src/objectsview.cpp
        ./include/ofstreewidget.hxx
        ./src/ofstreewidget.cpp
        ./include/ogitorpreferenceswidget.hxx
        ./src/ogitorpreferenceswidget.cpp
        ./include/ogrewidget.hxx
        ./src/ogrewidget.cpp
        ./include/paths.hxx
        ./src/paths.cpp
        ./include/preferencesmanager.hxx
        ./src/preferencesmanager.cpp
        ./include/projectfilesview.hxx
        ./src/projectfilesview.cpp
        ./include/propertiesviewcustom.hxx
        ./src/propertiesviewcustom.cpp
        ./include/propertiesviewgeneral.hxx
        ./src/propertiesviewgeneral.cpp
        ./include/qtogitorsystem.hxx
        ./src/qtogitorsystem.cpp
        ./include/sceneview.hxx
        ./src/sceneview.cpp
        ./include/settingsdialog.hxx
        ./src/settingsdialog.cpp
        ./include/shortcuts.hxx
        ./src/shortcuts.cpp
        ./include/shortcutsettings.hxx
        ./src/shortcutsettings.cpp
        ./include/templateview.hxx
        ./src/templateview.cpp
        ./include/terraintoolswidget.hxx
        ./src/terraintoolswidget.cpp
        ./include/uiterrainsquare.hxx
        ./src/uiterrainsquare.cpp
        ./include/userdefined.hxx
        ./src/userdefined.cpp
    )

    SET(MOC_SRCS2
        ./include/application.hxx
        ./include/aboutdialog.hxx
        ./include/actiontoolbar.hxx
        ./include/addscriptactiondialog.hxx
        ./include/addtemplatedialog.hxx
        ./include/manageTerrainGraphicsView.hxx
        ./include/manageTerrainDialog.hxx
        ./include/calculateblendmapdialog.hxx
        ./include/colourpicker.hxx
        ./include/complexproperties.hxx
        ./include/createterraindialog.hxx
        ./include/entityview.hxx
        ./include/eucliddialog.hxx
	    ./include/extendedTreeView.hxx
        ./include/importheightmapdialog.hxx
        ./include/layerview.hxx
        ./include/lineeditwithhistory.hxx
        ./include/mainwindow.hxx
        ./include/magickwidget.hxx
        ./include/objectsview.hxx
        ./include/ofstreewidget.hxx
        ./include/ogitorpreferenceswidget.hxx
        ./include/ogrewidget.hxx
        ./include/preferencesmanager.hxx
        ./include/projectfilesview.hxx
        ./include/propertiesviewcustom.hxx
        ./include/propertiesviewgeneral.hxx
        ./include/sceneview.hxx
        ./include/settingsdialog.hxx
        ./include/shortcuts.hxx
        ./include/shortcutsettings.hxx
        ./include/templateview.hxx
        ./include/terraintoolswidget.hxx
        ./include/uiterrainsquare.hxx
        ./include/userdefined.hxx
    )
    
    if(Qt5LinguistTools_FOUND)
        SET(QTOGITOR_TS_FILES
            languages/ogitor_da.ts
            languages/ogitor_de.ts
            languages/ogitor_es.ts
            languages/ogitor_fr.ts
            languages/ogitor_it.ts
            languages/ogitor_nl.ts
            languages/ogitor_pt_BR.ts
            languages/ogitor_ru.ts
            languages/ogitor_tr.ts
            languages/ogitor_zh.ts
        )
        qt5_add_translation(QTOGITOR_TS_FILES ${QTOGITOR_TS_FILES})
        add_custom_target(translations DEPENDS ${QTOGITOR_TS_FILES})
    else()
       SET(QTOGITOR_TS_FILES "")
    endif()

    if(OGITOR_MESHMAGICK_INTERFACE)
        set(OGITOR_QT_PREPROCESSOR "${OGITOR_QT_PREPROCESSOR} -DOGITOR_MESHMAGICK_INTERFACE")
    endif(OGITOR_MESHMAGICK_INTERFACE)

    message(STATUS "INFO QTOGITOR PREPROCESSOR: " ${OGITOR_QT_PREPROCESSOR})

    qt5_wrap_cpp(MOC_SRCS ${MOC_SRCS2} OPTIONS ${OGITOR_QT_PREPROCESSOR})
    qt5_wrap_ui(UI_SRCS ${UI_FILES})

    source_group("Moc Files" FILES ${MOC_SRCS})
    source_group("Ui Files" FILES ${UI_FILES})

    # if we don't include this CMake will not include UI headers properly:
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
    include_directories(./include)
    include_directories(${DEPENDENCIES_INCLUDES})

    qt5_add_resources(RESOURCE_SRCS ./res/qtOgitor.qrc)
    source_group("Qrc Files" FILES ${RESOURCE_SRCS})

    set(SRCS ${SRCS} ${MOC_SRCS} ${UI_FILES})

    include_directories(../Ogitor/include
        ./QtPropertyBrowser/src
    )

    #if(WIN32)
        file(GLOB qtOgitor_res ./res/*.rc)
    #endif(WIN32)

    if(NOT APPLE)
        ogitor_add_executable(qtOgitor WIN32 ${SRCS} ${RESOURCE_SRCS} ${qtOgitor_res} ${QTOGITOR_TS_FILES})
    else()
        # The first time we build Ogitor.framework will not exist so create a dummy file if necessary
        find_file(OGITOR_FRAMEWORK_FILE Ogitor.framework "${CMAKE_BINARY_DIR}/Ogitor/lib/Debug/" NO_SYSTEM_ENVIRONMENT_PATH)
        if(NOT OGITOR_FRAMEWORK_FILE)
            file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/Ogitor/lib/Debug/Ogitor.framework")
        endif()
        set(COPY_FRAMEWORKS ${OGRE_LIBRARIES} ${Cg_LIBRARY_FWK} "${CMAKE_BINARY_DIR}/Ogitor/lib/Debug/Ogitor.framework")
        set(COPY_RESOURCES "../Mac/resources.cfg" "../Mac/plugins.cfg" "${RUNPATH}/Media")
        set(COPY_PLUGINS
            ${OGRE_RenderSystem_GL_LIBRARY_DBG}
            ${OGRE_Plugin_ParticleFX_LIBRARY_DBG}
            ${OGRE_Plugin_OctreeSceneManager_LIBRARY_DBG}
            ${OGRE_Plugin_CgProgramManager_LIBRARY_DBG}
            "${RUNPATH}/Plugins/Icons"
        )

        set(COPY_COMPONENTS
            ${OGRE_Terrain_LIBRARY}
            ${OGRE_Paging_LIBRARY}
        )

        # Note: For OSX we have to add resources here as well
        add_executable(qtOgitor MACOSX_BUNDLE
            ${SRCS}
            ${RESOURCE_SRCS}
            ${qtOgitor_res}
            ${QTOGITOR_TS_FILES}
        )
        #add_dependencies(qtOgitor OgCaelum)
        #add_dependencies(qtOgitor OgHydrax)
        #add_dependencies(qtOgitor OgSkyx)
        #add_dependencies(qtOgitor OgModularZone)
        add_dependencies(qtOgitor DotSceneSerializer)
        add_dependencies(qtOgitor OFS)
        add_dependencies(qtOgitor ImageConverter)
        add_dependencies(qtOgitor GenericTextEditor)
        add_dependencies(qtOgitor GenericImageEditor)
        add_dependencies(qtOgitor OgAngelScript)
        include(BundleUtilities)
        get_target_property(qtOgitorLocation qtOgitor LOCATION)
        get_dotapp_dir(${qtOgitorLocation} qtOgitorLocationAppdir)
        set(MakeBundle
            cp ${QtSolutions_PropertyBrowser_BINARY_DIR}/${CMAKE_CFG_INTDIR}/libQtSolutions_PropertyBrowser*.dylib ${OGRE_LIBRARIES}/../../libOgreTerrain*.dylib ${OGRE_LIBRARIES}/../../libOgrePaging*.dylib ${qtOgitorLocationAppdir}/Contents/Components && 
            cp ${OGRE_LIBRARIES}/../../libRenderSystem_GL*.dylib ${OGRE_LIBRARIES}/../../libPlugin_OctreeSceneManager*.dylib ${OGRE_LIBRARIES}/../../libPlugin_ParticleFX*.dylib ${OGRE_Plugin_CgProgramManager_LIBRARY_DBG} ${qtOgitorLocationAppdir}/Contents/Plugins && 
            cp ../lib/${CMAKE_CFG_INTDIR}/libOgreOfsPlugin*.dylib ${qtOgitorLocationAppdir}/Contents/Plugins &&
            cp -r ${OGRE_LIBRARY} ${qtOgitorLocationAppdir}/Contents/Frameworks && 
            cp -r ../lib/${CMAKE_CFG_INTDIR}/Ogitor.framework ${qtOgitorLocationAppdir}/Contents/Frameworks && 
            cp ../Plugins/lib/${CMAKE_CFG_INTDIR}/libOgCaelum*.dylib ../Plugins/lib/${CMAKE_CFG_INTDIR}/libOgHydrax*.dylib ../Plugins/lib/${CMAKE_CFG_INTDIR}/libOgModularZone*.dylib ../Plugins/lib/${CMAKE_CFG_INTDIR}/libOgSkyx*.dylib ../Plugins/lib/${CMAKE_CFG_INTDIR}/libDotSceneSerializer*.dylib ../Dependencies/lib/libGenericTextEditor*.dylib ../Dependencies/lib/libImageConverter*.dylib ../Dependencies/lib/libGenericImageEditor*.dylib ../Dependencies/lib/libOFS*.dylib ${qtOgitorLocationAppdir}/Contents/Plugins/Ogitor && 
            cp -r ../../RunPath/Media ${qtOgitorLocationAppdir}/Contents/Resources && 
            cp -r ../../RunPath/Plugins/Icons ${qtOgitorLocationAppdir}/Contents/Plugins && 
            cp ../../Mac/plugins.cfg ../../Mac/resources.cfg ${qtOgitorLocationAppdir}/Contents/Resources
        )
        ADD_CUSTOM_COMMAND(TARGET qtOgitor POST_BUILD COMMAND mkdir -p ${qtOgitorLocationAppdir}/Contents/Resources && mkdir -p ${qtOgitorLocationAppdir}/Contents/Components && mkdir -p ${qtOgitorLocationAppdir}/Contents/Plugins/Ogitor && mkdir -p ${qtOgitorLocationAppdir}/Contents/Frameworks)
        ADD_CUSTOM_COMMAND(TARGET qtOgitor POST_BUILD COMMAND ${MakeBundle})
    endif(NOT APPLE)
	
    # tell CMake that qtOgitor might depend on project/media
    if(OGITOR_DOWNLOAD_SAMPLEPROJECT)
        add_dependencies(qtOgitor SampleProject)
    endif(OGITOR_DOWNLOAD_SAMPLEPROJECT)
    if(OGITOR_DOWNLOAD_SAMPLEMEDIA)
        add_dependencies(qtOgitor SampleMedia)
    endif(OGITOR_DOWNLOAD_SAMPLEMEDIA)

    if(OGITOR_MESHMAGICK_INTERFACE)
        target_link_libraries(qtOgitor QtPropertyBrowser ${QT_QTMAIN_LIBRARY} ${QT_LIBRARIES} ${OGRE_LIBRARIES} Ogitor meshmagick_shared_lib ImageConverter GenericTextEditor GenericImageEditor TinyXML)
    else(OGITOR_MESHMAGICK_INTERFACE)
        target_link_libraries(qtOgitor QtPropertyBrowser ${QT_QTMAIN_LIBRARY} ${QT_LIBRARIES} ${OGRE_LIBRARIES} Ogitor ImageConverter GenericTextEditor GenericImageEditor TinyXML)
    endif(OGITOR_MESHMAGICK_INTERFACE)

    if(APPLE)
        find_package(Cocoa REQUIRED)
        target_link_libraries(qtOgitor ${Cocoa_LIBRARIES})
    endif()

    if(WIN32)
        set_target_properties(qtOgitor PROPERTIES DEBUG_POSTFIX _d)
		set_property(TARGET qtOgitor PROPERTY WIN32_EXECUTABLE 1)
        target_link_libraries(qtOgitor Qt5::WinMain)
    endif(WIN32)
    if(MSVC_IDE)
        add_custom_command(TARGET qtOgitor PRE_BUILD
            COMMAND if not exist ..\\RunPath\\bin mkdir ..\\RunPath\\bin)
        add_custom_command(TARGET qtOgitor POST_BUILD
            COMMAND copy \"$(TargetPath)\" ..\\RunPath\\bin)
    endif(MSVC_IDE)
	
	qt5_use_modules(qtOgitor Widgets)

    if(UNIX AND NOT APPLE)
        #get_target_property(OGITORFILE qtOgitor LOCATION)
        #add_custom_command(TARGET qtOgitor POST_BUILD
        #    COMMAND cp -r ${OGITORFILE} ../RunPath/bin/${TARGET})

		install(TARGETS qtOgitor
			RUNTIME DESTINATION bin
			CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
    endif(UNIX AND NOT APPLE)

    if(UNIX)
    
        if(OGITOR_DIST)
            install(
                DIRECTORY ${CMAKE_SOURCE_DIR}/qtOgitor/share
                DESTINATION ${PREFIX}
            )

            if(HAS_KDE)
                add_subdirectory(icons)
                kde4_update_iconcache()
            endif(HAS_KDE)

        endif(OGITOR_DIST)
    endif(UNIX)
    
    if(Qt5LinguistTools_FOUND)
        install(FILES ${QTOGITOR_TS_FILES} DESTINATION ${OGITOR_LANGUAGE_INSTALL_PATH})
    endif()

    if(WIN32)
        if(OGITOR_MESHMAGICK_INTERFACE)
            set_target_properties(qtOgitor PROPERTIES
                COMPILE_DEFINITIONS "OGITOR_MESHMAGICK_INTERFACE")
        endif(OGITOR_MESHMAGICK_INTERFACE)

        install(TARGETS qtOgitor
            RUNTIME DESTINATION bin
            CONFIGURATIONS Debug Release RelWithDebInfo MinSizeRel)
        
        if (MSVC)
            configure_file(${Ogitor_SOURCE_DIR}/CMakeModules/VisualStudioUserFile.vcproj.user.in ${CMAKE_CURRENT_BINARY_DIR}/QtOgitor.vcproj.user @ONLY)
            configure_file(${Ogitor_SOURCE_DIR}/CMakeModules/VisualStudioUserFile.vcxproj.user.in ${CMAKE_CURRENT_BINARY_DIR}/QtOgitor.vcxproj.user @ONLY)
        endif ()
    endif(WIN32)
endif(NOT Qt5Widgets_FOUND)

# vim: set sw=2 ts=2 noet:
