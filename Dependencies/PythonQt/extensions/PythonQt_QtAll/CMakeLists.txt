cmake_minimum_required(VERSION 2.6)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
cmake_policy(SET CMP0003 NEW)

project(PythonQt_QtAll)

set(QT_MIN_VERSION "4.5.0")
find_package(Qt4 COMPONENTS QtSvg REQUIRED)

if(NOT QT4_FOUND)
    message(SEND_ERROR "Failed to find Qt 4.5 or greater.")
else(NOT QT4_FOUND)

    include(${QT_USE_FILE})

    set(HEADER_FILES
        PythonQt_QtAll.h
        ../../generated_cpp/com_trolltech_qt_core/com_trolltech_qt_core0.h
        ../../generated_cpp/com_trolltech_qt_core/com_trolltech_qt_core1.h
        ../../generated_cpp/com_trolltech_qt_core/com_trolltech_qt_core2.h
        ../../generated_cpp/com_trolltech_qt_gui/com_trolltech_qt_gui0.h
        ../../generated_cpp/com_trolltech_qt_gui/com_trolltech_qt_gui1.h
        ../../generated_cpp/com_trolltech_qt_gui/com_trolltech_qt_gui2.h
        ../../generated_cpp/com_trolltech_qt_gui/com_trolltech_qt_gui3.h
        ../../generated_cpp/com_trolltech_qt_gui/com_trolltech_qt_gui4.h
        ../../generated_cpp/com_trolltech_qt_gui/com_trolltech_qt_gui5.h
        ../../generated_cpp/com_trolltech_qt_gui/com_trolltech_qt_gui6.h
        ../../generated_cpp/com_trolltech_qt_gui/com_trolltech_qt_gui7.h
        ../../generated_cpp/com_trolltech_qt_gui/com_trolltech_qt_gui8.h
        ../../generated_cpp/com_trolltech_qt_gui/com_trolltech_qt_gui9.h
        ../../generated_cpp/com_trolltech_qt_gui/com_trolltech_qt_gui10.h
        ../../generated_cpp/com_trolltech_qt_svg/com_trolltech_qt_svg0.h
)


    set(SOURCE_FILES
        PythonQt_QtAll.cpp
        ../../generated_cpp/com_trolltech_qt_core/com_trolltech_qt_core0.cpp
        ../../generated_cpp/com_trolltech_qt_core/com_trolltech_qt_core1.cpp
        ../../generated_cpp/com_trolltech_qt_core/com_trolltech_qt_core2.cpp
        ../../generated_cpp/com_trolltech_qt_core/com_trolltech_qt_core_init.cpp
        ../../generated_cpp/com_trolltech_qt_gui/com_trolltech_qt_gui0.cpp
        ../../generated_cpp/com_trolltech_qt_gui/com_trolltech_qt_gui1.cpp
        ../../generated_cpp/com_trolltech_qt_gui/com_trolltech_qt_gui2.cpp
        ../../generated_cpp/com_trolltech_qt_gui/com_trolltech_qt_gui3.cpp
        ../../generated_cpp/com_trolltech_qt_gui/com_trolltech_qt_gui4.cpp
        ../../generated_cpp/com_trolltech_qt_gui/com_trolltech_qt_gui5.cpp
        ../../generated_cpp/com_trolltech_qt_gui/com_trolltech_qt_gui6.cpp
        ../../generated_cpp/com_trolltech_qt_gui/com_trolltech_qt_gui7.cpp
        ../../generated_cpp/com_trolltech_qt_gui/com_trolltech_qt_gui8.cpp
        ../../generated_cpp/com_trolltech_qt_gui/com_trolltech_qt_gui9.cpp
        ../../generated_cpp/com_trolltech_qt_gui/com_trolltech_qt_gui10.cpp
        ../../generated_cpp/com_trolltech_qt_gui/com_trolltech_qt_gui_init.cpp
        ../../generated_cpp/com_trolltech_qt_svg/com_trolltech_qt_svg0.cpp
        ../../generated_cpp/com_trolltech_qt_svg/com_trolltech_qt_svg_init.cpp
    )

    link_libraries(${QT_QTMAIN_LIBRARY} ${QT_LIBRARIES} PythonQt)

    add_library(PythonQt_QtAll SHARED ${HEADER_FILES} ${SOURCE_FILES}  )
    SET(LIBRARY_OUTPUT_PATH  ../../../lib)

    set_target_properties(PythonQt_QtAll PROPERTIES
    COMPILE_DEFINITIONS PYTHONQT_QTALL_EXPORTS
    )

    IF(WIN32)
        add_custom_command( TARGET PythonQt_QtAll PRE_BUILD
            COMMAND if not exist ..\\..\\..\\..\\RunPath\\bin mkdir ..\\..\\..\\..\\RunPath\\bin )
        add_custom_command( TARGET PythonQt_QtAll POST_BUILD
            COMMAND copy \"$(TargetPath)\" ..\\..\\..\\..\\RunPath\\bin )
    ENDIF(WIN32)

    set_target_properties(PythonQt_QtAll PROPERTIES FOLDER Dependencies) 

    target_link_libraries(PythonQt_QtAll PythonQt)

    IF(WIN32)
    install(TARGETS PythonQt_QtAll
        RUNTIME DESTINATION bin
        CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
    ENDIF(WIN32)

    IF(UNIX AND NOT APPLE)
        install(TARGETS PythonQt_QtAll
            LIBRARY DESTINATION lib
            CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
    ENDIF(UNIX AND NOT APPLE)

endif(NOT QT4_FOUND)
