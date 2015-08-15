//--
// Modified by Alex Bradbury (asb@asbradbury.org) to demonstrate embedding SDL2+OpenGL
// in a Qt application, originally part of Sonic Pi.
//
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

//This class sets up and handles the QT main window

// Standard stuff
#include <fstream>
#include <iostream>
#include <string>
#include <math.h>
#include <sstream>
#include <assert.h>

using namespace std;

// Python includes
#include "interpreter.hpp"

// SDL stuff
#include <SDL2/SDL.h>

// Qt stuff
#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QIcon>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QDockWidget>
#include <QPoint>
#include <QSettings>
#include <QSize>
#include <QStatusBar>
#include <QTextEdit>
#include <QToolBar>
#include <QProcess>
#include <QFont>
#include <QTabWidget>
#include <QString>
#include <QStringList>
#include <QSplitter>
#include <QTextStream>
#include <QPixmap>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QCheckBox>
#include <QScrollArea>
#include <QScrollBar>
#include <QLineEdit>

// QScintilla stuff
#include <Qsci/qsciapis.h>
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexerpython.h>

#include "config.hpp"
#include "mainwindow.h"
#include "game_main.hpp"
#include "h_tab_bar.hpp"
#include "input_handler.hpp"
#include "input_manager.hpp"
#include "event_manager.hpp"

// Game window stuff
#define GLM_FORCE_RADIANS

#include <glog/logging.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <chrono>
#include <functional>
#include <glm/vec2.hpp>
#include <memory>
#include <random>
#include <ratio>
#include <string>
#include <utility>
#include <vector>

// Need to access the SDL_Window internals to set the opengl flag
struct SDL_Window
{
    const void *magic;
    Uint32 id;
    char *title;
    SDL_Surface *icon;
    int x, y;
    int w, h;
    int min_w, min_h;
    int max_w, max_h;
    Uint32 flags;
};
typedef struct SDL_Window SDL_Window;

MainWindow::MainWindow(GameMain *exGame):
    colourPalette(palette())

{
    LOG(INFO) << "Constructing MainWindow..." << std::endl;

    game = exGame;
    this->setUnifiedTitleAndToolBarOnMac(true);

    // create workspace with textWidget
    textWidget = new QHTabWidget();
    textWidget->setTabsClosable(false);
    textWidget->setMovable(false);
    textWidget->setTabPosition(QTabWidget::East);
    textWidget->setUsesScrollButtons(true);

    // create workspaces and add them to the textWidget
    for(int ws = 0; ws < workspace_max; ws++)
    {
        workspaces[ws] = new QsciScintilla;
        QString w = QString("%1").arg(QString::number(ws + 1));
        textWidget->addTab(workspaces[ws], w);
    }

    lexer = new QsciLexerPython;
    lexer->setAutoIndentStyle(QsciScintilla::AiMaintain);

    // Autocompletion stuff
    api = new QsciAPIs(lexer);
    QStringList api_names;
    // yes, really
#include "api_list.h"
    for (int api_iter = 0; api_iter < api_names.size(); ++api_iter)
    {
        api->add(api_names.at(api_iter));
    }
    api->prepare();
    QFont font("Monospace");
    font.setStyleHint(QFont::Monospace);
    lexer->setDefaultFont(font);

    // Setup terminal panes
    terminalLayout = new QVBoxLayout;

    terminalDisplay = new QTextEdit;
    terminalDisplay->setReadOnly(true);
    terminalDisplay->setLineWrapMode(QTextEdit::NoWrap);\
    terminalDisplay->document()->setMaximumBlockCount(1000);
    terminalDisplay->zoomIn(1);
    terminalDisplay->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    terminalDisplay->setFocusPolicy(Qt::NoFocus);

    QPushButton *buttonClear = new QPushButton("c");
    terminalDisplay->addScrollBarWidget(buttonClear,Qt::AlignTop);
    terminalButtonLayout = new QHBoxLayout;

    // Setup terminal buttons
    buttonRun = new QPushButton("Run");
    buttonSpeed = new QPushButton("Speed: Slow");

    buttonRun->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    buttonSpeed->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    setRunning(false);
    setFast(false);

    terminalButtonLayout->addWidget(buttonRun);
    terminalButtonLayout->addWidget(buttonSpeed);

    terminalButtonLayout->setContentsMargins(0,0,0,0);

    buttons = new QWidget;
    buttons->setLayout(terminalButtonLayout);

    terminalLayout->addWidget(terminalDisplay);
    terminalLayout->addWidget(buttons);

    terminalLayout->setContentsMargins(0,0,0,0);

    terminalLayout->setStretchFactor(terminalDisplay,4);
    terminalLayout->setStretchFactor(buttons,1);

    terminal = new QWidget;
    terminal->setLayout(terminalLayout);

    for(int ws = 0; ws < workspace_max; ws++)
    {
        initWorkspace(workspaces[ws], ws);
    }

    setTabs(1);

    // Setup draggable splitter for script embedWindow and terminal
    splitter = new QSplitter(Qt::Horizontal);

    splitter->addWidget(textWidget);
    splitter->addWidget(terminal);

    splitter->setCollapsible(0,0);
    splitter->setCollapsible(1,0);
    splitter->setStretchFactor(0,5);
    splitter->setStretchFactor(1,3);

    splitter->setFocusPolicy(Qt::NoFocus);

    // Setup embedWindow
    windowLayout = new QVBoxLayout;

    gameWidget = new QWidget;

    gameWidget->setAttribute(Qt::WA_NativeWindow);

    gameWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    windowLayout->addWidget(gameWidget);
    windowLayout->addWidget(splitter);
    windowLayout->setStretchFactor(gameWidget,3);
    windowLayout->setStretchFactor(splitter,2);

    windowLayout->setContentsMargins(0,0,0,0);

    mainWidget = new QWidget;

    createToolBar();

    setWindowTitle(tr("Pyland"));
    mainWidget->setWindowIcon(QIcon("/images/icon.png"));

    setColourScheme(250,250,197,245,245,165);

    mainWidget->setAutoFillBackground(true);

    mainWidget->setMinimumSize(1080,756);

    this->setContextMenuPolicy(Qt::NoContextMenu);
    this->setCentralWidget(mainWidget);

    LOG(INFO) << gameWidget->winId() << "\n";
    /*
        int result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
        if (result != 0)
        {
            LOG(INFO) << "failed to init SDL\n";
        }
    */
    mainWidget->setLayout(windowLayout);

    embedWindow = SDL_CreateWindowFrom((void*)(gameWidget->winId()));

    LOG(INFO) << "created context\n";
    gameWidget->installEventFilter(this);
    gameWidget->setMouseTracking(true);
    gameWidget->setFocusPolicy(Qt::StrongFocus);
    eventTimer = new QTimer(this);
    eventTimer->setSingleShot(false);
    eventTimer->setInterval(0);
    connect(eventTimer, SIGNAL(timeout()), this, SLOT(timerHandler()));
    eventTimer->start();

    int width = (gameWidget->width());
    int height = gameWidget->height();
    anyOutput = false;
    executeIndex = 1;

    SDL_SetWindowSize(embedWindow, width, height);
    glViewport(0, 0, width, height);
    embedWindow->flags |= SDL_WINDOW_OPENGL;
    SDL_GL_LoadLibrary(NULL);
    glContext = SDL_GL_CreateContext(embedWindow);

    connect(buttonRun,SIGNAL(released()),this,SLOT (clickRun()));
    connect(buttonSpeed,SIGNAL(released()),this,SLOT (clickSpeed()));
    connect(buttonClear,SIGNAL(released()),this,SLOT (clearTerminal()));

    this->showMaximized();

    LOG(INFO) << "Constructed MainWindow" << std::endl;
}

MainWindow::~MainWindow()
{
    //delete all objects created with new

    LOG(INFO) << "Destructing MainWindow..." << std::endl;
    delete textWidget;

    for(int ws = 0; ws < workspace_max; ws++)
    {
        delete buttonIn[ws];
        delete buttonOut[ws];
        delete zoomWidget[ws];
        delete zoomLayout[ws];
        delete workspaces[ws];
    }


    delete lexer;
    api->clear();
    delete api;

    delete terminalDisplay;

    delete buttonRun;
    delete buttonSpeed;
    delete buttonClear;

    delete buttons;

    delete terminalButtonLayout;

    delete terminal;
    delete terminalLayout;

    delete splitter;
    delete gameWidget;

    delete mainWidget;

    delete textWorld;
    delete textLevel;
    delete textCoins;
    delete textTotems;
    delete textLayout;
    delete textInfoWidget;

    delete toolBar;

    delete windowLayout;

    delete eventTimer;

    LOG(INFO) << "Destructed MainWindow" << std::endl;

}

void MainWindow::initWorkspace(QsciScintilla* ws, int i)
{
    ws->setAutoIndent(true);
    ws->setIndentationsUseTabs(false);
    ws->setBackspaceUnindents(true);
    ws->setTabIndents(true);
    ws->setMatchedBraceBackgroundColor(QColor("dimgray"));
    ws->setMatchedBraceForegroundColor(QColor("white"));
    ws->setIndentationWidth(2);
    ws->setIndentationGuides(true);
    ws->setIndentationGuidesForegroundColor(QColor("deep pink"));
    ws->setBraceMatching(QsciScintilla::SloppyBraceMatch);
    ws->setCaretLineVisible(true);
    ws->setCaretLineBackgroundColor(QColor("whitesmoke"));
    ws->setFoldMarginColors(QColor("whitesmoke"),QColor("whitesmoke"));
    ws->setMarginLineNumbers(0, true);
    ws->setMarginWidth(0, "100000");
    ws->setMarginsForegroundColor(QColor("gray"));
    ws->setMarginsForegroundColor(QColor("dark gray"));
    ws->setMarginsFont(QFont("Menlo",5, -1, true));
    ws->setUtf8(true);
    ws->setText("");
    ws->setLexer(lexer);
    ws->zoomIn(13);
    ws->setAutoCompletionThreshold(3);
    ws->setAutoCompletionSource(QsciScintilla::AcsAPIs);
    ws->setSelectionBackgroundColor("DeepPink");
    ws->setSelectionForegroundColor("white");
    ws->setCaretWidth(5);
    ws->setMarginWidth(1,5);
    ws->setCaretForegroundColor("deep pink");

    //Read 9 python scripts and display in scintilla widget

    std::string player_scripts_location = Config::get_instance()["files"]["player_scripts"];
    std::string path = player_scripts_location + "/" + std::to_string(i+1) + ".py";

    LOG(INFO) << "Reading in python scripts..." << endl;

    ifstream fin(path, ios::in);

    std::string script = "";

    while(fin)
    {
        if(fin.bad())
        {
            LOG(INFO) << "Error reading in python script: " << path << endl;
            return;
        }
        else if(fin.eof())
        {
            break;
        }

        std::string curString;

        std::getline(fin,curString);

        script = script + curString;

        if (!fin.eof())
        {
            script = script + "\n";
        }
    }

    QString qScript = QString(script.c_str());

    ws->setText(qScript);

    fin.close();

    //Create zoom buttons for text widget
    zoomLayout[i] = new QHBoxLayout;

    buttonIn[i] = new QPushButton("+");
    buttonOut[i] = new QPushButton("-");                 //Text do not seem centered in buttons

    buttonIn[i]->setMaximumWidth(20);
    buttonOut[i]->setMaximumWidth(20);
    buttonIn[i]->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Preferred);
    buttonOut[i]->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Preferred);

    zoomLayout[i]->addWidget(buttonIn[i]);
    zoomLayout[i]->addWidget(buttonOut[i]);

    zoomLayout[i]->setContentsMargins(0,0,0,0);
    zoomLayout[i]->setSpacing(0);

    zoomWidget[i] = new QWidget;

    zoomWidget[i]->setLayout(zoomLayout[i]);
    zoomWidget[i]->setMaximumWidth(40);

    ws->addScrollBarWidget(zoomWidget[i],Qt::AlignLeft);

    //Connect buttons to functions
    connect(buttonIn[i],SIGNAL(released()),this,SLOT (zoomFontIn()));
    connect(buttonOut[i],SIGNAL(released()),this,SLOT (zoomFontOut()));

}

void MainWindow::createToolBar()
{
    toolBar = new QToolBar;
    toolBar->setFloatable(false);
    toolBar->setMovable(false);

    textLayout = new QHBoxLayout;

    textWorld = new QLabel("");
    textLevel = new QLabel("");
    textCoins = new QLabel("");
    textTotems = new QLabel("");

    //Use the required variables
    //textWorld->setText("World: 1");
    //textLevel->setText("Level: 1");
    //textCoins->setText("Coins: 0");
    //textTotems->setText("Totems: 0/5");

    textLayout->addWidget(textWorld);
    textLayout->addWidget(textLevel);
    textLayout->addWidget(textCoins);
    textLayout->addWidget(textTotems);
    textInfoWidget = new QWidget();
    textInfoWidget->setLayout(textLayout);

    textInfoWidget->setFocusPolicy(Qt::NoFocus);
    textInfoWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    toolBar->addWidget(textInfoWidget);

    addToolBar(toolBar);
}

SDL_Scancode MainWindow::parseKeyCode(QKeyEvent *keyEvent)
{
    //Hard coded keyboard bindings from QT to SDL for none numerical/alphabetical keys
    //None numerical/alphabetical keys return native virtual keys greater than 6000 that do not directly map to SDL keys
    switch (keyEvent->key())
    {
    case Qt::Key_Enter:
        return SDL_SCANCODE_RETURN;
    case Qt::Key_Return:
        return SDL_SCANCODE_RETURN;
    case Qt::Key_Left:
        return SDL_SCANCODE_LEFT;
    case Qt::Key_Right:
        return SDL_SCANCODE_RIGHT;
    case Qt::Key_Up:
        return SDL_SCANCODE_UP;
    case Qt::Key_Down:
        return SDL_SCANCODE_DOWN;
    case Qt::Key_Escape:
        return SDL_SCANCODE_ESCAPE;
    case Qt::Key_Tab:
        return SDL_SCANCODE_TAB;
    case Qt::Key_Backspace:
        return SDL_SCANCODE_BACKSPACE;
    case Qt::Key_Insert:
        return SDL_SCANCODE_INSERT;
    case Qt::Key_Delete:
        return SDL_SCANCODE_DELETE;
    case Qt::Key_Pause:
        return SDL_SCANCODE_PAUSE;
    case Qt::Key_Print:
        return SDL_SCANCODE_PRINTSCREEN;
    case Qt::Key_SysReq:
        return SDL_SCANCODE_SYSREQ;
    case Qt::Key_Clear:
        return SDL_SCANCODE_CLEAR;
    case Qt::Key_Home:
        return SDL_SCANCODE_HOME;
    case Qt::Key_PageUp:
        return SDL_SCANCODE_PAGEUP;
    case Qt::Key_PageDown:
        return SDL_SCANCODE_PAGEDOWN;
    case Qt::Key_Shift:
        return SDL_SCANCODE_LSHIFT;
    case Qt::Key_Control:
        return SDL_SCANCODE_LCTRL;
    case Qt::Key_Alt:
        return SDL_SCANCODE_LALT;
    case Qt::Key_AltGr:
        return SDL_SCANCODE_RALT;
    case Qt::Key_CapsLock:
        return SDL_SCANCODE_CAPSLOCK;
    case Qt::Key_NumLock:
        return SDL_SCANCODE_NUMLOCKCLEAR;
    case Qt::Key_ScrollLock:
        return SDL_SCANCODE_SCROLLLOCK;
    case Qt::Key_F1:
        return SDL_SCANCODE_F1;
    case Qt::Key_F2:
        return SDL_SCANCODE_F2;
    case Qt::Key_F3:
        return SDL_SCANCODE_F3;
    case Qt::Key_F4:
        return SDL_SCANCODE_F4;
    case Qt::Key_F5:
        return SDL_SCANCODE_F5;
    case Qt::Key_F6:
        return SDL_SCANCODE_F6;
    case Qt::Key_F7:
        return SDL_SCANCODE_F7;
    case Qt::Key_F8:
        return SDL_SCANCODE_F8;
    case Qt::Key_F9:
        return SDL_SCANCODE_F9;
    case Qt::Key_F10:
        return SDL_SCANCODE_F10;
    case Qt::Key_F11:
        return SDL_SCANCODE_F11;
    case Qt::Key_F12:
        return SDL_SCANCODE_F12;
    case Qt::Key_Menu:
        return SDL_SCANCODE_MENU;
    default:
        switch (keyEvent->nativeScanCode())
        {
        //Map keypad numbers to standard numbers for executing scripts
        //Numlock must be on
        case 87:
            return SDL_SCANCODE_1;
        case 88:
            return SDL_SCANCODE_2;
        case 89:
            return SDL_SCANCODE_3;
        case 83:
            return SDL_SCANCODE_4;
        case 84:
            return SDL_SCANCODE_5;
        case 85:
            return SDL_SCANCODE_6;
        case 79:
            return SDL_SCANCODE_7;
        case 80:
            return SDL_SCANCODE_8;
        case 81:
            return SDL_SCANCODE_9;
        default:
            //Remaining keys can be mapped directly
            return SDL_GetScancodeFromKey(keyEvent->nativeVirtualKey());
        }
    }
}

Uint8 MainWindow::parseButton(QMouseEvent *mouseEvent)
{
    //Convert from QT mouse button to SDL mouse button
    switch (mouseEvent->button())
    {
    case Qt::LeftButton:
        return SDL_BUTTON_LEFT;
    case Qt::RightButton:
        return SDL_BUTTON_RIGHT;
    case Qt::MidButton:
        return SDL_BUTTON_MIDDLE;
    case Qt::XButton1:
        return SDL_BUTTON_X1;
    case Qt::XButton2:
        return SDL_BUTTON_X2;
    default:
        return SDL_BUTTON_LEFT;
    }


}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    QKeyEvent *keyEvent = NULL;
    QMouseEvent *mouseEvent = NULL;
    //QFocusEvent *focusEvent = NULL;
    //QResizeEvent *resizeEvent = NULL;
    //Check if QT event is a keyboard or mouse event, and push it to SDL
    if (event->type() == 6)//QEvent::KeyPress)
    {
        keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key())
        {
            SDL_Event sdlEvent;
            sdlEvent.type = SDL_KEYDOWN;
            sdlEvent.key.state = SDL_PRESSED;
            sdlEvent.key.keysym.scancode= parseKeyCode(keyEvent);
            SDL_PushEvent(&sdlEvent);
        }
    }
    else if (event->type() == 7)//QEvent::KeyRelease
    {
        keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key())
        {
            SDL_Event sdlEvent;
            sdlEvent.type = SDL_KEYUP;
            sdlEvent.key.state = SDL_PRESSED;
            sdlEvent.key.keysym.scancode= parseKeyCode(keyEvent);//SDL_GetScancodeFromKey(keyEvent->nativeVirtualKey());//(SDL_GetScancodeFromKey(keyEvent->nativeVirtualKey()));
            SDL_PushEvent(&sdlEvent);
        }
    }
    else if (event->type() == 2)//QEvent::MouseButtonPress
    {
        mouseEvent = static_cast<QMouseEvent*>(event);
        SDL_Event sdlEvent;
        sdlEvent.type = SDL_MOUSEBUTTONDOWN;
        sdlEvent.button.state = SDL_PRESSED;
        sdlEvent.button.x = mouseEvent->x();
        sdlEvent.button.y = mouseEvent->y();
        sdlEvent.button.button = parseButton(mouseEvent);
        SDL_PushEvent(&sdlEvent);
    }
    else if (event->type() == 3)//QEvent::MouseButtonRelease
    {
        mouseEvent = static_cast<QMouseEvent*>(event);
        SDL_Event sdlEvent;
        sdlEvent.type = SDL_MOUSEBUTTONUP;
        sdlEvent.button.state = SDL_PRESSED;
        sdlEvent.button.x = mouseEvent->x();
        sdlEvent.button.y = mouseEvent->y();
        sdlEvent.button.button = parseButton(mouseEvent);
        SDL_PushEvent(&sdlEvent);
    }
    else if (event->type() == 5)  //QEvent::MouseMove
    {
        mouseEvent = static_cast<QMouseEvent*>(event);
        SDL_Event sdlEvent;
        sdlEvent.type = SDL_MOUSEMOTION;
        sdlEvent.button.state = SDL_PRESSED;
        sdlEvent.motion.x = mouseEvent->x();
        sdlEvent.motion.y = mouseEvent->y();
        SDL_PushEvent(&sdlEvent);
    }
    else if(event->type() == 25)  //QEvent::WindowDeactivate
    {
        //When the window is no longer at the front
        SDL_Event sdlEvent;
        sdlEvent.type = SDL_WINDOWEVENT;
        sdlEvent.window.event = SDL_WINDOWEVENT_HIDDEN;
        sdlEvent.window.windowID = SDL_GetWindowID(embedWindow);
        SDL_PushEvent(&sdlEvent);
    }
    else if(event->type() == 24)  //QEvent::WindowActivate
    {
        SDL_Event sdlEvent;
        sdlEvent.type = SDL_WINDOWEVENT;
        sdlEvent.window.event = SDL_WINDOWEVENT_SHOWN;
        sdlEvent.window.windowID = SDL_GetWindowID(embedWindow);
        SDL_PushEvent(&sdlEvent);
    }
    else if(event->type() == 14)  //QEvent::Resize
    {
        SDL_Event sdlEvent;
        sdlEvent.type = SDL_WINDOWEVENT;
        sdlEvent.window.event = SDL_WINDOWEVENT_RESIZED;
        sdlEvent.window.windowID = SDL_GetWindowID(embedWindow);
        sdlEvent.window.data1 = splitter->width();
        sdlEvent.window.data2 = splitter->height();
        SDL_PushEvent(&sdlEvent);
    }
    else
    {
        return QObject::eventFilter(obj, event);
    }
    return true;
}

void MainWindow::timerHandler()
{
    //Execute the game loop for this frame
    game->game_loop(gameWidget->underMouse());
}

//Show the entire scripter panel
void MainWindow::showScripterPanel()
{
    splitter->show();
}

//Hide the entire scripter panel
void MainWindow::hideScripterPanel()
{
    splitter->hide();
}

//Allow the player to use the entire scripter panel
void MainWindow::enableScripterPanel()
{
    splitter->setEnabled(true);
}

//Prevent the player from using the entire scripter panel
void MainWindow::disableScripterPanel()
{
    splitter->setEnabled(false);
}

//Allow the player to edit scripts
void MainWindow::enableScripter()
{
    textWidget->setEnabled(true);
}

//Prevent the player from editing scripts
void MainWindow::disableScripter()
{
    textWidget->setEnabled(false);
}

void MainWindow::setTabs(int num){
    //textWidget->currentWidget()->hide();
    //textWidget->removeTab(9);

    std::cout << "num is " << std::to_string(num) << std::endl;

    currentTabs = num;

    std::cout << "sEG? " << std::to_string(num) << std::endl;

    textWidget->clear();
     std::cout << "seg2? " << std::to_string(num) << std::endl;
//    int initialCount = textWidget->count();
//    for(int ws = 0; ws < workspace_max; ws++)
//    {
//        std::cout << "ws is " << std::to_string(ws) << std::endl;
//        std::cout << "count is " << std::to_string(textWidget->count()) << std::endl;
//        if ((num-1) < ws){
//            std::cout << "case one " << std::endl;
//            //if (initialCount >= (ws+1)){
//               // std::cout << "case two " << std::endl;
//                //textWidget->removeTab(ws);
//            //}
//        }
//        else{
//            std::cout << "case three " << std::endl;
//            //if (initialCount < (ws+1)){
//                std::cout << "case four " << std::endl;
//                QString w = QString("%1").arg(QString::number(ws + 1));
//                textWidget->addTab(workspaces[ws],w);
//            //}
//        }
//    }

//    if (num == 1){
//        //splitter->removeWidget(textWidget);
//        //splitter->removeWidget(terminal);
//
//        textWidget->hide();
//
//        splitter->insertWidget(1,workspaces[0]);
//
//        splitter->refresh();
//        //splitter->addWidget(terminal);
//    }

    if ((num < 1 ) || (num > workspace_max)){
        LOG(INFO) << "May only set between 1 and workspace_max ("<< std::to_string(workspace_max) << ") tabs" << std::endl;
        return;
    }

    for(int ws = 0; ws < num; ws++)
    {
        QString w = QString("%1").arg(QString::number(ws + 1));
        textWidget->addTab(workspaces[ws],w);
    }

//    textWidget->removeTab(0);
//    textWidget->removeTab(1);
//    textWidget->removeTab(2);
//    textWidget->removeTab(3);
//    textWidget->removeTab(4);
//    textWidget->removeTab(5);
//    textWidget->removeTab(6);
//    textWidget->removeTab(7);
//    textWidget->removeTab(8);
//    textWidget->removeTab(9);

    //textWidget->clear();

    std::cout << "Got ehre " << std::endl;
    //textWidget->removeTab(7);
    //textWidget->removeTab(6);

    //textWidgetLayout->addWidget(workspaces[0]);



    //If one, put QSciScintilla there, otherwise put tabs

    //for(int ws = 0; ws < workspace_max; ws++)
    //{

    //}
}

//When the QT window is closed
void MainWindow::closeEvent(QCloseEvent *event)
{
    event->accept();
}

//Called when the run button is clicked
//Run the currently open script
void MainWindow::clickRun()
{
    runCode(0);
}

//Script defines the current script to execute
//If zero the currently open script is run
void MainWindow::runCode(int script)
{
    if (script_running)
    {
        setRunning(false);
        updateSpeed();
        InputHandler::get_instance()->run_list(InputHandler::INPUT_HALT);
        //game->getCallbackState().stop();
        //TODO, create hooks into script-runner to stop the correct script.
    }
    else
    {
        int index;
        QsciScintilla *ws;

        if (script == 0)
        {
            index = getCurrentScript();
            ws = (QsciScintilla*)textWidget->currentWidget();
        }
        else
        {
            if (script <= currentTabs){
                index = script;
                ws = (QsciScintilla*)textWidget->widget(script-1);  //Subtract one since first widget is at qt index 0
            }
            else{
                LOG(INFO) << "Script "<< std::to_string(script) << " is not available to be run" << std::endl;
                return;
            }
        }

        //read in the player scripts (as defined in the config file)
        std::string player_scripts_location = Config::get_instance()["files"]["player_scripts"];
        std::string path = player_scripts_location + "/" + std::to_string(index) + ".py";

        //Save script as 'Script 1'/'Script 2' etc
        //Also save as 'Current Script.py' as temporary measure before new input manager
        //This python script is always run in bootstrapper.py (in start)
        ofstream fout(path.c_str(), ios::out|ios::trunc);
        //ofstream foutcopy(player_scripts_location + "/current.py", ios::out|ios::trunc);

        if(!(fout.good()))// && foutcopy.good()))
        {
            LOG(INFO) << "Output file is bad" << endl;
            return;
        }

        fout << ws->text().toStdString();
        //foutcopy << ws->text().toStdString();

        fout.close();
        //foutcopy.close();

        setRunning(true);
        updateSpeed();
        if (script == 0)
        {
            executeIndex = getCurrentScript();
        }
        else
        {
            executeIndex = script;
        }
        InputHandler::get_instance()->run_list(InputHandler::INPUT_RUN); //Run this list of events registered against run in the input handler
    }
    setGameFocus();
}

//Called when the speed button is clicked
void MainWindow::clickSpeed()
{
    toggleSpeed();
}

//Toggle the speed setting
void MainWindow::toggleSpeed()
{
    setFast(!fast);
    updateSpeed();
    setGameFocus();
}

//If a script is being run and the speed is set to fast, update the game speed
//Otherwise keep the game at normal speed
void MainWindow::updateSpeed()
{
    if (fast && script_running)
    {
        auto now(std::chrono::steady_clock::now());
        auto time_passed = now - game->get_start_time();

        float completion(time_passed / std::chrono::duration<float>(6.0f));
        completion = std::min(completion, 1.0f);

        // Using an easing function from the internetz:
        //
        //     start + (c⁵ - 5·c⁴ + 5·c³) change
        //
        float eased(1.0f + 511.0f * (
                        + 1.0f * completion * completion * completion * completion * completion
                        - 5.0f * completion * completion * completion * completion
                        + 5.0f * completion * completion * completion
                    ));

        EventManager::get_instance()->time.set_game_seconds_per_real_second(eased);
    }
    else
    {
        EventManager::get_instance()->time.set_game_seconds_per_real_second(1.0);
    }
}

//Output text to the terminal
//If error is set the text is read, otherwise it is black
void MainWindow::pushTerminalText(std::string text, bool error)
{
    if (error)
    {
        terminalDisplay->setTextColor(QColor("red"));
    }
    QString qtext = QString::fromStdString(text+"\n");
    terminalDisplay->insertPlainText(qtext);
    terminalDisplay->verticalScrollBar()->setValue(terminalDisplay->verticalScrollBar()->maximum());
    terminalDisplay->setTextColor(QColor("black"));
}

//Increase the font size of the current script
void MainWindow::zoomFontIn()
{
    ((QsciScintilla*)textWidget->currentWidget())->zoomIn(3);
    setGameFocus();
}

//Decrease the font size of the current script
void MainWindow::zoomFontOut()
{
    ((QsciScintilla*)textWidget->currentWidget())->zoomOut(3);
    setGameFocus();
}

//Give the game widget focus so keyboard events are handled
void MainWindow::setGameFocus()
{
    gameWidget->setFocus();
}

//Update the world text in the tool bar
void MainWindow::setWorld(std::string text){
    QString qtext = QString::fromStdString("World: "+text);
    textWorld->setText(qtext);
}

//Update the level text in the tool bar
void MainWindow::setLevel(std::string text){
    QString qtext = QString::fromStdString("Level: "+text);
    textLevel->setText(qtext);
}

//Update the coin text in the tool bar
void MainWindow::setCoins(int value){
    QString qtext = QString::fromStdString("Coins: "+std::to_string(value));
    textCoins->setText(qtext);
}

//Update the totem text in the tool bar
void MainWindow::setCurTotems(int value,bool show){
  //Update the number of totems collected on the current level
  if (show){
      textTotems->show();
      QString qtext = QString::fromStdString("Totems: "+std::to_string(value)+"/5");
      textTotems->setText(qtext);
  }
  else
  {
      textTotems->hide();
  }
}

//Insert text to the end of the currently open text editor tab
void MainWindow::insertToTextEditor(std::string text)
{
    QsciScintilla *ws;
    ws = (QsciScintilla*)textWidget->currentWidget();

    QString qtext = QString::fromStdString(text);

    QString priorText = ws->text();

    clearTextEditor();
    ws->insert(qtext);
    ws->insert(priorText);
}

//Clear the text in the currently open text editor tab
void MainWindow::clearTextEditor()
{
    QsciScintilla *ws;
    ws = (QsciScintilla*)textWidget->currentWidget();

    ws->clear();
}

//Get the text from the currently open text editor tab
std::string MainWindow::getEditorText()
{
    QsciScintilla *ws;
    ws = (QsciScintilla*)textWidget->currentWidget();

    return ws->text().toStdString();
}

//Clear the text from the terminal
void MainWindow::clearTerminal()
{
    terminalDisplay->clear();
}

//Update the run button when it has been clicked
//This gets called when clicking on the Run/Halt button
//and from engine.cpp update_status, when the script terminates
void MainWindow::setRunning(bool option)
{
    script_running = option;

    if (script_running)
    {
        buttonRun->setText("Halt");
    }
    else
    {
        buttonRun->setText("Run");
    }

    updateSpeed();

}

//Update the fast button when it has been clicked
void MainWindow::setFast(bool option)
{
    fast = option;

    if (fast)
    {
        buttonSpeed->setText("Speed: Fast");
    }
    else
    {
        buttonSpeed->setText("Speed: Slow");
    }

}

//Specify whether there has been any output to the terminal
//during the current script execution
void MainWindow::setAnyOutput(bool option)
{
    anyOutput = option;
}

//Set the colours of the QT interface
void MainWindow::setColourScheme(int r1, int g1, int b1, int r2, int g2, int b2)
{

    textInfoWidget->setStyleSheet(("background-color: rgb("+std::to_string(r2)+","+std::to_string(g2)+","+std::to_string(b2)+");"+
                                   "border: 5px rgb("+std::to_string(r2)+","+std::to_string(g2)+","+std::to_string(b2)+");"+
                                   "font: 17pt;").c_str());

    colourPalette.setColor(QPalette::Background,QColor(r1,g1,b1));
    colourPalette.setColor(QPalette::Button,QColor(r2,g2,b2));
    mainWidget->setPalette(colourPalette);
}

SDL_Window* MainWindow::getSDLWindow()
{
    return embedWindow;
}

//Get the current width of the QT game widget
//so the SDL window can be displayed with the correct size
int MainWindow::getGameWidgetWidth()
{
    return gameWidget->width();
}

//Get the current height of the QT game widget
//so the SDL window can be displayed with the correct size
int MainWindow::getGameWidgetHeight()
{
    return gameWidget->height();
}

//Find out if any text has been output to the terminal
//during the current script execution
bool MainWindow::getAnyOutput()
{
    return anyOutput;
}

//Get the currently open script tab number
int MainWindow::getCurrentScript()
{
    return (textWidget->currentIndex())+ 1;
}

//Get the number of the script that is to be run
//This may be different to the open one (due to keybindings)
int MainWindow::getExecuteScript()
{
    return executeIndex;
}
