project(QtOgitor)
set(QT_MIN_VERSION "4.5.0")
find_package(Qt4 COMPONENTS QtSvg REQUIRED)

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

if(NOT QT4_FOUND)
    message(SEND_ERROR "Failed to find Qt 4.5 or greater.")
else(NOT QT4_FOUND)
    include(${QT_USE_FILE})
    set(CMAKE_PACKAGE_QTGUI TRUE)

    file(GLOB UI_FILES *.ui)

    set(SRCS
        ./include/aboutdialog.hxx
        ./src/aboutdialog.cpp
        ./include/actiontoolbar.hxx
        ./src/actiontoolbar.cpp
        ./include/addscriptactiondialog.hxx
        ./src/addscriptactiondialog.cpp
        ./include/addtemplatedialog.hxx
        ./src/addtemplatedialog.cpp
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
        ./include/paths.h
        ./src/paths.cpp
        ./include/preferencesmanager.hxx
        ./src/preferencesmanager.cpp
        ./include/projectfilesview.hxx
        ./src/projectfilesview.cpp
        ./include/propertiesviewcustom.hxx
        ./src/propertiesviewcustom.cpp
        ./include/propertiesviewgeneral.hxx
        ./src/propertiesviewgeneral.cpp
        ./include/qtogitorsystem.h
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
        ./include/welcome.hxx
        ./src/welcome.cpp
    )

    SET(MOC_SRCS2
        ./include/aboutdialog.hxx
        ./include/actiontoolbar.hxx
        ./include/addscriptactiondialog.hxx
        ./include/addtemplatedialog.hxx
        ./include/manageTerrainDialog.hxx
        ./include/calculateblendmapdialog.hxx
        ./include/colourpicker.hxx
        ./include/complexproperties.hxx
        ./include/createterraindialog.hxx
        ./include/entityview.hxx
        ./include/eucliddialog.hxx
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
        ./include/welcome.hxx
    )

    if(OGITOR_MESHMAGICK_INTERFACE)
        set(OGITOR_QT_PREPROCESSOR "${OGITOR_QT_PREPROCESSOR} -DOGITOR_MESHMAGICK_INTERFACE")
    endif(OGITOR_MESHMAGICK_INTERFACE)

    message(STATUS "INFO QTOGITOR PREPROCESSOR: " ${OGITOR_QT_PREPROCESSOR})

    qt4_wrap_cpp(MOC_SRCS ${MOC_SRCS2} OPTIONS ${OGITOR_QT_PREPROCESSOR})
    qt4_wrap_ui(UI_SRCS ${UI_FILES})

    source_group("Moc Files" FILES ${MOC_SRCS})
    source_group("Ui Files" FILES ${UI_FILES})

    # if we don't include this CMake will not include ui headers properly:
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
    include_directories(./include)
    include_directories(${DEPENDENCIES_INCLUDES})

    qt4_add_resources(RESOURCE_SRCS ./res/qtOgitor.qrc)
    source_group("Qrc Files" FILES ${RESOURCE_SRCS})

    set(SRCS ${SRCS} ${MOC_SRCS} ${UI_FILES})

    include_directories(../Ogitor/include
        ./qtpropertybrowser-2.5/src
    )

    if(WIN32)
        file(GLOB qtOgitor_res ./res/*.rc)
    endif(WIN32)

    if(NOT APPLE)
        add_executable(qtOgitor WIN32 ${SRCS} ${RESOURCE_SRCS} ${qtOgitor_res})
    else()
        # The first time we build Ogitor.framework will not exist so create a dummy file if necessary
        find_file(OGITOR_FRAMEWORK_FILE Ogitor.framework "${CMAKE_BINARY_DIR}/Ogitor/lib/Debug/" NO_SYSTEM_ENVIRONMENT_PATH)
        if(NOT OGITOR_FRAMEWORK_FILE)
            file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/Ogitor/lib/Debug/Ogitor.framework")
        endif()

        set(COPY_FRAMEWORKS ${OGRE_LIBRARIES} ${Cg_LIBRARY_FWK} "${CMAKE_BINARY_DIR}/Ogitor/lib/Debug/Ogitor.framework")
        set(COPY_RESOURCES "${MAC_PATH}/resources.cfg" "${MAC_PATH}/plugins.cfg" "${RUNPATH}/Media")
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
        )
        add_dependencies(qtOgitor OgCaelum)
        add_dependencies(qtOgitor OgHydrax)
        add_dependencies(qtOgitor OgSkyx)
        add_dependencies(qtOgitor OgModularZone)
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
            cp ${QtSolutions_PropertyBrowser_BINARY_DIR}/${CMAKE_CFG_INTDIR}/libQtSolutions_PropertyBrowser*.dylib ${OGRE_Terrain_LIBRARY_DBG} ${OGRE_Paging_LIBRARY_DBG} ${qtOgitorLocationAppdir}/Contents/Components && 
            cp ${OGRE_RenderSystem_GL_LIBRARY_DBG} ${OGRE_Plugin_OctreeSceneManager_LIBRARY_DBG} ${OGRE_Plugin_ParticleFX_LIBRARY_DBG} ${OGRE_Plugin_CgProgramManager_LIBRARY_DBG} ${qtOgitorLocationAppdir}/Contents/Plugins && 
            cp -r /Library/Frameworks/OGRE.framework ${qtOgitorLocationAppdir}/Contents/Frameworks && 
            cp -r ../lib/${CMAKE_CFG_INTDIR}/Ogitor.framework ${qtOgitorLocationAppdir}/Contents/Frameworks && 
            cp ../Plugins/lib/${CMAKE_CFG_INTDIR}/libOgCaelum*.dylib ../Plugins/lib/${CMAKE_CFG_INTDIR}/libOgHydrax*.dylib ../Plugins/lib/${CMAKE_CFG_INTDIR}/libOgModularZone*.dylib ../Plugins/lib/${CMAKE_CFG_INTDIR}/libOgSkyx*.dylib ../Plugins/lib/${CMAKE_CFG_INTDIR}/libDotSceneSerializer*.dylib ${qtOgitorLocationAppdir}/Contents/Plugins/Ogitor && 
            cp -r ../RunPath/Media ${qtOgitorLocationAppdir}/Contents/Resources && 
            cp -r ../RunPath/Plugins/Icons ${qtOgitorLocationAppdir}/Contents/Plugins && 
            cp ../Mac/plugins.cfg ../Mac/resources.cfg ${qtOgitorLocationAppdir}/Contents/Resources
        )
        ADD_CUSTOM_COMMAND(TARGET qtOgitor POST_BUILD COMMAND mkdir -p ${qtOgitorLocationAppdir}/Contents/Resources && mkdir -p ${qtOgitorLocationAppdir}/Contents/Components && mkdir -p ${qtOgitorLocationAppdir}/Contents/Plugins/Ogitor && mkdir -p ${qtOgitorLocationAppdir}/Contents/Frameworks)
        ADD_CUSTOM_COMMAND(TARGET qtOgitor POST_BUILD COMMAND ${MakeBundle})
    endif(NOT APPLE)

    if(WIN32)
        set_target_properties(qtOgitor PROPERTIES DEBUG_POSTFIX _d)
    endif(WIN32)

    if(OGITOR_MESHMAGICK_INTERFACE)
        TARGET_LINK_LIBRARIES(qtOgitor QtSolutions_PropertyBrowser ${QT_QTMAIN_LIBRARY} ${QT_LIBRARIES} ${OGRE_LIBRARIES} ${OGRE_Paging_LIBRARY} ${OGRE_Terrain_LIBRARY} Ogitor meshmagick_shared_lib ImageConverter GenericTextEditor GenericImageEditor ${Boost_REGEX_LIBRARY})
    else(OGITOR_MESHMAGICK_INTERFACE)
        TARGET_LINK_LIBRARIES(qtOgitor QtSolutions_PropertyBrowser ${QT_QTMAIN_LIBRARY} ${QT_LIBRARIES} ${OGRE_LIBRARIES} ${OGRE_Paging_LIBRARY} ${OGRE_Terrain_LIBRARY} Ogitor ImageConverter GenericTextEditor GenericImageEditor ${Boost_REGEX_LIBRARY})
    endif(OGITOR_MESHMAGICK_INTERFACE)
    
    if(UNIX AND NOT APPLE)
        TARGET_LINK_LIBRARIES(qtOgitor optimized QtSolutions_PropertyBrowser ${Boost_LIBRARIES})
    elseif(APPLE)
        TARGET_LINK_LIBRARIES(qtOgitor QtSolutions_PropertyBrowser)
    elseif(WIN32)
        TARGET_LINK_LIBRARIES(qtOgitor optimized QtSolutions_PropertyBrowser)
    endif()

    if(UNIX AND NOT APPLE)
        target_link_libraries(qtOgitor optimized QtSolutions_PropertyBrowser ${Boost_LIBRARIES})
    elseif(APPLE)
        target_link_libraries(qtOgitor QtSolutions_PropertyBrowser)
    elseif(WIN32)
        target_link_libraries(qtOgitor optimized QtSolutions_PropertyBrowser)
    endif()

    if(APPLE)
        install(TARGETS qtOgitor BUNDLE DESTINATION ${PREFIX}bin CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
        install(TARGETS qtOgitor BUNDLE DESTINATION ${PREFIX}Debug CONFIGURATIONS Debug)
    endif()

    if(WIN32)
        add_custom_command(TARGET qtOgitor PRE_BUILD
            COMMAND if not exist ..\\RunPath\\bin mkdir ..\\RunPath\\bin)
        add_custom_command(TARGET qtOgitor POST_BUILD
            COMMAND copy \"$(TargetPath)\" ..\\RunPath\\bin)
    endif(WIN32)

    if(UNIX AND NOT APPLE)
        get_target_property(OGITORFILE qtOgitor LOCATION)
        add_custom_command(TARGET qtOgitor POST_BUILD
            COMMAND cp -r ${OGITORFILE} ../RunPath/bin/${TARGET})
    endif(UNIX AND NOT APPLE)

    if(UNIX AND NOT APPLE)
        install(TARGETS qtOgitor
                RUNTIME DESTINATION bin
                CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
    endif(UNIX AND NOT APPLE)

    if(UNIX AND NOT APPLE)
    
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
        
        INSTALL(CODE "EXECUTE_PROCESS(COMMAND sh ${CMAKE_SOURCE_DIR}/gen-qt-projectfile ${CMAKE_SOURCE_DIR} OUTPUT_FILE ${CMAKE_SOURCE_DIR}/qtOgitor/languages/translate.pro)")
        INSTALL(CODE "EXECUTE_PROCESS(COMMAND lrelease ${CMAKE_SOURCE_DIR}/qtOgitor/languages/translate.pro)")
        INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/qtOgitor/languages
            DESTINATION ${CMAKE_INSTALL_PREFIX}/share/ogitor
            PATTERN     "*.qm"
            PATTERN     "*.ts" EXCLUDE
            PATTERN     "*.pro" EXCLUDE
            PATTERN     "*.exe" EXCLUDE
            PATTERN     "*.bat" EXCLUDE
        )
        INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/RunPath/languages
            DESTINATION ${CMAKE_INSTALL_PREFIX}/share/ogitor
            PATTERN     "qt_*.qm"
            PATTERN     "ogitor_*.qm" EXCLUDE
        )
    else(UNIX AND NOT APPLE)
        # ie Windows
        install(DIRECTORY ${CMAKE_SOURCE_DIR}/RunPath/languages
            DESTINATION ${PREFIX}
            PATTERN "translate.pro" EXCLUDE
            PATTERN "*.ts" EXCLUDE
            PATTERN "*.exe" EXCLUDE
            PATTERN "*.bat" EXCLUDE
        )
    endif(UNIX AND NOT APPLE)

    if(WIN32)
        set(QT_DLL_DIR ${QT_BINARY_DIR})

        if(OGITOR_MESHMAGICK_INTERFACE)
            set_target_properties(qtOgitor PROPERTIES
                COMPILE_DEFINITIONS "QT_QTPROPERTYBROWSER_IMPORT;OGITOR_MESHMAGICK_INTERFACE")
        else(OGITOR_MESHMAGICK_INTERFACE)
            set_target_properties(qtOgitor PROPERTIES
                COMPILE_DEFINITIONS QT_QTPROPERTYBROWSER_IMPORT)
        endif(OGITOR_MESHMAGICK_INTERFACE)

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
            CONFIGURATIONS Release)
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
            CONFIGURATIONS Release)
        install(FILES ${QT_PLUGINS_DIR}/iconengines/qsvgicond4.dll
            DESTINATION bin/iconengines
            CONFIGURATIONS Debug)
        install(FILES ${QT_PLUGINS_DIR}/sqldrivers/qsqlite4.dll
            DESTINATION bin/sqldrivers
            CONFIGURATIONS Release)
        install(FILES ${QT_PLUGINS_DIR}/sqldrivers/qsqlited4.dll
            DESTINATION bin/sqldrivers
            CONFIGURATIONS Debug)
        install(TARGETS qtOgitor
            RUNTIME DESTINATION bin
            CONFIGURATIONS Release)
        
        if (MSVC)
            configure_file(${Ogitor_SOURCE_DIR}/CMakeModules/VisualStudioUserFile.vcproj.user.in ${CMAKE_CURRENT_BINARY_DIR}/QtOgitor.vcproj.user @ONLY)
            configure_file(${Ogitor_SOURCE_DIR}/CMakeModules/VisualStudioUserFile.vcxproj.user.in ${CMAKE_CURRENT_BINARY_DIR}/QtOgitor.vcxproj.user @ONLY)
  endif ()
    endif(WIN32)
endif(NOT QT4_FOUND)

# vim: set sw=2 ts=2 noet:
