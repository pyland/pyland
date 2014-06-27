NULL=

RM = - rm -f

CXX      = ${COMPILER}
CXXFLAGS = -std=c++11 -g

WARN.g++.all := -Wall -Wextra -Wconversion
WARN.g++.4.7 := -Wzero-as-null-pointer-constant
WARN.g++.4.8 := $(wflags.gcc.4.7)
WARN.clang.all := -Wall -Wextra
WARN.clang.3.2 := -Weverything
CXXFLAGS += $(WARN.$(CXX).all) $(WARN.$(CXX).$(4.8))

CPPFLAGS = -DDEBUG
LDFLAGS  =
LDLIBS   =

BOOST_LDLIBS    = -l${LIBBOOST_PYTHON} -lboost_filesystem -lboost_system -lboost_regex
BOOST_CPPFLAGS  =
BOOST_CXXFLAGS  =

PYTHON_LDLIBS   = $(shell python${PYTHON_VERSION}-config --ldflags)
PYTHON_CPPFLAGS = $(shell python${PYTHON_VERSION}-config --cflags)
PYTHON_CPPFLAGS := $(filter-out -Wstrict-prototypes,$(PYTHON_CPPFLAGS))
PYTHON_CXXFLAGS =

all: interpreter wrapper_functions.so



interpreter: interpreter.o wrapper_functions.o api.o debug.o
	$(CXX) -o interpreter interpreter.o wrapper_functions.o api.o debug.o $(BOOST_LDLIBS) $(PYTHON_LDLIBS) $(LDFLAGS) $(LDLIBS) $(CXXFLAGS)
	@echo

interpreter.o: interpreter.cpp
	$(CXX) -c interpreter.cpp $(BOOST_CPPFLAGS) $(PYTHON_CPPFLAGS) $(PYTHON_CXXFLAGS) $(CPPFLAGS) $(CXXFLAGS)
	@echo

wrapper_functions.o: wrapper_functions.cpp
	$(CXX) -c wrapper_functions.cpp $(BOOST_CPPFLAGS) $(PYTHON_CPPFLAGS) $(PYTHON_CXXFLAGS) $(CPPFLAGS) $(CXXFLAGS)
	@echo

wrapper_functions.so: wrapper_functions.cpp
	$(CXX) -shared -fPIC wrapper_functions.cpp -o wrapper_functions.so $(BOOST_LDLIBS) $(PYTHON_LDLIBS) $(BOOST_CPPFLAGS) $(PYTHON_CPPFLAGS) $(PYTHON_CXXFLAGS) $(CPPFLAGS) $(CXXFLAGS)
	@echo

api.o: api.cpp debug.o
	$(CXX) -c api.cpp debug.o $(BOOST_CPPFLAGS) $(PYTHON_CPPFLAGS) $(PYTHON_CXXFLAGS) $(CPPFLAGS) $(CXXFLAGS)
	@echo

debug.o: debug.cpp
	$(CXX) -c debug.cpp $(CPPFLAGS) $(CXXFLAGS)
	@echo



clean:
	$(RM) interpreter.o
	$(RM) interpreter
	$(RM) wrapper_functions.so
	$(RM) wrapper_functions.o
	$(RM) api.o
