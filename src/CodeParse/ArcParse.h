#ifndef ARCPARSE_H
#define ARCPARSE_H
#include "scaralib.h"


class ArcParse
{
public:
    ArcParse();
    //圆的函数
    static bool getCirclePoint(const ArrayXd& p1,const ArrayXd& p2,const ArrayXd& p3,ArrayXd& p4,ArrayXd& p5,ArrayXd& p6);
    static void getMidPoint(double x0,double y0,double z0,double r,double x1,double y1,double z1,double x2,double y2,double z2,double &x,double &y,double &z);
    static int  isSplitArc(const ArrayXd& p1,const ArrayXd& p2,const ArrayXd& p3,ArrayXd& p4,ArrayXd& p5);
    static bool getCenterCoordAndRadius(double x1, double y1, double z1,double x2, double y2, double z2, double x3, double y3, double z3,double &x0,double &y0,double &z0,double &r);
    static double getArcLength(double x1, double y1, double z1,double x2, double y2, double z2, double x3, double y3, double z3);
};
#endif // ARCPARSE_H
