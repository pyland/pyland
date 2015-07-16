//New include calls

//#include <tuple>

//#include <QApplication>
//#include <MainWindow>
//#include <SDL_Winow>

////#ifndef MAINWINDOW_H
////#define MAINWINDOW_H
////
////#include <QMainWindow>
////
////// Standard stuff
////#include <iostream>
////#include <math.h>
////#include <sstream>
////#include <assert.h>
////
////// SDL stuff
////#include <SDL2/SDL.h>
////#include <GL/glu.h>
////
////// Qt stuff
////#include <QAction>
////#include <QApplication>
////#include "mainwindow.h"


#ifndef GAME_INIT_H
#define GAME_INIT_H

//#include <QMainWindow>
//#include <QDialog>
//#include <QLabel>
//#include <QSplashScreen>
//#include <QCheckBox>
//#include <QListWidgetItem>
//#include <QListWidget>
//#include <QProcess>
//#include <QFuture>
//#include <iostream>
//#include <Qsci/qscilexerpython.h>

//#include "mainwindow.h"
#include <SDL2/SDL.h>

class QApplication;

//std::tuple<void*,void*,void*> game_init(int argc, char *argv[]);

class GameInit{
private:
    QApplication* app;
//    MainWindow *mainWin;
    SDL_Window* sdlWin;

public:
    GameInit();

    QApplication* getApp();
//    MainWindow* getMainWin();
    SDL_Window* getSdlWin();
    void execApp();


};

#endif
