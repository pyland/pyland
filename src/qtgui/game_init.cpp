//--
// This file is part of Sonic Pi: http://sonic-pi.net
// Full project source: https://github.com/samaaron/sonic-pi
// License: https://github.com/samaaron/sonic-pi/blob/master/LICENSE.md
//
// Copyright 2013, 2014 by Sam Aaron (http://sam.aaron.name).
// All rights reserved.
//
// Permission is granted for use, copying, modification, distribution,
// and distribution of modified versions of this work as long as this
// notice is included.
//++
#include "game_main.hpp"
#include "game_init.hpp"
#include "mainwindow.h"

#include <QSplashScreen>
#include <QPixmap>
#include <QBitmap>

// Standard stuff
#include <iostream>
#include <math.h>
#include <sstream>
#include <assert.h>
#include <glog/logging.h>

// SDL stuff
#include <SDL2/SDL.h>
#include <GL/glu.h>
// Qt stuff
#include <QApplication>
#include "parsingfunctions.hpp"

GameInit::GameInit(int argc, char *argv[], GameMain *exGame)
{
    create_apih_from_wrapper();
    app = new QApplication(argc,argv);
    app->setStyle("gtk");
    app->setAttribute(Qt::AA_NativeWindows, true);

    MainWindow* mainWin = new MainWindow(exGame);

    //Get the SDL window from the widget in the QT interface, so it can be drawn to in game_main
    sdlWin = mainWin->getSDLWindow();
}

QApplication* GameInit::getApp(){
    return app;
}

MainWindow* GameInit::getMainWin(){
    return mainWin;
}

SDL_Window* GameInit::getSdlWin(){
    return sdlWin;
}

void GameInit::execApp(){
    app->exec();
}

void GameInit::showMain(){
    mainWin->showMax();
}

void GameInit::delApp(){
    delete app;
}

void GameInit::delMainWin(){
    delete mainWin;
}



