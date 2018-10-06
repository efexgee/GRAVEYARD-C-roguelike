# supported targets:
#   all, game, <default>: game
#   ansic: test compile as ANSI C
#   clean: clean up
#   wtf: print out some implicit rules used by this make file

# define variable with list of libraries, name of variable is defined by make itself, it uses this in the default rule for linking
LDLIBS = -lcurses -lm

# first target in the file is automatic default, "all" is a traditional name
# this target has no steps, it is just used to make it dependent on all the real targets so they are all built (only one right now)
all: game

# this executable is dependent on two C files
# in order to build, make will search for implicit rules how to build and it will find the rule from .c to .o (compile to object code)
# and another rule to go from .o to file with no extension (link object files and libs, which is where LDLIBS variable is used)
# NOTE that "game" is actually a binary created, so this won't run again if the file "game" is newer than game.c and level.c
# NOTE that other targets like all, clean, ansic do not correspond to actual files created, so they always run if called, because their
# target file is not there
game: game.c level/level.c level/level.h mob/mob.c mob/mob.h chemistry/chemistry.c chemistry/chemistry.h

# clean up stuff, one step (note steps are tab-indented lines, each of which is executed as shell command in a subprocess using $(SHELL)
# as the executable)
clean:
	rm -f game ansic

# target for ANSI C compilation, forks another copy of make, running with the additional variable CFLAGS set to options to use for all compiles
# this sub-make builds the target "all" explicitly just to make sure we don't create infinite loop by making "ansic" by mistake
ansic:
	$(MAKE) CFLAGS="-ansi -pedantic" all

c18:
	$(MAKE) CFLAGS="-std=c18" all


test_suite: chemistry/chemistry.c tests/chemistry/check_chemistry.c simulation/min_heap.c tests/simulation/check_min_heap.c tests/check_check.c
	$(CC) $^ -lcheck -lm -o $@

# print out some implicit rules used in this file so you can see how variables are used by implicit rules
wtf:
	$(MAKE) --silent -p -f /dev/null | egrep -A 5 \
		-e '^.c.o:' \
		-e '^.c:' \
		-e 'COMPILE.c =' \
		-e 'LINK.c ='
