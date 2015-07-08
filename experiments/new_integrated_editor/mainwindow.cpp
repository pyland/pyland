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

// SDL stuff
#include <SDL2/SDL.h>
#include <GL/glu.h>

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

// QScintilla stuff
#include <Qsci/qsciapis.h>
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexerpython.h>

#include "mainwindow.h"

#include "h_tab_bar.hpp"

// Need to access the SDL_Window internals to set the opengl flag
struct SDL_Window {
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

MainWindow::MainWindow(QApplication &app) {

  this->setUnifiedTitleAndToolBarOnMac(true);

  tabs = new QHTabWidget();
  tabs->setTabsClosable(false);
  tabs->setMovable(false);
  tabs->setTabPosition(QTabWidget::East);

  // create workspaces and add them to the tabs
  for(int ws = 0; ws < workspace_max; ws++) {
	  workspaces[ws] = new QsciScintilla;
	  QString w = QString("%1").arg(QString::number(ws + 1));
	  tabs->addTab(workspaces[ws], w);
  }

  lexer = new QsciLexerPython;
  lexer->setAutoIndentStyle(QsciScintilla::AiMaintain);

  // Autocompletion stuff
  QsciAPIs* api = new QsciAPIs(lexer);
  QStringList api_names;
  // yes, really
  #include "api_list.h"
  for (int api_iter = 0; api_iter < api_names.size(); ++api_iter) {
	  api->add(api_names.at(api_iter));
  }
  api->prepare();
  QFont font("Monospace");
  font.setStyleHint(QFont::Monospace);
  lexer->setDefaultFont(font);

  // Setup output and error panes
  //outputPane = new QTextEdit;
  errorPane = new QTextEdit;

  //outputPane->setReadOnly(true);
  errorPane->setReadOnly(true);
  errorPane->setLineWrapMode(QTextEdit::NoWrap);
//  outputPane->setLineWrapMode(QTextEdit::NoWrap);
//#if defined(Q_OS_WIN)
//  outputPane->setFontFamily("Courier New");
//#elif defined(Q_OS_MAC)
//  outputPane->setFontFamily("Menlo");
//#else
//  outputPane->setFontFamily("Bitstream Vera Sans Mono");
//#endif

//  outputPane->document()->setMaximumBlockCount(1000);
  errorPane->document()->setMaximumBlockCount(1000);

//  outputPane->zoomIn(1);
  errorPane->zoomIn(1);
  //errorPane->setMaximumHeight(100);

  QVBoxLayout *sideWidgetLayout = new QVBoxLayout;
  //tabs->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
  //sideWidgetLayout->addWidget(tabs,0,0,0);
  //errorPane->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

  QWidget *newmainWidget = new QWidget;

  QWidget *mainWidget = new QWidget;
  mainWidget->setAttribute(Qt::WA_NativeWindow);
  //sideWidgetLayout->addWidget(mainWidget);
  //setCentralWidget(mainWidget);
  ////sideWidgetLayout->addWidget(mainWidget);
  for(int ws = 0; ws < workspace_mable to x; ws++) {
	  initWorkspace(workspaces[ws]);
  }

  splitter = new QSplitter(Qt::Horizontal);

  splitter->addWidget(tabs);
  splitter->addWidget(errorPane);

  //splitter->setSizes([2,1)];
  splitter->setCollapsible(0,0);
  splitter->setCollapsible(1,0);
  splitter->setStretchFactor(0,5);
  splitter->setStretchFactor(1,3);

  ////sideWidgetLayout->addWidget(splitter);

  //sideWidgetLayout->setStretchFactor(splitter,3);
  //sideWidgetLayout->setStretchFactor(mainWidget,0);

  sideWidgetLayout->addWidget(mainWidget);
  sideWidgetLayout->addWidget(splitter);

  bool result1 = sideWidgetLayout->setStretchFactor(mainWidget,3);

  std::cout << result1 << "test";

  bool result2 = sideWidgetLayout->setStretchFactor(splitter,2);

  std::cout << result2 << "testing";


  sideWidgetLayout->setStretch(1,2);
  //sideWidgetLayout->addStretch(1);



  //sideWidgetLayout->setStrechFactor(splitter,2);

  newmainWidget->setWindowTitle(tr("Pyland"));



  newmainWidget->setLayout(sideWidgetLayout);
  newmainWidget->showMaximized();

  //addDockWidget(Qt::RightDockWidgetArea, errorPane);

  //////policy = widg.sizePolicy();
  //////policy.setVerticalStretch(1);
  //////widg.setSizePolicy(policy);


//  QWidget *dummySideWidget = new QWidget;
//  dummySideWidget->setLayout(sideWidgetLayout);
//  QDockWidget *sideWidget = new QDockWidget(this);
//  sideWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
//  sideWidget->setFloating(false);
//  sideWidget->setAllowedAreas(Qt::BottomDockWidgetArea);
//  sideWidget->setWidget(dummySideWidget);
//  addDockWidget(Qt::BottomDockWidgetArea, sideWidget);

  //createActions();
  //createToolBar();
  //createStatusBar();



  //this->showMaximized();

  std::cout << mainWidget->winId() << "\n";

  int result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  if (result != 0) {
	std::cout << "failed to init SDL\n";
  }

  embedWindow = SDL_CreateWindowFrom((void*)(mainWidget->winId()));
  SDL_SetWindowSize(embedWindow, 200, 200);
  glViewport(0, 0, 200, 200);
  embedWindow->flags |= SDL_WINDOW_OPENGL;
  SDL_GL_LoadLibrary(NULL);
  glContext = SDL_GL_CreateContext(embedWindow);
  glClearColor(0.25f, 0.50f, 1.0f, 1.0f);
  std::cout << "created context\n";
  mainWidget->installEventFilter(this);
  mainWidget->setFocusPolicy(Qt::StrongFocus);
  eventTimer = new QTimer(this);
  eventTimer->setSingleShot(false);
  eventTimer->setInterval(0);
  connect(eventTimer, SIGNAL(timeout()), this, SLOT(timerHandler()));
  eventTimer->start();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
  QKeyEvent *keyEvent = NULL;
  if (event->type() == QEvent::KeyPress) {
	keyEvent = static_cast<QKeyEvent*>(event);
	if (keyEvent->key()) {
	  SDL_Event sdlEvent;
	  sdlEvent.type = SDL_KEYDOWN;
	  sdlEvent.key.state = SDL_PRESSED;
	  SDL_PushEvent(&sdlEvent);
	  std::cout << "got a Qt keydown event\n";
	}
  } else {
	return QObject::eventFilter(obj, event);
  }
  return true;
}

void MainWindow::timerHandler() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
	switch(event.type) {
	  case SDL_KEYDOWN:
		std::cout << " got an SDL keydown event\n";
		break;
	}
  }
  glClear(GL_COLOR_BUFFER_BIT);
  SDL_GL_SwapWindow(embedWindow);
}

void MainWindow::initWorkspace(QsciScintilla* ws) {
  ws->setAutoIndent(true);
  ws->setIndentationsUseTabs(false);
  ws->setBackspaceUnindents(true);
  ws->setTabIndents(true);
  ws->setMatchedBraceBackgroundColor(QColor("dimgray"));
  ws->setMatchedBraceForegroundColor(QColor("white"));

  ws->setIndentationWidth(2);
  ws->setIndentationGuides(true);
  ws->setIndentationGuidesForegroundColor(QColor("deep pink"));
  ws->setBraceMatching( QsciScintilla::SloppyBraceMatch);
  ws->setCaretLineVisible(true);
  ws->setCaretLineBackgroundColor(QColor("whitesmoke"));
  ws->setFoldMarginColors(QColor("whitesmoke"),QColor("whitesmoke"));
  ws->setMarginLineNumbers(0, true);
  ws->setMarginWidth(0, "1000000");
  ws->setMarginsBackgroundColor(QColor("whitesmoke"));
  ws->setMarginsForegroundColor(QColor("dark gray"));
  ws->setMarginsFont(QFont("Menlo",5, -1, true));
  ws->setUtf8(true);
  ws->setText("");
  ws->setLexer(lexer);
  ws->zoomIn(13);
  ws->setAutoCompletionThreshold(5);
  ws->setAutoCompletionSource(QsciScintilla::AcsAPIs);
  ws->setSelectionBackgroundColor("DeepPink");
  ws->setSelectionForegroundColor("white");
  ws->setCaretWidth(5);
  ws->setCaretForegroundColor("deep pink");

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
  errorPane->clear();
  errorPane->hide();
  statusBar()->showMessage(tr("Running...."), 2000);
  std::string code = ((QsciScintilla*)tabs->currentWidget())->text().toStdString();
}

void MainWindow::zoomFontIn()
{
  ((QsciScintilla*)tabs->currentWidget())->zoomIn(1);
}

void MainWindow::zoomFontOut()
{
  ((QsciScintilla*)tabs->currentWidget())->zoomOut(1);
}


void MainWindow::documentWasModified()
{
  setWindowModified(textEdit->isModified());
}


void MainWindow::clearOutputPanels()
{
	outputPane->clear();
	errorPane->clear();
}

void MainWindow::createActions()
{

  runAct = new QAction(QIcon(":/images/run.png"), tr("&Run"), this);
  runAct->setShortcut(tr("Ctrl+R"));
  runAct->setStatusTip(tr("Run the code in the current workspace"));
  runAct->setToolTip(tr("Run the code in the current workspace (Ctrl-R)"));
  connect(runAct, SIGNAL(triggered()), this, SLOT(runCode()));

  stopAct = new QAction(QIcon(":/images/stop.png"), tr("&Stop"), this);
  stopAct->setShortcut(tr("Ctrl+Q"));
  stopAct->setStatusTip(tr("Stop all running code"));
  stopAct->setToolTip(tr("Stop all running code (Ctrl-Q)"));

  saveAsAct = new QAction(QIcon(":/images/save.png"), tr("&Save &As..."), this);
  saveAsAct->setStatusTip(tr("Save the current workspace under a new name"));
  saveAsAct->setToolTip(tr("Save the current workspace under a new name"));
  connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

  textIncAct = new QAction(QIcon(":/images/size_up.png"), tr("&Increase &Text &Size"), this);
  textIncAct->setStatusTip(tr("Make text bigger"));
  textIncAct->setToolTip(tr("Make text bigger"));
  connect(textIncAct, SIGNAL(triggered()), this, SLOT(zoomFontIn()));

  textDecAct = new QAction(QIcon(":/images/size_down.png"), tr("&Decrease &Text &Size"), this);
  textDecAct->setStatusTip(tr("Make text smaller"));
  textDecAct->setToolTip(tr("Make text smaller"));
  connect(textDecAct, SIGNAL(triggered()), this, SLOT(zoomFontOut()));

  QAction *reloadAct = new QAction(this);
  reloadAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_U));
  addAction(reloadAct);
}

void MainWindow::createToolBar()
{

  QWidget *spacer = new QWidget();
  spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

  toolBar = addToolBar(tr("Tools"));

  toolBar->setIconSize(QSize(270/3, 111/3));
  toolBar->addAction(runAct);
  toolBar->addAction(stopAct);

  toolBar->addAction(saveAsAct);
  toolBar->addWidget(spacer);

  toolBar->addAction(textDecAct);
  toolBar->addAction(textIncAct);

}


void MainWindow::createStatusBar()
{
	statusBar()->showMessage(tr("Ready"));
}
