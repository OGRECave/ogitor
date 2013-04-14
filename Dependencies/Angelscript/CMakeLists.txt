cmake_minimum_required(VERSION 2.8)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
cmake_policy(SET CMP0003 NEW)

project(Angelscript)

set(ANGELSCRIPT_SOURCE
	source/as_atomic.cpp
	source/as_builder.cpp
	source/as_bytecode.cpp
	source/as_callfunc.cpp
	source/as_compiler.cpp
	source/as_configgroup.cpp
	source/as_context.cpp
	source/as_datatype.cpp
	source/as_gc.cpp
	source/as_generic.cpp
	source/as_globalproperty.cpp
	source/as_memory.cpp
	source/as_module.cpp
	source/as_objecttype.cpp
	source/as_outputbuffer.cpp
	source/as_parser.cpp
	source/as_restore.cpp
	source/as_scriptcode.cpp
	source/as_scriptengine.cpp
	source/as_scriptfunction.cpp
	source/as_scriptnode.cpp
	source/as_scriptobject.cpp
	source/as_string.cpp
	source/as_string_util.cpp
	source/as_thread.cpp
	source/as_tokenizer.cpp
	source/as_typeinfo.cpp
	source/as_variablescope.cpp
)

include(CheckTypeSize)
CHECK_TYPE_SIZE("void*" OGITOR_PTR_SIZE BUILTIN_TYPES_ONLY)
if (OGITOR_PTR_SIZE EQUAL 8)
  set(OGITOR_PLATFORM_X64 TRUE)
else ()
  set(OGITOR_PLATFORM_X64 FALSE)
endif ()

if(OGITOR_PLATFORM_X64)
	if (WIN32)
		list(APPEND ANGELSCRIPT_SOURCE source/as_callfunc_x64_msvc.cpp)
	elseif (UNIX)
		list(APPEND ANGELSCRIPT_SOURCE source/as_callfunc_x64_gcc.cpp)
	endif ()
else()
	list(APPEND ANGELSCRIPT_SOURCE source/as_callfunc_x86.cpp)
endif(OGITOR_PLATFORM_X64)

set(ANGELSCRIPT_HEADERS
	include/angelscript.h
	source/as_builder.h
	source/as_bytecode.h
	source/as_bytecode.h
	source/as_callfunc.h
	source/as_compiler.h
	source/as_config.h
	source/as_configgroup.h
	source/as_context.h
	source/as_datatype.h
	source/as_debug.h
	source/as_generic.h
	source/as_map.h
	source/as_memory.h
	source/as_module.h
	source/as_objecttype.h
	source/as_outputbuffer.h
	source/as_parser.h
	source/as_property.h
	source/as_restore.h
	source/as_scriptcode.h
	source/as_scriptengine.h
	source/as_scriptfunction.h
	source/as_scriptnode.h
	source/as_scriptobject.h
	source/as_symboltable.h
	source/as_string.h
	source/as_string_util.h
	source/as_texts.h
	source/as_thread.h
	source/as_tokendef.h
	source/as_tokenizer.h
	source/as_typeinfo.h
	source/as_variablescope.h
)

set(ANGELSCRIPT_ADDON_HEADERS
	add_on/scriptarray/scriptarray.h
	add_on/scriptany/scriptany.h
	add_on/scriptdictionary/scriptdictionary.h
	add_on/scriptmath/scriptmath.h
	add_on/scriptmath3d/scriptmath3d.h
	add_on/scriptfile/scriptfile.h
	add_on/scriptstdstring/scriptstdstring.h
	add_on/scriptbuilder/scriptbuilder.h
	add_on/scripthelper/scripthelper.h
	add_on/vector3/as_ScriptOgreVector3.h
	add_on/quaternion/as_ScriptOgreQuaternion.h
	add_on/debugger/debugger.h
)

set(ANGELSCRIPT_ADDON_SOURCE
	add_on/scriptarray/scriptarray.cpp
	add_on/scriptany/scriptany.cpp
	add_on/scriptdictionary/scriptdictionary.cpp
	add_on/scriptmath/scriptmath.cpp
	add_on/scriptmath3d/scriptmath3d.cpp
	add_on/scriptfile/scriptfile.h
	add_on/scriptstdstring/scriptstdstring.cpp
	add_on/scriptstdstring/scriptstdstring_utils.cpp
	add_on/scriptbuilder/scriptbuilder.cpp
	add_on/scripthelper/scripthelper.h
	add_on/vector3/as_ScriptOgreVector3.cpp
	add_on/quaternion/as_ScriptOgreQuaternion.cpp
	add_on/debugger/debugger.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/add_on/scriptarray
	${CMAKE_CURRENT_SOURCE_DIR}/add_on/scriptany
	${CMAKE_CURRENT_SOURCE_DIR}/add_on/scriptdictionary
	${CMAKE_CURRENT_SOURCE_DIR}/add_on/scriptmath
	${CMAKE_CURRENT_SOURCE_DIR}/add_on/scriptmath3d
	${CMAKE_CURRENT_SOURCE_DIR}/add_on/scriptstdstring
	${CMAKE_CURRENT_SOURCE_DIR}/add_on/scriptfile
	${CMAKE_CURRENT_SOURCE_DIR}/add_on/scriptbuilder
	${CMAKE_CURRENT_SOURCE_DIR}/add_on/scripthelper
	${CMAKE_CURRENT_SOURCE_DIR}/add_on/debugger
)

add_definitions("-D_CRT_SECURE_NO_WARNINGS -DANGELSCRIPT_EXPORT -D_LIB")

# Fix x64 issues on Linux
if(("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64" OR "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "amd64") AND NOT APPLE)
	add_definitions(-fPIC)
endif()

if( OGITOR_PLATFORM_X64 )
	if (WIN32)
		#enable_language(ASM_MASM) // not needed ?
		ADD_CUSTOM_COMMAND(
		  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/as_callfunc_x64_msvc_asm.obj
		  COMMAND "ml64" /Fo ${CMAKE_CURRENT_BINARY_DIR}/as_callfunc_x64_msvc_asm.obj /c ${CMAKE_CURRENT_SOURCE_DIR}/source/as_callfunc_x64_msvc_asm.asm
		  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/as_callfunc_x64_msvc_asm.asm
		 ) 
		 set(ANGELSCRIPT_SOURCE ${ANGELSCRIPT_SOURCE} ${CMAKE_CURRENT_BINARY_DIR}/as_callfunc_x64_msvc_asm.obj)
	endif()
endif()

ogitor_add_library(Angelscript STATIC ${ANGELSCRIPT_ADDON_SOURCE} ${ANGELSCRIPT_ADDON_HEADERS} ${ANGELSCRIPT_SOURCE} ${ANGELSCRIPT_HEADERS})
set(LIBRARY_OUTPUT_PATH  ../lib)
