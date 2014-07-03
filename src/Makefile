NULL=
RM = - rm -f

CXX       = ${COMPILER}
CXXFLAGS += -std=c++11 -O3
CPPFLAGS += -I./ -Ipython_embed/
CPPFLAGS += -MMD -MP -MF dependencies/$*.d
LDFLAGS   =
LDLIBS    =

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

define colorecho
      @tput setaf 6
      @echo $1
      @
endef

bold   = $(shell tput sgr0)$(shell tput bold)
normal = $(shell tput sgr0)
red    = $(shell tput setaf 1)


GCC_VERSION = $(shell expr `$(CC) -dumpversion`)
GCC_MAJOR = $(shell expr `echo $(GCC_VERSION)` | cut -f1 -d.)
GCC_MINOR = $(shell expr `echo $(GCC_VERSION)` | cut -f2 -d.)

COMPILER_SUPPORTS_DIAGNOSTICS_COLOR = $(shell expr $(GCC_MAJOR) \>= 4 \& $(GCC_MINOR) \>= 9)
ifeq "$(COMPILER_SUPPORTS_DIAGNOSTICS_COLOR)" "1"
    CXXFLAGS += -fdiagnostics-color=auto
endif

-fdiagnostics-color=auto
### Program Files ###

EXECUTABLE = main.bin

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

PYTHON_SHARED_OBJS = \
	python_embed/wrapper_functions.so	


HEADER_DEPENDS_ROOT = \
	${EXECUTABLE:.bin=.d}        \
	${BASE_OBJS:.o=.d}           \
	${PYTHON_OBJS:.o=.d}         \
	${PYTHON_SHARED_OBJS:.so=.d}

HEADER_DEPENDS = $(addprefix dependencies/,${HEADER_DEPENDS_ROOT})

### What to run ###

all: dependencies_directories $(EXECUTABLE) python_embed/wrapper_functions.so

debug: CXXFLAGS += -g
debug: CXXFLAGS +=  -fno-elide-constructors
debug: CXXFLAGS +=  -O0
debug: CPPFLAGS += -DDEBUG
debug: all

dependencies_directories:
	@mkdir -p dependencies
	@mkdir -p dependencies/python_embed

$(EXECUTABLE): $(BASE_OBJS) $(PYTHON_OBJS)
	@echo -e "\n${bold}${red}[ Compiling $(EXECUTABLE) ]${normal}\n"
	$(CXX) -o $@ \
		$(BASE_OBJS) $(PYTHON_OBJS)            \
		$(GRAPHICS_LDFLAGS) $(GRAPHICS_LDLIBS) \
		$(X11_LDFLAGS) $(X11_LDLIBS)           \
		$(SDL_LDFLAGS) $(SDL_LDLIBS)           \
		$(PYTHON_LDFLAGS) $(PYTHON_LDLIBS)     \
		$(BOOST_LDFLAGS) $(BOOST_LDLIBS)       \
		$(CXXFLAGS) $(LDFLAGS) $(LDLIBS)

$(BASE_OBJS): %.o : %.cpp
	@echo -e "\n${bold}[ Compiling base object file${red}" $*.o "${normal}${bold}from${red}" $*.cpp "${normal}${bold}]${normal}\n"
	$(CXX) -c $*.cpp -o $*.o \
		$(SDL_CPPFLAGS)         \
		$(GRAPHICS_CPPFLAGS)    \
		$(X11_CPPFLAGS)         \
		$(CXXFLAGS) $(CPPFLAGS)
	@echo

$(PYTHON_OBJS): %.o : %.cpp
	@echo -e "\n${bold}[ Compiling Python object file${red}" $*.o "${normal}${bold}from${red}" $*.cpp "${normal}${bold}]${normal}\n"
	$(CXX) -c $*.cpp -o $*.o \
		$(PYTHON_CPPFLAGS) $(PYTHON_CXXFLAGS) \
		$(CXXFLAGS) $(CPPFLAGS)

$(PYTHON_SHARED_OBJS): %.so : %.cpp
	@echo -e "\n${bold}[ Compiling Python shared object file${red}" $*.so "${normal}${bold}from${red}" $*.cpp "${normal}${bold}]${normal}\n"
	$(CXX) -shared -fPIC $*.cpp -o $*.so \
		$(PYTHON_LDLIBS) $(PYTHON_CPPFLAGS) \
		$(BOOST_LDLIBS) $(BOOST_CPPFLAGS)   \
		$(CXXFLAGS) $(CPPFLAGS)

debug:
	@echo -e "\n${bold}${red}Complied in debug${normal}"



clean:
	$(RM) $(EXECUTABLE) $(PYTHON_SHARED_OBJS) $(BASE_OBJS) $(PYTHON_OBJS) $(HEADER_DEPENDS)

-include ${HEADER_DEPENDS}

.PHONY : all
.PHONY : clean
.PHONY : debug