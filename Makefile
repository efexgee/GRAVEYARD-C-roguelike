# supported targets:
#   all, <default>: update dependencies on .h files, then build game
#   run: build strict, then execute with stderr redirected to errors FIFO
#   console: run error console printing contents of errors FIFO
#   game: just build game without updating dependencies
#   depend: explicitly rebuild dependencies
#   ansic: test compile as ANSI C
#   clean: clean up
#   wtf: print out some implicit rules used by this make file
#   strict: build game, disallowing warnings
#   debug: build game for debugging

# define variable with list of libraries, name of variable is defined by make itself, it uses this in the default rule for linking
LDLIBS = -lcurses -lm

# first target in the file is automatic default, "all" is a traditional name
# this target has no steps, it is just used to make it dependent on all the real targets so they are all built (only one right now)
all: depend game

# this project contains multiple C files, which are dependent on header files that we find using makedepend, so header files  are not listed here
SRCS  = game.c level/level.c mob/mob.c los/los.c colors.c
OBJS := $(patsubst %.c,%.o,$(SRCS))

# this target is dependent on all objects
# the rules for building are taken from the implicit rule from multiple .o files to a file with no extension 
# (link object files and libs, which is where LDLIBS variable is used)
# NOTE that "game" is actually a binary created, so this won't run again if the file "game" is newer than all the .o files
game: $(OBJS)

# in order to build the object files, make will search for implicit rules how to build and it will find the rule from .c to .o (compile to object code)
# so we don't need to specify any targets to build those

# NOTE that other targets like all, depend, clean, ansic do not correspond to actual files created, so they always run if called, because their
# target file is not there
depend:
	makedepend -- -I/usr/local/include -- $(SRCS) > /dev/null 2>&1
	@sed -e '1,/^# DO NOT DELETE/d' < Makefile
strict:
	$(MAKE) CFLAGS="-Werror" game

debug:
	$(MAKE) clean
	$(MAKE) CFLAGS="-g" LDFLAGS="-g" game

# clean up stuff, one step (note steps are tab-indented lines, each of which is executed as shell command in a subprocess using $(SHELL)
# as the executable)
clean:
	rm -f game ansic $(OBJS)

# target for ANSI C compilation, forks another copy of make, running with the additional variable CFLAGS set to options to use for all compiles
# this sub-make builds the target "all" explicitly just to make sure we don't create infinite loop by making "ansic" by mistake
ansic:
	$(MAKE) CFLAGS="-ansi -pedantic" all

# print out some implicit rules used in this file so you can see how variables are used by implicit rules
wtf:
	$(MAKE) --silent -p -f /dev/null | egrep -A 5 \
		-e '^.c.o:' \
		-e '^.c:' \
		-e 'COMPILE.c =' \
		-e 'LINK.c ='

errors:
	mkfifo errors
run: strict errors
	./game 2> errors || reset
# This only checks that "errors" exists, not whether it's a pipe
console: errors
	tail -f errors 

# DO NOT DELETE

game.o: /usr/include/time.h /usr/include/features.h
game.o: /usr/include/stdc-predef.h /usr/include/sys/cdefs.h
game.o: /usr/include/bits/wordsize.h /usr/include/bits/long-double.h
game.o: /usr/include/gnu/stubs.h /usr/include/bits/time.h
game.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
game.o: /usr/include/bits/types/clock_t.h /usr/include/bits/types/time_t.h
game.o: /usr/include/bits/types/struct_tm.h
game.o: /usr/include/bits/types/struct_timespec.h
game.o: /usr/include/bits/types/clockid_t.h /usr/include/bits/types/timer_t.h
game.o: /usr/include/bits/types/struct_itimerspec.h
game.o: /usr/include/bits/types/locale_t.h
game.o: /usr/include/bits/types/__locale_t.h /usr/include/stdlib.h
game.o: /usr/include/bits/libc-header-start.h /usr/include/bits/waitflags.h
game.o: /usr/include/bits/waitstatus.h /usr/include/bits/floatn.h
game.o: /usr/include/bits/floatn-common.h /usr/include/sys/types.h
game.o: /usr/include/bits/stdint-intn.h /usr/include/endian.h
game.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
game.o: /usr/include/bits/byteswap-16.h /usr/include/bits/uintn-identity.h
game.o: /usr/include/sys/select.h /usr/include/bits/select.h
game.o: /usr/include/bits/types/sigset_t.h
game.o: /usr/include/bits/types/__sigset_t.h
game.o: /usr/include/bits/types/struct_timeval.h /usr/include/sys/sysmacros.h
game.o: /usr/include/bits/sysmacros.h /usr/include/bits/pthreadtypes.h
game.o: /usr/include/bits/thread-shared-types.h
game.o: /usr/include/bits/pthreadtypes-arch.h /usr/include/alloca.h
game.o: /usr/include/bits/stdlib-float.h /usr/include/ncurses.h
game.o: /usr/include/ncurses_dll.h /usr/include/stdint.h
game.o: /usr/include/bits/wchar.h /usr/include/bits/stdint-uintn.h
game.o: /usr/include/stdio.h /usr/include/bits/types/__FILE.h
game.o: /usr/include/bits/types/FILE.h /usr/include/bits/libio.h
game.o: /usr/include/bits/_G_config.h /usr/include/bits/types/__mbstate_t.h
game.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
game.o: /usr/include/unctrl.h /usr/include/curses.h /usr/include/string.h
game.o: /usr/include/strings.h level/level.h mob/mob.h mob/mob.h los/los.h
game.o: /usr/include/math.h /usr/include/bits/math-vector.h
game.o: /usr/include/bits/libm-simd-decl-stubs.h
game.o: /usr/include/bits/flt-eval-method.h /usr/include/bits/fp-logb.h
game.o: /usr/include/bits/fp-fast.h
game.o: /usr/include/bits/mathcalls-helper-functions.h
game.o: /usr/include/bits/mathcalls.h /usr/include/bits/iscanonical.h
game.o: /usr/include/assert.h
level/level.o: /usr/include/stdio.h /usr/include/bits/libc-header-start.h
level/level.o: /usr/include/features.h /usr/include/stdc-predef.h
level/level.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
level/level.o: /usr/include/bits/long-double.h /usr/include/gnu/stubs.h
level/level.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
level/level.o: /usr/include/bits/types/__FILE.h
level/level.o: /usr/include/bits/types/FILE.h /usr/include/bits/libio.h
level/level.o: /usr/include/bits/_G_config.h
level/level.o: /usr/include/bits/types/__mbstate_t.h
level/level.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
level/level.o: /usr/include/string.h /usr/include/bits/types/locale_t.h
level/level.o: /usr/include/bits/types/__locale_t.h /usr/include/strings.h
level/level.o: level/level.h /usr/include/stdlib.h
level/level.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
level/level.o: /usr/include/bits/floatn.h /usr/include/bits/floatn-common.h
level/level.o: /usr/include/sys/types.h /usr/include/bits/types/clock_t.h
level/level.o: /usr/include/bits/types/clockid_t.h
level/level.o: /usr/include/bits/types/time_t.h
level/level.o: /usr/include/bits/types/timer_t.h
level/level.o: /usr/include/bits/stdint-intn.h /usr/include/endian.h
level/level.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
level/level.o: /usr/include/bits/byteswap-16.h
level/level.o: /usr/include/bits/uintn-identity.h /usr/include/sys/select.h
level/level.o: /usr/include/bits/select.h /usr/include/bits/types/sigset_t.h
level/level.o: /usr/include/bits/types/__sigset_t.h
level/level.o: /usr/include/bits/types/struct_timeval.h
level/level.o: /usr/include/bits/types/struct_timespec.h
level/level.o: /usr/include/sys/sysmacros.h /usr/include/bits/sysmacros.h
level/level.o: /usr/include/bits/pthreadtypes.h
level/level.o: /usr/include/bits/thread-shared-types.h
level/level.o: /usr/include/bits/pthreadtypes-arch.h /usr/include/alloca.h
level/level.o: /usr/include/bits/stdlib-float.h mob/mob.h
mob/mob.o: /usr/include/stdio.h /usr/include/bits/libc-header-start.h
mob/mob.o: /usr/include/features.h /usr/include/stdc-predef.h
mob/mob.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
mob/mob.o: /usr/include/bits/long-double.h /usr/include/gnu/stubs.h
mob/mob.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
mob/mob.o: /usr/include/bits/types/__FILE.h /usr/include/bits/types/FILE.h
mob/mob.o: /usr/include/bits/libio.h /usr/include/bits/_G_config.h
mob/mob.o: /usr/include/bits/types/__mbstate_t.h
mob/mob.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
mob/mob.o: mob/mob.h /usr/include/stdlib.h /usr/include/bits/waitflags.h
mob/mob.o: /usr/include/bits/waitstatus.h /usr/include/bits/floatn.h
mob/mob.o: /usr/include/bits/floatn-common.h /usr/include/sys/types.h
mob/mob.o: /usr/include/bits/types/clock_t.h
mob/mob.o: /usr/include/bits/types/clockid_t.h
mob/mob.o: /usr/include/bits/types/time_t.h /usr/include/bits/types/timer_t.h
mob/mob.o: /usr/include/bits/stdint-intn.h /usr/include/endian.h
mob/mob.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
mob/mob.o: /usr/include/bits/byteswap-16.h /usr/include/bits/uintn-identity.h
mob/mob.o: /usr/include/sys/select.h /usr/include/bits/select.h
mob/mob.o: /usr/include/bits/types/sigset_t.h
mob/mob.o: /usr/include/bits/types/__sigset_t.h
mob/mob.o: /usr/include/bits/types/struct_timeval.h
mob/mob.o: /usr/include/bits/types/struct_timespec.h
mob/mob.o: /usr/include/sys/sysmacros.h /usr/include/bits/sysmacros.h
mob/mob.o: /usr/include/bits/pthreadtypes.h
mob/mob.o: /usr/include/bits/thread-shared-types.h
mob/mob.o: /usr/include/bits/pthreadtypes-arch.h /usr/include/alloca.h
mob/mob.o: /usr/include/bits/stdlib-float.h
los/los.o: los/los.h /usr/include/math.h
los/los.o: /usr/include/bits/libc-header-start.h /usr/include/features.h
los/los.o: /usr/include/stdc-predef.h /usr/include/sys/cdefs.h
los/los.o: /usr/include/bits/wordsize.h /usr/include/bits/long-double.h
los/los.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
los/los.o: /usr/include/bits/typesizes.h /usr/include/bits/math-vector.h
los/los.o: /usr/include/bits/libm-simd-decl-stubs.h
los/los.o: /usr/include/bits/floatn.h /usr/include/bits/floatn-common.h
los/los.o: /usr/include/bits/flt-eval-method.h /usr/include/bits/fp-logb.h
los/los.o: /usr/include/bits/fp-fast.h
los/los.o: /usr/include/bits/mathcalls-helper-functions.h
los/los.o: /usr/include/bits/mathcalls.h /usr/include/bits/iscanonical.h
los/los.o: /usr/include/assert.h /usr/include/stdlib.h
los/los.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
los/los.o: /usr/include/sys/types.h /usr/include/bits/types/clock_t.h
los/los.o: /usr/include/bits/types/clockid_t.h
los/los.o: /usr/include/bits/types/time_t.h /usr/include/bits/types/timer_t.h
los/los.o: /usr/include/bits/stdint-intn.h /usr/include/endian.h
los/los.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
los/los.o: /usr/include/bits/byteswap-16.h /usr/include/bits/uintn-identity.h
los/los.o: /usr/include/sys/select.h /usr/include/bits/select.h
los/los.o: /usr/include/bits/types/sigset_t.h
los/los.o: /usr/include/bits/types/__sigset_t.h
los/los.o: /usr/include/bits/types/struct_timeval.h
los/los.o: /usr/include/bits/types/struct_timespec.h
los/los.o: /usr/include/sys/sysmacros.h /usr/include/bits/sysmacros.h
los/los.o: /usr/include/bits/pthreadtypes.h
los/los.o: /usr/include/bits/thread-shared-types.h
los/los.o: /usr/include/bits/pthreadtypes-arch.h /usr/include/alloca.h
los/los.o: /usr/include/bits/stdlib-float.h /usr/include/stdio.h
los/los.o: /usr/include/bits/types/__FILE.h /usr/include/bits/types/FILE.h
los/los.o: /usr/include/bits/libio.h /usr/include/bits/_G_config.h
los/los.o: /usr/include/bits/types/__mbstate_t.h
los/los.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
colors.o: /usr/include/ncurses.h /usr/include/ncurses_dll.h
colors.o: /usr/include/stdint.h /usr/include/bits/libc-header-start.h
colors.o: /usr/include/features.h /usr/include/stdc-predef.h
colors.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
colors.o: /usr/include/bits/long-double.h /usr/include/gnu/stubs.h
colors.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
colors.o: /usr/include/bits/wchar.h /usr/include/bits/stdint-intn.h
colors.o: /usr/include/bits/stdint-uintn.h /usr/include/stdio.h
colors.o: /usr/include/bits/types/__FILE.h /usr/include/bits/types/FILE.h
colors.o: /usr/include/bits/libio.h /usr/include/bits/_G_config.h
colors.o: /usr/include/bits/types/__mbstate_t.h /usr/include/bits/stdio_lim.h
colors.o: /usr/include/bits/sys_errlist.h /usr/include/unctrl.h
colors.o: /usr/include/curses.h
