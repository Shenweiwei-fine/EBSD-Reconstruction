#ifndef MYSPLITLINE_H
#define MYSPLITLINE_H

#include <QMouseEvent>
#include <QPainter>
#include <QLabel>


class MySplitLine : public QWidget
{

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *e);
    void mouseReleaseEvent(QMouseEvent *E);

public:
    MySplitLine(QWidget *parent = nullptr);

signals:
    void changeofPos();

private:
    QPoint pressed;
    QPoint movin;



};

#endif // MYSPLITLINE_H
