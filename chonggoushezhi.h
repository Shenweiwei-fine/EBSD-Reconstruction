#ifndef CHONGGOUSHEZHI_H
#define CHONGGOUSHEZHI_H

#include <QDialog>

namespace Ui {
class chonggoushezhi;
}

class chonggoushezhi : public QDialog
{
    Q_OBJECT

public:
    explicit chonggoushezhi(QWidget *parent = nullptr);
    ~chonggoushezhi();

    QString getOR();
    int getType();
    int getIteration();

private:
    Ui::chonggoushezhi *ui;
};

#endif // CHONGGOUSHEZHI_H
