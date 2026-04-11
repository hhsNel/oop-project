SRCDIR = src
MODULES = math geometry graphics engine rendering util
BUILDDIR = build
RESDIR = res

TARGET = isekai-doom
SRC = $(foreach MODULE, $(MODULES), $(wildcard $(SRCDIR)/$(MODULE)/*.cpp))
RES = $(wildcard $(RESDIR)/*)

OBJ = $(SRC:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)
RESOBJ = $(RES:$(RESDIR)/%=$(BUILDDIR)/res/%.o)

CPP := g++
CFLAGS += -Wall -Wextra -Werror -Wshadow -fstack-protector-strong -fPIE -I$(SRCDIR)
CXXFLAGS += -std=c++26 -freflection $(CFLAGS)
OCFLAGS += -I binary -O elf64-x86-64
RES_EXPORT_FLAGS = $(foreach RESFILE, $(RES), -Wl,--export-dynamic-symbol=_binary_$(shell echo '$(RESFILE)' | sed 's/[^a-zA-Z0-9]/_/g')_start -Wl,--export-dynamic-symbol=_binary_$(shell echo '$(RESFILE)' | sed 's/[^a-zA-Z0-9]/_/g')_end)
LDFLAGS += -pie $(RES_EXPORT_FLAGS)

all: $(TARGET)

$(TARGET): $(OBJ) $(RESOBJ)
	$(CPP) $(LDFLAGS) -o $(TARGET) $(OBJ) $(RESOBJ)

$(BUILDDIR)/res/%.o: $(RESDIR)/% $(BUILDDIR)
	objcopy $(OCFLAGS) $< $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp $(BUILDDIR)
	$(CPP) $(CXXFLAGS) -c $< -o $@

$(BUILDDIR):
	mkdir -p $(BUILDDIR)/res $(foreach MODULE, $(MODULES), $(BUILDDIR)/$(MODULE))

clean:
	rm -rf $(BUILDDIR)

.PHONY: all clean

