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
#include <iostream>
#include <math.h>
#include <sstream>
#include <assert.h>

// Python includes
#include "interpreter.hpp"

// SDL stuff
#include <SDL2/SDL.h>
//#include <GL/glu.h>

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

#include "mainwindow.h"
#include "game_main.hpp"
#include "h_tab_bar.hpp"

// Game window stuff
#define GLM_FORCE_RADIANS

#include <glog/logging.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <chrono>
#include <functional>
#include <glm/vec2.hpp>
#include <iostream>
#include <memory>
#include <random>
#include <ratio>
#include <string>
#include <sstream>
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

    terminalButtonLayout = new QHBoxLayout;

    // Setup terminal buttons
    buttonRun = new QPushButton("Run");
    buttonSpeed = new QPushButton("Speed: Slow");

    buttonRun->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    buttonSpeed->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

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

    windowLayout->addWidget(gameWidget);
    windowLayout->addWidget(splitter);
    windowLayout->setStretchFactor(gameWidget,3);
    windowLayout->setStretchFactor(splitter,2);

    windowLayout->setContentsMargins(0,0,0,0);

    mainWidget = new QWidget;

    createActions();
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

    std::cout << gameWidget->winId() << "\n";
/*
    int result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    if (result != 0)
    {
        LOG(INFO) << "failed to init SDL\n";
    }
*/
    mainWidget->setLayout(windowLayout);

    embedWindow = SDL_CreateWindowFrom((void*)(gameWidget->winId()));

    SDL_SetWindowSize(embedWindow, 600, 420);
    glViewport(0, 0, 600,420);
    embedWindow->flags |= SDL_WINDOW_OPENGL;
    SDL_GL_LoadLibrary(NULL);
    glContext = SDL_GL_CreateContext(embedWindow);
    LOG(INFO) << "created context\n";
    gameWidget->installEventFilter(this);
    gameWidget->setFocusPolicy(Qt::StrongFocus);
    eventTimer = new QTimer(this);
    eventTimer->setSingleShot(false);
    eventTimer->setInterval(0);
    connect(eventTimer, SIGNAL(timeout()), this, SLOT(timerHandler()));
    eventTimer->start();

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

    delete buttons;

    delete terminalButtonLayout;

    delete terminal;
    delete terminalLayout;

    delete splitter;
    delete gameWidget;

    delete mainWidget;

    delete textInfo;
    delete toolBar;

    delete windowLayout;

    delete eventTimer;

    LOG(INFO) << "Destructed MainWindow" << std::endl;

}

SDL_Window* MainWindow::getSDLWindow(){
    return embedWindow;
}

void MainWindow::showMax(){
    this->showMaximized();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
/*
    QKeyEvent *keyEvent = NULL;
    if (event->type() == QEvent::KeyPress)
    {
        keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key())
        {
            SDL_Event sdlEvent;
            sdlEvent.type = SDL_KEYDOWN;
            sdlEvent.key.state = SDL_PRESSED;
            SDL_PushEvent(&sdlEvent);
            std::cout << "got a Qt keydown event\n";
        }
    }
    else
    {
        return QObject::eventFilter(obj, event);
    }
*/
    return true;
}

void MainWindow::timerHandler()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch(event.type)
        {
        case SDL_KEYDOWN:
            std::cout << " got an SDL keydown event\n";
            break;
        }
    }
    game->game_loop();
    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapWindow(embedWindow);
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
    ws->setAutoCompletionSource(QsciScintilla::AcsAll);
    ws->setSelectionBackgroundColor("DeepPink");
    ws->setSelectionForegroundColor("white");
    ws->setCaretWidth(5);
    ws->setMarginWidth(1,5);
    ws->setCaretForegroundColor("deep pink");

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

bool MainWindow::saveAs()
{
    return false;
}

void MainWindow::runCode()
{
    //terminalDisplay->clear();
    //terminalDisplay->hide();
    //std::string code = ((QsciScintilla*)textWidget->currentWidget())->text().toStdString();
    setGameFocus();
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


void MainWindow::clearOutputPanels()
{
    terminalDisplay->clear();
}

void MainWindow::createActions()
{
    connect(buttonRun,SIGNAL(released()),this,SLOT (runCode()));
    //connect(buttonSpeed,SIGNAL(released()),this,SLOT (setGameFocus()));
    //connect(terminalDisplay,SIGNAL(clicked()),this,SLOT (setGameFocus()));
    //connect(splitter,SIGNAL(splitterMoved()),this,SLOT (setGameFocus()));
    //connect(textInfo,SIGNAL(selectionChanged()),this,SLOT (setGameFocus()));
}

void MainWindow::setGameFocus()
{
    gameWidget->setFocus();
}

void MainWindow::createToolBar()
{
    toolBar = new QToolBar;
    toolBar->setFloatable(false);
    toolBar->setMovable(false);

    textInfo = new QTextEdit("");
    textInfo->setContextMenuPolicy(Qt::NoContextMenu);
    textInfo->setFontPointSize(17.0);
    textInfo->setTextInteractionFlags(Qt::NoTextInteraction);

    //Insert current information
    textInfo->insertPlainText("World: 1   Level: 1    Totems: 0/5");

    textInfo->setWordWrapMode(QTextOption::NoWrap);
    textInfo->setAlignment(Qt::AlignLeft);
    textInfo->setMinimumWidth(400);
    textInfo->setMaximumHeight(38);
    textInfo->setReadOnly(true);
    textInfo->setStyleSheet("background-color: rgb(245,245,165);border: rgb(245,245,165);");
    textInfo->setFocusPolicy(Qt::NoFocus);

    toolBar->addWidget(textInfo);

    addToolBar(toolBar);
}


void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}
