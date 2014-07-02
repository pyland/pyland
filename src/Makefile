NULL=

RM = - rm -f

CXX      = ${COMPILER}
CXXFLAGS = -std=c++11 -O3
CPPFLAGS = -I./ -Ipython_embed/
LDFLAGS  =
LDLIBS   =

WARN.g++.all := -Wall -Wextra -Wconversion
WARN.g++.4.7 := -Wzero-as-null-pointer-constant
WARN.g++.4.8 := $(wflags.gcc.4.7)
WARN.clang.all := -Wall -Wextra
WARN.clang.3.2 := -Weverything
CXXFLAGS += $(WARN.$(CXX).all) $(WARN.$(CXX).$(4.8))

ifeq ($(PLATFORM),desktop)
	GL_CPPFLAGS = $(shell pkg-config gl --cflags)
	GL_LDFLAGS  = 
	GL_LDLIBS   = $(shell pkg-config gl --libs)

	GRAPHICS_CPPFLAGS  = $(GL_CPPFLAGS)
	GRAPHICS_LDFLAGS = $(GL_LDFLAGS)
	GRAPHICS_LDLIBS  = $(GL_LDLIBS)
	CPPFLAGS += -DUSE_GL
else
	# From hello_triangle
	GLES_CPPFLAGS  = -I/opt/vc/include
	GLES_CPPFLAGS += -I/opt/vc/include/interface/vmcs_host/linux
	GLES_CPPFLAGS += -I/opt/vc/include/interface/vcos/pthreads
	GLES_LDFLAGS   = -L/opt/vc/lib/
	GLES_LDLIBS    = -lGLESv2 -lEGL -lbcm_host -lpthread

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

# Prevent warnings about -Wstrict-prototypes not being valid for C++
PYTHON_CPPFLAGS := $(filter-out -Wstrict-prototypes,$(PYTHON_CPPFLAGS))



### Program Files ###

EXECUTABLE = main.bin

BASE_SOURCES = \
	main.cpp          \
	game_window.cpp   \
	input_manager.cpp \
	debug.cpp

BASE_OBJS = $(BASE_SOURCES:.cpp=.o)

PYTHON_SOURCES = \
	python_embed/interpreter.cpp       \
	python_embed/wrapper_functions.cpp \
	python_embed/playerthread.cpp      \
	python_embed/api.cpp               \
	python_embed/locks.cpp

PYTHON_OBJS = $(PYTHON_SOURCES:.cpp=.o)



### What to run ###

all: $(EXECUTABLE) $(BASE_SOURCES) $(PYTHON_SOURCES) python_embed/wrapper_functions.so

debug: CXXFLAGS += -g
debug: CXXFLAGS +=  -fno-elide-constructors
debug: CXXFLAGS +=  -O0
debug: CPPFLAGS += -DDEBUG
debug: all


$(EXECUTABLE): $(BASE_OBJS) $(PYTHON_OBJS)
	@echo "[ Compiling $(EXECUTABLE) ]"
	@echo
	$(CXX) -o $@ \
		$(BASE_OBJS) $(PYTHON_OBJS) \
		$(CXXFLAGS) $(LDFLAGS) $(LDLIBS) $(GRAPHICS_LDFLAGS) $(GRAPHICS_LDLIBS) \
		$(X11_LDFLAGS) $(X11_LDLIBS) $(SDL_LDFLAGS) $(SDL_LDLIBS) \
		$(PYTHON_LDFLAGS) $(PYTHON_LDLIBS) $(BOOST_LDFLAGS) $(BOOST_LDLIBS)

$(BASE_OBJS): $(BASE_SOURCES)
	@echo "[ Compiling base object file $@ ]"
	@echo
	$(CXX) -c ${basename $@}.cpp -o $@ \
		$(CXXFLAGS) $(CPPFLAGS) \
		$(SDL_CPPFLAGS) $(GRAPHICS_CPPFLAGS) $(X11_CPPFLAGS)
	@echo

$(PYTHON_OBJS): $(PYTHON_SOURCES)
	@echo "[ Compiling Python object file $@ ]"
	@echo
	$(CXX) -c ${basename $@}.cpp -o $@ \
		$(CXXFLAGS) $(CPPFLAGS) \
		$(PYTHON_CPPFLAGS) $(PYTHON_CXXFLAGS)
	@echo

python_embed/wrapper_functions.so: python_embed/wrapper_functions.cpp
	@echo "[ Compiling $@ ]"
	@echo
	$(CXX) -shared -fPIC $< -o $@ \
		$(CPPFLAGS) $(CXXFLAGS) \
		$(PYTHON_LDLIBS) $(PYTHON_CPPFLAGS) \
		$(BOOST_LDLIBS) $(BOOST_CPPFLAGS)
	@echo


clean:
	$(RM) $(EXECUTABLE)
	$(RM) python_embed/wrapper_functions.so
	$(RM) $(BASE_OBJS)
	$(RM) $(PYTHON_OBJS)


.PHONY : clean
.PHONY : debug
