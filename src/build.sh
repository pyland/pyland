#!/bin/sh

B=$(tput sgr0)$(tput bold)
R=$(tput sgr0)$(tput bold)$(tput setaf 3)
N=$(tput sgr0)

FORMAT="$(tput cuu1)$(tput cuf $(tput cols))$(tput cub 6)${B}${R}%E${B}$N"

/usr/bin/time -f "$FORMAT" make \
	SANITIZE=1                     \
	PLATFORM=desktop               \
	PYTHON_VERSION=3.4             \
	LIBBOOST_PYTHON=boost_python3  \
	--output-sync=target --keep-going --jobs=99 $@
