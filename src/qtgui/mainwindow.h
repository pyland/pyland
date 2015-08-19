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
class QsciAPIs;
class QProcess;
class QTextEdit;
class QSplitter;
class SonicPiLexer;
class QString;
class QSlider;
class QPalette;
class GameMain;



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(GameMain *exGame);
    ~MainWindow();

    void showScripterPanel();
    void hideScripterPanel();
    void enableScripterPanel();
    void disableScripterPanel();
    void enableScripter();
    void disableScripter();

    void setTabs(int num);

    void updateSpeed();
    void pushTerminalText(std::string text, bool error);
    void setWorld(std::string text);
    void setLevel(std::string text);
    void setCoins(int value);
    void setCurTotems(int value,bool show);
    void insertToTextEditor(std::string text);
    void clearTextEditor();
    std::string getEditorText();
    void runCode(int script);
    void toggleSpeed();
    void setRunning(bool option);
    void setFast(bool option);
    void setAnyOutput(bool option);
    void setColourScheme(int r1, int g1, int b1, int r2, int g2, int b2);
    SDL_Window* getSDLWindow();
    int getGameWidgetWidth();
    int getGameWidgetHeight();
    bool getAnyOutput();
    int getCurrentScript();
    int getExecuteScript();

protected:
    SDL_Scancode parseKeyCode(QKeyEvent *keyEvent);
    Uint8 parseButton(QMouseEvent *mouseEvent);
    bool eventFilter(QObject *obj, QEvent *event);
    void closeEvent(QCloseEvent *event);

private slots:
    void zoomFontIn();
    void zoomFontOut();
    void setGameFocus();
    void timerHandler();
    void clearTerminal();
    void clickRun();
    void clickSpeed();

private:
    void initWorkspace(QsciScintilla* ws, int i);
    void createToolBar();
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
    //indicates whether a script is running or not
    bool script_running;
    QPushButton *buttonSpeed;
    //whether the script setting fast or slow
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

    QPalette colourPalette;

    SDL_GLContext glContext;
    SDL_Window *embedWindow;
    QTimer *eventTimer;

    GameMain *game;

    //Specifies whether there has been any output during the current code execution,
    //it determine if output separate lines are needed
    bool anyOutput;

    //Index of the script to be executed
    int executeIndex;

    int currentTabs;
};

#endif
