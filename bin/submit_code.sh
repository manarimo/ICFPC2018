#!/usr/bin/env bash

cd `dirname "$0"`
cd ..

git archive master --output source.zip
venv/bin/python scripts/submit.py source.zip sourcecode
rm source.zip