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
#include <QHBoxLayout>
#include <QVBoxLayout>
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
class GameMain;
class QsciAPIs;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(GameMain *exGame);
    ~MainWindow();

    void showMax();
    void setRunning(bool option);
    void setFast(bool option);
    void updateSpeed();
    void pushTerminalText(std::string text, bool error);
    void updateToolBar();
    SDL_Window* getSDLWindow();
    int getGameWidgetWidth();
    int getGameWidgetHeight();
protected:
    SDL_Scancode parseKeyCode(QKeyEvent *keyEvent);
    Uint8 parseButton(QMouseEvent *mouseEvent);
    bool eventFilter(QObject *obj, QEvent *event);
    void closeEvent(QCloseEvent *event);

private slots:
    void runCode();
    void toggleSpeed();
    bool saveAs();
    void documentWasModified();
    void zoomFontIn();
    void zoomFontOut();
    void setGameFocus();
    void timerHandler();
    void clearTerminal();

private:
    void initWorkspace(QsciScintilla* ws, int i);
    void createToolBar();
    void createStatusBar();
    std::string number_name(int);
    std::string workspaceFilename(QsciScintilla* text);
    QsciScintilla* filenameToWorkspace(std::string filename);

    QsciLexerPython *lexer;

    QsciScintilla *textEdit;

    static const int workspace_max = 9;
    QsciScintilla *workspaces[workspace_max];
    QWidget *zoomWidget[workspace_max];
    QHBoxLayout *zoomLayout[workspace_max];
    QPushButton *buttonIn[workspace_max];
    QPushButton *buttonOut[workspace_max];

    QTextEdit *terminalDisplay;
    QSplitter *splitter;
    QPushButton *buttonRun;
    bool running;
    QPushButton *buttonSpeed;
    bool fast;
    QWidget *mainWidget;
    QTabWidget *textWidget;
    QWidget *gameWidget;

    QLabel *textWorld;
    QLabel *textLevel;
    QLabel *textCoins;
    QLabel *textTotems;
    QHBoxLayout *textLayout;
    QWidget *textInfoWidget;

    QToolBar *toolBar;
    QsciAPIs* api;
    QWidget *buttons;
    QWidget *terminal;
    QVBoxLayout *terminalLayout;
    QHBoxLayout *terminalButtonLayout;
    QPushButton *buttonClear;
    QVBoxLayout *windowLayout;

    SDL_GLContext glContext;
    SDL_Window *embedWindow;
    QTimer *eventTimer;

    GameMain *game;

};

#endif
