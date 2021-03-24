#ifndef DATASET_H
#define DATASET_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QtMath>
#include <QDataStream>
#include <QMessageBox>
#include <mat3.h>
#include <matrix.h>
#include <qqueue.h>
#include <QTime>


//--------------------定义---------------------







class DataSet : public QObject
{
    Q_OBJECT

public:

    QString longFileName;
    QString shortFileName;
    bool haveEular ,haveIPFx, haveIPFy, haveIPFz ;
    bool isASelection, hasASelection;
    int xCells, yCells; //像素点数
    int direction;  //进向
    float xStep, yStep;   //步长
    int totalColumns;     //总列数
    int eular1Column;

    QString string[100];        //储存各个列的标题
    QList<float> array[100];    //储存每一列的数据
    QList<Mat3<double>> orientation;    //储存和array对应的像素点取向阵

private:

    struct Node{
        int j;
        double prob;
        int area;

        Node(int jj, double pp, int aa){j=jj;prob=pp;area=aa;}
    };

    quint8 rr,gg,bb;
    float e1,e2,e3;
    float xx,yy,zz;

    QList<QList<int>>    pixVarient;    //pixel point based varient indexation
    QList<QList<int>>    ausPixVarient;    //austenite pixel indexation
    QList<QList<int>>    connect;
    QList<Mat3<double>>    revAvrgOPA;
    QList<double>          revWeight;

    QQueue<Node>    nodeQueue;

    QList<QList<Mat3<double>>> G;
    QList<Mat3<double>> gm,ga;
    QList<QList<int>> N;
    QList<QList<QList<Node>>>   W;


public:

    explicit DataSet(QObject *parent = nullptr);
    DataSet(const DataSet& a);
    DataSet& operator=(const DataSet& a);
    bool setPresets(int xx, int yy, int direc);

    QString readData(QString fileName="\0");
    void reserveSelectedOnly(QRect rect);
    bool WriteData(int type);
    QString givePreview(int n);
    QString drawEular(int selected = 0, QString fileName="myEularMap.bmp");
    QString drawIPF(int xyz, int selected = 0 ,QString fileName="myIPFMap.bmp");

    float& px(int xsc, int ysc, int col){    //这个xsc和ysc是在屏幕上显示的xsc,ysc，为了画图用的，主要是因为bmp格式的存储顺序反了一下
        return array[col][xsc+yCells*xCells-ysc*xCells-xCells];
    }
    int pxIndex(int x, int y, int indexed=0){  //这是根据dataset里的实际坐标得出array里的下标, 0 indexed 表示第一个点是0,0
        return x-indexed+(y-indexed)*xCells;
    }

    QString deSuffix(QString name){
        QString str = nullptr;
        int n = name.length();
        int ii;

        for (  ii = n-1 ; ii>=0 ; ii--)
        {
            if (name[ii]=='.')  break;
        }
        for (int jj =0 ; jj<ii ; jj++ )
        {
            str.append(name[jj]);
        }

        return str;
    }
    void setLongName(QString longName){
        QFileInfo info(longName);
        if (info.exists()){
            longFileName = info.absoluteFilePath();
            shortFileName= deSuffix(info.fileName());
        }
    }
    void setShortName(QString shortName){   setLongName(shortName);}
    QString Name(int type, int selected=0);



    /*
    * 第一部分，确定每个Varient包含哪些像素点
    */

    void generateOrientation(); //计算每个像素的取向阵
    void makePixVarient_Connection();   //求相邻的像素点们

    /*
     * 第二部分，相邻varient投票，最后长大
     */

    void addLine2N(int i, int j){ /*qDebug()<<i<<j;*/N[j][i]=N[i][j]=1;}
    QList<Mat3<double>> loadORs(QString fileName,  int length);
    void initMAtrices(QString OR);          //N在makeVarientCoNnectionGraph就已经建好了
    void generateWeightConnection();        //相邻grain给中间grain往24个OPA上投票
    void findOPA();                         //每个grain只保留一个OPA
    void growth(int times=1);               //长大，可以迭代几次

    QList<double> orient2Eular(const Mat3<double> orient);

    /*
     * 第三部分，母奥空间
     */

    void calcuRevAvrgOPA();              //计算母奥平均取向的逆
    void findEuerOPA(double meshSize=10);  //10->2->0.5


public slots:

    QRect setSelectedAreaData(QList<QPoint*>& region);
    DataSet RECONSTRUCT(QString OR, int iteration);     //未事先框选
    DataSet RECON(QString OR, int iteration);       //事先框选


};

#endif // DATASET_H
