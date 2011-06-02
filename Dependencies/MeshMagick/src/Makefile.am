INCLUDES = ${OGRE_CFLAGS} -I$(top_srcdir)/include

lib_LTLIBRARIES = libmeshmagick.la
libmeshmagick_la_SOURCES = \
	MeshMagick.cpp \
	MmEditableBone.cpp \
	MmEditableMesh.cpp \
	MmEditableSkeleton.cpp \
	MmInfoTool.cpp \
	MmInfoToolFactory.cpp \
	MmMeshMergeTool.cpp \
	MmMeshMergeToolFactory.cpp \
	MmMeshUtils.cpp \
	MmOgreEnvironment.cpp \
	MmOptimiseTool.cpp \
	MmOptimiseToolFactory.cpp \
	MmOptionsParser.cpp \
	MmRenameTool.cpp \
	MmRenameToolFactory.cpp \
	MmStatefulMeshSerializer.cpp \
	MmStatefulSkeletonSerializer.cpp \
	MmTool.cpp \
	MmToolManager.cpp \
	MmToolsUtils.cpp \
	MmTransformTool.cpp \
	MmTransformToolFactory.cpp 
libmeshmagick_la_LIBADD = ${OGRE_LIBS}


bin_PROGRAMS = meshmagick
meshmagick_SOURCES = main.cpp 
meshmagick_LDADD = -lmeshmagick
meshmagick_DEPENDENCIES = libmeshmagick.la
