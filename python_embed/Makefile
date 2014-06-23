NULL=

RM = - rm -f

CXX      = g++
CXXFLAGS = -std=c++11 -g
CPPFLAGS =
LDFLAGS  =
LDLIBS   =
PYTHON_VERSION = $(shell python3 -c 'import sys; print("{0.major}.{0.minor}".format(sys.version_info))')

BOOST_LDLIBS    = -lboost_python3 -lboost_filesystem -lboost_system
BOOST_CPPFLAGS  =
BOOST_CXXFLAGS  =

PYTHON_LDLIBS   = $(shell python${PYTHON_VERSION}-config --ldflags)
PYTHON_CPPFLAGS = $(shell python${PYTHON_VERSION}-config --cflags)
PYTHON_CXXFLAGS =

all: interpreter wrapper_functions.so



interpreter: interpreter.o wrapper_functions.o api.o
	$(CXX) -o interpreter interpreter.o wrapper_functions.o api.o $(BOOST_LDLIBS) $(PYTHON_LDLIBS) $(LDFLAGS) $(LDLIBS)


interpreter.o: interpreter.cpp api.o
	$(CXX) -c interpreter.cpp api.o $(BOOST_CPPFLAGS) $(PYTHON_CPPFLAGS) $(PYTHON_CXXFLAGS) $(CPPFLAGS) $(CXXFLAGS)

wrapper_functions.o: wrapper_functions.cpp
	$(CXX) -c wrapper_functions.cpp $(BOOST_CPPFLAGS) $(PYTHON_CPPFLAGS) $(PYTHON_CXXFLAGS) $(CPPFLAGS) $(CXXFLAGS)

wrapper_functions.so: wrapper_functions.cpp
	$(CXX) -shared -fPIC wrapper_functions.cpp -o wrapper_functions.so $(BOOST_LDLIBS) $(PYTHON_LDLIBS) $(BOOST_CPPFLAGS) $(PYTHON_CPPFLAGS) $(PYTHON_CXXFLAGS) $(CPPFLAGS) $(CXXFLAGS)

api.o: api.cpp
	$(CXX) -c api.cpp $(BOOST_CPPFLAGS) $(PYTHON_CPPFLAGS) $(PYTHON_CXXFLAGS) $(CPPFLAGS) $(CXXFLAGS)



clean:
	$(RM) interpreter.o
	$(RM) interpreter
	$(RM) wrapper_functions.so
	$(RM) wrapper_functions.o
	$(RM) api.o
