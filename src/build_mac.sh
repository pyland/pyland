#!/bin/bash

PYTHON_VERSION=3.4 LIBBOOST_PYTHON=boost_python-py34 PLATFORM=desktop SANATIZE=1 make COMPILER=/usr/local/opt/gcc/bin/g++-4.9 GL_CPPFLAGS=-I/opt/X11/include GL_LDLIBS=-L/opt/X11/lib X11_CPPFLAGS=-I/opt/X11/include X11_LDLIBS="-L/opt/X11/lib -lX11" $@
