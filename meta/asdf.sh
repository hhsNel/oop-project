#!/bin/bash

set -x

./convert.sh game-idea.md

pdfunite sklad.pdf out.pdf uml.pdf merged.pdf

rm out.pdf

