#ifndef PAINTDISPLAY_H
#define PAINTDISPLAY_H

#include <QWidget>
#include "region.h"
#include <math.h>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include "boundarydialog.h"
#include "setvaluedialog.h"

namespace Ui {
class PaintDisplay;
}

class PaintDisplay : public QWidget
{
    Q_OBJECT

public:
    explicit PaintDisplay(QWidget *parent = 0);
    ~PaintDisplay();

private:
    Ui::PaintDisplay *ui;

    QVector < Region > allRegions;

    Region currentRegion;
    Node currentNode;
    Region setRegion;
    int setRegionNumber;

    double top, bottom, left, right;

    int currentMode;

    QPen PointPen;
    QPen SegmentPen;
    QPen currentPointPen;
    QPen currentSegmentPen;

    BoundaryDialog *boundary;
    SetValueDialog *setvalue;

    Node mouseNode;
    Node mouseSegmentA;
    Node mouseSegmentB;

    int movenode;
    int movenodeR;

private:
    void paintEvent(QPaintEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);

    bool sameNode(Node a, Node b);
    bool onSegment(Node a, Node b, Node c);
    bool inRegion(Node a, Region b);

    QPointF coorTrans(QPointF point);
    QPointF coorInvTrans(QPointF point);

    QVector < Region > sort(QVector < Region > regions);

public:
    void addRegion(Region newregion);
    QVector < Region > getAllRegion();
    void clear();

signals:
    void mousePoint(QPointF mouse);
    void sendRegions(QVector < Region > allregions, double a, double b, double c, double d);

public slots:
    void setMode(int x);
    void getBoundary(double a, double b, double c, double d);
    void setValues(double rho, double maxrho, double minrho, double minangle, double maxlength);
    void clearSet();
    void send();
    void importRegion(QVector <Region > importregions, double a, double b, double c, double d);
    void triangleRegions(QVector <Region > triangleregions, double a, double b, double c, double d);

};

#endif // PAINTDISPLAY_H
