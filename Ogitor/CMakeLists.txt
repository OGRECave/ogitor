include_directories(${DEPENDENCIES_INCLUDES})
include_directories(include)

set(Ogitor_hdrs
	./include/AxisGizmo.h
	./include/BaseEditor.h
	./include/BaseSerializer.h
	./include/BillboardSetEditor.h
	./include/CameraEditor.h
	./include/CameraVisualHelper.h
	./include/AxisGizmo.h
	./include/BaseEditor.h
	./include/BaseSerializer.h
	./include/BillboardSetEditor.h
	./include/CameraEditor.h
	./include/CameraVisualHelper.h
	./include/DefaultEvents.h
	./include/EditableMeshEditor.h
	./include/EntityEditor.h
	./include/Event.h
	./include/EventManager.h
	./include/FolderEditor.h
	./include/LightEditor.h
	./include/LightVisualHelper.h
	./include/MarkerEditor.h
	./include/MultiSelEditor.h
	./include/NodeEditor.h
	./include/OBBoxRenderable.h
	./include/OFSDataStream.h
	./include/OFSSceneSerializer.h
	./include/Ogitors.h
	./include/OgitorsClipboardManager.h
	./include/OgitorsDefinitions.h
	./include/OgitorsDelegates.h
	./include/OgitorsExports.h
	./include/OgitorsView.h
	./include/OgitorsMasterView.h
	./include/OgitorsPhysics.h
	./include/OgitorsPrerequisites.h
	./include/OgitorsProperty.h
	./include/OgitorsRoot.h
	./include/OgitorsScriptConsole.h
	./include/OgitorsScriptInterpreter.h
	./include/OgitorsSingleton.h
	./include/OgitorsSystem.h
	./include/OgitorsUndoManager.h
	./include/OgitorsUtils.h
	./include/OgitorsPaging.h
	./include/OgitorsPagedWorldSection.h
	./include/PagingEditor.h
	./include/PGInstanceManager.h
	./include/PGInstanceEditor.h
	./include/Selection2D.h
	./include/TerrainGroupEditor.h
	./include/TerrainGroupUndo.h
	./include/TerrainMaterialGeneratorB.h
	./include/TerrainPageEditor.h
	./include/ParticleEditor.h
	./include/PlaneEditor.h
	./include/SceneManagerEditor.h
	./include/StringId.h
	./include/TerrainEditor.h
	./include/ViewGrid.h
	./include/ViewportEditor.h
	./include/VisualHelper.h
	./include/tinystr.h
	./include/tinyxml.h
)

set(Ogitor_srcs
	./src/AxisGizmo.cpp
	./src/BaseEditor.cpp
	./src/BillboardSetEditor.cpp
	./src/CameraEditor.cpp
	./src/CameraVisualHelper.cpp
	./src/DefaultEvents.cpp
	./src/EditableMeshEditor.cpp
	./src/EditableMeshEditorEditing.cpp
	./src/EntityEditor.cpp
	./src/EventManager.cpp
	./src/FolderEditor.cpp
	./src/LightEditor.cpp
	./src/LightVisualHelper.cpp
	./src/MarkerEditor.cpp
	./src/MultiSelEditor.cpp
	./src/NodeEditor.cpp
	./src/OBBoxRenderable.cpp
	./src/OFSDataStream.cpp
	./src/OFSSceneSerializer.cpp
	./src/OgitorsClipboardManager.cpp
	./src/OgitorsView.cpp
	./src/OgitorsMasterView.cpp
	./src/OgitorsPhysics.cpp
	./src/OgitorsProperty.cpp
	./src/OgitorsRoot.cpp
	./src/OgitorsRootUtilityFunctions.cpp
	./src/OgitorsRootRegExp.cpp
	./src/OgitorsScriptConsole.cpp
	./src/OgitorsScriptInterpreter.cpp
	./src/OgitorsSystem.cpp
	./src/OgitorsUndoManager.cpp
	./src/OgitorsUtils.cpp
	./src/OgitorsPaging.cpp
	./src/OgitorsPagedWorldSection.cpp
	./src/TerrainGroupEditor.cpp
	./src/TerrainGroupEditorEditing.cpp
	./src/TerrainGroupEditorUpdate.cpp
	./src/TerrainGroupEditorImportExport.cpp
	./src/TerrainGroupUndo.cpp
	./src/TerrainMaterialGeneratorB.cpp
	./src/TerrainPageEditor.cpp
	./src/TerrainPageEditorGrass.cpp
	./src/TerrainPageEditorImportExport.cpp
	./src/ParticleEditor.cpp
	./src/PlaneEditor.cpp
	./src/PGInstanceManager.cpp
	./src/PGInstanceEditor.cpp
	./src/SceneManagerEditor.cpp
	./src/Selection2D.cpp
  ./src/StringId.cpp
	./src/ViewGrid.cpp
	./src/ViewportEditor.cpp
	./src/ViewportEditorEvents.cpp
	./src/VisualHelper.cpp
	./src/tinystr.cpp
	./src/tinyxml.cpp
	./src/tinyxmlerror.cpp
	./src/tinyxmlparser.cpp
)

add_library(Ogitor SHARED ${Ogitor_hdrs} ${Ogitor_srcs})
set(LIBRARY_OUTPUT_PATH ../lib)


if(OGITOR_MESHMAGICK_INTERFACE)
	set(OGITOR_PREPROCESSOR "${OGITOR_PREPROCESSOR},OGITOR_MESHMAGICK_INTERFACE")
endif(OGITOR_MESHMAGICK_INTERFACE)

set_target_properties(Ogitor PROPERTIES COMPILE_DEFINITIONS "${OGITOR_PREPROCESSOR}")

message(STATUS "INFO OGITOR PREPROCESSOR: " ${OGITOR_PREPROCESSOR})

message(STATUS ${OGRE_LIBRARY})
target_link_libraries(Ogitor log4cplus ${OGRE_LIBRARY} ${OGRE_Terrain_LIBRARY} ${OGRE_Paging_LIBRARY} PagedGeometry OFS ${Boost_LIBRARIES})

if(APPLE)
	find_package(CARBON REQUIRED)
	message(STATUS ${Carbon_LIBRARY_FWK})
	target_link_libraries(Ogitor ${Carbon_LIBRARY_FWK})

	# Setup to be a framework
	set_target_properties(Ogitor PROPERTIES FRAMEWORK TRUE)
	set_target_properties(Ogitor PROPERTIES PUBLIC_HEADER "${HEADER_FILES}")

	# Framework is called 'Ogitor'
	set_target_properties(Ogitor PROPERTIES OUTPUT_NAME Ogitor)
	# Set the INSTALL_PATH so that Frameworks can be local
	set_target_properties(Ogitor PROPERTIES BUILD_WITH_INSTALL_RPATH 1 INSTALL_NAME_DIR "@executable_path/../Frameworks")
endif(APPLE)

if(WIN32)
	add_custom_command(TARGET Ogitor PRE_BUILD 
		COMMAND if not exist ..\\RunPath\\bin mkdir ..\\RunPath\\bin)
	add_custom_command(TARGET Ogitor POST_BUILD 
		COMMAND copy \"$(TargetPath)\" ..\\RunPath\\bin)

	install(TARGETS Ogitor RUNTIME DESTINATION bin CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
endif(WIN32)

# vim: set sw=2 ts=2 noet:
