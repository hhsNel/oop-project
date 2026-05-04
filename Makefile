SRCDIR = src
MODULES = math geometry graphics engine combat combat/weapons rendering util input rendering/drm-kms entities
BUILDDIR = build
RESDIR = res
TESTDIR = tests
MANUALTESTDIR = manual-tests

TARGET = isekai-doom
SRC = $(foreach MODULE, $(MODULES), $(wildcard $(SRCDIR)/$(MODULE)/*.cpp))
RES = $(wildcard $(RESDIR)/*)
RES_HEADER = $(SRCDIR)/res.h
TEST_SRCS = $(wildcard $(TESTDIR)/*.cpp)
TEST_EXPS = $(wildcard $(TESTDIR)/*.exp)
MANUAL_TESTS = $(wildcard $(MANUALTESTDIR)/*.cpp)

OBJ = $(SRC:$(SRCDIR)/%.cpp=$(BUILDDIR)/%.o)
RESOBJ = $(RES:$(RESDIR)/%=$(BUILDDIR)/res/%.o)
TEST_BINS = $(TEST_SRCS:.cpp=.out)
MANUAL_TEST_BINS = $(MANUAL_TESTS:.cpp=.out)

CXX = g++
#CFLAGS = -Wall -Wextra -Werror -Wshadow -fstack-protector-strong -fPIE -I$(SRCDIR) -D_USE_MATH_DEFINES -march=native -ffast-math -O3 -g -pg
CFLAGS = -Wall -Wextra -Werror -Wshadow -fstack-protector-strong -fPIE -I$(SRCDIR) -D_USE_MATH_DEFINES -O2 -g -pg
CXXFLAGS = $(CFLAGS)
#CXXFLAGS += -std=c++26 -freflection $(CFLAGS)
OCFLAGS += -I binary -O elf64-x86-64 --add-section .note.GNU-stack=/dev/null --set-section-flags .note.GNU-stack=noload,readonly
RES_EXPORT_FLAGS = $(foreach RESFILE, $(RES), -Wl,--export-dynamic-symbol=_binary_$(shell echo '$(RESFILE)' | sed 's/[^a-zA-Z0-9]/_/g')_start -Wl,--export-dynamic-symbol=_binary_$(shell echo '$(RESFILE)' | sed 's/[^a-zA-Z0-9]/_/g')_end)
#LDFLAGS += -pie $(RES_EXPORT_FLAGS)
LDFLAGS += -pie $(RES_EXPORT_FLAGS) -pg

all: $(TARGET)

$(TARGET): $(OBJ) $(RESOBJ)
	$(CXX) $(LDFLAGS) -o $(TARGET) $(OBJ) $(RESOBJ)

$(BUILDDIR)/res/%.o: $(RESDIR)/% $(BUILDDIR)
	objcopy $(OCFLAGS) $< $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp $(BUILDDIR) $(RES_HEADER)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(RES_HEADER): $(RES)
	./tools/generate-resources.sh $@ $(RESDIR)

tests/%.out: tests/%.cpp $(filter-out src/main.o, $(OBJ)) $(RESOBJ)
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $^ -o $@

check: $(TEST_BINS)
	@for script in $(TEST_EXPS); do \
		expect $$script || exit 1; \
	done
	@echo ALL TESTS PASS

manual-tests/%.out: manual-tests/%.cpp $(filter-out src/main.o, $(OBJ)) $(RESOBJ)
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $^ -o $@

manual-check: $(MANUAL_TEST_BINS)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)/res $(foreach MODULE, $(MODULES), $(BUILDDIR)/$(MODULE))

clean:
	rm -rf $(BUILDDIR)
	rm -f $(RES_HEADER)
	rm -f $(TEST_BINS)
	rm -f $(MANUAL_TEST_BINS)

.PHONY: all clean check manual-check

