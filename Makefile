SRCDIR = src
MODULES = math geometry graphics engine rendering util
BUILDDIR = build
RESDIR = res
TESTDIR = tests

TARGET = isekai-doom
SRC = $(foreach MODULE, $(MODULES), $(wildcard $(SRCDIR)/$(MODULE)/*.cpp))
RES = $(wildcard $(RESDIR)/*)
RES_HEADER = $(SRCDIR)/res.h
TEST_SRCS = $(wildcard $(TESTDIR)/*.cpp)
TEST_EXPS = $(wildcard $(TESTDIR)/*.exp)

OBJ = $(SRC:$(SRCDIR)/%.cpp=$(BUILDDIR)/%.o)
RESOBJ = $(RES:$(RESDIR)/%=$(BUILDDIR)/res/%.o)
TEST_BINS = $(TEST_SRCS:.cpp=.out)

CXX = g++
CFLAGS = -Wall -Wextra -Werror -Wshadow -fstack-protector-strong -fPIE -I$(SRCDIR)
CXXFLAGS = $(CFLAGS)
#CXXFLAGS += -std=c++26 -freflection $(CFLAGS)
OCFLAGS += -I binary -O elf64-x86-64
RES_EXPORT_FLAGS = $(foreach RESFILE, $(RES), -Wl,--export-dynamic-symbol=_binary_$(shell echo '$(RESFILE)' | sed 's/[^a-zA-Z0-9]/_/g')_start -Wl,--export-dynamic-symbol=_binary_$(shell echo '$(RESFILE)' | sed 's/[^a-zA-Z0-9]/_/g')_end)
LDFLAGS += -pie $(RES_EXPORT_FLAGS)

all: $(TARGET)

$(TARGET): $(OBJ) $(RESOBJ)
	@echo obj: $(OBJ)
	@echo resobj: $(RESOBJ)
	$(CXX) $(LDFLAGS) -o $(TARGET) $(OBJ) $(RESOBJ)

$(BUILDDIR)/res/%.o: $(RESDIR)/% $(BUILDDIR)
	objcopy $(OCFLAGS) $< $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp $(BUILDDIR) $(RES_HEADER)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(RES_HEADER): $(RES)
	./tools/generate-resources.sh $@ $(RESDIR)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)/res $(foreach MODULE, $(MODULES), $(BUILDDIR)/$(MODULE))

clean:
	rm -rf $(BUILDDIR)
	rm -f $(RES_HEADER)
	rm -f $(TEST_BINS)

tests/%.out: tests/%.cpp $(filter-out src/main.o, $(OBJ))
	$(CXX) $(CXXFLAGS) $^ -o $@

check: $(TEST_BINS)
	for script in $(TEST_EXPS); do \
		expect $$script || exit 1; \
	done

.PHONY: all clean check

