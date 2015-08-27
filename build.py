#!/usr/bin/python

#TODO: make it so that the python version isn't needed as a command-line argument! (Use the version of python that is used to "build" the game, and detect if the python version is high enough!")

import os
import subprocess
import sys
import getopt

def mkdir(directory):
    if not os.path.exists(directory):
        os.makedirs(directory)

compiler = ""
desktop = False
python_version = ""
jobs = ""

try:
    opts, args = getopt.getopt(sys.argv[1:],"dj:c:p:",["desktop", "jobs=", "compiler=", "python_version="])
except getopt.GetoptError:
    print('build.py -c <compiler> -p <python_version>')
    sys.exit(2)


platform = ""
for opt, arg in opts:
    if opt in ("-d", "--desktop"):
        platform = "PLATFORM=desktop "
    elif opt in ("-c", "compiler"):
        compiler = arg
    elif opt in ("-j", "--jobs"):
        jobs = " -j" + arg
    elif opt in ("-p", "--python_version"):
        python_version = arg

if not (compiler and python_version):
    print('build.py -c <compiler> -p <python_version>')
    sys.exit(2)

compile_string = platform + "COMPILER=" + compiler + " PYTHON_VERSION=" + python_version + " LIBBOOST_PYTHON=boost_python-py" + python_version.replace(".", "") + " make" + jobs
print(compile_string)

os.chdir(os.path.dirname(os.path.realpath(__file__)))
mkdir("game/jsonnet") #TODO: move this to the makefile!!!
mkdir("game/python_embed")

os.system("rm game/database.db")
os.system("sqlite3 game/database.db < game/database_source.sql")
os.chdir("src")
os.system(compile_string)
os.chdir("..")
os.system("cp src/jsonnet/jsonnet game/jsonnet/jsonnet") #TODO: make it so that these are compiled in the right places directly
os.system("cp src/python_embed/wrapper_functions.so game/python_embed/wrapper_functions.so")
