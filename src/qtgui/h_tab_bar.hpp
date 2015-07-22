#ifndef H_TAB_BAR_H
#define H_TAB_BAR_H

#include<QStylePainter>

class QHTabWidget : public QTabWidget
{
public:
    QHTabWidget() : QTabWidget() {
        setTabBar(new QHTabBar());
    }

};

class QHTabBar : public QTabBar
{
private:
    QSize tabSizeHint(int) const {
        return QSize(25, 36);
    }
    void paintEvent(*QPaintEvent) {
        QStylePainter stylePainter = new QStylePainter(this);
        for (int i = 0; i < count(); i++) {
            QStyleOptionTabV3 tabStyleOption;
            initStyleOption(&tabStyleOption,i);

            QString tabText = tabStyleOption.text;
            tabStyleOption.text = QString();
            stylePainter.drawControl(QStyle::CE_TabBarTab,tabStyleOption);

            QPainter tabPainter;
            tabPainter.begin(this);

            QRect curRect = tabRect(i);
            tabPainter.drawText(curRect, Qt::AlignCenter, tabText);
            tabPainter.end();
        }
    }

};

#endif
