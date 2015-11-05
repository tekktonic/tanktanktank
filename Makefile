
KEYBOARD = 0
CONTROLLER = 1

INPUT ?= $(KEYBOARD)

CFLAGS = -Wall -Wextra -std=c11 -g -DINPUT=$(INPUT) -DKEYBOARD=$(KEYBOARD) -DCONTROLLER=$(CONTROLLER) -O0 -pedantic
LDFLAGS = -lSDL2 -lSDL2_image -lSDL2_mixer -ljansson -lchipmunk

UNAME_S = $(shell uname -s)

ifeq ($(PROFILE), yes)
	CFLAGS += -pg
	LDFLAGS += -pg
endif

ifeq ($(UNAME_S), Linux)
	LDFLAGS += -lm -lpthread
	CFLAGS += -D_REENTRANT -I/usr/include/SDL2
	BINARY = tanktanktank
else ifeq ("",$(shell echo $(UNAME_S)) | grep -E 'MSYS|MINGW')
	BINARY = tanktanktank
else 
	BINARY = tanktanktank
	CFLAGS += -w -Wl,-subsystem,windows -IC:/Program\ Files\ \(x86\)/Windows\ Kits/8.1/Include/um -IC:/Program\ Files\ \(x86\)/Windows\ Kits/8.1/Include/shared -I/include
		LDFLAGS += -lmingw32 -lSDL2main -lm -L/lib *.dll
endif

OBJS = tanktanktank.o hint.o hashtbl.o obj.o tonic.o room.o player.o jsonutils.o globals.o bullet.o collisiontypes.o enemy.o gun.o viewport.o
all: tanktanktank
tanktanktank: $(OBJS)

clean:
	find . -name "*.o" -exec rm {} \;
	find . -name "*~" -exec rm {} \;
	find . -name "\#*\#" -exec rm {} \;
	find . -name ".\#*" -exec rm {} \;
	find . -name $(BINARY) -exec rm {} \;
