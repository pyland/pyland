NULL=

RM = - rm -f

CXX      = ${COMPILER}
CXXFLAGS = -std=c++11 -g -Wall -Wextra -O2
CPPFLAGS =
LDFLAGS  =
LDLIBS   =

# Taken from hello_triangle example
GLES_CPPFLAGS   = -I/opt/vc/include -I/opt/vc/include/interface/vmcs_host/linux -I/opt/vc/include/interface/vcos/pthreads
GLES_LDFLAGS    = -L/opt/vc/lib/
GLES_LDLIBS     = -lGLESv2 -lEGL -lbcm_host -lpthread

X11_CPPFLAGS    = $(shell pkg-config x11 --cflags)
X11_LDFLAGS     =
X11_LDLIBS      = $(shell pkg-config x11 --libs)

BOOST_CPPFLAGS  =
BOOST_LDFLAGS   =
BOOST_LDLIBS    = -l${LIBBOOST_PYTHON} -lboost_filesystem -lboost_system

PYTHON_CPPFLAGS = $(shell python${PYTHON_VERSION}-config --cflags)
PYTHON_LDFLAGS  = $(shell python${PYTHON_VERSION}-config --ldflags)
PYTHON_LDLIBS   = $(shell python${PYTHON_VERSION}-config --libs)

# Insert all here.

debug: CPPFLAGS += -DDEBUG
debug: all

all:



game_window.o: game_window.cpp
	$(CXX) -c -o game_window.o game_window.cpp $(CXXFLAGS) $(CPPFLAGS) $(SDL_CFLAGS) $(GLES_CFLAGS) $(X11_CFLAGS)



clean:
	$(RM) game_window.o

.PHONY : clean
.PHONY : debug
