CONFIG       += release

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
}

HEADERS       = mainwindow.h \
                api_list.h     \
		parsingfunctions.hpp   \
		h_tab_bar.hpp 	\
		game_window.hpp \
		game_init.hpp \

SOURCES       = mainwindow.cpp \
		game_init.cpp \
		game_window.cpp \
		moc_mainwindow.cpp \

LIBS         += -lqscintilla2 -lSDL2 -lGL
