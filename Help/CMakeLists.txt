
#execute_process(
#  COMMAND	%QTDIR%/bin/qhelpgenerator ogitor.qhp -o ogitor.qch
#  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Help)
#execute_process(
#  COMMAND	%QTDIR%/bin/qcollectiongenerator collection_ogitor.qhcp -o collection_ogitor.qhc
#  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Help)

if(WIN32)
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

if(UNIX)
	install(FILES
		collection_ogitor.qhc
		ogitor.qch
		DESTINATION bin/Help)
endif(UNIX)

# vim: set sw=2 ts=2 noet:
