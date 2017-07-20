# .compile object code that is used by all executables (genLibrary and errors)
# .compile all used executables
# .redirect both to bin/ directory

# VARIABLES
CC = gcc
CCFLAGS = -O2 -Wall -Wextra
AUXOBJECTS = ../bin/genLibrary.o ../bin/errors.o
EXECUTABLES = ../bin/link ../bin/const ../bin/filter ../bin/window ../bin/spawn ../bin/controller

# AUXILIARY FUNCTION : change all executables to the /bin folder
define relocate
	mv $(1) ../bin
endef

# MAKE COMMANDS
all: $(AUXOBJECTS) $(EXECUTABLES)

../bin/errors.o: errors.c errors.h
	$(CC) $(CCFLAGS) -c errors.c
	$(call relocate,errors.o)

../bin/genLibrary.o: genLibrary.c genLibrary.h
	$(CC) $(CCFLAGS) -c genLibrary.c
	$(call relocate,genLibrary.o)

../bin/link: link.c $(AUXOBJECTS)
	$(CC) $(CCFLAGS) -o link link.c $(AUXOBJECTS)
	$(call relocate,link)

../bin/const: const.c $(AUXOBJECTS)
	$(CC) $(CCFLAGS) -o const const.c $(AUXOBJECTS)
	$(call relocate,const)

../bin/filter: filter.c $(AUXOBJECTS)
	$(CC) $(CCFLAGS) -o filter filter.c $(AUXOBJECTS)
	$(call relocate,filter)

../bin/window: window.c $(AUXOBJECTS)
	$(CC) $(CCFLAGS) -o window window.c $(AUXOBJECTS)
	$(call relocate,window)

../bin/spawn: spawn.c $(AUXOBJECTS)
	$(CC) $(CCFLAGS) -o spawn spawn.c $(AUXOBJECTS)
	$(call relocate,spawn)

../bin/controller: controller.c $(AUXOBJECTS)
	$(CC) $(CCFLAGS) -o controller controller.c $(AUXOBJECTS)
	$(call relocate,controller)

clean:
	rm $(EXECUTABLES) $(AUXOBJECTS)
