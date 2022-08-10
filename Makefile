# Use clang as the C compiler
CC = clang

# Flags to pass to clang:
# -Iinclude tells clang to look for #include files in the "include" folder
# -Wall turns on all warnings
# -g adds filenames and line numbers to the executable for useful stack traces
# -fno-omit-frame-pointer allows stack traces to be generated
#   (take CS 24 for a full explanation)
# -fsanitize=address enables asan
CFLAGS = -Iinclude -Wall -g -fno-omit-frame-pointer -fsanitize=address

# Compiler flag that links the program with the math library
LIB_MATH = -lm
# Compiler flags that link the program with the math and SDL libraries.
# Note that $(...) substitutes a variable's value, so this line is equivalent to
# LIBS = -lm -lSDL2 -lSDL2_gfx
LIBS = $(LIB_MATH) -lm -lSDL2 -lSDL2_gfx -lSDL2_image -lSDL2_ttf -lSDL2_mixer

DEMOS = test background
STUDENT_LIBS = collision forces interaction vector list body scene polygon shape sprite gameplay initialize controls levels menu leaderboard test_util
TEST_LIBS = polygon


# List of compiled .o files corresponding to STUDENT_LIBS, e.g. "out/vector.o".
STUDENT_OBJS = $(addprefix out/,$(STUDENT_LIBS:=.o))
# List of test suite executables, e.g. "bin/test_suite_vector"
#TEST_BINS = $(addprefix bin/test_suite_,$(TEST_LIBS))
# List of demo executables, i.e. "bin/bounce".
DEMO_BINS = $(addprefix bin/,$(DEMOS))
# All executables (the concatenation of TEST_BINS and DEMO_BINS)
BINS = $(DEMO_BINS) $(ANIMATION_BINS)


# ----------------------------------------------------------------------------------

all: $(BINS)

# Any .o file in "out" is built from the corresponding C file.
# Although .c files can be directly compiled into an executable, first building
# .o files reduces the amount of work needed to rebuild the executable.
# For example, if only vector.c was modified since the last build, only vector.o
# gets recompiled, and clang reuses the other .o files to build the executable.


# Unlike "all", this target has a build command.
# "$^" is a special variable meaning "the source files"
# and $@ means "the target file", so the command tells clang
# to compile the source C file into the target .o file.
out/%.o: library/%.c 
	$(CC) -c $(CFLAGS) $^ -o $@
out/%.o: tests/%.c
	$(CC) -c $(CFLAGS) $^ -o $@
out/demo-%.o: demo/%.c # or "demo"; in this case, add "demo-" to the .o filename
	$(CC) -c $(CFLAGS) $^ -o $@


# Builds the demos by linking the necessary .o files.
# Unlike the out/%.o rule, this uses the LIBS flags and omits the -c flag,
# since it is building a full executable.

bin/%: out/demo-%.o out/sdl_wrapper.o out/sdl_window.o out/sdl_draw.o $(STUDENT_OBJS)
	$(CC) $(CFLAGS) $(LIBS) $^ -o $@


# Builds the test suite executables from the corresponding test .o file
# and the library .o files. The only difference from the demo build command
# is that it doesn't link the SDL libraries.
bin/test_suite_%: out/test_suite_%.o out/test_util.o $(STUDENT_OBJS)
	$(CC) $(CFLAGS) $(LIB_MATH) $^ -o $@

bin/%_tests: out/%_tests.o out/test_util.o $(STUDENT_OBJS)
	$(CC) $(CFLAGS) $(LIB_MATH) $^ -o $@


# ---------------------------------------------------------------------------

# Runs the tests.
test: $(TEST_BINS)
	set -e; for f in $(TEST_BINS); do $$f; echo; done

# Removes all compiled files.
clean:
	rm -f out/* bin/*

demos: $(DEMO_BINS)
	set -e; for f in $(DEMO_BINS); do $$f; echo; done


# This special rule tells Make that "all", "clean", and "test" are rules
# that don't build a file.
.PHONY: all clean test

# Tells Make not to delete the .o files after the executable is built
.PRECIOUS: out/%.o out/demo-%.o out/animation-%.o
