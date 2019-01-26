#ifndef REGION_H
#define REGION_H
#include <QVector>
#include <QColor>
#include <math.h>
#include <QString>
#include <QPointF>

class Node
{
public:
    double x;
    double y;

public:
    Node()
    {
        x = 0;
        y = 0;
    }
    Node(double a, double b)
    {
        x = a;
        y = b;
    }
    Node(QPointF a)
    {
        x = a.x();
        y = a.y();
    }

    void clear()
    {
        x = 999999999999999;
        y = 999999999999999;
    }
    void setX(double a)
    {
        x = a;
    }
    void setY(double b)
    {
        y = b;
    }
    void setPoint(double a, double b)
    {
        x = a;
        y = b;
    }
    void setPoint(QPointF a)
    {
        x = a.x();
        y = a.y();
    }
    bool isEmpty()
    {
        if(x == 999999999999999 && y == 999999999999999)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    QPointF coor()
    {
        return  QPointF(x ,y);
    }
};

class Region
{
public:
    QVector < Node > nodes;
    float Rho;
    float maxRho;
    float minRho;
    float minAngle;
    float maxLength;
    Node center;
    int number;

public:
    Region()
    {
        nodes.clear();
        Rho = 1;
        maxRho = 1000000;
        minRho = 0.000001;
        minAngle = 30;
        maxLength = 0;
        center.clear();
        number = -1;
    }

    Region(QVector < Node > points)
    {
        nodes = points;
        Rho = 1;
        maxRho = 1000000;
        minRho = 0.000001;
        minAngle = 30;
        maxLength = 0;
        center.clear();
        number = -1;
    }

    void setValue(float rho,
                  float maxrho,
                  float minrho,
                  float minangle,
                  float maxlength)
    {
        Rho = rho;
        maxRho = maxrho;
        minRho = minrho;
        minAngle = minangle;
        maxLength = maxlength;

    }

    void setRho(float rho)
    {
        Rho = rho;
    }

    void setmaxRho(float maxrho)
    {
        maxRho = maxrho;
    }

    void setminRho(float minrho)
    {
        minRho = minrho;
    }

    void setminAngle(float minangle)
    {
        minAngle = minangle;
    }

    void setmaxLength(float maxlength)
    {
        maxLength = maxlength;
    }
    void clear()
    {
        nodes.clear();
        Rho = 1;
        maxRho = 1000000;
        minRho = 0.000001;
        minAngle = 30;
        maxLength = 0;
        center.clear();
        number = -1;
    }
    bool isEmpty()
    {
        if(nodes.isEmpty())
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    void setNumber(int a)
    {
        number = a;
    }
    double area()
    {
        int point_num = nodes.count() - 1;
        if(point_num < 3)
            return 0.0;

        double s = (nodes.at(0).y)*((nodes.last().x) - (nodes.at(1).x));
        for(int i = 1; i < point_num ; ++i)
        {
            s += (nodes.at(i).y)*((nodes.at(i - 1).x) - (nodes.at(i + 1).x));
        }
        return fabs(s/2.0);
    }
    QColor color()
    {
        int r, g, b, a;
        double x = log10(Rho);
        if(x <= -1)
        {
            r = 0;
            g = 0;
            b = 255;
        }
        else if(x > -1 && x <= 1)
        {
            r = 0;
            g = (x + 1) * 127;
            b = 255;
        }
        else if(x > 1 && x <= 3)
        {
            r = 0;
            g = 255;
            b = 255 - (x - 1) * 127;
        }
        else if(x > 3 && x <= 5)
        {
            r = (x - 3) * 127;
            g = 255;
            b = 0;
        }
        else
        {
            r = 255;
            g = 255;
            b = 0;
        }
        a = 150;
        return QColor(r, g, b, a);
    }
};




#endif // REGION_H
