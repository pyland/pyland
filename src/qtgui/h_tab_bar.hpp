#ifndef H_TAB_BAR_H
#define H_TAB_BAR_H

#include<QStylePainter>

class QHTabBar : public QTabBar
{
protected:
    QSize tabSizeHint(int) const {
        return QSize(25, 36);
    }
    void paintEvent(QPaintEvent *) {
        QStylePainter stylePainter(this);
        for (int i = 0; i < count(); i++) {
            QStyleOptionTabV3 tab;
            initStyleOption(&tab,i);
            QString tabText = tab.text;
            tab.text = QString();

            stylePainter.drawControl(QStyle::CE_TabBarTab,tab);

            QPainter painter;
            painter.begin(this);
            QRect rect = tabRect(i);
            painter.drawText(rect, Qt::AlignCenter, tabText);
            painter.end();
        }
    }

};

class QHTabWidget : public QTabWidget
{
public:
    QHTabWidget() : QTabWidget() {
        setTabBar(new QHTabBar());
    }

};

#endif
