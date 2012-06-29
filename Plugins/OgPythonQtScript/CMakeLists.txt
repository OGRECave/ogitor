cmake_minimum_required(VERSION 2.6)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
cmake_policy(SET CMP0003 NEW)

project(OgPythonQtScript)

set(QT_MIN_VERSION "4.5.0")
find_package(Qt4 REQUIRED)

if(NOT QT4_FOUND)
    message(SEND_ERROR "Failed to find Qt 4.5 or greater.")
else(NOT QT4_FOUND)
    include(${QT_USE_FILE})
    set(CMAKE_PACKAGE_QTGUI TRUE)

    set(HEADER_FILES
        PythonqtInterpreter.h
        PythonqtScriptrunner.h
    )

    set(SOURCE_FILES
        PythonqtInterpreter.cpp
        PythonqtScriptrunner.cpp
    )

    set(MOC_SRCS2
        PythonqtScriptrunner.h
    )

    qt4_wrap_cpp(MOC_SRCS ${MOC_SRCS2})

    set(OgPythonQtScript_VERSION 0)
    set(OgPythonQtScript_VERSION_MAJOR 1)

    include_directories(include)

    if(UNIX AND NOT APPLE)
        # Extra Qt include directories for our braindead gcc. :)
        include_directories(/usr/include/qt4)
    endif(UNIX AND NOT APPLE)
    # Extra Qt include directories for our braindead VC10. :)
    include_directories($ENV{QTDIR}/include)
    include_directories(${QT_QTCORE_INCLUDE_DIR})
    include_directories(${QT_QTGUI_INCLUDE_DIR})
    include_directories(${PYTHON_INCLUDE_DIR})

    include_directories(include)
    include_directories(${DEPENDENCIES_INCLUDES})
    include_directories(${OGITOR_INCLUDES})
    message(STATUS "${OGITOR_INCLUDES}")

    include_directories(.)
    link_directories(${OGITOR_LIBPATH})
    link_directories(${DEPENDENCIES_LIBPATH})

    link_libraries( ${OGRE_LIBRARIES} Ogitor PythonQt)
    add_library(OgPythonQtScript SHARED ${HEADER_FILES} ${SOURCE_FILES} ${MOC_SRCS})
    set(LIBRARY_OUTPUT_PATH ../lib)

    add_dependencies(OgPythonQtScript Ogitor)
    add_dependencies(OgPythonQtScript PythonQt)

    if(NOT OGITOR_DIST)
        set_target_properties(OgPythonQtScript PROPERTIES INSTALL_RPATH ${OGRE_INSTALL_DIR}/lib/OGRE)
    endif(NOT OGITOR_DIST)

    set_target_properties(OgPythonQtScript PROPERTIES VERSION ${OgPythonQtScript_VERSION} SOVERSION ${OgPythonQtScript_VERSION_MAJOR})
    set_target_properties(OgPythonQtScript PROPERTIES COMPILE_DEFINITIONS PLUGIN_EXPORT)
    set_target_properties(OgPythonQtScript PROPERTIES FOLDER Plugins) 

    if(UNIX AND NOT APPLE)
        add_custom_command(TARGET OgPythonQtScript PRE_BUILD
            COMMAND mkdir -p ../../RunPath/Plugins)
        add_custom_command(TARGET OgPythonQtScript POST_BUILD
            COMMAND cp -r ../lib/libOgPythonQtScript.* ../../RunPath/Plugins/)
    endif(UNIX AND NOT APPLE)

    if(WIN32)
        add_custom_command(TARGET OgPythonQtScript PRE_BUILD
            COMMAND if not exist ..\\..\\RunPath\\Plugins mkdir ..\\..\\RunPath\\Plugins)
        add_custom_command(TARGET OgPythonQtScript POST_BUILD
            COMMAND copy \"$(TargetPath)\" ..\\..\\RunPath\\Plugins & if exist ..\\..\\RunPath\\Plugins\\OgPythonQtScript_d.dll move ..\\..\\RunPath\\Plugins\\OgPythonQtScript_d.dll ..\\..\\RunPath\\Plugins\\debug)

        install(TARGETS OgPythonQtScript
            RUNTIME DESTINATION Plugins
            CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
    endif(WIN32)

    IF(UNIX AND NOT APPLE)
        if(OGITOR_DIST)
            install(TARGETS OgPythonQtScript
                LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/ogitor
                CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
        endif(OGITOR_DIST)
    ENDIF(UNIX AND NOT APPLE)
endif(NOT QT4_FOUND)

# vim: set sw=2 ts=2 noet:
