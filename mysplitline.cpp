#include "mysplitline.h"

MySplitLine::MySplitLine(QWidget *parent)
{
    this->setParent(parent);
}

void MySplitLine::paintEvent(QPaintEvent *E){

    QPainter p(this);
    p.setPen(Qt::blue);
    p.setBrush(Qt::darkCyan);
    p.drawRect(0, 0, width() - 1, height() - 1);
}


void MySplitLine::mousePressEvent(QMouseEvent *e)
{
    e->accept();

    if (e->button() == Qt::LeftButton) {
        QWidget *tmp= this->parentWidget();
        pressed = tmp->mapFromGlobal(e->globalPos());
    }
}

void MySplitLine::mouseMoveEvent(QMouseEvent *e)
{
    if (e->buttons() & Qt::LeftButton) {
        QWidget *tmp= this->parentWidget();
        movin = tmp->mapFromGlobal(e->globalPos());
        e->accept();
    }
}

void MySplitLine::mouseReleaseEvent(QMouseEvent *e)
{

    if (e->button() != Qt::LeftButton)
        return;


    int xxx = this->x()+movin.x()-pressed.x();
    if (xxx > 500)
        xxx = 500;
    if (xxx < 100)
        xxx = 100;
    this->move(xxx,this->y());

    e->accept();

}
