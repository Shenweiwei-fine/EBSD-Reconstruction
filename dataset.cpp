#include "dataset.h"


//----------------------------------------

quint32 toReversedHex(int y)
{
    quint32 aa=y%0x100;
    quint32 bb=y%0x10000-aa;
    quint32 cc=y%0x1000000-bb-aa;
    quint32 dd=static_cast<int>(y)-cc-bb-aa;

    return (dd/0x1000000+cc/0x100+bb*0x100+aa*0x1000000);
}


//------------------拷贝构造、重载赋值-----------------


DataSet::DataSet(const DataSet& a)
{

    longFileName=a.longFileName;
    shortFileName=a.shortFileName;
    haveIPFx=haveIPFy=haveIPFz=haveEular=0;
    hasASelection=a.hasASelection;
    isASelection=a.isASelection;

    xCells=a.xCells;
    yCells=a.yCells;
    qDebug()<<"copy constructed a DataSet: "<<xCells<<"*"<<yCells;
    xStep=a.xStep;
    yStep=a.yStep;
    direction=a.direction;

    totalColumns=a.totalColumns;
    eular1Column=a.eular1Column;

    for (int ii=1 ; ii<= totalColumns ; ii++)
    {
        string[ii]=a.string[ii];
        array[ii]=a.array[ii];    }

}

//重载赋值只赋presets和data, 不赋状态
DataSet& DataSet::operator=(const DataSet& a)
{
    xCells=a.xCells;
    yCells=a.yCells;
    xStep=a.xStep;
    yStep=a.yStep;
    direction=a.direction;

    totalColumns=a.totalColumns;
    eular1Column=a.eular1Column;

    for (int ii=1 ; ii<= totalColumns ; ii++)
    {
        string[ii]=a.string[ii];
        array[ii].clear();
        array[ii].append(a.array[ii]);    }

    qDebug()<<totalColumns<<"****"<<array[1].length()<<"..."<<a.array[totalColumns].length();

    return *this;
}



//---------------构造函数和设置参数-------------------

DataSet::DataSet(QObject *parent) : QObject(parent)
{
    haveIPFx=haveIPFy=haveIPFz=haveEular=0;
    totalColumns=eular1Column=0;
    isASelection=hasASelection= 0;


}

bool DataSet::setPresets(int xxx, int yyy, int direc)
{
    xCells=xxx;
    yCells=yyy;
    direction = direc;

    qDebug()<<"set preset "<<xCells<<"*"<<yCells;
    return 1;
}



//-------------------读数据---------------------------

QString DataSet::readData(QString fileName)
{
    char ch;        //用来读制表字符
    float fl;       //用来转换字符串为数字
    eular1Column=0;   //表示欧拉角1在第几列
    totalColumns=0;

    QFile  ctfFile(fileName);
    if(!ctfFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"打开数据文件失败";
        return  "wrong code -1";
    }
    QTextStream test2(&ctfFile);

    QFileInfo info(ctfFile);
    longFileName = info.absoluteFilePath();
    shortFileName= deSuffix(info.fileName());

    //读每一列标题

    for (int ii = 1 ; ii<=100 ; ii++)
    {
        test2>>string[ii];      //列编号从1到totalcolumns.
        totalColumns++;

        if(string[ii].compare("Euler1")==0)
            eular1Column=ii;

        if(string[ii].compare("BS")==0)
            ii=1000;   //此时跳出循环
    }

    qDebug()<<"总列数："<<totalColumns;

    //把数据按列写入文件和数组

    QString newString[100];     //读文件的经手字符串们

    for (int j =1 ; j <= xCells*yCells ; j++ )
    {
        for (int ii = 1 ; ii<=totalColumns ; ii++)
        {
            test2>>newString[ii];
            test2>>ch;

            if(ch == '\r')
            {
                qDebug()<<"ohhh! whoo!";    //为什么>>读不到‘\r’
                test2>>ch;
                if (ch == '\n')  ii=totalColumns+26;   //跳出循环
            }
        }

        ///     这一段是判断ERROR与否的程序！！要想精度更高的话还可以判断一下MAD列。
        ///        if (newString[5].compare("0")!=0)
        ///            newString[eular1Column] = newString[eular1Column+1] = newString[eular1Column+2] = "0";

        for (int ii = 1 ; ii<=totalColumns ; ii++)
        {
            fl = newString[ii].toFloat();
            array[ii].append(fl);       //注意每个列是array[ii][0到总cells-1]
        }

        if (j%(xCells*yCells/10)==1)
            qDebug()<<j/(xCells*yCells/100)/10;   //进度报告

    }

    //标记属性，关闭文件

    isASelection=hasASelection=0;
    ctfFile.close();
    return shortFileName;
}


//------------------------画图--------------------------------

QString DataSet::drawEular(int selected , QString fileName)
{
    //创建文件并打开

    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly))
    {
        qDebug()<<"error when creating file";
        return "error when creating file";
    }
    QDataStream out(&file);

    //位图文件头

    quint8 B=0x42;
    quint8 M=0x4d;
    quint32 bfSize=toReversedHex(54+4*xCells*yCells);
    quint16 bfReserved1=0;
    quint16 bfReserved2=0;
    quint32 bfOffBits=0x36000000;

    quint32 biSize=0x28000000;
    quint32 biWidth=toReversedHex(xCells);
    quint32 biHeight=toReversedHex(yCells);
    quint16 biPlanes=0x0100;
    quint16 biBitCount=0x2000;
    quint32 biCompression=0;
    quint32 biSizeImage=toReversedHex(4*xCells*yCells);
    quint32 biXPelsPerMeter=0;
    quint32 biYPelsPerMeter=0;
    quint32 biClrUsed=0;
    quint32 biClrImportant=0;

    out<<B<<M<<bfSize<<bfReserved1<<bfReserved2<<bfOffBits
      <<biSize<<biWidth<<biHeight<<biPlanes<<biBitCount<<biCompression
     <<biSizeImage<<biXPelsPerMeter<<biYPelsPerMeter<<biClrUsed<<biClrImportant;

    //循环写入图片

//    int count=0;
    float point1=0.1;
    unsigned int pixel;


    for (int yy=0; yy<yCells ; yy++)
    {
        for (int xx=0; xx<xCells; xx++)
        {
            e1=px(xx,yy,eular1Column);
            e2=px(xx,yy,eular1Column+1);
            e3=px(xx,yy,eular1Column+2);
            rr=static_cast<quint8>(255-e1*255/360);
            gg=static_cast<quint8>(e2*255/90);
            bb=static_cast<quint8>(255-e3*255/90);
            pixel=bb*0x1000000+gg*0x10000+rr*0x100;
            if(e1+e2+e3<point1)
                pixel=0xffffff00;

            out<<pixel;
        }
        if(!((yy)%155)) qDebug()<<yy;
    }

    QFileInfo info(file);
    QString stttrr=info.fileName();
    file.close();
    haveEular=1;
    return stttrr;
}

QString DataSet::drawIPF(int xyz, int selected, QString fileName)
{
    //创建文件并打开

    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly))
    {
        qDebug()<<"error when creating file";
        return "error when creating file";
    }
    QDataStream out(&file);

    //位图文件头

    quint8 B=0x42;
    quint8 M=0x4d;
    quint32 bfSize=toReversedHex(54+4*xCells*yCells);
    quint16 bfReserved1=0;
    quint16 bfReserved2=0;
    quint32 bfOffBits=0x36000000;

    quint32 biSize=0x28000000;
    quint32 biWidth=toReversedHex(xCells);
    quint32 biHeight=toReversedHex(yCells);
    quint16 biPlanes=0x0100;
    quint16 biBitCount=0x2000;
    quint32 biCompression=0;
    quint32 biSizeImage=toReversedHex(4*xCells*yCells);
    quint32 biXPelsPerMeter=0;
    quint32 biYPelsPerMeter=0;
    quint32 biClrUsed=0;
    quint32 biClrImportant=0;

    out<<B<<M<<bfSize<<bfReserved1<<bfReserved2<<bfOffBits
      <<biSize<<biWidth<<biHeight<<biPlanes<<biBitCount<<biCompression
     <<biSizeImage<<biXPelsPerMeter<<biYPelsPerMeter<<biClrUsed<<biClrImportant;

    //循环写入图片

    float pie=3.14159265358979323846;
    float c1,c2,c3,s1,s2,s3;
    int count=0;
    int castCount;
    unsigned int pixel;
    float tmpx,tmpy,tmpz;

    QList<Mat3<double>> J=loadORs("Rotation_24.txt",24);

    if (xyz==1)
    {
        for (int yyyy=0; yyyy<yCells ; yyyy++)
        {
            if(!((yyyy)%85)) qDebug()<<yyyy;
            for (int xxxx=0; xxxx<xCells; xxxx++)
            {
                castCount=static_cast<int>(count);
                count++;
                e1=px(xxxx,yyyy,eular1Column);
                e2=px(xxxx,yyyy,eular1Column+1);
                e3=px(xxxx,yyyy,eular1Column+2);
                c1=qCos(e1/180*pie);
                c2=qCos(e2/180*pie);
                c3=qCos(e3/180*pie);
                s1=qSin(e1/180*pie);
                s2=qSin(e2/180*pie);
                s3=qSin(e3/180*pie);

                xx=c1*c3-c2*s1*s3;
                yy=s1*c3+c2*c1*s3;
                zz=s2*s3;
                if (xx<0)   xx= xx*(-1);
                if (yy<0)   yy= yy*(-1);
                if (zz<0)   zz= zz*(-1);
                if (xx < yy)
                {
                    float exchange= xx;
                    xx= yy;
                    yy= exchange;
                }
                if (xx > zz)
                {
                    float exchange= xx;
                    xx= zz;
                    zz= exchange;
                }
                if (xx < yy)
                {
                    float exchange= xx;
                    xx= yy;
                    yy= exchange;
                }

//  这是从等价晶向出发的计算IPF坐标的方法，但是有一点问题，有些点没能正确归类
//                for(int jj=0;jj<24;jj++)
//                {
//                    tmpx=J[jj][11]*xx+J[jj][12]*yy+J[jj][13]*zz;
//                    tmpy=J[jj][21]*xx+J[jj][22]*yy+J[jj][23]*zz;
//                    tmpz=J[jj][31]*xx+J[jj][32]*yy+J[jj][33]*zz;
//                    if(tmpx>=(tmpy+0.000001) && tmpz>=(tmpx+0.000001) && tmpy>=0.000001)
//                        break;
//                }
//                xx=tmpx;yy=tmpy;zz=tmpz;

                xx= xx/zz;
                yy= yy/zz;
                bb= static_cast<quint8>((qSin(1.57*yy)*0xFF));
                gg= static_cast<quint8>(qSin(1.57*(xx-yy))*0xFF);
                rr= static_cast<quint8>(qSin(1.57*(1-xx))*0xFF);
                pixel=bb*0x1000000+gg*0x10000+rr*0x100;
                if(e1+e2+e3<0.01)
                    pixel=0xffffff00;
                out<<pixel;
            }
        }
        haveIPFx=1;
    }
    if (xyz==2)
    {
        for (int yyyy=0; yyyy<yCells ; yyyy++)
        {
            if(!((yyyy)%85)) qDebug()<<yyyy;
            for (int xxxx=0; xxxx<xCells; xxxx++)
            {
                castCount=static_cast<int>(count);
                count++;
                e1=px(xxxx,yyyy,eular1Column);
                e2=px(xxxx,yyyy,eular1Column+1);
                e3=px(xxxx,yyyy,eular1Column+2);
                c1=qCos(e1/180*pie);
                c2=qCos(e2/180*pie);
                c3=qCos(e3/180*pie);
                s1=qSin(e1/180*pie);
                s2=qSin(e2/180*pie);
                s3=qSin(e3/180*pie);

                xx=-c1*s3-c2*s1*c3;
                yy=-s1*s3+c2*c1*c3;
                zz=s2*c3;

                if (xx<0)   xx= xx*(-1);
                if (yy<0)   yy= yy*(-1);
                if (zz<0)   zz= zz*(-1);
                if (xx < yy)
                {
                    float exchange= xx;
                    xx= yy;
                    yy= exchange;
                }
                if (xx > zz)
                {
                    float exchange= xx;
                    xx= zz;
                    zz= exchange;
                }
                if (xx < yy)
                {
                    float exchange= xx;
                    xx= yy;
                    yy= exchange;
                }

                xx= xx/zz;
                yy= yy/zz;
                bb= static_cast<quint8>((qSin(1.57*yy)*0xFF));
                gg= static_cast<quint8>(qSin(1.57*(xx-yy))*0xFF);
                rr= static_cast<quint8>(qSin(1.57*(1-xx))*0xFF);
                pixel=bb*0x1000000+gg*0x10000+rr*0x100;
                if(e1+e2+e3<0.01)
                    pixel=0xffffff00;
                out<<pixel;
            }
        }
        haveIPFy=1;
    }
    if (xyz==3)
    {
        for (int yyyy=0; yyyy<yCells ; yyyy++)
        {
            if(!((yyyy)%85)) qDebug()<<yyyy;
            for (int xxxx=0; xxxx<xCells; xxxx++)
            {
                castCount=static_cast<int>(count);
                count++;
                e1=px(xxxx,yyyy,eular1Column);
                e2=px(xxxx,yyyy,eular1Column+1);
                e3=px(xxxx,yyyy,eular1Column+2);
                c1=qCos(e1/180*pie);
                c2=qCos(e2/180*pie);
                c3=qCos(e3/180*pie);
                s1=qSin(e1/180*pie);
                s2=qSin(e2/180*pie);
                s3=qSin(e3/180*pie);

                xx=s1*s2;
                yy=-s2*c1;
                zz=c2;

                if (xx<0)   xx= xx*(-1);
                if (yy<0)   yy= yy*(-1);
                if (zz<0)   zz= zz*(-1);
                if (xx < yy)
                {
                    float exchange= xx;
                    xx= yy;
                    yy= exchange;
                }
                if (xx > zz)
                {
                    float exchange= xx;
                    xx= zz;
                    zz= exchange;
                }
                if (xx < yy)
                {
                    float exchange= xx;
                    xx= yy;
                    yy= exchange;
                }

                xx= xx/zz;
                yy= yy/zz;
                bb= static_cast<quint8>((qSin(1.57*yy)*0xFF));
                gg= static_cast<quint8>(qSin(1.57*(xx-yy))*0xFF);
                rr= static_cast<quint8>(qSin(1.57*(1-xx))*0xFF);
                pixel=bb*0x1000000+gg*0x10000+rr*0x100;
                if(e1+e2+e3<0.01)
                    pixel=0xffffff00;
                out<<pixel;
            }
        }
        haveIPFz=1;
    }


    QFileInfo info(file);
    QString stttrr=info.fileName();
    file.close();
    return  stttrr;
}


//----------------------选取后加一列记下来-----------------------

QRect DataSet::setSelectedAreaData(QList<QPoint*>& region)
{
    //是可以重复select的

    qDebug()<<"selection started";

    int n=region.length();
    int xx, yy;
    bool outOfRegion=1;     // 1 == out of region.
    QList<QList<int>> margin;

    QList<int> tmp;
    tmp.append(-1);
    tmp.append(40000);

    for (int i=0; i< yCells+5; i++)
    {
        margin.append(tmp);
    }
    for(int ii=0; ii<n; ii++)
    {
        for (int jj=1; jj < margin[region[ii]->y()].length(); jj++)
        {
            if (region[ii]->x() < margin[region[ii]->y()][jj]
                    &&  region[ii]->x() >= margin[region[ii]->y()][jj-1])
            {
                margin[region[ii]->y()].insert(jj,region[ii]->x());
                break;
            }
        }
    }

    int minx=xCells;
    int miny=yCells;
    int maxx=0;
    int maxy=0;

    for(int ii=0; ii<n ; ii++)
    {
        if (region[ii]->y() < miny)
            miny=region[ii]->y();
        if (region[ii]->x() < minx)
            minx=region[ii]->x();
        if (region[ii]->y() > maxy)
            maxy=region[ii]->y();
        if (region[ii]->x() > maxx)
            maxx=region[ii]->x();
    }
    qDebug()<<minx<<maxx<<miny<<maxy;

    for(yy=1; yy<= yCells; yy++)
    {
        for (xx=1; xx<= xCells; xx++)
        {
            if ( xx<minx || xx>maxx || yy<miny || yy>maxy)
            {
                array[totalColumns+1].append(2);     // 2表示在方框外
            }
            else
            {
                for (int jj=1; jj<margin[yy].length(); jj++)
                {
                    if (xx < margin[yy][jj]  &&
                            xx >= margin[yy][jj-1])
                    {
                        if (jj > 1 && (jj+1)%2 )
                            array[totalColumns+1].append(0);
                        else array[totalColumns+1].append(1);   // 1表示在方框内但还是在选区外
                        jj=margin[yy].length();     // 就是break
                    }
                }
            }


        }
        if (!(yy%150)) qDebug()<<yy/150;
    }

    string[totalColumns+1]="OUT";
    if (hasASelection)
        string[totalColumns+1]+="_dupl";
    totalColumns++;
    hasASelection=1;

    qDebug()<<"selection finished";
    return QRect(minx,miny,maxx-minx+1,maxy-miny+1);
}


//---------------------写出DataSet---------------------

bool DataSet::WriteData(int type){

    //防重名部分
    QString ends[20]={"1","2","3","4","5","6","7","8","9","10",
                      "11","12","13","14","15","16","17","18","19","20"
                     };
    int ii=0;
    QString tmpstr=shortFileName;
    while (QFile::exists(tmpstr))
    {
        tmpstr = shortFileName.append("(").append(ends[ii++]).append(")");
    }
    setShortName(tmpstr);

    //正式部分
    QFile  myFile(shortFileName);
    if(!myFile.open(QIODevice::WriteOnly ))
    {
        qDebug()<<"wrong code -2";
        return  false;
    }
    QTextStream out(&myFile);

    int newCount=0;
    float point1=0.1;

    if (type%10==0)    //只写欧拉角那三列(简易版)
    {

        out<<string[eular1Column]<<'\t'
          <<string[eular1Column+1]<<'\t'
         <<string[eular1Column+2]<<'\r'<<'\n';

        for (int ii = 1 ; ii<= xCells * yCells ; ii++)
        {
            if (type>9 && isASelection && array[totalColumns][newCount]>point1){
                //do nothing
            }
            else{
                out<<array[eular1Column][newCount]<<'\t'
                  <<array[eular1Column+1][newCount]<<'\t'
                 <<array[eular1Column+2][newCount]<<'\r'<<'\n';
            }
            newCount++;
        }

    }
    else
    {
        totalColumns -= type%10 -1;     //是否写最后一列

        out<<string[1];
        for (int ii=2; ii<=totalColumns; ii++)
            out<<'\t'<<string[ii];
        out<<'\r'<<'\n';

        for (int ii = 1 ; ii<= xCells * yCells ; ii++)
        {
            if (type>9 && isASelection && array[totalColumns+type%10-1][newCount]>1.5){
                //do nothing
            }
            else{
                out<<array[1][newCount];
                for (int ii=2; ii<=totalColumns; ii++)
                    out<<'\t'<<array[ii][newCount];
                out<<'\r'<<'\n';
            }
            newCount++;
        }

        totalColumns += type%10 -1;
    }

    myFile.close();
    qDebug()<<"finished writing selected area";
    return true;

}


//---------------------只保留选区，其他为0------------------------

void DataSet::reserveSelectedOnly(QRect rect)
{
    QList<float> tmpArray[100];    //储存每一列的数据
    double tmppp;

    for (int ii = 0 ; ii< xCells*yCells ; ii++)
    {
        tmppp=static_cast<double>(array[totalColumns][ii]);
        if ( 1.5 > tmppp)
            for(int jj=1; jj<=totalColumns; jj++)
                tmpArray[jj].append(array[jj][ii]);
    }

    for(int jj=1; jj<=totalColumns; jj++)
    {
        array[jj]=tmpArray[jj];
    }

    xCells=rect.width();
    yCells=rect.height();

    qDebug()<<"new cells:"<<xCells<<yCells;

    for (int ii=0; ii<xCells*yCells; ii++){
        if (array[totalColumns][ii]>0.2)
        {
            array[eular1Column][ii]=0;
            array[eular1Column+1][ii]=0;
            array[eular1Column+2][ii]=0;
        }
    }
}


//--------------------------外面预览调用---------------------

QString DataSet::givePreview(int n){

    QString str = "";
    QString tmp;

    if (n<0)
    {
        for (int ii=1; ii<=totalColumns; ii++)
        {
            str.append(string[ii]);
            str.append("\t");
        }
        str.append("\r\n");
        return str;
    }

    for (int ii=1; ii<=totalColumns; ii++)
    {
        str.append(QString::number(array[ii][n],'f',4));
        str.append("\t");
    }
    str.append("\r\n");
    return str;


}


//-------------------------找到晶粒，划分变体-----------------------

void DataSet::generateOrientation()
{
    Mat3<double> m;
    for (int ii=0; ii< array[1].length(); ii++)
    {
        m= Mat3<double>(array[eular1Column][ii],array[eular1Column+1][ii],array[eular1Column+2][ii]);
        orientation.append(m);
    }
}

void DataSet::makePixVarient_Connection()
{

    int numOfVarients=0;
    QList<int>  tmpList;

    connect.clear();        //connect[x][y]代表像素点x和y在同一个编号为它的变体; 默认为零.
    for( int y = 0; y<yCells ; y++)
        tmpList.append(0);
    for( int x = 0; x<xCells ; x++)
        connect.append(tmpList);

    Mat3<double>    m1,m2;
    int x,y;
    Node tmpNode(0,0,0);

    for( int xx =0 ; xx < xCells ; xx++)
    {
        for( int yy = 0; yy < yCells ; yy++)
        {
            e1=array[eular1Column][pxIndex(xx,yy)];
            e2=array[eular1Column+1][pxIndex(xx,yy)];
            e3=array[eular1Column+2][pxIndex(xx,yy)];
            if(e1+e2+e3<0.01)               //判断是不是白点
                continue;

            if(connect[xx][yy])
                continue;

            ++numOfVarients;
            connect[xx][yy]=numOfVarients;
            nodeQueue.clear();
            nodeQueue.append(Node(xx,0,yy));
            while(!nodeQueue.isEmpty())     //这是一个递归，每次从中心点出发走到周围的点，找相邻
            {
                tmpNode=nodeQueue.dequeue();
                x=tmpNode.j;
                y=tmpNode.area;
                m2 = orientation[pxIndex(x,y)];

                if(x<xCells-1){     //右
                    m1 = m2 * orientation[pxIndex(x+1,y)].transpose();
                    if(m1.trace()>2.98 && connect[x+1][y]==0)
                    {
                        nodeQueue.append(Node(x+1,0,y));
                        connect[x+1][y]=numOfVarients;
                    }
                }

                if(y<yCells-1){     //下
                    m1 = m2 * orientation[pxIndex(x,y+1)].transpose();
                    if(m1.trace()>2.98 && connect[x][y+1]==0)
                    {
                        nodeQueue.append(Node(x,0,y+1));
                        connect[x][y+1]=numOfVarients;
                    }
                }

                if(x>0){            //左
                    m1 = m2 * orientation[pxIndex(x-1,y)].transpose();
                    if(m1.trace()>2.98 && connect[x-1][y]==0)
                    {
                        nodeQueue.append(Node(x-1,0,y));
                        connect[x-1][y]=numOfVarients;
                    }
                }

                if(y>0){            //上
                    m1 = m2 * orientation[pxIndex(x,y-1)].transpose();
                    if(m1.trace()>2.98 && connect[x][y-1]==0)
                    {
                        nodeQueue.append(Node(x,0,y-1));
                        connect[x][y-1]=numOfVarients;
                    }
                }
            }
        }
    }

    int max=0;
    for( int x =0 ; x < xCells; x++)
        for( int y = 0; y<yCells ; y++)
            if(connect[x][y]>max)
                max=connect[x][y];

    pixVarient.clear();
    tmpList.clear();
    for(int n=max; n>0; n--)
        pixVarient.append(tmpList);

    for(int x=0; x < xCells ; x++)
        for(int y=0 ; y < yCells; y++)
            if(connect[x][y])
                pixVarient[connect[x][y]-1].append(pxIndex(x,y));   //在对应编号的变体中添加像素点的坐标

    QList<QList<int>> tmpVarient;
    QList<int>  reference, reverseRef;
    for(int ii=0; ii<pixVarient.length(); ii++)
    {
        reverseRef.append(-1);
        if(pixVarient[ii].length()>40)              //滤掉所有太小的晶粒, gm和pixVarient都不包含之。
        {
            tmpVarient.append(pixVarient[ii]);
            reference.append(ii);
            reverseRef[ii]=reference.length()-1;    //tmp[j]=pix[ref[j]], pix[i]=tmp[revref[i]]; ref单增，revref单减
        }
    }

    qDebug()<<reverseRef.length()<<reference.length();

    int center,right,below,left,upward;

    N.clear();
    tmpList.clear();
    for(int ii= tmpVarient.length(); ii>0; ii--)
       tmpList.append(0);
    for(int ii= tmpVarient.length(); ii>0; ii--)
       N.append(tmpList);

    for( int x = 1 ; x < xCells-1; x++)
    {
        for( int y = 1; y<yCells-1 ; y++)
        {
            center=connect[x][y];
            if( center==0 || reverseRef[center-1]==-1)
                continue;

            right=connect[x+1][y];
            if( right && reverseRef[right-1]==-1)       //是小变体，看成白点
                right=connect[x+1][y]=0;
            if( center!=right && center && right)       //不同变体、相邻、非白点，则添一条边
                addLine2N(reverseRef[center-1] ,reverseRef[right-1]);

            left=connect[x-1][y];
            if( left && reverseRef[left-1]==-1)
                left=connect[x-1][y]=0;
            if( center!=left && center && left)
                addLine2N(reverseRef[center-1] ,reverseRef[left-1]);

            below=connect[x][y+1];
            if( below && reverseRef[below-1]==-1)
                below=connect[x][y+1]=0;
            if( center!=below && center && below)
                addLine2N(reverseRef[center-1] ,reverseRef[below-1]);

            upward=connect[x][y-1];
            if( upward && reverseRef[upward-1]==-1)
                upward=connect[x][y-1]=0;
            if( center!=upward && center && upward)
                addLine2N(reverseRef[center-1] ,reverseRef[upward-1]);
        }
        if(!x%100)  qDebug()<<x;
    }

    pixVarient.clear();
    pixVarient = tmpVarient;
}


//--------------------------全局重构算法---------------------

QList<Mat3<double>> DataSet::loadORs(QString fileName, int length)
{
    QList<Mat3<double>> J;    // gamma_J_alpha
    double aij[9];
    QString str;

    QFile  file(fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"打开数据文件失败";
        return  J;
    }
    QTextStream fin(&file);

    for(int ii=0; ii<length; ii++)
    {
        for(int jj=0; jj<9 ;jj++)
        {
            fin>>str;
            aij[jj]=str.toDouble();
        }
        J.append(Mat3<double>(aij[0],aij[1],aij[2],aij[3],aij[4],aij[5],aij[6],aij[7],aij[8]));
    }

    return J;
}

void DataSet::initMAtrices(QString OR)
{
    //这个函数初始化了gm和G.

    Mat3<double> tmp;
    QList<Mat3<double>> tmpMat= QList<Mat3<double>>();
    QList<Mat3<double>> J;              //存储取向差矩阵
    int count;
    //Mat3<double> E= Mat3<double>(0);                  //这是单位阵
    Mat3<double> O= Mat3<double>(0,0,0,0,0,0,0,0,0);    //这是全0矩阵.

    double ux,uy,uz,theta,unitary,sumx,sumy,sumz;

    for( int ii=0 ; ii<pixVarient.length() ; ii++)
    {
        tmpMat.append(O);       //这是添一个O不是0
        theta=sumx=sumy=sumz=0;
        count=0;
        for (int jj=0; jj<pixVarient[ii].length(); jj++)
        {
            tmp=orientation[pixVarient[ii][jj]];
            theta+=qAcos((tmp.trace()-1)/2);
            ux=tmp[32]-tmp[23];
            uy=tmp[13]-tmp[31];
            uz=tmp[21]-tmp[12];
            unitary=sqrt(ux*ux +uy*uy +uz*uz);
            sumx+=ux/unitary;
            sumy+=uy/unitary;
            sumz+=uz/unitary;
            count++;
        }
        tmpMat[ii]=Mat3<double>(theta/count,sumx/count,sumy/count,sumz/count);     //平均晶粒取向
    }
    gm = tmpMat ;       //初始化gm.

    if(OR=="NW")
    {
        J=loadORs("NW_12_OR.txt",12);
        for(int m=0 ; m<12; m++)
        {
            G.append(gm);
            for(int ii=0; ii<pixVarient.length(); ii++)
                G[m][ii]=J[m]*gm[ii];
        }
    }
    if(OR=="GT")
    {
        J=loadORs("GT_24_OR.txt",24);
        for(int m=0 ; m<24; m++)
        {
            G.append(gm);
            for(int ii=0; ii<pixVarient.length(); ii++)
                G[m][ii]=J[m]*gm[ii];
        }
    }
    if(OR=="KS")
    {
        J=loadORs("KS_24_OR.txt",24);
        for(int m=0 ; m<24; m++)
        {
            G.append(gm);
            for(int ii=0; ii<pixVarient.length(); ii++)
                G[m][ii]=J[m]*gm[ii];
        }
    }                                                   //初始化G
}

void DataSet::generateWeightConnection()
{
    QList<Node> emptyList;
    QList<QList<Node>>  _24_emptyList;
    for(int c=0; c<24; c++)
        _24_emptyList.append(emptyList);
    for(int d=0; d<pixVarient.length(); d++)
        W.append(_24_emptyList);                    //初始化W

    int area_1,area_2;
    int m1,m2;
    Mat3<double> mat;
    double trace;

    for(int ii=0; ii<pixVarient.length(); ii++)
    {
        for(int jj=ii+1; jj<pixVarient.length(); jj++)
        {
            if(N[ii][jj])
            {
                area_1=pixVarient[ii].length();
                area_2=pixVarient[jj].length();
                for(m1=0 ; m1<G.length(); m1++)
                {
                    for(m2=0 ; m2<G.length() ; m2++)
                    {
                        mat=G[m1][ii]*G[m2][jj].transpose();
                        trace=mat.trace();
                        if(trace>2.9)
                        {
                            W[ii][m1].append(Node(jj,(trace-2.9),area_2));  //线性概率
                            W[jj][m2].append(Node(ii,(trace-2.9),area_1));
                        }
                    }
                }

                for(int kk=ii+1; kk<pixVarient.length(); kk++)
                {
                    if(N[jj][kk] && !N[ii][kk])
                    {
                        area_1=pixVarient[ii].length();
                        area_2=pixVarient[kk].length();
                        for(m1=0 ; m1<G.length(); m1++)
                        {
                            for(m2=0 ; m2<G.length() ; m2++)
                            {
                                mat=G[m1][ii]*G[m2][kk].transpose();
                                trace=mat.trace();
                                if(trace>2.9)
                                {
                                    W[ii][m1].append(Node(kk,(trace-2.9),area_2));  //线性概率
                                    W[kk][m2].append(Node(ii,(trace-2.9),area_1));
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void DataSet::findOPA()
{
    double currentMax;
    double totalArea;
    double tmp;
    int site=0;

    ga=gm;
    Mat3<double> O= Mat3<double>(0,0,0,0,0,0,0,0,0);    //这是全0矩阵.

    for(int ii=0; ii<pixVarient.length(); ii++)
    {
        currentMax=0;
        for(int m1=0 ; m1<G.length(); m1++)
        {
            totalArea=tmp=0;
            for(int jj=0; jj<W[ii][m1].length(); jj++)
            {
                tmp += W[ii][m1][jj].prob * W[ii][m1][jj].area;
                totalArea += W[ii][m1][jj].area;
            }
            tmp= tmp/totalArea;

            if(tmp>currentMax)
            {
                currentMax=tmp;
                site = m1;
            }
        }
        ga[ii] = G[site][ii];
        if(currentMax<0.0001)
            ga[ii]=O;
    }

    ausPixVarient=pixVarient;    // init ausPix

}

void DataSet::growth(int times)
{
    if(times<1) return;

    QList<QList<int>>   index;
    QList<int> tmpList;
    Mat3<double> tmpMat;
    int tt;
    bool flag;

    for(int ii=0 ; ii<ga.length() ; ii++)
    {
        flag = 0;
        for(int jj=ii+1; jj<ga.length() ; jj++)
        {
            if( N[ii][jj]==0 )    continue;

            tmpMat = ga[ii]*ga[jj].transpose();
            if( tmpMat.trace()<2.9 )    continue;           //大约5.5°，比较严格

            qDebug()<<times<<ii<<jj;

            for(tt=0; tt<index.length() ; tt++)
            {
                if( index[tt].indexOf(ii) > -1)         //找到index里含有ii的一行
                {
                    flag = 1;                       //表示ii在index里
                    index[tt].append(jj);
                    break;
                }
            }
            if(tt==index.length())                  //原先没有ii, 现在添加一个母奥晶粒。
            {
                flag=1;
                tmpList.clear();
                tmpList.append(ii);
                tmpList.append(jj);
                index.append(tmpList);
            }
        }

        if(flag == 0)           //没有成功加入新晶粒，原封不动。
        {
            tmpList.clear();
            tmpList.append(ii);
            index.append(tmpList);
        }
    }

    QList<Mat3<double>>  NEWga;
    QList<QList<int>>    newAusPix;
    double count=0;
    double ux,uy,uz,theta,unitary,sumx,sumy,sumz;

    tmpList.clear();
    for(int ii=0 ; ii<index.length() ; ii++)
    {
        theta=sumx=sumy=sumz=0;
        count=0;
        newAusPix.append(tmpList);
        tmpMat=Mat3<double>(0,0,0,0,0,0,0,0,0);
        for(int k=0; k< index[ii].length() ; k++)
        {
            newAusPix[ii].append(ausPixVarient[index[ii][k]]);

            count += ausPixVarient[index[ii][k]].length();

            tmpMat= ga[index[ii][k]];
            theta+= ausPixVarient[index[ii][k]].length() * qAcos((tmpMat.trace()-1)/2);
            ux=tmpMat[32]-tmpMat[23];
            uy=tmpMat[13]-tmpMat[31];
            uz=tmpMat[21]-tmpMat[12];
            unitary=sqrt(ux*ux +uy*uy +uz*uz);
            sumx += ausPixVarient[index[ii][k]].length() * ux/unitary;
            sumy += ausPixVarient[index[ii][k]].length() * uy/unitary;
            sumz += ausPixVarient[index[ii][k]].length() * uz/unitary;
        }
        tmpMat=Mat3<double>(theta/count,sumx/count,sumy/count,sumz/count);
        NEWga.append(tmpMat);
    }


    QList<QList<int>> tmpN;
    for(int ii= index.length(); ii>0; ii--)
       tmpList.append(0);
    for(int ii= index.length(); ii>0; ii--)
       tmpN.append(tmpList);


    for(int ii=0; ii<index.length(); ii++)
    {
        for(int jj=ii; jj<index.length();jj++)
        {
            for(int a=0; a<index[ii].length();a++)
            {
                for(int b=0; b<index[jj].length(); b++)
                {
                    if(N[index[ii][a]][index[jj][b]])
                        tmpN[ii][jj]=tmpN[jj][ii]=1;
                }
            }
        }
    }

    N=tmpN;
    ga=NEWga;
    ausPixVarient=newAusPix;

    growth(times-1);
}


//---------------选区重构使用的算法----------------------------

void DataSet::calcuRevAvrgOPA()
{
    int i,j,m1,m2;
    Mat3<double> tmp(0,0,0,0,0,0,0,0,0);
    revAvrgOPA.clear();
    QList<double>   list;

    qsrand(93);

    for( i=gm.length()-1; i>=0; i-=(qrand()%20+1))
    {
        if(pixVarient[i].length()<200 && rand()%10>5)
            continue;
        for( j=i-1; j>=0; j-=qrand()%20+1 )
        {
            if(pixVarient[j].length()<200 && rand()%10>5)
                continue;
            for( m1= G.length()-1; m1>=0 ; m1--)
            {
                for( m2= G.length()-1; m2>=0 ; m2--)
                {
                    tmp=G[m1][i]*(G[m2][j].transpose());
                    if(tmp.trace()>2.98)    //8°
                    {
                        revAvrgOPA.append(G[m1][i].transpose());
                        revAvrgOPA.append(G[m2][j].transpose());
                        revWeight.append((tmp.trace()-2.98)*pixVarient[i].length()/xCells);
                        revWeight.append((tmp.trace()-2.98)*pixVarient[j].length()/xCells);
                    }
                }
            }
        }
    }

    qDebug()<<revAvrgOPA.length();


    for( i=gm.length()-1; i>=0; i-=1)
    {
        if(pixVarient[i].length()<200 && rand()%10>7)
            continue;
        for( j=i-1; j>=0; j-=1 )
        {
            if(pixVarient[i].length()<200 && rand()%10>7)
                continue;
            if(N[i][j]<0.5)
                continue;
            for( m1= G.length()-1; m1>=0 ; m1--)
            {
                for( m2= G.length()-1; m2>=0 ; m2--)
                {
                    tmp=G[m1][i]*(G[m2][j].transpose());
                    if(tmp.trace()>2.98)    //8°
                    {
                        revAvrgOPA.append(G[m1][i].transpose());
                        revAvrgOPA.append(G[m2][j].transpose());
                        revWeight.append((tmp.trace()-2.98)*pixVarient[i].length()/xCells);
                        revWeight.append((tmp.trace()-2.98)*pixVarient[j].length()/xCells);
                    }
                }
            }
        }
    }

    qDebug()<<revAvrgOPA.length();

}

void DataSet::findEuerOPA(double meshSize)
{
    double pi=3.14159265358979323846;
    double currentMax=0;
    double sum;
    Mat3<double> tmp1,tmp2;

    double low1=    0;
    double low2=    0;
    double low3=    0;
    double high1=   360;
    double high2=   90 ;
    double high3=   90 ;

    //三次遍历，第一次角不要太大。
    for(double eular1=low1; eular1<high1; eular1+=meshSize)
    {
        for(double eular2=high2; eular2>low2; eular2-=meshSize)
        {
            for(double eular3=low3; eular3<high3; eular3+=meshSize)
            {
                tmp1=Mat3<double>(eular1,eular2,eular3);
                sum=0;
                for(int ii=revAvrgOPA.length()-1 ; ii>=0; ii--)
                {
                    tmp2=tmp1*revAvrgOPA[ii];
                    if(tmp2.trace()>2)
                        sum += 0.1/(3.02-tmp2.trace())*revWeight[ii] ;
                }
                if(sum>currentMax)
                {
                    currentMax=sum;
                    e1=eular1;
                    e2=eular2;
                    e3=eular3;
                }
            }
        }
    }

    low1=    e1-meshSize;
    low2=    e2-meshSize;
    low3=    e3-meshSize;
    high1=   e1+meshSize;
    high2=   e2+meshSize ;
    high3=   e3+meshSize ;
    meshSize = 0.2*meshSize;
    currentMax=0;

    for(double eular1=low1; eular1<high1; eular1+=meshSize)
    {
        for(double eular2=high2; eular2>low2; eular2-=meshSize)
        {
            for(double eular3=low3; eular3<high3; eular3+=meshSize)
            {
                tmp1=Mat3<double>(eular1,eular2,eular3);
                sum=0;
                for(int ii=revAvrgOPA.length()-1 ; ii>=0; ii--)
                {
                    tmp2=tmp1*revAvrgOPA[ii];
                    if(tmp2.trace()>2.7)
                        sum += 0.1/(3.02-tmp2.trace())*revWeight[ii] ;
                }
                if(sum>currentMax)
                {
                    currentMax=sum;
                    e1=eular1;
                    e2=eular2;
                    e3=eular3;
                }
            }
        }
    }

    low1=    e1-meshSize;
    low2=    e2-meshSize;
    low3=    e3-meshSize;
    high1=   e1+meshSize;
    high2=   e2+meshSize ;
    high3=   e3+meshSize ;
    meshSize = 0.25*meshSize;
    currentMax=0;

    for(double eular1=low1; eular1<high1; eular1+=meshSize)
    {

        for(double eular2=high2; eular2>low2; eular2-=meshSize)
        {
            for(double eular3=low3; eular3<high3; eular3+=meshSize)
            {
                tmp1=Mat3<double>(eular1,eular2,eular3);
                sum=0;
                for(int ii=revAvrgOPA.length()-1 ; ii>=0; ii--)
                {
                    tmp2=tmp1*revAvrgOPA[ii];
                    if(tmp2.trace()>2.7)
                        sum += 0.1/(3.02-tmp2.trace())*revWeight[ii] ;
                }
                if(sum>currentMax)
                {
                    currentMax=sum;
                    e1=eular1;
                    e2=eular2;
                    e3=eular3;
                }
            }
        }
    }

}


//---------------RECONSTRUCTION------------------------------

QList<double> DataSet::orient2Eular(Mat3<double> orient)
{
    QList<double> e123;
    double pi=3.14159265358979323846;

    double eular1 =180/pi*qAcos(orient[32]/sqrt(1-orient[33]*orient[33]));
    eular1 = orient[31]>=0? 360-eular1 : eular1;
    eular1 = eular1>180? eular1-180 : eular1+180;
    double eular2 =180/pi*qAcos(orient[33]);
    double eular3 =180/pi*qAcos(sqrt(orient[23]*orient[23]/(1-orient[33]*orient[33])));

    e123.append(eular1);
    e123.append(eular2);
    e123.append(eular3);
    return e123;
}

DataSet DataSet::RECON(QString OR, int iteration)
{
    //局部重构

    int i=400;
    generateOrientation();
    qDebug()<<i++;      //400
    makePixVarient_Connection();
    qDebug()<<i++;      //401
    initMAtrices(OR);
    qDebug()<<i++;      //402
    calcuRevAvrgOPA();
    qDebug()<<i++;      //403
    findEuerOPA(10);
    qDebug()<<i++;      //404

    qDebug()<<e1<<e2<<e3;

    Mat3<double> tmpmat=Mat3<double>(e1,e2,e3);
    tmpmat=tmpmat.transpose();
    Mat3<double> tmp2;
    int count=0;
    int total=0;
    for(int ii=0; ii<gm.length();ii++)
    {
        total+=pixVarient[ii].length();
        for(int m=0; m<G.length(); m++)
        {
            tmp2=G[m][ii]*tmpmat;
            if(tmp2.trace()>2.9)
            {
                count+=pixVarient[ii].length();
                break;
            }
        }
    }
    qDebug()<<count<<"/"<<total;

    DataSet data = *this;
    int m;
    for(int ii=0; ii< pixVarient.length(); ii++)
    {
        for(int jj=0; jj< pixVarient[ii].length(); jj++)
        {
            data.array[eular1Column][pixVarient[ii][jj]]=e1;
            data.array[eular1Column+1][pixVarient[ii][jj]]=e2;
            data.array[eular1Column+2][pixVarient[ii][jj]]=e3;
        }
    }

    return data;

}

DataSet DataSet::RECONSTRUCT(QString OR, int iteration)
{
    //全局重构

    int i=300;
    generateOrientation();    
    qDebug()<<i++;      //300
    makePixVarient_Connection();
    qDebug()<<i++;      //301
    initMAtrices(OR);
    qDebug()<<i++;      //302
    generateWeightConnection();
    qDebug()<<i++;      //303
    findOPA();
    qDebug()<<i++;      //304
    growth(iteration);
    qDebug()<<i++;      //305

    DataSet data = *this;

    for(int ii=0; ii< array[1].length(); ii++)
        data.array[eular1Column][ii]=data.array[eular1Column+1][ii]=data.array[eular1Column+2][ii]=0;

    double eular1, eular2, eular3;
    QList<double>   list;
    qDebug()<<ausPixVarient.length();

    for(int ii=0 ; ii< ausPixVarient.length() ; ii++)
    {
        list=orient2Eular(ga[ii]);
        eular1=list[0];
        eular2=list[1];
        eular3=list[2];

        for(int jj=0; jj<ausPixVarient[ii].length(); jj++)
        {
            data.array[eular1Column][ausPixVarient[ii][jj]]=eular1;
            data.array[eular1Column+1][ausPixVarient[ii][jj]]=eular2;
            data.array[eular1Column+2][ausPixVarient[ii][jj]]=eular3;
        }
    }

    return data;
}


