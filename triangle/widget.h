#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QVector>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QProcess>

#include "region.h"
#include "paintdisplay.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:

    void on_button_Clear_clicked();
    void on_button_AddRegion_clicked();
    void on_button_AddPoint_clicked();
    void on_button_EditPoint_clicked();
    void on_button_MovePoint_clicked();
    void on_button_DelePoint_clicked();
    void on_button_SetValues_clicked();
    void on_button_TriangleAll_clicked();
    void on_button_Save_clicked();
    void on_button_Import_clicked();

    void shouMouse(QPointF mouse);
    void flat(int x);
    void getRegions(QVector <Region > allregions, double a, double b, double c, double d);

signals:
    void setMode(int x);
    void clear();
    void get();
    void importRegion(QVector <Region > importregions, double a, double b, double c, double d);
    void triangleRegions(QVector <Region > triangleregions, double a, double b, double c, double d);

private:
    bool writepoly();
    bool triangle();
    bool readnodeele();

    bool inRegion(Node a, Region b);


private:
    Ui::Widget *ui;

    PaintDisplay *paintdisplay;

    QVector < Region > allRegionsbefore;
    QVector < Region > allRegions;
    double top, bottom, left, right;

};

#endif // WIDGET_H
