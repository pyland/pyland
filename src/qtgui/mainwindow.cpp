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

MainWindow::MainWindow(GameMain *exGame)
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
    terminalDisplay->setLineWrapMode(QTextEdit::NoWrap);
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

    QPalette colourPalette(palette());
    colourPalette.setColor(QPalette::Background,QColor(250,250,197));
    colourPalette.setColor(QPalette::Button,QColor(245,245,165));

    mainWidget->setPalette(colourPalette);
    mainWidget->setAutoFillBackground(true);

    mainWidget->setMinimumSize(600,420);

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

    SDL_SetWindowSize(embedWindow, width, height);
    glViewport(0, 0, width, height);
    embedWindow->flags |= SDL_WINDOW_OPENGL;
    SDL_GL_LoadLibrary(NULL);
    glContext = SDL_GL_CreateContext(embedWindow);

    connect(buttonRun,SIGNAL(released()),this,SLOT (runCode()));
    connect(buttonSpeed,SIGNAL(released()),this,SLOT (toggleSpeed()));
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

SDL_Window* MainWindow::getSDLWindow()
{
    return embedWindow;
}

void MainWindow::showMax()
{
    this->showMaximized();
}

SDL_Scancode MainWindow::parseKeyCode(QKeyEvent *keyEvent)
{
    //Hard coded keyboard bindings from QT to SDL for none numerical/alphabetical keys
    //None numerical/alphabetical keys return native virtual keys greater than 6000 that do not directly map to SDL keys
    switch (keyEvent->key())
    {
    case Qt::Key_Enter:
        return SDL_SCANCODE_KP_ENTER;
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
        return SDL_SCANCODE_KP_TAB;
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
        //Remaining keys can be mapped directly
        return SDL_GetScancodeFromKey(keyEvent->nativeVirtualKey());
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
    //connect(ws->horizontalScrollBar(),SIGNAL(sliderPressed()),this,SLOT (setGameFocus()));
    //connect(ws->verticalScrollBar(),SIGNAL(sliderPressed()),this,SLOT (setGameFocus()));

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->accept();
}

//This gets called when clicking on the Run/Halt button
//and from engine.cpp update_status, when the script terminates
void MainWindow::setRunning(bool option)
{
    running = option;

    if (running)
    {
        buttonRun->setText("Halt");
    }
    else
    {
        buttonRun->setText("Run");
    }

    updateSpeed();

}

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

bool MainWindow::saveAs()
{
    return false;
}

void MainWindow::runCode()
{

    if (running)
    {
        setRunning(false);
        updateSpeed();
        InputHandler::get_instance()->run_list(InputHandler::INPUT_HALT);
        //game->getCallbackState().stop();
        //TODO, create hooks into script-runner to stop the correct script.
    }
    else{
        QsciScintilla *ws = (QsciScintilla*)textWidget->currentWidget();

        int index = (textWidget->currentIndex())+ 1;

        //read in the player scripts (as defined in the config file)
        std::string player_scripts_location = Config::get_instance()["files"]["player_scripts"];
        std::string path = player_scripts_location + "/" + std::to_string(index) + ".py";

        //Save script as 'Script 1'/'Script 2' etc
        //Also save as 'Current Script.py' as temporary measure before new input manager
        //This python script is always run in bootstrapper.py (in start)
        ofstream fout(path.c_str(), ios::out|ios::trunc);
        ofstream foutcopy(player_scripts_location + "/current.py", ios::out|ios::trunc);

        if(!(fout.good() && foutcopy.good()))
        {
            LOG(INFO) << "Output file is bad" << endl;
            return;
        }

        fout << ws->text().toStdString();
        foutcopy << ws->text().toStdString();

        fout.close();
        foutcopy.close();

        setRunning(true);
        updateSpeed();
        InputHandler::get_instance()->run_list(InputHandler::INPUT_RUN); //Run this list of events registered against run in the input handler
    }
    setGameFocus();
}

void MainWindow::toggleSpeed(){
    setFast(!fast);
    updateSpeed();
    setGameFocus();
}

//If a script is being run and the speed is set to fast, update the game speed
//Otherwise keep the game at normal speed
void MainWindow::updateSpeed(){
    if (fast && running){
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
    else{
        EventManager::get_instance()->time.set_game_seconds_per_real_second(1.0);
    }
}

void MainWindow::pushTerminalText(std::string text, bool error){
    //terminalDisplay->insertPlainText(text);
    if (error){
        terminalDisplay->setTextColor(QColor("red"));
    }
    QString qtext = QString::fromStdString(text);
    terminalDisplay->insertPlainText(qtext);
    terminalDisplay->verticalScrollBar()->setValue(terminalDisplay->verticalScrollBar()->maximum());
    terminalDisplay->setTextColor(QColor("black"));
}

void MainWindow::zoomFontIn()
{
    ((QsciScintilla*)textWidget->currentWidget())->zoomIn(3);
    setGameFocus();
}

void MainWindow::zoomFontOut()
{
    ((QsciScintilla*)textWidget->currentWidget())->zoomOut(3);
    setGameFocus();
}


void MainWindow::documentWasModified()
{
    setWindowModified(textEdit->isModified());
}


void MainWindow::clearTerminal()
{
    terminalDisplay->clear();
}

//Give the game widget focus so keyboard events are handled
void MainWindow::setGameFocus()
{
    gameWidget->setFocus();
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
    textWorld->setText("World: 1");
    textLevel->setText("Level: 1");
    textCoins->setText("Coins: 0");
    textTotems->setText("Totems: 0/5");

    textLayout->addWidget(textWorld);
    textLayout->addWidget(textLevel);
    textLayout->addWidget(textCoins);
    textLayout->addWidget(textTotems);
    textInfoWidget = new QWidget();
    textInfoWidget->setLayout(textLayout);

    //textInfoWidget->setMinimumWidth(400);
    //textInfoWidget->setMaximumHeight(38);
    textInfoWidget->setStyleSheet("background-color: rgb(245,245,165);border: rgb(245,245,165);font: 17pt;");
    textInfoWidget->setFocusPolicy(Qt::NoFocus);
    textInfoWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    toolBar->addWidget(textInfoWidget);

    addToolBar(toolBar);
}

void MainWindow::updateToolBar()
{
    //Use the require variables
    textWorld->setText("World: 1");
    textLevel->setText("Level: 1");
    textCoins->setText("Coins: 0");
    textTotems->setText("Totems: 0/5");
}


void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

int MainWindow::getGameWidgetWidth(){
    return gameWidget->width();
}

int MainWindow::getGameWidgetHeight(){
    return gameWidget->height();
}
