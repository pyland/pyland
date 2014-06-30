NULL=

RM = - rm -f

CXX      = ${COMPILER}
CXXFLAGS = -std=c++11 -g -Wall -Wextra -O2
CPPFLAGS = -I./
LDFLAGS  =
LDLIBS   =

ifeq ($(PLATFORM),desktop)
	GL_CPPFLAGS   = $(shell pkg-config gl --cflags)
	GL_LDFLAGS  = 
	GL_LDLIBS   = $(shell pkg-config gl --libs)

	GRAPHICS_CPPFLAGS  = $(GL_CPPFLAGS)
	GRAPHICS_LDFLAGS = $(GL_LDFLAGS)
	GRAPHICS_LDLIBS  = $(GL_LDLIBS)
	CPPFLAGS += -DUSE_GL
else
# Taken from hello_triangle example
	GLES_CPPFLAGS   = -I/opt/vc/include -I/opt/vc/include/interface/vmcs_host/linux -I/opt/vc/include/interface/vcos/pthreads
	GLES_LDFLAGS  = -L/opt/vc/lib/
	GLES_LDLIBS   = -lGLESv2 -lEGL -lbcm_host -lpthread

	GRAPHICS_CPPFLAGS  = $(GLES_CPPFLAGS)
	GRAPHICS_LDFLAGS = $(GLES_LDFLAGS)
	GRAPHICS_LDLIBS  = $(GLES_LDLIBS)
	CPPFLAGS += -DUSE_GLES
endif

SDL_CPPFLAGS  = $(shell sdl2-config --cflags)
SDL_LDFLAGS   =
SDL_LDLIBS    = $(shell sdl2-config --libs)

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
all: main.bin



debug: CPPFLAGS += -DDEBUG
debug: all



game_window.o: game_window.cpp
	$(CXX) -c -o game_window.o game_window.cpp $(CXXFLAGS) $(CPPFLAGS) $(SDL_CPPFLAGS) $(GRAPHICS_CPPFLAGS) $(X11_CPPFLAGS)



main.bin: main.o game_window.o
	$(CXX) -o main.bin main.o game_window.o $(CXXFLAGS) $(LDFLAGS) $(LDLIBS) $(GRAPHICS_LDFLAGS) $(GRAPHICS_LDLIBS) $(X11_LDFLAGS) $(X11_LDLIBS) $(SDL_LDFLAGS) $(SDL_LDLIBS)

main.o: main.cpp
	$(CXX) -c -o main.o main.cpp $(CXXFLAGS) $(CPPFLAGS) $(GRAPHICS_CPPFLAGS)



clean:
	$(RM) game_window.o

.PHONY : clean
.PHONY : debug
