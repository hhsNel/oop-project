HEADERS := $(shell find ../../src -name '*.h')
HEADER_PDFS := $(patsubst ../../src/%.h,build/pdf/%.pdf,$(HEADERS))

all: report.pdf

report.pdf: title.pdf build/pdf/123.pdf uml.pdf build/pdf/headers.pdf build/pdf/4.pdf build/pdf/block-diagram.pdf build/pdf/5.pdf build/pdf/678.pdf
	pdfunite $^ report.pdf

build/pdf/%.pdf: %.md | build
	pandoc --from=gfm --to=pdf $< -o $@ -V geometry:margin=1in -V papersize:A4 -V pagestyle=empty
build/pdf/%.pdf: build/headers/%.md | build
	mkdir -p $(@D)
	pandoc --from=gfm --to=pdf $< -o $@ -V geometry:margin=1in -V papersize:A4 -V pagestyle=empty

build/headers/%.md: ../../src/%.h
	mkdir -p $(@D)
	echo -e "# $$(echo $< | sed 's|../../src/||g')\n\`\`\`cpp\n" > $@
	cat $< >> $@
	echo -e '\n```\n' >> $@

build/pdf/headers.pdf: $(HEADER_PDFS)
	pdfunite $^ $@

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

