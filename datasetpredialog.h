#ifndef DATASETPREDIALOG_H
#define DATASETPREDIALOG_H


#include <QDialog>

namespace Ui {
class dataSetPreDialog;
}

class dataSetPreDialog : public QDialog
{
    Q_OBJECT

public:

    explicit dataSetPreDialog(QWidget *parent = nullptr);
    ~dataSetPreDialog();
    int getxCells();
    int getyCells();
    int getDirection();
    void checkHaveDone();


private:



    Ui::dataSetPreDialog *ui;


};

#endif // DATASETPREDIALOG_H
