NULL=

RM = - rm -f

CXX      = ${COMPILER}
CXXFLAGS = -std=c++11 -O3
CPPFLAGS = -I./
LDFLAGS  =
LDLIBS   =

WARN.g++.all := -Wall -Wextra -Wconversion
WARN.g++.4.7 := -Wzero-as-null-pointer-constant
WARN.g++.4.8 := $(wflags.gcc.4.7)
WARN.clang.all := -Wall -Wextra
WARN.clang.3.2 := -Weverything
CXXFLAGS += $(WARN.$(CXX).all) $(WARN.$(CXX).$(4.8))

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

BASE_OBJS = \
	main.o          \
	game_window.o   \
	input_manager.o \
	debug.o

PYTHON_OBJS = \
	python_embed/interpreter.o       \
	python_embed/wrapper_functions.o \
	python_embed/playerthread.o      \
	python_embed/api.o               \
	python_embed/locks.o



all: main.bin python_embed/wrapper_functions.so

debug: CXXFLAGS += -g
debug: CXXFLAGS +=  -fno-elide-constructors
debug: CXXFLAGS +=  -O0
debug: CPPFLAGS += -DDEBUG
debug: all


main.bin: base python_embed
	$(CXX) -o $@ \
		$(BASE_OBJS) $(PYTHON_OBJS) \
		$(CXXFLAGS) $(LDFLAGS) $(LDLIBS) $(GRAPHICS_LDFLAGS) $(GRAPHICS_LDLIBS) \
		$(X11_LDFLAGS) $(X11_LDLIBS) $(SDL_LDFLAGS) $(SDL_LDLIBS) \
		$(PYTHON_LDFLAGS) $(PYTHON_LDLIBS) $(BOOST_LDFLAGS) $(BOOST_LDLIBS)

base: $(BASE_OBJS)
	$(CXX) -c $^ -o $@ \
		$(BASE_OBJS) $(PYTHON_OBJS) \
		$(CXXFLAGS) $(CPPFLAGS) \
		$(SDL_CPPFLAGS) $(GRAPHICS_CPPFLAGS) $(X11_CPPFLAGS)
	@echo

python_embed: $(PYTHON_OBJS)
	$(CXX) -c $^ -o $@ \
		$(CXXFLAGS) $(CPPFLAGS) \
		$(PYTHON_CPPFLAGS) $(PYTHON_CXXFLAGS)
	@echo

python_embed/wrapper_functions.so: python_embed/wrapper_functions.cpp
	$(CXX) -shared -fPIC $^ -o $@ \
		$(CPPFLAGS) $(CXXFLAGS) \
		$(PYTHON_LDLIBS) $(PYTHON_CPPFLAGS) \
		$(BOOST_LDLIBS) $(BOOST_CPPFLAGS)
	@echo


clean:
	$(RM) main.bin
	$(RM) python_embed/wrapper_functions.so
	$(RM) $(BASE_OBJS)
	$(RM) $(PYTHON_OBJS)


.PHONY : clean
.PHONY : debug
