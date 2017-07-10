CC = gcc
CCFLAGS = -O2 -Wall -Wextra

make:
	$(CC) $(CCFLAGS) -o spawn spawn.c genLibrary.c errors.c
	$(CC) $(CCFLAGS) -o filter filter.c genLibrary.c
	$(CC) $(CCFLAGS) -o window window.c genLibrary.c
	$(CC) $(CCFLAGS) -o const const.c genLibrary.c
	$(CC) $(CCFLAGS) -o controller controller.c genLibrary.c errors.c

