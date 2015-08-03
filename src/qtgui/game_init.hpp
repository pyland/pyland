#ifndef GAME_INIT_H
#define GAME_INIT_H
#include <SDL2/SDL.h>

class QApplication;
class MainWindow;
class GameMain;

class GameInit{
private:
    QApplication* app;
    MainWindow *mainWin;
    SDL_Window* sdlWin;

public:
    GameInit(int &argc, char **argv, GameMain* exGame);
    ~GameInit();

    QApplication* getApp();
    MainWindow* getMainWin();
    SDL_Window* getSdlWin();
    void pass_running_to_qt(bool option);
    void pass_text_to_qt(std::string text,bool error);
    void execApp();
    void showMain();
    int getGameWidth();
    int getGameHeight();

};

#endif
