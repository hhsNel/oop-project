HEADERS := $(filter-out ../../src/res.h,$(shell find ../../src -name '*.h'))
HEADER_PDFS := $(patsubst ../../src/%.h,build/pdf/%.pdf,$(HEADERS))
SOURCES := $(shell find ../../src -name '*.cpp')
SOURCE_PDFS := $(patsubst ../../src/%.cpp,build/sources-pdf/%.pdf,$(SOURCES))
PANDOC=pandoc --from=gfm --to=pdf -V geometry:margin=1in -V papersize:A4 -V pagestyle=empty --include-in-header=code-wrap.tex

all: report-full.pdf

report-full.pdf: report.pdf listing.pdf
	pdfunite $^ report-full.pdf

report.pdf: title.pdf build/pdf/123.pdf uml.pdf build/pdf/headers.pdf build/pdf/4.pdf build/pdf/block-diagram.pdf build/pdf/5.pdf build/pdf/678.pdf
	pdfunite $^ report.pdf

build/pdf/%.pdf: %.md code-wrap.tex | build
	$(PANDOC) $< -o $@
build/pdf/%.pdf: build/headers/%.md code-wrap.tex | build
	mkdir -p $(@D)
	$(PANDOC) $< -o $@

build/headers/%.md: ../../src/%.h
	mkdir -p $(@D)
	echo -e "# $$(echo $< | sed 's|../../src/||g')\n\`\`\`cpp\n" > $@
	cat $< >> $@
	echo -e '\n```\n' >> $@

build/pdf/headers.pdf: $(HEADER_PDFS)
	pdfunite $^ $@

build/sources-pdf/%.pdf: build/sources/%.md code-wrap.tex | build
	mkdir -p $(@D)
	$(PANDOC) $< -o $@

build/sources/%.md: ../../src/%.cpp
	mkdir -p $(@D)
	echo -e "# $$(echo $< | sed 's|../../src/||g')\n\`\`\`cpp\n" > $@
	cat $< >> $@
	echo -e '\n```\n' >> $@

listing.pdf: $(SOURCE_PDFS) | build
	pdfunite $^ listing.pdf

build/pdf/block-diagram.pdf: build
	mkdir -p build/block-diagram
	cp block-diagram.py build/block-diagram
	cd build/block-diagram && ./block-diagram.py
	mv build/block-diagram/block-diagram.pdf build/pdf
	rm -rf build/block-diagram

build:
	mkdir -p build/pdf

.PHONY: all clean
.SECONDARY:

clean:
	rm -rf build

