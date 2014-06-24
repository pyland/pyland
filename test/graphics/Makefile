# Makefile for graphics tests.

CC = gcc
CFLAGS = -g

SDL_CFLAGS    = $(shell pkg-config sdl --cflags)
SDL_LDLIBS    = $(shell pkg-config sdl --libs)

# Taken from hello_triangle example
GLES_CFLAGS   = -I/opt/vc/include -I/opt/vc/include/interface/vmcs_host/linux -I/opt/vc/include/interface/vcos/pthreads
GLES_LDLIBS   = -lGLESv2 -lEGL -lbcm_host -lpthread
GLES_LDFLAGS  = -L/opt/vc/lib/

X11_CFLAGS = $(shell pkg-config x11 --cflags)
X11_LDLIBS = $(shell pkg-config x11 --libs)

all: triangle texture_stress

triangle: triangle.o
	$(CC) -o triangle triangle.o $(CFLAGS) $(GLES_LDFLAGS) $(SDL_LDLIBS) $(GLES_LDLIBS) $(X11_LDLIBS)

triangle.o: triangle.c
	$(CC) -c -o triangle.o triangle.c $(CFLAGS) $(SDL_CFLAGS) $(GLES_CFLAGS) $(X11_CFLAGS)



texture_stress: texture_stress.o
	$(CC) -o texture_stress texture_stress.o $(CFLAGS) $(GLES_LDFLAGS) $(SDL_LDLIBS) $(GLES_LDLIBS) $(X11_LDLIBS)

texture_stress.o: texture_stress.c
	$(CC) -c -o texture_stress.o texture_stress.c $(CFLAGS) $(SDL_CFLAGS) $(GLES_CFLAGS) $(X11_CFLAGS)



clean:
	@rm -f triangle
	@rm -f triangle.o
	@rm -f texture_stress
	@rm -f texture_stress.o
