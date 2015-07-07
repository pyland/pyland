#!/bin/sh

set -ex

qmake-qt4 -o Makefile application.pro
make
