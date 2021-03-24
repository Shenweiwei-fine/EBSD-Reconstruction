#include "chonggoushezhi.h"
#include "ui_chonggoushezhi.h"

chonggoushezhi::chonggoushezhi(QWidget *parent):
    QDialog(parent),
    ui(new Ui::chonggoushezhi)
{
    ui->setupUi(this);

    ui->comboBox->addItem("全局");
    ui->comboBox->addItem("单晶粒");
    ui->spinBox->setValue(1);
    ui->spinBox->setRange(1,5);
    ui->comboBox_2->addItem("KS");
    ui->comboBox_2->addItem("NW");
    ui->comboBox_2->addItem("GT");
}

QString chonggoushezhi::getOR(){return ui->comboBox_2->currentText();}
int chonggoushezhi::getType(){return ui->comboBox->currentIndex();}
int chonggoushezhi::getIteration(){return ui->spinBox->value();}

chonggoushezhi::~chonggoushezhi()
{
    delete ui;
}
