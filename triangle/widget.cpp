#include "widget.h"
#include "ui_widget.h"
#include<stdio.h>
#include<iostream>

#define POLYFILE "temp.poly"
#define ELEFILE "temp.1.ele"
#define NODEFILE "temp.1.node"
#define TRIANGLE "triangle.exe"


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    showMaximized();
    paintdisplay = new PaintDisplay();
    ui->paint->addWidget(paintdisplay);
    ui->paint->setCurrentWidget(paintdisplay);
    ui->button_AddRegion->setDisabled(true);
    ui->button_AddPoint->setDisabled(true);
    ui->button_EditPoint->setDisabled(true);
    ui->button_MovePoint->setDisabled(true);
    ui->button_DelePoint->setDisabled(true);
    ui->button_Save->setDisabled(true);
    ui->button_SetValues->setDisabled(true);
    ui->button_TriangleAll->setDisabled(true);

    connect(this,
            SIGNAL(setMode(int)),
            paintdisplay,
            SLOT(setMode(int)));

    connect(this,
            SIGNAL(setMode(int)),
            this,
            SLOT(flat(int)));

    connect(this,
            SIGNAL(clear()),
            paintdisplay,
            SLOT(clearSet()));

    connect(paintdisplay,
            SIGNAL(mousePoint(QPointF)),
            this,
            SLOT(shouMouse(QPointF)));

    connect(this,
            SIGNAL(get()),
            paintdisplay,
            SLOT(send()));

    connect(paintdisplay,
            SIGNAL(sendRegions(QVector<Region>,double,double,double,double)),
            this,
            SLOT(getRegions(QVector<Region>,double,double,double,double)));

    connect(this,
            SIGNAL(importRegion(QVector<Region>,double,double,double,double)),
            paintdisplay,
            SLOT(importRegion(QVector<Region>,double,double,double,double)));

    connect(this,
            SIGNAL(triangleRegions(QVector<Region>,double,double,double,double)),
            paintdisplay,
            SLOT(triangleRegions(QVector<Region>,double,double,double,double)));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_button_Clear_clicked()
{
    ui->button_AddRegion->setDisabled(false);
    ui->button_AddPoint->setDisabled(false);
    ui->button_EditPoint->setDisabled(false);
    ui->button_MovePoint->setDisabled(false);
    ui->button_DelePoint->setDisabled(false);
    ui->button_Save->setDisabled(false);
    ui->button_SetValues->setDisabled(false);
    ui->button_TriangleAll->setDisabled(false);
    emit setMode(0);
    emit clear();
}

void Widget::on_button_AddRegion_clicked()
{
    emit setMode(0);
}

void Widget::on_button_AddPoint_clicked()
{
    emit setMode(1);
}

void Widget::on_button_EditPoint_clicked()
{
    emit setMode(2);
}

void Widget::on_button_MovePoint_clicked()
{
    emit setMode(3);
}

void Widget::on_button_DelePoint_clicked()
{
    emit setMode(4);
}

void Widget::on_button_SetValues_clicked()
{
    emit setMode(5);
}


void Widget::on_button_TriangleAll_clicked()
{
    writepoly();
    triangle();
    readnodeele();
    emit triangleRegions(allRegions, top, bottom, left, right);
}

void Widget::on_button_Save_clicked()
{
    QString savefilename = QFileDialog::getSaveFileName();
    QFile savefile(savefilename);
    emit get();
    savefile.open(QIODevice::WriteOnly | QIODevice::Text);
    int regionscount = allRegions.count();
    savefile.write(QString::number(regionscount).toLatin1().data());
    savefile.write(tr("\n").toLatin1().data());
    savefile.write((QString::number(top)+tr("\t")
                    +QString::number(bottom)+tr("\t")
                    +QString::number(left)+tr("\t")
                    +QString::number(right)).toLatin1().data());
    savefile.write(tr("\n").toLatin1().data());
    for(int i = 0; i < regionscount; i++)
    {
        Region currentregion = allRegions.at(i);
        int nodescount = currentregion.nodes.count();
        double area = currentregion.area();
        double rho = currentregion.Rho;
        double maxrho = currentregion.maxRho;
        double minrho = currentregion.minRho;
        double minangle = currentregion.minAngle;
        double maxlength = currentregion.maxLength;
        Node center = currentregion.center;
        savefile.write(QString::number(i).toLatin1().data());
        savefile.write(tr("\t").toLatin1().data());
        savefile.write(QString::number(area).toLatin1().data());
        savefile.write(tr("\t").toLatin1().data());
        savefile.write((tr("(")+QString::number(center.x)+tr(",")+QString::number(center.y)+tr(")")).toLatin1().data());
        savefile.write(tr("\t").toLatin1().data());
        savefile.write(QString::number(rho).toLatin1().data());
        savefile.write(tr("\t").toLatin1().data());
        savefile.write(QString::number(maxrho).toLatin1().data());
        savefile.write(tr("\t").toLatin1().data());
        savefile.write(QString::number(minrho).toLatin1().data());
        savefile.write(tr("\t").toLatin1().data());
        savefile.write(QString::number(minangle).toLatin1().data());
        savefile.write(tr("\t").toLatin1().data());
        savefile.write(QString::number(maxlength).toLatin1().data());
        savefile.write(tr("\t").toLatin1().data());
        for(int j = 0; j < nodescount; j++)
        {
            Node currentnode = currentregion.nodes.at(j);
            savefile.write((tr("(")+QString::number(currentnode.x)+tr(",")+QString::number(currentnode.y)+tr(")")).toLatin1().data());
            savefile.write(tr("\t").toLatin1().data());
        }
        savefile.write(tr("\n").toLatin1().data());
    }
    savefile.close();
}

void Widget::on_button_Import_clicked()
{
    QString importfilename = QFileDialog::getOpenFileName();
    QFile importfile(importfilename);
    importfile.open(QIODevice::ReadOnly | QIODevice::Text);
    QString line;
    QStringList linedata;
    line = importfile.readLine();
    int regionscount = line.toDouble();
    line = importfile.readLine();
    linedata = line.split(QRegExp("[, \t\n()]"),QString::SkipEmptyParts);
    line = linedata.at(0);
    top = line.toDouble();
    line = linedata.at(1);
    bottom = line.toDouble();
    line = linedata.at(2);
    left = line.toDouble();
    line = linedata.at(3);
    right = line.toDouble();
    if(regionscount <= 0)
    {
        return;
    }
    allRegions.clear();
    for(int i = 0; i < regionscount; i++)
    {
        Region currentregion;
        line = importfile.readLine();
        linedata = line.split(QRegExp("[, \t\n()]"),QString::SkipEmptyParts);
        int nodescount = (linedata.count() - 9)/2;
        QString dat;
        dat = linedata.at(2);
        currentregion.center.setX(dat.toDouble());
        dat = linedata.at(3);
        currentregion.center.setY(dat.toDouble());
        dat = linedata.at(4);
        currentregion.Rho = dat.toDouble();
        dat = linedata.at(5);
        currentregion.maxRho = dat.toDouble();
        dat = linedata.at(6);
        currentregion.minRho = dat.toDouble();
        dat = linedata.at(7);
        currentregion.minAngle = dat.toDouble();
        dat = linedata.at(8);
        currentregion.maxLength = dat.toDouble();
        for(int j = 0; j < nodescount; j++)
        {
            Node currentnode;
            dat = linedata.at(j * 2 + 9);
            currentnode.setX(dat.toDouble());
            dat = linedata.at(j * 2 + 10);
            currentnode.setY(dat.toDouble());
            currentregion.nodes << currentnode;
        }
        allRegions << currentregion;
    }
    importfile.close();
    emit importRegion(allRegions, top, bottom, left, right);
}

void Widget::shouMouse(QPointF mouse)
{
    double x = mouse.x();
    double y = mouse.y();
    ui->label_mouse->setText("( "+QString::number(x, 'f', 0)
                             +" , "+QString::number(y, 'f', 0)
                             +" )");
}

void Widget::flat(int x)
{
    ui->button_AddPoint->setFlat(false);
    ui->button_AddRegion->setFlat(false);
    ui->button_DelePoint->setFlat(false);
    ui->button_EditPoint->setFlat(false);
    ui->button_MovePoint->setFlat(false);
    ui->button_SetValues->setFlat(false);
    switch (x) {
    case 0: ui->button_AddRegion->setFlat(true);    break;
    case 1: ui->button_AddPoint->setFlat(true);     break;
    case 2: ui->button_EditPoint->setFlat(true);    break;
    case 3: ui->button_MovePoint->setFlat(true);    break;
    case 4: ui->button_DelePoint->setFlat(true);    break;
    case 5: ui->button_SetValues->setFlat(true);    break;
    default:    return; break;
    }
    return;
}

void Widget::getRegions(QVector<Region> allregions, double a, double b, double c, double d)
{
    allRegions.clear();
    allRegions = allregions;
    top = a;
    bottom = b;
    left = c;
    right = d;
    return;
}

bool Widget::writepoly()
{
    emit get();
    QVector < QPointF > points;
    QVector < int > segmentAnum;
    QVector < int > segmentBnum;
    QVector < int > regions;
    points.clear();
    segmentAnum.clear();
    segmentBnum.clear();
    int regionscount = allRegions.count();
    if(regionscount < 1)
    {
        return false;
    }
    for(int i = 0; i < regionscount ; i++)
    {
        Region currentregion = allRegions.at(i);
        int nodescount = currentregion.nodes.count();
        if(nodescount < 1)
        {
            continue;
        }
        for(int j = 0; j < nodescount; j++)
        {
            Node currentnode = currentregion.nodes.at(j);
            QPointF currentpoint = currentnode.coor();
            points << currentpoint;
            int pointscount = points.count();
            for(int k = 0; k < pointscount - 1; k++)
            {
                if(points.at(k) == currentpoint)
                {
                    points.removeLast();
                    break;
                }
            }
        }
    }
    int pointscount = points.count();
    int segmenta,segmentb;
    for(int i = 0; i < regionscount; i++)
    {
        Region currentregion = allRegions.at(i);
        int nodescount = currentregion.nodes.count();
        if(nodescount < 2)
        {
            continue;
        }
        for(int j = 0; j < nodescount - 1; j++)
        {
            Node currentnode = currentregion.nodes.at(j);
            Node nextnode = currentregion.nodes.at(j + 1);
            QPointF apoint = currentnode.coor();
            QPointF bpoint = nextnode.coor();
            for(int k = 0; k < pointscount; k++)
            {
                if(apoint == points.at(k))
                {
                    segmenta = k;
                    break;
                }
            }
            for(int k = 0; k < pointscount; k++)
            {
                if(bpoint == points.at(k))
                {
                    segmentb =k;
                }
            }
            segmentAnum << segmenta;
            segmentBnum << segmentb;
            int segmentscount = segmentAnum.count();
            for(int k = 0; k < segmentscount - 1; k++)
            {
                if(((segmentAnum.at(k) == segmenta) && (segmentBnum.at(k) == segmentb))
                        ||((segmentAnum.at(k) == segmentb) && (segmentBnum.at(k) == segmenta)))
                {
                    segmentAnum.removeLast();
                    segmentBnum.removeLast();
                }
            }
        }
    }
    for(int i = 0; i < regionscount; i++)
    {
        Node currentcenter = allRegions.at(i).center;
        if(currentcenter.isEmpty())
        {
            continue;
        }
        regions << i;
    }
    QFile file(POLYFILE);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug()<<"polyfile error";
        return false;
    }
    QString writeline;
    writeline = QString::number(pointscount) + tr(" 2 1 0\n");
    file.write(writeline.toLatin1().data());
    for(int i = 0; i < pointscount; i++)
    {
        writeline = QString::number(i + 1) + tr(" ")
                + QString::number(points.at(i).x()) + tr(" ")
                + QString::number(points.at(i).y()) + tr("\n");
        file.write(writeline.toLatin1().data());
    }
    int segmentscount = segmentAnum.count();
    writeline = QString::number(segmentscount) +   tr(" 0\n");
    file.write(writeline.toLatin1().data());
    for(int i = 0; i <  segmentscount; i++)
    {
        writeline = QString::number(i + 1) + tr(" ")
                + QString::number(segmentAnum.at(i) + 1) + tr(" ")
                + QString::number(segmentBnum.at(i) + 1) + tr("\n");
        file.write(writeline.toLatin1().data());
    }
    writeline = tr("0\n");
    file.write(writeline.toLatin1().data());
    int polyregions = regions.count();
    writeline = QString::number(polyregions) + tr("\n");
    file.write(writeline.toLatin1().data());
    for(int i = 0; i < polyregions; i++)
    {
        Region currentregion = allRegions.at(regions.at(i));
        QPointF center = currentregion.center.coor();
        double area = (currentregion.maxLength * currentregion.maxLength) / 2;
        writeline = QString::number(i + 1) + tr(" ")
                + QString::number(center.x()) + tr(" ")
                + QString::number(center.y()) + tr(" ")
                + QString::number(i + 1) + tr(" ")
                + QString::number(area) + tr("\n");
        file.write(writeline.toLatin1().data());
    }
    file.close();
}

bool Widget::triangle()
{
    QString triangleexe = TRIANGLE + tr(" -q30paAcjn ") + POLYFILE;
    FILE *fp;
    char buff[200];
    if((fp = _popen(triangleexe.toLatin1().data(),"r")) != nullptr)
    {
        qDebug()<<"Triangle begin.";
        while( (fgets(buff,200,fp))!=nullptr )
        {
            qDebug()<<QByteArray(buff);
        }
    }
    else
    {
        return false;
    }
    return true;
}

bool Widget::readnodeele()
{
    QFile nodefile(NODEFILE);
    if(!nodefile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return false;
    }
    QFile elefile(ELEFILE);
    if(!elefile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return false;
    }
    allRegionsbefore = allRegions;
    allRegions.clear();
    QVector < QPointF > points;
    QString line;
    QString dat;
    QStringList linedata;
    line = nodefile.readLine();
    linedata = line.split(QRegExp("[, \t\n()]"),QString::SkipEmptyParts);
    dat = linedata.at(0);
    int pointscount = dat.toInt();
    for(int i = 0; i < pointscount; i++)
    {
        QPointF currentpoint;
        line = nodefile.readLine();
        linedata = line.split(QRegExp("[, \t\n()]"),QString::SkipEmptyParts);
        dat = linedata.at(1);
        currentpoint.setX(dat.toDouble());
        dat = linedata.at(2);
        currentpoint.setY(dat.toDouble());
        points << currentpoint;
    }
    nodefile.close();
    qDebug()<<"nodefinished";
    line = elefile.readLine();
    linedata = line.split(QRegExp("[, \t\n()]"),QString::SkipEmptyParts);
    dat = linedata.at(0);
    int regionscount = dat.toInt();
    int oldregionscount = allRegionsbefore.count();
    for(int i = 0; i < regionscount; i++)
    {
        Region currentregion;
        QPointF a, b, c, center;
        line = elefile.readLine();
        linedata = line.split(QRegExp("[, \t\n()]"),QString::SkipEmptyParts);
        dat = linedata.at(1);
        a = QPointF(points.at(dat.toInt() - 1).x(), points.at(dat.toInt() - 1).y());
        currentregion.nodes << Node(a);
        dat = linedata.at(2);
        b = QPointF(points.at(dat.toInt() - 1).x(), points.at(dat.toInt() - 1).y());
        currentregion.nodes << Node(b);
        dat = linedata.at(3);
        c = QPointF(points.at(dat.toInt() - 1).x(), points.at(dat.toInt() - 1).y());
        currentregion.nodes << Node(c);
        currentregion.nodes << Node(a);
        center.setX((a.x() + b.x() + c.x())/3);
        center.setY((a.y() + b.y() + c.y())/3);
        for(int j = oldregionscount - 1; j >= 0; j--)
        {
            Region thisregion = allRegionsbefore.at(j);
            if(inRegion(Node(center), thisregion))
            {
                currentregion.setRho(thisregion.Rho);
                currentregion.setmaxRho(thisregion.maxRho);
                currentregion.setminRho(thisregion.minRho);
                currentregion.setminAngle(thisregion.minAngle);
                currentregion.setmaxLength(thisregion.maxLength);
                break;
            }
        }
        allRegions << currentregion;
    }
    elefile.close();
    return true;
}

bool Widget::inRegion(Node a, Region b)
{
    bool status=false;
    int nvert = b.nodes.count();
    double* vertx = new double[nvert];
    double* verty = new double[nvert];
    for(int k = 0; k < nvert; k++)
    {
        vertx[k] = b.nodes.at(k).x;
        verty[k] = b.nodes.at(k).y;
    }
    double testx = a.x;
    double testy = a.y;
    int i,j;
    for (i = 0, j = nvert-1; i < nvert; j = i++) {
        if ( ((verty[i]>testy) != (verty[j]>testy)) &&
             (testx < (vertx[j]-vertx[i]) * (testy-verty[i]) / (verty[j]-verty[i]) + vertx[i]) )
            status = !status;
    }
    return status;
}
