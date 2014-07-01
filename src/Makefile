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
BOOST_LDLIBS    = -l${LIBBOOST_PYTHON} -lboost_filesystem -lboost_system -lboost_regex

PYTHON_CPPFLAGS = $(shell python${PYTHON_VERSION}-config --cflags)
PYTHON_LDFLAGS  = $(shell python${PYTHON_VERSION}-config --ldflags)
PYTHON_LDLIBS   = $(shell python${PYTHON_VERSION}-config --libs)

# Insert all here.
all: main.bin

python_embed:
	@$(MAKE) -C python_embed debug


debug: CPPFLAGS += -DDEBUG
debug: all



main.bin: main.o python_embed game_window.o input_manager.o
	$(CXX) -o main.bin main.o game_window.o input_manager.o \
		python_embed/interpreter.o python_embed/wrapper_functions.o python_embed/playerthread.o \
		python_embed/api.o python_embed/debug.o python_embed/locks.o \
		$(CXXFLAGS) $(LDFLAGS) $(LDLIBS) $(GRAPHICS_LDFLAGS) $(GRAPHICS_LDLIBS) \
		$(X11_LDFLAGS) $(X11_LDLIBS) $(SDL_LDFLAGS) $(SDL_LDLIBS) \
		$(PYTHON_LDFLAGS) $(PYTHON_LDLIBS) $(BOOST_LDFLAGS) $(BOOST_LDLIBS)



game_window.o: game_window.cpp
	$(CXX) -c -o game_window.o game_window.cpp $(CXXFLAGS) $(CPPFLAGS) $(SDL_CPPFLAGS) $(GRAPHICS_CPPFLAGS) $(X11_CPPFLAGS)

input_manager.o: input_manager.cpp
	$(CXX) -c -o input_manager.o input_manager.cpp $(CXXFLAGS) $(CPPFLAGS) $(SDL_CPPFLAGS)

main.o: main.cpp
	$(CXX) -c -o main.o main.cpp $(CXXFLAGS) $(CPPFLAGS) $(GRAPHICS_CPPFLAGS)



clean:
	$(RM) main.bin
	$(RM) game_window.o
	$(RM) input_manager.o
	$(RM) main.o

.PHONY : clean
.PHONY : debug
.PHONY : python_embed
