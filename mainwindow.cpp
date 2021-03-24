#include "mainwindow.h"
#include "ui_mainwindow.h"



QString MainWindow::deSuffix(QString name){
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

QString MainWindow::avoidSameName(QString name){
    QString tmp= name;
    bool flag=true;
    int version=1;
    int ii;
    while (flag)
    {
        for (ii=0; ii<nameList.length();ii++)
        {
            if (!tmp.compare(nameList[ii]))
            {
                tmp=name+QString("(%1)").arg(version);
                version++;
                break;
            }
        }
        if (ii==nameList.length())  flag=false;
    }
    return tmp;
}


//-----------------构造函数、布局---------------------

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setStatusBar(nullptr);
    menubarHeight=this->menuBar()->height();

    current.currentDataSet=nullptr;

    view= new QGraphicsView(this);
    browser=new QTextEdit(this);
    view->hide();
    scene = new QGraphicsScene(this);
    splitLine= new MySplitLine(this);

    this->setMinimumSize(600,400);

    ui->treeWidget->resize(200,300);
    ui->dockWidget->setAllowedAreas(Qt::RightDockWidgetArea);
    ui->dockWidget->resize(120,800);
    splitLine->setGeometry(200,menubarHeight,20,this->height()-menubarHeight);
    splitLine->setFixedWidth(20);
    splitLine->show();


    //拖动可以改变宽高
    this->setGeometry(0,0,800,800);

    mode=0;
    curWidth=W1;
    curColor=red;

    reconOR="";
    reconType=iterations=-1;

}

void MainWindow::mouseReleaseEvent(QMouseEvent *E)
{


    // 这个没能达成随时resize的效果唉



    if (E->button() != Qt::LeftButton)
        return;

    manualResize();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    manualResize();
    QWidget::resizeEvent(event);
}

void MainWindow::manualResize()
{
    int height=this->height() - menubarHeight;
    splitLine->resize(splitLine->width(),height);
    ui->treeWidget->resize(splitLine->x(),height);

    int width=this->width() - ui->treeWidget->width() - 120;
    view->setGeometry( ui->treeWidget->width() + 20 , menubarHeight , width , height);
    browser->setGeometry( ui->treeWidget->width() + 20 , menubarHeight , width , height);

    ui->dockWidget->resize(120,height);
}

//changeFigure:更新按钮的图片（暂时还是改文字，图片要再加）

void MainWindow::changeFigure()
{
    switch (mode){
    case 1:
        ui->pushButton->setText("pen");
        break;
    case 0:
        ui->pushButton->setText("cursor");
    }

    switch (curWidth) {
    case W1 :
        ui->pushButton_3->setText("w=1");
        pen.setWidth(1);
        break;
    case W2 :
        ui->pushButton_3->setText("w=2");
        pen.setWidth(2);
        break;
    case W3 :
        ui->pushButton_3->setText("w=4");
        pen.setWidth(4);
        break;
    case W4:        //最后剩W4，w=6
        ui->pushButton_3->setText("w=6");
        pen.setWidth(6);
    }

    switch (curColor) {
    case white :
        ui->pushButton_2->setText("white");
        pen.setColor(Qt::white);
        break;
    case black:
        ui->pushButton_2->setText("black");
        pen.setColor(Qt::black);
        break;
    case red:
        ui->pushButton_2->setText("red");
        pen.setColor(Qt::red);
        break;
    case yellow:
        ui->pushButton_2->setText("yellow");
        pen.setColor(Qt::yellow);
        break;
    case green:
        ui->pushButton_2->setText("green");
        pen.setColor(Qt::green);
        break;
    case blue:    //最后剩蓝色
        ui->pushButton_2->setText("blue");
        pen.setColor(Qt::blue);
    }
}






//-------------------------导入与导出------------------

bool MainWindow::setPreset(bool changeName)
{
    PreDialog *preDialog= new PreDialog(this);
    Qt::WindowFlags flags=preDialog->windowFlags();
    preDialog->setWindowFlags(flags | Qt::MSWindowsFixedSizeDialogHint);
    int returnValue=preDialog->exec();  //模态显示

    if (returnValue==QDialog::Accepted)
    {
        if (nullptr==preDialog->getFileName())    return 0;

        dataPreset.xCells=preDialog->getxCells();
        dataPreset.yCells=preDialog->getyCells();
        dataPreset.direction=preDialog->getDirection();
        current.currentDataSet->setPresets(dataPreset.xCells, dataPreset.yCells, dataPreset.direction);

        qDebug()<<"setPreset";

        if (changeName)
        {
            current.currentDataSet->setLongName(preDialog->getFileName());
            QString newName=  current.currentDataSet->longFileName;

            int n= dataSetList.length();
            for (int ii=0; ii< n ;ii++)
            {
                if (dataSetList[ii]->longFileName==newName)
                {
                    qDebug()<<"请不要添加相同的文件";
                    delete preDialog;
                    return 0;
                }
            }
        }
        delete preDialog;
        return 1;
    }
    delete preDialog;
    return 0;
}

void MainWindow::daoru()
{
    current.currentDataSet=new DataSet();
    if (setPreset(1))
        dataSetList.append(current.currentDataSet);
    else{
        delete current.currentDataSet;
        if (!dataSetList.isEmpty())
            changeCurrent(*current.cDataSetItem);
        return;
    }

    nameList.append(current.currentDataSet->shortFileName);
    current.currentDataSet->readData(current.currentDataSet->longFileName);

    addFolderNDataset(*current.currentDataSet);
}





//---------------画图部分-----------------------

void MainWindow::drawEular(){

    if (!current.currentDataSet)
        return;

    if (!current.currentDataSet->haveEular)
    {

        QString fileName= "Eular_"+current.currentDataSet->shortFileName;
        fileName=avoidSameName(fileName);
        nameList.append(fileName);
        fileName+=".bmp";               //这四行顺序不能错

        fileName=current.currentDataSet->drawEular(0,fileName);

        //在TreeWidget新建一个item

        QTreeWidgetItem *item= new QTreeWidgetItem(current.cFolder);
        item->setFlags(Qt::ItemIsSelectable );
        item->setText(0,deSuffix(fileName));
        item->setText(1,"Eular");
        QFileInfo info(fileName);
        item->setText(2,info.absoluteFilePath());
        on_treeWidget_itemClicked(item);

        showGraph(item->text(2));
    }
}

void MainWindow::drawIPFx()
{
    if (!current.currentDataSet)
        return;

    if (!current.currentDataSet->haveIPFx)
    {
        QString fileName= "IPFx_"+current.currentDataSet->shortFileName;
        fileName=avoidSameName(fileName);
        nameList.append(fileName);
        fileName+=".bmp";               //这四行顺序不能错

        fileName= current.currentDataSet->drawIPF(1,0,fileName);

        //在TreeWidget新建一个item

        QTreeWidgetItem *item= new QTreeWidgetItem();
        item->setFlags(Qt::ItemIsSelectable);
        item->setText(0,deSuffix(fileName));
        item->setText(1,"IPFx");
        QFileInfo info(fileName);
        item->setText(2,info.absoluteFilePath());
        current.cFolder->addChild(item);
        on_treeWidget_itemClicked(item);

        showGraph(item->text(2));

    }
}

void MainWindow::drawIPFy()
{
    if (!current.currentDataSet)
        return;

    if (!current.currentDataSet->haveIPFy)
    {
        QString fileName= "IPFy_"+current.currentDataSet->shortFileName;
        fileName=avoidSameName(fileName);
        nameList.append(fileName);
        fileName+=".bmp";               //这四行顺序不能错

        fileName= current.currentDataSet->drawIPF(2,0,fileName);

        //在TreeWidget新建一个item

        QTreeWidgetItem *item= new QTreeWidgetItem();
        item->setFlags(Qt::ItemIsSelectable );
        item->setText(0,deSuffix(fileName));
        item->setText(1,"IPFy");
        QFileInfo info(fileName);
        item->setText(2,info.absoluteFilePath());
        current.cFolder->addChild(item);
        on_treeWidget_itemClicked(item);

        showGraph(item->text(2));
    }
}

void MainWindow::drawIPFz()
{
    if (!current.currentDataSet)
        return;

    if (!current.currentDataSet->haveIPFz)
    {
        QString fileName= "IPFz_"+current.currentDataSet->shortFileName;
        fileName=avoidSameName(fileName);
        nameList.append(fileName);
        fileName+=".bmp";               //这四行顺序不能错

        fileName= current.currentDataSet->drawIPF(3,0,fileName);

        QTreeWidgetItem *item= new QTreeWidgetItem();
        item->setFlags(Qt::ItemIsSelectable);
        item->setText(0,deSuffix(fileName));
        item->setText(1,"IPFz");
        QFileInfo info(fileName);
        item->setText(2,info.absoluteFilePath());
        current.cFolder->addChild(item);
        on_treeWidget_itemClicked(item);


        showGraph(item->text(2));
    }
}





//-----------------显示图或数据摘要----------------

void MainWindow::showGraph(QString fileName){

    qDebug()<<"show graph"<<fileName;

    pixmap= new QPixmap(fileName);

    browser->hide();
    scene->clear();
    view->setVisible(1);

    view->setGeometry(0,0,800,800);
    QRectF rect(0,0,pixmap->width(),pixmap->height());
    scene->setSceneRect(rect);
    view->setScene(scene);

    QGraphicsPixmapItem* move= scene->addPixmap(*pixmap);
    move->setPos(0,0);
    move->setFlags(QGraphicsItem::ItemIsFocusScope);

    scene->clearSelection();
    view->update();
    manualResize();
}

void MainWindow::showDataAbstract(DataSet* set){

    qDebug()<<"show data abstract";

    view->hide();
    browser->setGeometry(200,0,this->width()-200,this->height());
    browser->clear();
    browser->show();

    browser->insertPlainText("Preview:\n\n");
    for (int ii =-1; ii<this->height()/100 ; ii++)
    browser->insertPlainText(set->givePreview(ii));

    browser->insertPlainText("\nProperties:\n\n");
    browser->insertPlainText(QString("xCells: %1").arg(set->xCells));
    browser->insertPlainText(QString("\tyCells: %1").arg(set->yCells));
    browser->insertPlainText(QString("\tDirection: %1").arg(set->direction));
    browser->insertPlainText("\r\n");
    browser->insertPlainText(set->haveEular ? "Eular: Y\t" : "Eular: N\t");
    browser->insertPlainText(set->haveIPFx ? "IPFx: Y\t" : "IPFx: N\t");
    browser->insertPlainText(set->haveIPFy ? "IPFy: Y\t" : "IPFy: N\t");
    browser->insertPlainText(set->haveIPFz ? "IPFz: Y\t\r\n" : "IPFz: N\t\r\n");
    browser->insertPlainText(set->isASelection ? "Is a selection: Y\t" : "is a selection: N\t");
    browser->insertPlainText(set->hasASelection ? "Has a selection: Y" : "Has a selection: N");

    browser->update();
    manualResize();
}

void MainWindow::showFolderAbstract(){

    qDebug()<<"show folder abstract";

    view->hide();
    browser->setGeometry(200,0,this->width()-200,this->height());
    browser->clear();
    browser->show();

    browser->insertPlainText("Preview:\n\n");
    browser->insertPlainText("Has:\t\n");
    int n= current.cFolder->childCount();
    QString str;
    for (int ii=0; ii<n ;ii++)
    {
        str=current.cFolder->child(ii)->text(1);

        if (!str.compare("Eular"))
            browser->insertPlainText("\nEular:\t");
        if (!str.compare("Dataset"))
            browser->insertPlainText("\nDataset:\t");
        if (!str.compare("IPFx"))
            browser->insertPlainText("\nIPFx:\t");
        if (!str.compare("IPFy"))
            browser->insertPlainText("\nIPFy:\t");
        if (!str.compare("IPFz"))
            browser->insertPlainText("\nIPFz:\t");
        browser->insertPlainText(current.cFolder->child(ii)->text(0));

    }

    browser->update();
    manualResize();

}




//------------------树文件增减和实时确认--------------

QString MainWindow::nameFolder(){


    // 这个函数好像没用。可以删。


    int aaa=ui->treeWidget->topLevelItemCount();
    QString name =QString( "folder_%1").arg(aaa+1);

    return name;
}

void MainWindow::addFolderNDataset(DataSet& dataSet)
{
    QTreeWidgetItem* item= new QTreeWidgetItem();
    item->setText(0,avoidSameName(QString("Folder")));  //不要用namefolder命名，用avoidsamename
    nameList.append(item->text(0));         //dataset的名字是新建dataset的时候加进去的，folder是在这里新建的所以加在这里
    item->setText(1,"Folder");
    item->setFlags(Qt::ItemIsSelectable);
    ui->treeWidget->addTopLevelItem(item);

    QTreeWidgetItem* cItem= new QTreeWidgetItem();
    cItem->setText(0,dataSet.shortFileName);
    cItem->setText(1,"Dataset");
    cItem->setText(2,dataSet.longFileName);
    cItem->setFlags(Qt::ItemIsSelectable);

    item->addChild(cItem);
    ui->treeWidget->setCurrentItem(cItem);
    on_treeWidget_itemClicked(cItem);
    item->setExpanded(1);

    qDebug()<<"added: "<<current.currentDataSet->shortFileName;
}

void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item){

    ui->treeWidget->clearSelection();
    item->setSelected(true);
    changeCurrent(*item);
}

void MainWindow::changeCurrent(QTreeWidgetItem& item){

    if (mode == 1)  return;     //铅笔模式下在框选区域，currentBundle不会变。

    if(!item.text(1).compare("Dataset"))    //如果是dataset
    {
        current.cDataSetItem=&item;
        current.cFolder=current.cDataSetItem->parent();
        current.cMapItem=nullptr;

        int n= dataSetList.length();
        for (int ii=0; ii<n ;ii++)
        {
            if (dataSetList[ii]->longFileName==item.text(2))
                current.currentDataSet=dataSetList[ii];
        }
        qDebug()<<"changed current item to:  "<<current.currentDataSet->shortFileName;
        showDataAbstract(current.currentDataSet);

        return;
    }

    if(!item.text(1).compare("Folder"))
    {
        item.setExpanded(1);
        current.cFolder=&item;

        int n= item.childCount();
        for (int ii=0; ii<n ;ii++)
        {
            if ( ! item.child(ii)->text(1).compare("Dataset") )
                current.cDataSetItem=item.child(ii);
        }  

        n= dataSetList.length();
        for (int ii=0; ii<n ;ii++)
        {
            if (dataSetList[ii]->longFileName==current.cDataSetItem->text(2))
                current.currentDataSet=dataSetList[ii];
        }
        qDebug()<<"changed current item to:   "<<current.currentDataSet->shortFileName;
        showFolderAbstract();

        return;
    }
    else
    {
        current.cMapItem=&item;
        current.cFolder=current.cMapItem->parent();

        int n= item.parent()->childCount();
        for (int ii=0; ii<n ;ii++)
        {
            if ( ! item.parent()->child(ii)->text(1).compare("Dataset") )
                current.cDataSetItem=item.parent()->child(ii);
        }

        n= dataSetList.length();
        for (int ii=0; ii<n ;ii++)
        {
            if (dataSetList[ii]->longFileName==current.cDataSetItem->text(2))
                current.currentDataSet=dataSetList[ii];
        }

        qDebug()<<"changed current item to:   "<<current.currentDataSet->shortFileName;
        showGraph(item.text(2));
        return;
    }


}



//---------------------选取部分------------------

void MainWindow::mousePressEvent(QMouseEvent *ev)
{
    int leftWidth = ui->treeWidget->width() + 20 ;


    switch (mode) {

        //铅笔模式是1

    case 1:

        //左键的功能是加一个点进队列

        if(ev->button()==Qt::LeftButton)
        {
            QPoint *point= new QPoint(ev->pos());
            if (point->x()<leftWidth) break;
            if (point->y()<menubarHeight)   break;
            if (point->x()>this->width()-120) break;
            point->setX(point->x()-leftWidth);
            point->setY(point->y()-menubarHeight);
            QPoint tmpp= view->mapToScene(*point).toPoint();
            delete point;
            point= new QPoint(tmpp);

            if(!dotstack.isEmpty())
            {
                QPoint *lastPoint =dotstack.pop();
                dotstack.push(lastPoint);
                dotstack.push(point);
                scene->addLine(lastPoint->rx(),lastPoint->ry(),point->rx(),point->ry(),pen);
            }
            else
            {
                dotstack.push(point);
                scene->addEllipse(point->rx(),point->ry(),2,2,pen);
            }
        }

        //右键的功能是如果非空的话，删去最近一个点

        if(ev->button()==Qt::RightButton)
        {
            if(!dotstack.isEmpty())
            {
                QPoint *deletePoint =dotstack.pop();
                delete deletePoint;

                QPoint *lastPop;
                QPoint *thisPop;
                QStack<QPoint*> tmp;

                scene->clear();
                scene->addPixmap(*pixmap);

                if(!dotstack.isEmpty())
                {
                    thisPop= dotstack.pop();
                    tmp.push(thisPop);
                    scene->addEllipse(thisPop->rx(),thisPop->ry(),1,1,pen);
                }
                else return;

                while(!dotstack.isEmpty())
                {
                    lastPop= thisPop;
                    thisPop= dotstack.pop();
                    tmp.push(thisPop);
                    scene->addEllipse(thisPop->rx(),thisPop->ry(),1,1,pen);
                    scene->addLine(lastPop->rx(),lastPop->ry(),thisPop->rx(),thisPop->ry(),pen);
                }

                while (!tmp.isEmpty())
                {
                    thisPop= tmp.pop();
                    dotstack.push(thisPop);
                }

            }

        }
        break;

    default: break;
    }

    scene->update();
}

//selectedArea：用来把顶点list转换成边上点list，然后把后续工作完成。

QList<QPoint*> MainWindow::selectedArea(QList<QPoint*> list)
{
    region.clear();

    int n=list.length();
    list.append(list[0]);

    for (int ii=1; ii<=n; ii++)
    {
        int vec1x=list[ii]->rx()-list[ii-1]->rx();
        int vec1y=list[ii]->ry()-list[ii-1]->ry();
        for(int jj=1; jj<= abs(vec1y); jj++)
        {
            QPoint *newPoint= new QPoint(list[ii-1]->rx()+(jj*vec1x)/abs(vec1y),
                                        list[ii-1]->ry()+jj*vec1y/abs(vec1y));
            region.append(newPoint);
        }
    }

        //注意如果一行里有奇数个点，将这一行中那个顶点再添加一遍。

    int selectedN= region.length();
    int count;

    for (int ii=1; ii<= n; ii++)
    {
        count=0;
        for(int jj=0; jj< selectedN; jj++)
            if (region[jj]->y()==list[ii]->y())
                count++;
        if (count%2) region.append(list[ii]);
    }

    QRect rect= current.currentDataSet->setSelectedAreaData(region);
    if (rect.width()<1 || rect.height()<1)
    {
        qDebug()<<"未选择选区";
        return region;
    }


        //下面新建一个folder和dataset

    DataSet* newDataset= new DataSet();
    *newDataset = *current.currentDataSet;      //等号已重载，只把presets和数据拷过来
    newDataset->isASelection=1;

    QString newFileName="slct_"+current.currentDataSet->shortFileName;
    newFileName=avoidSameName(newFileName);
    nameList.append(newFileName);
    newFileName+=".txt";
    QFile newFile(newFileName);
    if(!newFile.open(QIODevice::WriteOnly ))
    {
        qDebug()<<"wrong code -2";
        return  QList<QPoint*>({nullptr});
    }
    newFile.close();
    QFileInfo  newFileInfo(newFile);
    newDataset->setLongName(newFileInfo.absoluteFilePath());
    newDataset->reserveSelectedOnly(rect);
    dataSetList.append(newDataset); //必须先加进list，后加进treeweidget
    addFolderNDataset(*newDataset);

    return region;
}


//-------------------重构函数----------------------

void MainWindow::RECONSTRUCT(QString OR ,int type, int iteration)
{
    DataSet* newDataset= new DataSet();
    if(type==0)
        *newDataset = current.currentDataSet->RECONSTRUCT( OR, iteration );
    if(type==1)
        *newDataset = current.currentDataSet->RECON( OR, iteration );         //等号已重载，只把presets和数据拷过来
    newDataset->isASelection=0;

    QString newFileName="recon_"+current.currentDataSet->shortFileName;
    newFileName=avoidSameName(newFileName);
    nameList.append(newFileName);
    newFileName+=".txt";
    QFile newFile(newFileName);
    if(!newFile.open(QIODevice::WriteOnly ))
    {
        qDebug()<<"wrong code -3";
        return  ;
    }
    newFile.close();
    QFileInfo  newFileInfo(newFile);
    newDataset->setLongName(newFileInfo.absoluteFilePath());
    dataSetList.append(newDataset); //必须先加进list，后加进treeweidget
    addFolderNDataset(*newDataset);

}

//----------------------部分槽函数放在这里------------------

void MainWindow::on_actionzhanshitingkaowu_triggered(){
    ui->dockWidget->setVisible(true);
}





//---------------------析构---------------------------

MainWindow::~MainWindow()
{
    delete ui;
}

