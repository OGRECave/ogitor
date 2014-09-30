
#execute_process(
#  COMMAND	%QTDIR%/bin/qhelpgenerator ogitor.qhp -o ogitor.qch
#  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Help)
#execute_process(
#  COMMAND	%QTDIR%/bin/qcollectiongenerator collection_ogitor.qhcp -o collection_ogitor.qhc
#  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Help)

if(WIN32)
    execute_process(
        COMMAND  %QTDIR%/bin/qhelpgenerator ogitor.qhp -o ogitor.qch
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Help)

    execute_process(
        COMMAND  %QTDIR%/bin/qcollectiongenerator collection_ogitor.qhcp -o collection_ogitor.qhc
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Help)

    install(FILES
        collection_ogitor.qhc
        ogitor.qch
        DESTINATION bin/Help
        CONFIGURATIONS Debug)

    install(FILES
        collection_ogitor.qhc
        ogitor.qch
        DESTINATION bin/Help
        CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
endif(WIN32)

if(UNIX AND NOT APPLE)
    execute_process(
        COMMAND  qhelpgenerator ogitor.qhp -o ogitor.qch
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Help)

    execute_process(
        COMMAND  qcollectiongenerator collection_ogitor.qhcp -o collection_ogitor.qhc
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Help)

    if(OGITOR_DIST)
        install(FILES
            collection_ogitor.qhc
            ogitor.qch
            DESTINATION ${CMAKE_INSTALL_PREFIX}/share/ogitor/Help)
    endif(OGITOR_DIST)
endif(UNIX AND NOT APPLE)

# vim: set sw=2 ts=2 noet:
