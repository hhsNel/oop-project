SRCDIR = src
MODULES = math geometry assets engine combat combat/weapons rendering util input rendering/drm-kms entities entities/monsters world_objects systems audio audio/alsa game
BUILDDIR = build
RESDIR = res
TESTDIR = tests
MANUALTESTDIR = manual-tests

TARGET = isekai-doom
SRC = $(SRCDIR)/main.cpp $(foreach MODULE, $(MODULES), $(wildcard $(SRCDIR)/$(MODULE)/*.cpp))
RES = $(shell find $(RESDIR) -type f)
RES_HEADER = $(SRCDIR)/res.h
TEST_SRCS = $(wildcard $(TESTDIR)/*.cpp)
TEST_EXPS = $(wildcard $(TESTDIR)/*.exp)
MANUAL_TESTS = $(wildcard $(MANUALTESTDIR)/*.cpp)

OBJ = $(SRC:$(SRCDIR)/%.cpp=$(BUILDDIR)/%.o)
RESOBJ = $(RES:$(RESDIR)/%=$(BUILDDIR)/$(RESDIR)/%.o)
TEST_BINS = $(TEST_SRCS:.cpp=.out)
MANUAL_TEST_BINS = $(MANUAL_TESTS:.cpp=.out)

CXX = g++
#CFLAGS = -Wall -Wextra -Werror -Wshadow -Wpedantic -fstack-protector-strong -fPIE -I$(SRCDIR) -D_USE_MATH_DEFINES -g -O3 -march=native -ffast-math
CFLAGS = -Wall -Wextra -Werror -Wshadow -Wpedantic -fstack-protector-strong -fPIE -I$(SRCDIR) -D_USE_MATH_DEFINES -g -O2 -fno-omit-frame-pointer
CXXFLAGS = $(CFLAGS) -std=c++26 -freflection
OCFLAGS += -I binary -O elf64-x86-64 --add-section .note.GNU-stack=/dev/null --set-section-flags .note.GNU-stack=noload,readonly
RES_EXPORT_FLAGS = $(foreach RESFILE, $(RES), -Wl,--export-dynamic-symbol=_binary_$(shell echo '$(RESFILE)' | sed 's/[^a-zA-Z0-9]/_/g')_start -Wl,--export-dynamic-symbol=_binary_$(shell echo '$(RESFILE)' | sed 's/[^a-zA-Z0-9]/_/g')_end)
#LDFLAGS += -pie $(RES_EXPORT_FLAGS) -Wl,-z,relro,-z,now -flto=auto
LDFLAGS += -pie $(RES_EXPORT_FLAGS) -Wl,-z,relro,-z,now -lasound

all: $(TARGET)

$(TARGET): $(OBJ) $(RESOBJ)
	$(CXX) -o $(TARGET) $(OBJ) $(RESOBJ) $(LDFLAGS)

$(BUILDDIR)/res/%.o: $(RESDIR)/% | $(BUILDDIR)
	objcopy $(OCFLAGS) $< $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp $(RES_HEADER) | $(BUILDDIR)
	$(CXX) -o $@ $(CXXFLAGS) -c $<

$(RES_HEADER): $(RES)
	./tools/generate-resources.sh $@ $(RESDIR)

tests/%.out: tests/%.cpp $(filter-out $(BUILDDIR)/main.o, $(OBJ)) $(RESOBJ)
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LDFLAGS)

check: $(TEST_BINS)
	@for script in $(TEST_EXPS); do \
		expect $$script || exit 1; \
	done
	@echo ALL TESTS PASS

manual-tests/%.out: manual-tests/%.cpp $(filter-out $(BUILDDIR)/main.o, $(OBJ)) $(RESOBJ)
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LDFLAGS)

manual-check: $(MANUAL_TEST_BINS)

$(BUILDDIR):
	mkdir -p $(foreach RESMOD, $(shell find $(RESDIR) -type d), $(BUILDDIR)/$(RESMOD)) $(foreach MODULE, $(MODULES), $(BUILDDIR)/$(MODULE))

clean:
	rm -rf $(BUILDDIR)
	rm -f $(RES_HEADER)
	rm -f $(TEST_BINS)
	rm -f $(MANUAL_TEST_BINS)

.PHONY: all clean check manual-check

