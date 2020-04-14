
#Modified from https://gist.github.com/maxtruxa/4b3929e118914ccef057f8a05c614b0f

# output binary
GAME := platformer
EDTR := editor

# source files
GAMESRCS := \
	bullet.cpp \
	charfills.cpp \
	world.cpp \
	door.cpp \
	effects.cpp \
	endinggate.cpp \
	entity.cpp \
	forcefield.cpp \
	gamelayer.cpp \
	game_main.cpp \
	layer.cpp \
	meta.cpp \
	particles.cpp \
	physicalentities.cpp \
	pickups.cpp \
	player.cpp \
	rain.cpp \
	savepoint.cpp \
	water.cpp \
    canvas.cpp \
    data.cpp \
    gameleveldata.cpp \
    savedata.cpp \
    newtestentity.cpp

EDTRSRCS := \
	meta.cpp \
    world.cpp \
    entity.cpp \
	layer.cpp \
	dummyentity.cpp \
	editables.cpp \
	charfills.cpp \
	editor_main.cpp \
    canvas.cpp \
    data.cpp \
    editorleveldata.cpp

# files included in the tarball generated by 'make dist' (e.g. add LICENSE file)
DISTFILES := $(BIN)

# filename of the tar archive generated by 'make dist'
DISTOUTPUT := $(BIN).tar.gz

# intermediate directory for generated object files
OBJDIR := .o
# intermediate directory for generated dependency files
DEPDIR := .d

# object files, auto generated from source files
GAMEOBJS := $(patsubst %,$(OBJDIR)/%.o,$(basename $(GAMESRCS)))
EDTROBJS := $(patsubst %,$(OBJDIR)/%.o,$(basename $(EDTRSRCS)))
# dependency files, auto generated from source files
GAMEDEPS := $(patsubst %,$(DEPDIR)/%.d,$(basename $(GAMESRCS)))
EDTRDEPS := $(patsubst %,$(DEPDIR)/%.d,$(basename $(EDTRSRCS)))
# compilers (at least gcc and clang) don't create the subdirectories automatically
$(shell mkdir -p $(dir $(GAMEOBJS)) >/dev/null)
$(shell mkdir -p $(dir $(GAMEDEPS)) >/dev/null)

# C compiler
CC := gcc
# C++ compiler
CXX := g++
# linker
LD := g++
# tar
TAR := tar

NOWARN := -Wno-reorder -Wno-narrowing -Wno-sign-compare -Wno-unused-parameter
# C flags
CFLAGS := -std=c11
# C++ flags
CXXFLAGS := -std=c++11
# C/C++ flags
CPPFLAGS := -g -Wall -Wextra -c -I/opt/raylib/src -O3 $(NOWARN)
# linker flags
ifeq ($(OS),Windows_NT)
	LDFLAGS :=  -Wall -I/opt/raylib/src -L/opt/raylib/release/libs/linux -O3 -lraylib -lopengl32 -lgdi32 -lwinmm -Iexternal -lwinpthread -static-libgcc -static-libstdc++ $(NOWARN)
else
	LDFLAGS := -Wall -I/opt/raylib/src -L/opt/raylib/release/libs/linux  -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -O3 $(NOWARN)
endif
# flags required for dependency generation; passed to compilers
DEPFLAGS = -MT $@ -MD -MP -MF $(DEPDIR)/$*.Td

# compile C source files
COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) -c -o $@
# compile C++ source files
COMPILE.cc = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) -c -o $@
# link object files to binary
LINK.o = $(LD) -o $@ $(LDFLAGS) $(LDLIBS)
# precompile step
PRECOMPILE =
# postcompile step
POSTCOMPILE = mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d

all: $(EDTR) $(GAME)

dist: $(DISTFILES)
	$(TAR) -cvzf $(DISTOUTPUT) $^

.PHONY: clean
clean:
	$(RM) -r $(OBJDIR) $(DEPDIR)

.PHONY: distclean
distclean: clean
	$(RM) $(BIN) $(DISTOUTPUT)

.PHONY: install
install:
	@echo no install tasks configured

.PHONY: uninstall
uninstall:
	@echo no uninstall tasks configured

.PHONY: check
check:
	@echo no tests configured

.PHONY: help
help:
	@echo available targets: all dist clean distclean install uninstall check

$(EDTR): $(EDTROBJS)
	$(LD) -o $@ $^ $(LDFLAGS) $(LDLIBS)

$(GAME): $(GAMEOBJS)
	$(LD) -o $@ $^ $(LDFLAGS) $(LDLIBS)

$(OBJDIR)/%.o: %.c
$(OBJDIR)/%.o: %.c $(DEPDIR)/%.d
	$(PRECOMPILE)
	$(COMPILE.c) $<
	$(POSTCOMPILE)

$(OBJDIR)/%.o: %.cpp
$(OBJDIR)/%.o: %.cpp $(DEPDIR)/%.d
	$(PRECOMPILE)
	$(COMPILE.cc) $<
	$(POSTCOMPILE)

$(OBJDIR)/%.o: %.cc
$(OBJDIR)/%.o: %.cc $(DEPDIR)/%.d
	$(PRECOMPILE)
	$(COMPILE.cc) $<
	$(POSTCOMPILE)

$(OBJDIR)/%.o: %.cxx
$(OBJDIR)/%.o: %.cxx $(DEPDIR)/%.d
	$(PRECOMPILE)
	$(COMPILE.cc) $<
	$(POSTCOMPILE)

.PRECIOUS: $(DEPDIR)/%.d
$(DEPDIR)/%.d: ;

-include $(DEPS)



