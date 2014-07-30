#!/usr/bin/env python3

import subprocess
import sys

if not len(sys.argv) > 1:
	print("You need to at least give the Python version number!")
	raise SystemExit

flags = subprocess.check_output(["python{}-config".format(sys.argv[1])] + sys.argv[2:])
sys.stdout.buffer.write(
	b" ".join(flags
		.replace(b"-arch i386", b"")
		.replace(b"-arch x86_64", b"")
		.replace(b"-Wstrict-prototypes", b"")
		.split()
	) + b"\n"
)
