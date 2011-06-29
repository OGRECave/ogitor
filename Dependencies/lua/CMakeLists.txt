CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(Lua)

SET(LUA_MAJOR_VERSION 5)
SET(LUA_MINOR_VERSION 1)
SET(LUA_PATCH_VERSION 4)

SET(LUA_VERSION ${LUA_MAJOR_VERSION}.${LUA_MINOR_VERSION}.${LUA_PATCH_VERSION})
SET(LUA_COMPATIBILITY_VERSION ${LUA_MAJOR_VERSION}.${LUA_MINOR_VERSION}.0)
SET(LUA_SOVERSION ${LUA_MAJOR_VERSION}.${LUA_MINOR_VERSION}.0)

OPTION(WANTS_BUILD_STATIC_LIBRARY "Set to ON to build static library." ON)

SET(LUA_LIBRARY_NAME lua)

SET(LUA_CORE
	${Lua_SOURCE_DIR}/src/lapi.c
	${Lua_SOURCE_DIR}/src/lcode.c
	${Lua_SOURCE_DIR}/src/ldebug.c
	${Lua_SOURCE_DIR}/src/ldo.c
	${Lua_SOURCE_DIR}/src/ldump.c
	${Lua_SOURCE_DIR}/src/lfunc.c
	${Lua_SOURCE_DIR}/src/lgc.c
	${Lua_SOURCE_DIR}/src/llex.c
	${Lua_SOURCE_DIR}/src/lmem.c
	${Lua_SOURCE_DIR}/src/lobject.c
	${Lua_SOURCE_DIR}/src/lopcodes.c
	${Lua_SOURCE_DIR}/src/lparser.c
	${Lua_SOURCE_DIR}/src/lstate.c
	${Lua_SOURCE_DIR}/src/lstring.c
	${Lua_SOURCE_DIR}/src/ltable.c 
	${Lua_SOURCE_DIR}/src/ltm.c 
	${Lua_SOURCE_DIR}/src/lundump.c
	${Lua_SOURCE_DIR}/src/lvm.c
	${Lua_SOURCE_DIR}/src/lzio.c
)

SET(LUA_STANDARD_LIBRARY
	${Lua_SOURCE_DIR}/src/lauxlib.c
	${Lua_SOURCE_DIR}/src/lbaselib.c
	${Lua_SOURCE_DIR}/src/ldblib.c
	${Lua_SOURCE_DIR}/src/liolib.c
	${Lua_SOURCE_DIR}/src/lmathlib.c
	${Lua_SOURCE_DIR}/src/loslib.c
	${Lua_SOURCE_DIR}/src/ltablib.c
	${Lua_SOURCE_DIR}/src/lstrlib.c
	${Lua_SOURCE_DIR}/src/loadlib.c
	${Lua_SOURCE_DIR}/src/linit.c
)

SET(PUBLIC_HEADERS
	${Lua_SOURCE_DIR}/src/lua.h 
	${Lua_SOURCE_DIR}/src/lualib.h
	${Lua_SOURCE_DIR}/src/lauxlib.h
	${Lua_SOURCE_DIR}/src/luaconf.h
)

IF(CMAKE_SYSTEM MATCHES "Linux.*")
	# enable dlopen() and mkstemp() support, etc.
	SET(LUA_C_FLAGS "${LUA_C_FLAGS} -DLUA_USE_LINUX")
	# Lua uses libm's math functions and libdl's dlopen.
	SET(LUA_LINKER_FLAGS "m;dl;${LUA_LINKER_FLAGS}")
ENDIF(CMAKE_SYSTEM MATCHES "Linux.*")

ADD_LIBRARY(lua_library_static STATIC ${PUBLIC_HEADERS} ${LUA_STANDARD_LIBRARY} ${LUA_CORE} ${RESOURCE_FILES})

SET_TARGET_PROPERTIES(lua_library_static PROPERTIES OUTPUT_NAME "lua")
SET_TARGET_PROPERTIES(lua_library_static PROPERTIES COMPILE_FLAGS "${LUA_C_FLAGS}")

IF(UNIX)
	SET_TARGET_PROPERTIES(lua_library_static PROPERTIES
		VERSION ${LUA_VERSION}
		SOVERSION ${LUA_SOVERSION}
	)
ENDIF(UNIX)

ADD_EXECUTABLE(lua_executable ${Lua_SOURCE_DIR}/src/lua.c)
TARGET_LINK_LIBRARIES(lua_executable lua_library_static ${LUA_LINKER_FLAGS})
IF(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
	TARGET_LINK_LIBRARIES(lua_executable m)
ENDIF()
SET_TARGET_PROPERTIES(lua_executable PROPERTIES OUTPUT_NAME "lua")

#ADD_EXECUTABLE(luac 
#	${Lua_SOURCE_DIR}/src/luac.c
#	${Lua_SOURCE_DIR}/src/print.c
#)
#TARGET_LINK_LIBRARIES(luac lua_library_static ${LUA_LINKER_FLAGS})
