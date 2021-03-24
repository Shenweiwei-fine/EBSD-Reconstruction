#include "datasetpredialog.h"
#include "ui_datasetpredialog.h"

dataSetPreDialog::dataSetPreDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dataSetPreDialog)
{
    ui->setupUi(this);



}

int dataSetPreDialog::getxCells()
{
    QString str= ui->lineEditX->text();
    return str.toInt();
}

int dataSetPreDialog::getyCells()
{
    QString str= ui->lineEditY->text();
    return str.toInt();
}

int dataSetPreDialog::getDirection()
{
    return 1;
}

void dataSetPreDialog::checkHaveDone()
{
    QString y= ui->lineEditY->text();
    QString x= ui->lineEditX->text();
    int xx=x.toInt();
    int yy=y.toInt();

    if (xx>0 && yy>0)
        ui->label_4->setText("done");
}

dataSetPreDialog::~dataSetPreDialog()
{
    delete ui;
}
