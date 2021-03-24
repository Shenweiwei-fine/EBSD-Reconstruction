#ifndef PREDIALOG_H
#define PREDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QGraphicsScene>
#include <QDebug>

namespace Ui {
class PreDialog;
}

class PreDialog : public QDialog
{
    Q_OBJECT

public:

    explicit PreDialog(QWidget *parent = nullptr);
    ~PreDialog();

    int getxCells();
    int getyCells();
    int getDirection();
    QString getFileName();
    void checkHaveDone();

private:

    Ui::PreDialog *ui;

private slots:

    void on_buttonOpen_clicked();
};

#endif // PREDIALOG_H
