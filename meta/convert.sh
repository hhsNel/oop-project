#!/bin/bash

pandoc --from=gfm --to=pdf opisy.md -o opisy.pdf -V geometry:margin=1in -V papersize:A4
