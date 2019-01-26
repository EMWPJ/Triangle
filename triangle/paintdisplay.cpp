#include "paintdisplay.h"
#include "ui_paintdisplay.h"
#include <QInputDialog>
#include <QDebug>
#include <QPolygonF>

PaintDisplay::PaintDisplay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PaintDisplay)
{
    ui->setupUi(this);
    setMouseTracking(true);
    PointPen = QPen(Qt::blue, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    SegmentPen = QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    currentPointPen = QPen(Qt::red, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    currentSegmentPen = QPen(Qt::red, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

    currentMode = 0;
    top = 0;bottom = 0;left = 0;right = 0;movenode = -1;movenodeR = -1;
    boundary = new BoundaryDialog();
    setvalue = new SetValueDialog();
    connect(boundary,
            SIGNAL(sendBoundary(double,double,double,double)),
            this,
            SLOT(getBoundary(double,double,double,double)));
    connect(setvalue,
            SIGNAL(sendValues(double,double,double,double,double)),
            this,
            SLOT(setValues(double,double,double,double,double)));
}

PaintDisplay::~PaintDisplay()
{
    delete ui;
}

void PaintDisplay::clearSet()
{
    boundary->open();
}

void PaintDisplay::setMode(int x)
{
    currentMode = x;
    currentNode.clear();
    currentRegion.clear();
    setRegion.clear();
    update();
}

void PaintDisplay::clear()
{
    currentRegion.clear();
    currentNode.clear();
    currentMode = -1;
    allRegions.clear();
    boundary->open();
}

void PaintDisplay::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        Node currentpoint;
        currentpoint.setPoint(coorTrans(e->pos()));
        switch (currentMode)
        {
        case 0:     //AddRegion
        {
            int regionscount = allRegions.count();
            if(regionscount <=0 )
            {
                e->accept();
                return;
            }
            for(int i = 0; i < regionscount; i++)
            {
                Region currentregion = allRegions.at(i);
                int nodescount = currentregion.nodes.count();
                if(nodescount <= 0)
                {
                    continue;
                }
                for(int j = 0; j < nodescount - 1; j++)
                {
                    Node currentnode = currentregion.nodes.at(j);
                    if(sameNode(currentnode, currentpoint))
                    {
                        currentRegion.nodes << currentnode;
                        e->accept();
                        update();
                        return;
                    }
                }
            }
            for(int i = 0; i < regionscount; i++)
            {
                Region currentregion = allRegions.at(i);
                int nodescount = currentregion.nodes.count();
                if(nodescount <= 1)
                {
                    continue;
                }
                for(int j = 0; j < nodescount - 1; j++)
                {
                    Node currentnode = currentregion.nodes.at(j);
                    Node nextnode = currentregion.nodes.at(j + 1);
                    if(onSegment(currentpoint, currentnode, nextnode))
                    {
                        currentregion.nodes.insert(j + 1, currentpoint);
                        allRegions.replace(i, currentregion);
                    }
                }
            }
            currentRegion.nodes << currentpoint;
            update();
            e->accept();
            break;
        }
        case 1:       //AddPoint
        {
            int regionscount = allRegions.count();
            if(regionscount <= 0)
            {
                e->accept();
                return;
            }
            for(int i = 0; i < regionscount; i++)
            {
                int nodescount = allRegions.at(i).nodes.count();
                if(nodescount < 3)
                {
                    e->accept();
                    break;
                }
                for(int j = 0; j < nodescount - 1; j++)
                {
                    if(onSegment(currentpoint,
                                 allRegions.at(i).nodes.at(j),
                                 allRegions.at(i).nodes.at(j + 1)))
                    {
                        Region replace = Region(allRegions.at(i).nodes);
                        replace.nodes.insert(j + 1, currentpoint);
                        allRegions.replace(i, replace);
                        update();
                        e->accept();
                        return;
                    }
                }
            }
            break;
        }
        case 2:       //EditPoint
        {
            int regionscount = allRegions.count();
            if(regionscount <= 0)
            {
                return;
            }
            for(int i = 0; i < regionscount; i++)
            {
                int nodescount = allRegions.at(i).nodes.count();
                if(nodescount < 3)
                {
                    break;
                }
                for(int j = 0; j < nodescount - 1; j++)
                {
                    if(sameNode(currentpoint, allRegions.at(i).nodes.at(j)))
                    {
                        bool isOK=true;
                        double x=QInputDialog::getDouble(NULL, "Set X",
                                                             "Input the X",
                                                             0,left,right,2,
                                                             &isOK);
                        double y=QInputDialog::getDouble(NULL, "Set Y",
                                                             "Input the Y",
                                                             0,bottom,top,2,
                                                             &isOK);
                        Region replace = Region(allRegions.at(i).nodes);
                        Node replacenode = Node(x, y);
                        replace.nodes.replace(j, replacenode);
                        replace.nodes.replace(replace.nodes.count() - 1,replace.nodes.first());
                        allRegions.replace(i, replace);
                        update();
                        e->accept();
                        return;
                    }
                }
            }
            break;
        }
        case 3:       //MovePoint
        {
            int regionscount = allRegions.count();
            if(regionscount <= 0)
            {
                e->accept();
                return;
            }
            for(int i = 0; i < regionscount; i++)
            {
                int nodescount = allRegions.at(i).nodes.count();
                if(nodescount < 3)
                {
                    break;
                }
                for(int j = 0; j < nodescount - 1; j++)
                {
                    if(sameNode(currentpoint, allRegions.at(i).nodes.at(j)))
                    {
                        movenodeR = i;
                        movenode = j;
                        e->accept();
                        return;
                    }
                }
            }
            break;
        }
        case 4:       //DelePoint
        {
            int regionscount = allRegions.count();
            if(regionscount <= 0)
            {
                e->accept();
                return;
            }
            for(int i = 0; i < regionscount; i++)
            {
                int nodescount = allRegions.at(i).nodes.count();
                if(nodescount <= 4)
                {
                    e->accept();
                    break;
                }
                for(int j = 0; j < nodescount - 1; j++)
                {
                    if(sameNode(currentpoint, allRegions.at(i).nodes.at(j)))
                    {
                        Region replaceRegion = allRegions.at(i);
                        replaceRegion.nodes.removeAt(j);
                        replaceRegion.nodes.replace(replaceRegion.nodes.count() - 1,replaceRegion.nodes.first());
                        allRegions.replace(i, replaceRegion);
                        e->accept();
                        update();
                        e->accept();
                        return;
                    }
                }
            }
            break;
        }
        case 5:       //SetValues
        {
            Node currentpoint = Node(coorTrans(e->pos()));
            int regionscount =allRegions.count();
            if(regionscount <= 0)
            {
                e->accept();
                return;
            }
            else if(regionscount == 1)
            {
                setRegion = allRegions.at(0);
                setRegion.center.setPoint(coorTrans(e->pos()));
                setRegionNumber = 0;
                setvalue->open();
                e->accept();
                update();
                return;
            }
            else if(regionscount > 1)
            {
                allRegions = sort(allRegions);
                for(int i = regionscount - 1; i >= 0; i--)
                {
                    if(inRegion(currentpoint, allRegions.at(i)))
                    {
                        setRegion = allRegions.at(i);
                        setRegion.center.setPoint(coorTrans(e->pos()));
                        setRegionNumber = i;
                        setvalue->open();
                        e->accept();
                        update();
                        return;
                    }
                }
            }
            break;
        }
            e->accept();
            break;
        }
        e->accept();
        return;
    }
    else if(e->button() == Qt::RightButton)
    {
        if(currentMode == 0)
        {
            if(currentRegion.nodes.count() < 3)
            {
                return;
            }
            Node currentpoint;
            currentpoint = currentRegion.nodes.at(0);
            currentRegion.nodes << currentpoint;
            allRegions << currentRegion;
            currentRegion.clear();
            update();
            e->accept();
            return;
        }
        else if(currentMode == 3 && movenode != -1 && movenodeR != -1)
        {
            movenode = -1;
            movenodeR = -1;
        }
        else
        {
            e->accept();
            return;
        }
    }
}

void PaintDisplay::mouseMoveEvent(QMouseEvent *e)
{
    QPointF mouse = coorTrans(e->pos());
    emit mousePoint(mouse);
    Node currentpoint = Node(coorTrans(e->pos()));
    mouseNode.clear();
    mouseSegmentA.clear();
    mouseSegmentB.clear();
    int regionscount = allRegions.count();
    if(regionscount > 0)
    {
        for(int i = 0; i < regionscount; i++)
        {
            Region currentregion = allRegions.at(i);
            int nodescount = currentregion.nodes.count();
            if(nodescount <= 0)
            {
                continue;
            }
            for(int j = 0; j < nodescount - 1; j++)
            {
                if(sameNode(currentregion.nodes.at(j), currentpoint))
                {
                    mouseNode = currentregion.nodes.at(j);
                    break;
                }
            }
            for(int j = 0; j < nodescount - 1; j++)
            {
                if(onSegment(currentpoint, currentregion.nodes.at(j), currentregion.nodes.at(j + 1)))
                {
                    mouseSegmentA = currentregion.nodes.at(j);
                    mouseSegmentB = currentregion.nodes.at(j + 1);
                    break;
                }
            }
        }
        update();
    }
    if(currentMode == 3 && movenode != -1 && movenodeR != -1)
    {
        Node replaceNode = Node(coorTrans(e->pos()));
        Region replaceRegion = allRegions.at(movenodeR);
        replaceRegion.nodes.replace(movenode, replaceNode);
        replaceRegion.nodes.replace(replaceRegion.nodes.count() - 1,replaceRegion.nodes.first());
        allRegions.replace(movenodeR, replaceRegion);
        update();
        e->accept();
        return;
    }
}

void PaintDisplay::paintEvent(QPaintEvent *e)
{
    //qDebug()<<"update";
    QPainter painter(this);
    painter.setPen(PointPen);
    int regionscount = allRegions.count();
    //qDebug()<<"regionscount"<<regionscount;
    if(regionscount <= 0)
    {
        return;
    }

    QVector < Region > polygens = allRegions;//sort(allRegions);
    for(int i = 0; i < polygens.count() ; i++)
    {
        Region currentregion = polygens.at(i);
        int currentnodescount = currentregion.nodes.count();
        QBrush regionbrush(currentregion.color(),Qt::SolidPattern);
        painter.setPen(SegmentPen);
        painter.setBrush(regionbrush);
        QPolygonF currentpolygon;
        for(int j = 0; j < currentnodescount - 1; j++)
        {
            Node currentpoint = currentregion.nodes.at(j);
            currentpolygon << coorInvTrans(currentpoint.coor());
        }
        painter.drawPolygon(currentpolygon);
    }

    for(int i = 0; i < regionscount; i++)
    {
        int nodescount = allRegions.at(i).nodes.count();
        //qDebug()<<"nodescount"<<nodescount;
        if(nodescount < 3)
        {
            break;
        }
        painter.setPen(PointPen);
        for(int j = 0; j < nodescount - 1; j++)
        {
            QPointF currentnode;
            currentnode.setX(allRegions.at(i).nodes.at(j).x);
            currentnode.setY(allRegions.at(i).nodes.at(j).y);
            //qDebug()<<"currentnode"<<coorInvTrans(currentnode);
            painter.drawPoint(coorInvTrans(currentnode));
        }
        painter.setPen(SegmentPen);
        for(int j = 0; j < nodescount - 1; j++)
        {
            QPointF a, b;
            a.setX(allRegions.at(i).nodes.at(j).x);
            a.setY(allRegions.at(i).nodes.at(j).y);
            b.setX(allRegions.at(i).nodes.at(j + 1).x);
            b.setY(allRegions.at(i).nodes.at(j + 1).y);
            painter.drawLine(coorInvTrans(a), coorInvTrans(b));
        }
    }

    int currentpointscount = currentRegion.nodes.count();
    if(currentpointscount == 1)
    {
        painter.setPen(currentPointPen);
        QPointF currentpoint;
        currentpoint.setX(currentRegion.nodes.at(0).x);
        currentpoint.setY(currentRegion.nodes.at(0).y);
        painter.drawPoint(coorInvTrans(currentpoint));
    }
    else if (currentpointscount > 1)
    {
        painter.setPen(currentPointPen);
        for(int i = 0; i < currentpointscount; i++)
        {
            QPointF currentpoint;
            currentpoint.setX(currentRegion.nodes.at(i).x);
            currentpoint.setY(currentRegion.nodes.at(i).y);
            painter.drawPoint(coorInvTrans(currentpoint));
            //qDebug()<<"currentpoint"<<currentpoint;
        }
        painter.setPen(currentSegmentPen);
        for(int i = 0; i < currentpointscount-1; i++)
        {
            QPointF currentpoint, nextpoint;
            currentpoint.setX(currentRegion.nodes.at(i).x);
            currentpoint.setY(currentRegion.nodes.at(i).y);
            nextpoint.setX(currentRegion.nodes.at(i+1).x);
            nextpoint.setY(currentRegion.nodes.at(i+1).y);
            painter.drawLine(coorInvTrans(currentpoint),
                             coorInvTrans(nextpoint));
        }
    }

    if(!setRegion.isEmpty())
    {
        int nodescount = setRegion.nodes.count();
        {
            painter.setPen(currentSegmentPen);
            for(int i = 0; i < nodescount - 1; i++)
            {
                Node aa = setRegion.nodes.at(i);
                Node bb = setRegion.nodes.at(i + 1);
                QPointF a,b;
                a = aa.coor();
                b = bb.coor();
                painter.drawLine(coorInvTrans(a), coorInvTrans(b));
            }
        }
    }

    if(!mouseNode.isEmpty())
    {
        painter.setPen(currentPointPen);
        painter.drawPoint(coorInvTrans(mouseNode.coor()));
    }
    if(!mouseSegmentA.isEmpty() && !mouseSegmentB.isEmpty())
    {
        painter.setPen(currentSegmentPen);
        painter.drawLine(coorInvTrans(mouseSegmentA.coor()),
                         coorInvTrans(mouseSegmentB.coor()));
    }


}

bool PaintDisplay::sameNode(Node a, Node b)
{
    if(sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2))
            <sqrt(pow(left - right, 2) + pow(top - bottom, 2))/100)
    {
        return true;
    }
    else
    {
        return false;
    }

}

bool PaintDisplay::onSegment(Node a, Node b, Node c)
{
//    if((a.x - b.x)*(a.x - c.x) > 0)
//    {
//        return false;
//    }
//    if((a.y - b.y)*(a.y - c.y) > 0)
//    {
//        return false;
//    }r
    double ab = sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
    double ac = sqrt((a.x - c.x)*(a.x - c.x) + (a.y - c.y)*(a.y - c.y));
    double bc = sqrt((b.x - c.x)*(b.x - c.x) + (b.y - c.y)*(b.y - c.y));
    if((ab + ac - bc)/bc < 0.001)
    {
        return true;
    }
    return false;
}

bool PaintDisplay::inRegion(Node a, Region b)
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

void PaintDisplay::getBoundary(double a, double b, double c, double d)
{
    top = a;
    bottom = b;
    left = c;
    right = d;
    allRegions.clear();
    Node lefttop(left, top);
    Node leftbottom(left, bottom);
    Node rightbottom(right, bottom);
    Node righttop(right, top);
    Region first;
    first.nodes << lefttop << leftbottom
                << rightbottom << righttop << lefttop;
    allRegions << first;
    update();
}

QPointF PaintDisplay::coorTrans(QPointF point)
{
    double width = (right - left)/(this->width() - 2);
    double height = (bottom - top)/(this->height() - 2);
    QPointF coor;
    coor.setX((point.x())* width + left);
    coor.setY((point.y())* height + top);
    return coor;
}

QPointF PaintDisplay::coorInvTrans(QPointF point)
{
    double width = (right - left)/(this->width() - 2);
    double height = (bottom - top)/(this->height() - 2);
    QPointF coorInv;
    coorInv.setX((point.x() - left)/ width);
    coorInv.setY((point.y() -top)/ height);
    return coorInv;
}

QVector < Region > PaintDisplay::sort(QVector<Region> regions)
{
    int regionscount = regions.count();
    if(regionscount <= 1)
    {
        return regions;
    }
    for(int i = 0; i < regionscount -1; i++)
    {
        for(int j = i + 1; j < regionscount; j++)
        {
            Region iregion = regions.at(i);
            double iarea = iregion.area();
            Region jregion = regions.at(j);
            double jarea = jregion.area();
            if(iarea < jarea)
            {
                regions.replace(i ,jregion);
                regions.replace(j ,iregion);
            }
        }
    }
    return regions;
}

void PaintDisplay::setValues(double rho,
                             double maxrho,
                             double minrho,
                             double minangle,
                             double maxlength)
{
    setRegion.Rho = rho;
    setRegion.maxRho = maxrho;
    setRegion.minRho = minrho;
    setRegion.minAngle = minangle;
    setRegion.maxLength = maxlength;
    allRegions.replace(setRegionNumber, setRegion);
    allRegions = sort(allRegions);
    return;
}

void PaintDisplay::send()
{
    allRegions = sort(allRegions);
    emit sendRegions(allRegions, top, bottom, left, right);
    return;
}

void PaintDisplay::importRegion(QVector<Region> importregions, double a, double b, double c, double d)
{
    allRegions.clear();
    allRegions = importregions;
    top = a;
    bottom = b;
    left = c;
    right = d;
    update();
    return;
}

void PaintDisplay::triangleRegions(QVector<Region> triangleregions, double a, double b, double c, double d)
{
    allRegions.clear();
    allRegions = triangleregions;
    top = a;
    bottom = b;
    left = c;
    right = d;
    update();
    return;
}
