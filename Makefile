# supported targets:
#   all, game, <default>: game
#   ansic: test compile as ANSI C
#   clean: clean up

LDLIBS = -lcurses

all: game
clean: 
	rm -f game ansic

.INTERMEDIATE: ansic.o
ansic.o: game.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) -ansi -pedantic -o $@ $<
