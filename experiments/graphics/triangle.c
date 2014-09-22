/* Square in OpenGL ES */

#include <stdio.h>
#include <stdlib.h>

#include <GLES/gl.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <SDL.h>
#include <SDL_syswm.h>

#include <X11/Xlib.h>


/* I don't need to be neat today. */
int shutingdown = 0;

int width = 640;
int height = 400;

SDL_SysWMinfo wmInfo;
int windowX = 0;
int windowY = 0;

EGLDisplay display;
EGLSurface surface;
EGLContext context;

EGLConfig config;
EGLint configCount;



static int setup_sdl () {
  int result;
  SDL_Surface* surface;
  XWindowAttributes attributes;
  Window child;

  result = SDL_Init (SDL_INIT_VIDEO);
  
  if (result != 0) {
    return 1;
  }

  /* We don't need the surface other than to check success. */
  surface = SDL_SetVideoMode (width, height, 8, SDL_RESIZABLE);

  if (surface == NULL) {
    SDL_Quit();
    return 1;
  }
  
  SDL_GetWMInfo(&wmInfo);
  /* XGetInputFocus(wmInfo.info.x11.display, &w, &revertTo); */
  /* XGetWindowAttributes(wmInfo.info.x11.display, wmInfo.info.x11.window, &attributes); */
  XTranslateCoordinates(wmInfo.info.x11.display, wmInfo.info.x11.window, XDefaultRootWindow(wmInfo.info.x11.display), 0, 0, &windowX, &windowY, &child);
  /* windowX = attributes.x; */
  /* windowY = attributes.y; */

  return 0;
}

static int setup_ogles () {
  EGLBoolean result;

  static EGL_DISPMANX_WINDOW_T nativeWindow;
  DISPMANX_ELEMENT_HANDLE_T dispmanElement;
  DISPMANX_DISPLAY_HANDLE_T dispmanDisplay;
  DISPMANX_UPDATE_HANDLE_T dispmanUpdate;

  VC_RECT_T destination;
  VC_RECT_T source;
  
  static const EGLint attributeList[] = {
    EGL_RED_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_ALPHA_SIZE, 8,
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT, /* (???) */
    EGL_NONE
  };

  /* Get an EGL display connection */

  display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  if (display == EGL_NO_DISPLAY) {
    fprintf(stderr, "Error getting display.\n");
    return 1;
  }

  /* Initialize EGL display connection */
  result = eglInitialize(display, NULL, NULL);
  if (result == EGL_FALSE) {
    fprintf(stderr, "Error initializing display connection.\n");
    return 1;
  }

  /* Get frame buffer configuration */
  result = eglChooseConfig(display, attributeList, &config, 1, &configCount);
  if (result == EGL_FALSE) {
    fprintf(stderr, "Error getting frame buffer configuration.\n");
    return 1;
  }

  /* Create EGL rendering context */
  context = eglCreateContext(display, config, EGL_NO_CONTEXT, NULL);
  if (context == EGL_NO_CONTEXT) {
    fprintf(stderr, "Error creating rendering context.\n");
    return 1;
  }

  /* Create EGL window surface */

  destination.x = windowX;
  destination.y = windowY;
  destination.width  = width;
  destination.height = height;
  source.x = 0;
  source.y = 0;
  source.width  = width  << 16; /* (???) */
  source.height = height << 16; /* (???) */

  dispmanDisplay = vc_dispmanx_display_open(0); /* (???) */
  dispmanUpdate  = vc_dispmanx_update_start(0); /* (???) */

  dispmanElement = vc_dispmanx_element_add(dispmanUpdate, dispmanDisplay,
					   0/*layer*/, &destination, 0/*src*/,
					   &source, DISPMANX_PROTECTION_NONE,
					   0 /*alpha*/, 0/*clamp*/, 0/*transform*/); /* (???) */
  
  nativeWindow.element = dispmanElement;
  nativeWindow.width = width; /* (???) */
  nativeWindow.height = height; /* (???) */
  vc_dispmanx_update_submit_sync(dispmanUpdate); /* (???) */

  surface = eglCreateWindowSurface(display, config, &nativeWindow, NULL);
  if (surface == EGL_NO_SURFACE) {
    fprintf (stderr, "Error error creating window surface.\n");
    return 1;
  }

  /* Connect the context to the surface */
  result = eglMakeCurrent(display, surface, surface, context);
  if (result == EGL_FALSE) {
    fprintf(stderr, "Error connecting context to surface.\n");
    return 1;
  }

  /* CONGRATULATIONS! Now you can do OpenGL as normal! */

  return 0;
}

static int setup_ogl () {
  glClearColor(0.25f, 0.50f, 1.0f, 1.0f);
  
  glEnable(GL_CULL_FACE);
  
  glMatrixMode(GL_MODELVIEW);

  return 0;
}

static void draw_square () {
  glClear(GL_COLOR_BUFFER_BIT);
  eglSwapBuffers(display, surface);
  /* STUB */
}

static void clean_up_ogles () {
  /* Release OpenGL resources */
  eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
  eglDestroySurface(display, surface);
  eglDestroyContext(display, context);
  eglTerminate(display);
}


static int resize_olges () {
  EGLBoolean result;
  
  VC_RECT_T destination;
  VC_RECT_T source;
  
  static EGL_DISPMANX_WINDOW_T nativeWindow;
  DISPMANX_ELEMENT_HANDLE_T dispmanElement;
  DISPMANX_DISPLAY_HANDLE_T dispmanDisplay;
  DISPMANX_UPDATE_HANDLE_T dispmanUpdate;
  
  destination.x = 0;
  destination.y = 0;
  destination.width  = width;
  destination.height = height;
  source.x = 0;
  source.y = 0;
  source.width  = width  << 16; /* (???) */
  source.height = height << 16; /* (???) */

  dispmanDisplay = vc_dispmanx_display_open(0); /* (???) */
  dispmanUpdate  = vc_dispmanx_update_start(0); /* (???) */

  dispmanElement = vc_dispmanx_element_add(dispmanUpdate, dispmanDisplay,
					   0/*layer*/, &destination, 0/*src*/,
					   &source, DISPMANX_PROTECTION_NONE,
					   0 /*alpha*/, 0/*clamp*/, 0/*transform*/); /* (???) */
  
  nativeWindow.element = dispmanElement;
  nativeWindow.width = width; /* (???) */
  nativeWindow.height = height; /* (???) */
  vc_dispmanx_update_submit_sync(dispmanUpdate); /* (???) */

  eglDestroySurface(display, surface);
  eglDestroyContext(display, context);
  
  surface = eglCreateWindowSurface(display, config, &nativeWindow, NULL);
  if (surface == EGL_NO_SURFACE) {
    fprintf (stderr, "Error error creating window surface.\n");
    return 1;
  }

  /* Connect the context to the surface */
  result = eglMakeCurrent(display, surface, surface, context);
  if (result == EGL_FALSE) {
    fprintf(stderr, "Error connecting context to surface.\n");
    return 1;
  }

  setup_ogl();

  return 0;
}

static void update_controls () {
  int needReset = 0;
  
  SDL_Event event;
  XWindowAttributes attributes;
  int newWindowX;
  int newWindowY;
  
  Window child;

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      shutingdown = 1;
      break;
    case SDL_VIDEORESIZE:
      width = event.resize.w;
      height = event.resize.h;
      SDL_SetVideoMode (width, height, 8, SDL_RESIZABLE);
      needReset = 1;
      break;
    }
  }
  
  XTranslateCoordinates(wmInfo.info.x11.display, wmInfo.info.x11.window, XDefaultRootWindow(wmInfo.info.x11.display), 0, 0, &newWindowX, &newWindowY, &child);
  
  if (windowX != newWindowX || windowY != newWindowY) {
    windowX = newWindowX;
    windowY = newWindowY;
    needReset = 1;
  }

  if (needReset) {
    fprintf (stderr, "Relocate/Resize: %i, %i ; %i, %i\n", windowX, windowY, width, height);
    
    clean_up_ogles();
    setup_ogles();
    setup_ogl();
  }
}

static void clean_up_sdl () {
  SDL_Quit ();
}



int main (int argc, char** argv) {
  /* Raspberry Pi specific initialisation. */
  bcm_host_init ();

  /* Setup an SDL window for basic input and window management. */
  if (setup_sdl() != 0) {
    fprintf (stderr, "Error initialising SDL.\n");
    return 1;
  }

  /* Perform OpenGL ES specific initialisation. */
  if (setup_ogles() != 0) {
    fprintf (stderr, "Error initialising OpenGL ES.\n");
    return 1;
  }

  /* Sets up general OpenGL (not ES specific), such as projection/model
   * matrices.
   */
  if (setup_ogl() != 0) {
    fprintf (stderr, "Error setting up OpenGL (not ES specific code).\n");
    return 1;
  }

  while (!shutingdown) {
    /* Handle window motion/resizing and quit requests. */
    update_controls ();

    /* Draws a square. */
    draw_square ();
  }

  /* Cleanly put the toys back in the box. */
  clean_up_ogles ();
  clean_up_sdl ();
  
  return 0;
}
