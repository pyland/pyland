# Makefile for graphics tests.

CXX = g++-4.7
CXXFLAGS = -g -Wall -Wextra -O2 -DDEBUG

SDL_CFLAGS    = $(shell sdl2-config --cflags)
SDL_LDLIBS    = $(shell sdl2-config --libs)

all: game_window_test



game_window_test: game_window_test.o game_window.o
	$(CXX) -o game_window_test game_window.o game_window_test.o $(CXXFLAGS) $(SDL_CFLAGS) $(SDL_LDLIBS)

game_window_test.o: game_window_test.cpp
	$(CXX) -c -o game_window_test.o game_window_test.cpp $(CXXFLAGS) $(SDL_CFLAGS)


game_window.o: game_window.cpp
	$(CXX) -c -o game_window.o game_window.cpp $(CXXFLAGS) $(SDL_CFLAGS)



clean:
	- rm -f game_window_test
	- rm -f game_window_test.o
	- rm -f game_window.o
