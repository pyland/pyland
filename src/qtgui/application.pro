CONFIG       += release

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
}

HEADERS       = mainwindow.h \
                api_list.h     \
		parsingfunctions.hpp   \
		h_tab_bar.hpp 	\
		game_window.hpp \

SOURCES       = mainwindow.cpp \
		game_init.cpp \
		game_window.cpp \

RESOURCES     = application.qrc \

LIBS         += -lqscintilla2 -lSDL2 -lGL

ICON = images/app.icns
