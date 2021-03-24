#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <dataset.h>
#include <predialog.h>
#include "chonggoushezhi.h"
#include <mysplitline.h>

#include <QStack>
#include <QTextEdit>
#include <QGraphicsView>
#include <QGraphicsItem>

#include <QTreeWidget>
#include <QTreeWidgetItem>



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:

    Ui::MainWindow *ui;

    int menubarHeight;

    struct DataPreset{
        int xCells, yCells;
        int direction;
        double errorLimit;
    }   dataPreset;

    struct CurrentBundle{
        DataSet* currentDataSet;
        QTreeWidgetItem* cDataSetItem;
        QTreeWidgetItem* cMapItem;
        QTreeWidgetItem* cFolder;
    }   current;
    QList<DataSet*> dataSetList;
    QList<QString> nameList;    //存的是所有dataSet的短名字
    QTextEdit *browser;
    QGraphicsView *view;
    QGraphicsScene *scene;    
    QPixmap *pixmap;
    MySplitLine *splitLine;

    QStack<QPoint*> dotstack;
    QList<QPoint*> region;

    QPen pen;
    int mode;
    enum LineWidth{
        W1=9,W2=10,W3,W4
    }   curWidth;
    enum Colors{
        white=100,black=101,red,yellow,green,blue
    }   curColor;    

    int reconType;
    QString reconOR;
    int iterations;

protected:
    void mouseReleaseEvent(QMouseEvent *E);

public:

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *ev);

    void changeFigure();
    QList<QPoint*> selectedArea(QList<QPoint*> list);
    QString nameFolder();
    QString deSuffix(QString name);
    QString avoidSameName(QString name);




private slots:

    void daoru();
    void drawEular();
    void drawIPFx();
    void drawIPFy();
    void drawIPFz();
    bool setPreset(bool changeName);
    bool setPreset(){return setPreset(0);}
    void manualResize();
    void showGraph(QString fileName);
    void showDataAbstract(DataSet* set);
    void showFolderAbstract();
    void addFolderNDataset(DataSet& dataSet);
    void changeCurrent(QTreeWidgetItem& item);
    void RECONSTRUCT(QString OR="KS", int type=0, int iteration=1);


        //----------------ui界面对应的槽函数------------------------

    void on_pushButton_clicked(){
        mode= (mode+1) % 2;
        changeFigure();
    }

    void on_pushButton_2_clicked(){
        if (curColor==white) curColor=black;
        else{
            qDebug()<<"换颜色";
            if (curColor==blue) curColor=white;
            if (curColor==green) curColor=blue;
            if (curColor==yellow) curColor=green;
            if (curColor==red) curColor=yellow;
            if (curColor==black) curColor=red;
        }
        changeFigure();
    }

    void on_pushButton_3_clicked(){
        if (curWidth==W4)   curWidth=W1;
        else{
            if (curWidth==W3)   curWidth=W4;
            if (curWidth==W2)   curWidth=W3;
            if (curWidth==W1)   curWidth=W2;
        }
        changeFigure();
    }

    void on_pushButton_4_clicked(){    on_actionwanchengkuangxuan_triggered();}

    void on_actiondaoru_triggered(){   daoru();}

    void on_actiondaochu_triggered(){
        if(current.currentDataSet==nullptr)
            return;
        current.currentDataSet->WriteData(22);

    }

    void on_actionwenbenwenjian_triggered(){
        if(current.currentDataSet==nullptr)
            return;

        current.currentDataSet->WriteData(22);
    }

    void on_actioncsv_triggered(){}

    void on_actionweitu_triggered(){}

    void on_actionhuizhiEular_triggered(){  drawEular();}

    void on_actionIPF_x_triggered(){    drawIPFx();}

    void on_actionIPF_y_triggered(){    drawIPFy();}

    void on_actionIPF_z_triggered(){    drawIPFz();}

    void on_actionhuizhiBM_triggered(){    qDebug()<<"BM图还没做";}

    void on_actionchongxinxuanzejinxiang_triggered(){ setPreset();}

    void on_actiontianjiatuli_triggered(){
        QPixmap tuli= QPixmap("IPFtuli.png");
        QGraphicsPixmapItem* sss= scene->addPixmap(tuli);
        sss->setPos(scene->width()-tuli.width(),scene->height()-tuli.height());
        sss->setAcceptDrops(true);
        scene->update();
    }

    void on_actiontianjiabilichi_triggered(){
        QPen tmppen;
        tmppen.setColor(Qt::red);
        tmppen.setWidth(5);
        scene->addLine(scene->width()-125,scene->height()-170,scene->width()-25,scene->height()-170,tmppen);
        QGraphicsTextItem *text= scene->addText("20um");
        text->setDefaultTextColor(Qt::red);
        text->setPos(scene->width()-90,scene->height()-195);
        scene->update();
    }

    void on_actionzhanshitingkaowu_triggered();

    void on_actionwanchengkuangxuan_triggered()
    {
        //把dotstack清空，存进一个dotlist，然后给selectedArea(tmplist)处理
        //真是脑子坏了为什么一开始不用list呢

        if (dotstack.isEmpty())  return;

        QList<QPoint*> tmplist;
        QPoint *start =dotstack.pop();
        tmplist.append(start);
        QPoint *end=nullptr;

        while(!dotstack.isEmpty())
        {
            end= dotstack.pop();
            tmplist.append(end);
        }
        if(end)
        {
            scene->addLine(start->x(),start->y(),end->x(),end->y(),pen);
            scene->update();
            selectedArea(tmplist);
        }
        dotstack.clear();
    }

    void on_treeWidget_itemClicked(QTreeWidgetItem *item);

    void on_actionchonggoushezhi_triggered(){
        chonggoushezhi *shezhi =new chonggoushezhi(this);
        int returnValue= shezhi->exec();    //模态显示
        if (returnValue==QDialog::Accepted)
        {
            reconOR=shezhi->getOR();            //读出和处理旋转矩阵设置
            reconType=shezhi->getType();
            iterations=shezhi->getIteration();
            qDebug()<<reconOR<<reconType<<iterations;
        }
    }

    void on_actionchonggou_triggered(){
        if(reconOR=="NULL" || reconType<0 || iterations<0)
            on_actionchonggoushezhi_triggered();
        if(current.currentDataSet)
            RECONSTRUCT(reconOR,reconType,iterations);}

};
#endif // MAINWINDOW_H
