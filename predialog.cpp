#include "predialog.h"
#include "ui_predialog.h"

PreDialog::PreDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreDialog)
{
    ui->setupUi(this);



    ui->label_4->setPixmap(QPixmap("jinxiang.jpg").scaledToWidth(100));




}


int PreDialog::getxCells()
{
    QString str= ui->lineEditX->text();
    return str.toInt();
}

int PreDialog::getyCells()
{
    QString str= ui->lineEditY->text();
    return str.toInt();
}

QString PreDialog::getFileName()
{
    return ui->lineEditFileName->text();
}

int PreDialog::getDirection()
{
    qDebug()<<"进向";
    return 1;
}

void PreDialog::on_buttonOpen_clicked(){

    QString selectedFilter;
    QString fileName = QFileDialog::getOpenFileName(this,
                                tr("QFileDialog::getOpenFileName()"),
                                "C:/Qt/saveMy/build-RECONSTRUCTION-Desktop_Qt_5_14_1_MinGW_32_bit-Debug",
                                tr("All Files (*);;Text Files (*.txt)"),
                                &selectedFilter,
                                QFileDialog::ReadOnly);
    if (!fileName.isEmpty())
    {
        ui->lineEditFileName->setText(fileName);
        ui->labelOpenFile->setText(fileName);
    }
}

void PreDialog::checkHaveDone()
{
    QString y= ui->lineEditY->text();
    QString x= ui->lineEditX->text();
}

PreDialog::~PreDialog()
{
    delete ui;
}
