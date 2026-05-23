#!/bin/bash

set -x

pandoc --from=gfm --to=pdf $@ -o out.pdf -V geometry:margin=1in -V papersize:A4

