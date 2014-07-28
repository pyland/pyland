# Makefile for graphics tests.

SOURCE_ROOT =../../src

CXX = $(COMPILER)
CXXFLAGS = -g -Wall -Wextra -O3 -std=c++11
CPPFLAGS = -I$(SOURCE_ROOT) -I$(SOURCE_ROOT)/input_management

SDL_CFLAGS    = $(shell sdl2-config --cflags)
SDL_LDLIBS    = $(shell sdl2-config --libs) -lSDL2_image -lSDL2_ttf

# Taken from hello_triangle example
GLES_CFLAGS   = -I/opt/vc/include -I/opt/vc/include/interface/vmcs_host/linux -I/opt/vc/include/interface/vcos/pthreads
GLES_LDFLAGS  = -L/opt/vc/lib/
GLES_LDLIBS   = -lGLESv2 -lEGL -lbcm_host -lpthread

GL_CFLAGS   = $(shell pkg-config gl --cflags)
GL_LDFLAGS  = 
GL_LDLIBS   = $(shell pkg-config gl --libs)

X11_CFLAGS = $(shell pkg-config x11 --cflags)
X11_LDLIBS = $(shell pkg-config x11 --libs)

GLOG_CPPFLAGS = $(shell pkg-config libglog --cflags)
GLOG_CXXFLAGS =
GLOG_LDFLAGS  = 
GLOG_LDLIBS   = $(shell pkg-config libglog --libs)

ifeq ($(PLATFORM),desktop)
	GRAPHICS_CFLAGS  = $(GL_CFLAGS)
	GRAPHICS_LDFLAGS = $(GL_LDFLAGS)
	GRAPHICS_LDLIBS  = $(GL_LDLIBS)
	CPPFLAGS += -DUSE_GL
else
	GRAPHICS_CFLAGS  = $(GLES_CFLAGS)
	GRAPHICS_LDFLAGS = $(GLES_LDFLAGS)
	GRAPHICS_LDLIBS  = $(GLES_LDLIBS)
	CPPFLAGS += -DUSE_GLES
endif



all: main

debug: CPPFLAGS += -DDEBUG -O0
debug: all



main: main.o shader.o text_font.o typeface.o text.o image.o filters.o game_window.o input_manager.o keyboard_input_event.o mouse_input_event.o mouse_state.o input_event.o lifeline.o lifeline_controller.o 
	$(CXX) -o main main.o shader.o text_font.o typeface.o text.o image.o filters.o game_window.o input_manager.o keyboard_input_event.o mouse_input_event.o mouse_state.o input_event.o lifeline.o lifeline_controller.o $(CXXFLAGS) $(SDL_LDLIBS) $(GRAPHICS_LDFLAGS) $(GRAPHICS_LDLIBS) $(X11_LDLIBS) $(GLOG_LDLIBS)


main.o: main.cpp
	$(CXX) -c -o main.o main.cpp $(CXXFLAGS) $(CPPFLAGS) $(SDL_CFLAGS) $(GRAPHICS_CFLAGS) $(GLOG_CPPFLAGS)

shader.o: $(SOURCE_ROOT)/shader.cpp
	$(CXX) -c -o shader.o $(SOURCE_ROOT)/shader.cpp $(CXXFLAGS) $(CPPFLAGS) $(SDL_CFLAGS) $(GRAPHICS_CFLAGS) $(X11_CFLAGS) $(GLOG_CPPFLAGS)

text_font.o: $(SOURCE_ROOT)/text_font.cpp
	$(CXX) -c -o text_font.o $(SOURCE_ROOT)/text_font.cpp $(CXXFLAGS) $(CPPFLAGS) $(SDL_CFLAGS) $(GRAPHICS_CFLAGS) $(X11_CFLAGS) $(GLOG_CPPFLAGS)

typeface.o: $(SOURCE_ROOT)/typeface.cpp
	$(CXX) -c -o typeface.o $(SOURCE_ROOT)/typeface.cpp $(CXXFLAGS) $(CPPFLAGS) $(SDL_CFLAGS) $(GRAPHICS_CFLAGS) $(X11_CFLAGS) $(GLOG_CPPFLAGS)

text.o: $(SOURCE_ROOT)/text.cpp
	$(CXX) -c -o text.o $(SOURCE_ROOT)/text.cpp $(CXXFLAGS) $(CPPFLAGS) $(SDL_CFLAGS) $(GRAPHICS_CFLAGS) $(X11_CFLAGS) $(GLOG_CPPFLAGS)

image.o: $(SOURCE_ROOT)/image.cpp
	$(CXX) -c -o image.o $(SOURCE_ROOT)/image.cpp $(CXXFLAGS) $(CPPFLAGS) $(SDL_CFLAGS) $(GRAPHICS_CFLAGS) $(X11_CFLAGS) $(GLOG_CPPFLAGS)

game_window.o: $(SOURCE_ROOT)/game_window.cpp
	$(CXX) -c -o game_window.o $(SOURCE_ROOT)/game_window.cpp $(CXXFLAGS) $(CPPFLAGS) $(SDL_CFLAGS) $(GRAPHICS_CFLAGS) $(X11_CFLAGS) $(GLOG_CPPFLAGS)

input_manager.o: $(SOURCE_ROOT)/input_management/input_manager.cpp
	$(CXX) -c -o input_manager.o $(SOURCE_ROOT)/input_management/input_manager.cpp $(CXXFLAGS) $(CPPFLAGS) $(SDL_CFLAGS) $(GRAPHICS_CFLAGS) $(X11_CFLAGS) $(GLOG_CPPFLAGS)

filters.o: $(SOURCE_ROOT)/input_management/filters.cpp
	$(CXX) -c -o filters.o $(SOURCE_ROOT)/input_management/filters.cpp $(CXXFLAGS) $(CPPFLAGS) $(SDL_CFLAGS) $(GRAPHICS_CFLAGS) $(X11_CFLAGS) $(GLOG_CPPFLAGS)

input_event.o: $(SOURCE_ROOT)/input_management/input_event.cpp
	$(CXX) -c -o input_event.o $(SOURCE_ROOT)/input_management/input_event.cpp $(CXXFLAGS) $(CPPFLAGS) $(SDL_CFLAGS) $(GRAPHICS_CFLAGS) $(X11_CFLAGS) $(GLOG_CPPFLAGS)

keyboard_input_event.o: $(SOURCE_ROOT)/input_management/keyboard_input_event.cpp
	$(CXX) -c -o keyboard_input_event.o $(SOURCE_ROOT)/input_management/keyboard_input_event.cpp $(CXXFLAGS) $(CPPFLAGS) $(SDL_CFLAGS) $(GRAPHICS_CFLAGS) $(X11_CFLAGS) $(GLOG_CPPFLAGS)

mouse_input_event.o: $(SOURCE_ROOT)/input_management/mouse_input_event.cpp
	$(CXX) -c -o mouse_input_event.o $(SOURCE_ROOT)/input_management/mouse_input_event.cpp $(CXXFLAGS) $(CPPFLAGS) $(SDL_CFLAGS) $(GRAPHICS_CFLAGS) $(X11_CFLAGS) $(GLOG_CPPFLAGS)

mouse_state.o: $(SOURCE_ROOT)/input_management/mouse_state.cpp
	$(CXX) -c -o mouse_state.o $(SOURCE_ROOT)/input_management/mouse_state.cpp $(CXXFLAGS) $(CPPFLAGS) $(SDL_CFLAGS) $(GRAPHICS_CFLAGS) $(X11_CFLAGS) $(GLOG_CPPFLAGS)

lifeline.o: $(SOURCE_ROOT)/lifeline.cpp
	$(CXX) -c -o lifeline.o $(SOURCE_ROOT)/lifeline.cpp $(CXXFLAGS) $(CPPFLAGS) $(SDL_CFLAGS) $(GRAPHICS_CFLAGS) $(X11_CFLAGS) $(GLOG_CPPFLAGS)

lifeline_controller.o: $(SOURCE_ROOT)/lifeline_controller.cpp
	$(CXX) -c -o lifeline_controller.o $(SOURCE_ROOT)/lifeline_controller.cpp $(CXXFLAGS) $(CPPFLAGS) $(SDL_CFLAGS) $(GRAPHICS_CFLAGS) $(X11_CFLAGS) $(GLOG_CPPFLAGS)



clean:
	- rm -f main
	- rm -f *.o
