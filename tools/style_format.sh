#!/bin/sh

find src/ -iname *.h -o -iname *.cpp -o -iname *.inl | xargs clang-format -i
