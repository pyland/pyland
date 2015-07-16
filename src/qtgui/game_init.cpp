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

#include "game_init.hpp"
#include "mainwindow.h"

//#include <QApplication>
#include <QSplashScreen>
#include <QPixmap>
#include <QBitmap>

// Standard stuff
#include <iostream>
#include <math.h>
#include <sstream>
#include <assert.h>

// SDL stuff
#include <SDL2/SDL.h>
#include <GL/glu.h>

// Qt stuff
#include <QApplication>


#include "parsingfunctions.hpp"

//#include <tuple>

//std::tuple<void*,void*,void*> game_init(int argc, char *argv[])
GameInit::GameInit()
{
    //Q_INIT_RESOURCE(application);
    create_apih_from_wrapper();
    //app = new QApplication(argc, argv);
    app = new QApplication(0, *"");
    app->setStyle("gtk");
    app->setAttribute(Qt::AA_NativeWindows, true);

    MainWindow* mainWin = new MainWindow();

    //sdlWin = mainWin->getSDLWindow();


    //return std::tuple<void*,void*,void*> (mainWin->getSDLWindow(),app,mainWin);

    int ret = app->exec();
//
    delete mainWin;
    delete app;
//
//    return ret;
}

QApplication* GameInit::getApp(){
    return app;
}

//MainWindow* getMainWin(){
//    return mainWin;
//}

SDL_Window* GameInit::getSdlWin(){
    return sdlWin;
}

void GameInit::execApp(){
    //app->exec();
}



