# makefile for building Lua
# see ../INSTALL for installation instructions
# see ../Makefile and luaconf.h for further customization

# == CHANGE THE SETTINGS BELOW TO SUIT YOUR ENVIRONMENT =======================

# Your platform. See PLATS for possible values.
PLAT= none

#CC= gcc
CC= devgcc
CFLAGS=-g -O0 -Wall -DLUA_USE_MKSTEMP -DLUA_USE_APICHECK
AR= ar rcu
RANLIB= ranlib
RM= rm -f
CAT=cat

# == END OF USER SETTINGS. NO NEED TO CHANGE ANYTHING BELOW THIS LINE =========

LUA_A= liblua.a
MEGA_C= mega.c
MEGA_O= mega.o
CORE_C=	lapi.c lcode.c ldebug.c ldo.c ldump.c lfunc.c lgc.c llex.c lmem.c \
	lobject.c lopcodes.c lparser.c lstate.c lstring.c ltable.c ltm.c  \
	lundump.c lvm.c lzio.c
LIB_C= lauxlib.c lbaselib.c ldblib.c liolib.c lmathlib.c loslib.c ltablib.c \
	lstrlib.c loadlib.c linit.c

$(LUA_A): $(MEGA_O)
	$(AR) $@ $?
	$(RANLIB) $@

$(MEGA_C): $(LIB_C) $(CORE_C) 

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: test.c $(LUA_A)
	$(CC) $(CFLAGS) $< $(LUA_A) -lm -o $@

clean:
	$(RM) $(MEGA_O) test

$(MEGA_O): lapi.c lua.h luaconf.h lapi.h lobject.h llimits.h ldebug.h lstate.h
$(MEGA_O): ltm.h lzio.h lmem.h ldo.h lfunc.h lgc.h lstring.h ltable.h lundump.h
$(MEGA_O): lvm.h lcode.c lcode.h llex.h lopcodes.h lparser.h ldebug.c ldo.c
$(MEGA_O): ldump.c lfunc.c lgc.c llex.c lmem.c lobject.c lopcodes.c lparser.c
$(MEGA_O): lstate.c lstring.c ltable.c ltm.c lundump.c lvm.c lzio.c lauxlib.c
$(MEGA_O): lauxlib.h lbaselib.c lualib.h ldblib.c liolib.c lmathlib.c loslib.c
$(MEGA_O): ltablib.c lstrlib.c loadlib.c linit.c
