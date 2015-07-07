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


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QLabel>
#include <QSplashScreen>
#include <QCheckBox>
#include <QListWidgetItem>
#include <QListWidget>
#include <QProcess>
#include <QFuture>
#include <iostream>
#include <Qsci/qscilexerpython.h>
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

class QAction;
class QMenu;
class QsciScintilla;
class QProcess;
class QTextEdit;
class SonicPiLexer;
class QString;
class QSlider;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();


//protected:
//    void closeEvent(QCloseEvent *event);
//    bool eventFilter(QObject *obj, QEvent *event);
//
//private slots:
//    void runCode();
//    bool saveAs();
//    void documentWasModified();
//    void zoomFontIn();
//    void zoomFontOut();
//    void timerHandler();
//
private:

//    void initWorkspace(QsciScintilla* ws);
//    void clearOutputPanels();
//    void createActions();
//    void createToolBar();
//    void createStatusBar();
//    std::string number_name(int);
//    std::string workspaceFilename(QsciScintilla* text);
//    QsciScintilla* filenameToWorkspace(std::string filename);
//

      QsciAPIs*  api;
//
//    QsciScintilla *textEdit;
    static const int workspace_max = 10;
    QsciScintilla *workspaces[workspace_max];
    QTextEdit *outputPane;
    QTextEdit *errorPane;

    QTabWidget *tabs;

//    QWidget * dummySideWidget;
//    QWidget * mainWidget;
//    QDockWidget * sideWidget;
//    QVBoxLayout *sideWidgetLayout;

    QsciLexerPython *lexer;
//
//    QToolBar *toolBar;
//
//    QAction *runAct;
//    QAction *stopAct;
//    QAction *saveAct;
//    QAction *textIncAct;
//    QAction *textDecAct;
//
//    QAction *saveAsAct;
//    QAction *exitAct;
//    QAction *cutAct;
//    QAction *copyAct;
//    QAction *pasteAct;
//    QAction *reloadAct;
//
//    QCheckBox *print_output;
//    QCheckBox *check_args;
//
//    QMap<QString, QString> *map;
//
//    QLabel *imageLabel;
//
//    SDL_GLContext glContext;
//    SDL_Window *embedWindow;
//    QTimer *eventTimer;
//
//    QWidget *spacer;
//


};

#endif
