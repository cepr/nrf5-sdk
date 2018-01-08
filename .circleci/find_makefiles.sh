#!/bin/bash

for i in external components examples
do
  find $i -name Makefile -type f
done | while read
do
  echo "      - run: cd `dirname "$REPLY"` && make"
done
