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

class QAction;
class QMenu;
class QsciScintilla;
class QProcess;
class QTextEdit;
class QSplitter;
class SonicPiLexer;
class QString;
class QSlider;

//class QDialog;
//class QLabel;
//class QSplashScreen;
//class QCheckBox;
//class QListWidgetItem;
//class QListWidget;
//class QProcess;
//class QFuture;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    SDL_Window* getSDLWindow();

protected:
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void runCode();
    bool saveAs();
    void documentWasModified();
    void zoomFontIn();
    void zoomFontOut();
    void setGameFocus();
    void timerHandler();

private:
    //Challenge* pick_challenge(ChallengeData* challenge_data);

    void initGameWindow();
    void initWorkspace(QsciScintilla* ws);
    void clearOutputPanels();
    void createActions();
    void createToolBar();
    void createStatusBar();
    std::string number_name(int);
    std::string workspaceFilename(QsciScintilla* text);
    QsciScintilla* filenameToWorkspace(std::string filename);

    QsciLexerPython *lexer;

    QsciScintilla *textEdit;
    static const int workspace_max = 9;
    QsciScintilla *workspaces[workspace_max];
    QTextEdit *terminalDisplay;
    QSplitter *splitter;

    QPushButton *buttonIn;
    QPushButton *buttonOut;
    QPushButton *buttonRun;
    QPushButton *buttonSpeed;

    QTabWidget *textWidget;

    QTextEdit *textInfo;

    QWidget *gameWidget;

    QToolBar *toolBar;

    QAction *runAct;
    QAction *stopAct;
    QAction *saveAct;
    QAction *textIncAct;
    QAction *textDecAct;

    QAction *saveAsAct;
    QAction *exitAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;

    QCheckBox *print_output;
    QCheckBox *check_args;

    QMap<QString, QString> *map;

    QLabel *imageLabel;

    SDL_GLContext glContext;
    SDL_Window *embedWindow;
    QTimer *eventTimer;

};

#endif
