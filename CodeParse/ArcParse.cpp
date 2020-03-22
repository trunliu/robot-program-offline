#include "ArcParse.h"
#include "CommonFun/Common.h"
#include "CommonFun/CommonFun.h"
ArcParse::ArcParse()
{

}
bool ArcParse::getCirclePoint(const ArrayXd& p1,const ArrayXd& p2,const ArrayXd& p3,ArrayXd& p4,ArrayXd& p5,ArrayXd& p6)
{
       double x1 = p1[0];
       double y1 = p1[1];
       double z1 = p1[2];
       double x2 = p2[0];
       double y2 = p2[1];
       double z2 = p2[2];
       double x3 = p3[0];
       double y3 = p3[1];
       double z3 = p3[2];
       double x0, y0, z0,r;
       if(!getCenterCoordAndRadius(x1,y1,z1,x2,y2,z2,x3,y3,z3,x0,y0,z0,r))
           return false;
       double x,y,z;
       getMidPoint(x0,y0,z0,r,x1,y1,z1,x2,y2,z2,x,y,z);
       p4[0] = x;
       p4[1] = y;
       p4[2] = z;
       getMidPoint(x0,y0,z0,r,x2,y2,z2,x3,y3,z3,x,y,z);
       p5[0] = x;
       p5[1] = y;
       p5[2] = z;
       getMidPoint(x0,y0,z0,r,x3,y3,z3,x1,y1,z1,x,y,z);
       p6[0] = x;
       p6[1] = y;
       p6[2] = z;
       return true;
}
void ArcParse::getMidPoint(double x0,double y0,double z0,double r,double x1,double y1,double z1,double x2,double y2,double z2,double &x,double &y,double &z)
{
    Vector3d v1(x1 - x0,y1 - y0,z1 - z0);
    Vector3d v2(x2 - x0,y2 - y0,z2 - z0);
    v1.normalize();
    v2.normalize();
    Vector3d v = v1 + v2;
    v = v.normalized() * r;
    x = v[0] + x0;
    y = v[1] + y0;
    z = v[2] + z0;
}

int ArcParse::isSplitArc(const ArrayXd& p1,const ArrayXd& p2,const ArrayXd& p3,ArrayXd& p4,ArrayXd& p5)
{
       double x1 = p1[0];
       double y1 = p1[1];
       double z1 = p1[2];
       double x2 = p2[0];
       double y2 = p2[1];
       double z2 = p2[2];
       double x3 = p3[0];
       double y3 = p3[1];
       double z3 = p3[2];
       double x0, y0, z0,r;
       if(!getCenterCoordAndRadius(x1,y1,z1,x2,y2,z2,x3,y3,z3,x0,y0,z0,r))
           return -1;
       double OAOB = (x1 - x0) * (x2 - x0) + (y1 - y0) * (y2 - y0) + (z1 - z0) * (z2 - z0);
       double OBOC = (x2 - x0) * (x3 - x0) + (y2 - y0) * (y3 - y0) + (z2 - z0) * (z3 - z0);
       double angle1 = acos(OAOB / (r * r));
       double angle2 = acos(OBOC / (r * r));
       double angle = (angle1 + angle2) * 180 / Pi;
       if(angle < 180)
           return 1;
       else
       {
           double x,y,z;
           getMidPoint(x0,y0,z0,r,x1,y1,z1,x2,y2,z2,x,y,z);
           p4[0] = x;
           p4[1] = y;
           p4[2] = z;
           qDebug() << "中点1：" << x << "  " << y << "  " << z ;
           getMidPoint(x0,y0,z0,r,x2,y2,z2,x3,y3,z3,x,y,z);
           p5[0] = x;
           p5[1] = y;
           p5[2] = z;
           qDebug() << "中点2：" << x << " " << y << " " << z ;
           return 2;
       }
}

bool ArcParse::getCenterCoordAndRadius(double x1, double y1, double z1,double x2, double y2, double z2, double x3, double y3, double z3,double &x0,double &y0,double &z0,double &r)
{
   if (CommonFun::isThreePointInLine(x1, y1, z1, x2, y2, z2, x3, y3, z3))
   {
       qDebug() << "三点共线" ;
       return false;
   }
   double ABAB, ABAC, ACAC, a, b;
   ABAB = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) + (z2 - z1) * (z2 - z1);
   ABAC = (x2 - x1) * (x3 - x1) + (y2 - y1) * (y3 - y1) + (z2 - z1) * (z3 - z1);
   ACAC = (x3 - x1) * (x3 - x1) + (y3 - y1) * (y3 - y1) + (z3 - z1) * (z3 - z1);
   a = ACAC * (ABAB - ABAC) / (ACAC * ABAB - ABAC * ABAC) / 2.0;
   b = (ABAB * ABAC - ABAB * ACAC) / (ABAC * ABAC - ACAC * ABAB) / 2.0;
   //圆心坐标和半径
   x0 = x1 + a * (x2 - x1) + b * (x3 - x1);
   y0 = y1 + a * (y2 - y1) + b * (y3 - y1);
   z0 = z1 + a * (z2 - z1) + b * (z3 - z1);
   r = sqrt((x0 - x1) * (x0 - x1) + (y0 - y1) * (y0 - y1) + (z0 - z1) * (z0 - z1));
   return true;
}

double ArcParse::getArcLength(double x1, double y1, double z1,double x2, double y2, double z2, double x3, double y3, double z3)
{
    double x0,y0,z0,r;
    getCenterCoordAndRadius(x1,y1,z1,x2,y2,z2,x3,y3,z3,x0,y0,z0,r);
     double OAOB = (x1 - x0) * (x2 - x0) + (y1 - y0) * (y2 - y0) + (z1 - z0) * (z2 - z0);
     double OBOC = (x2 - x0) * (x3 - x0) + (y2 - y0) * (y3 - y0) + (z2 - z0) * (z3 - z0);
     double angle1 = acos(OAOB / (r * r));
     double angle2 = acos(OBOC / (r * r));
     double angle = (angle1 + angle2);
     qDebug() << "圆心角：" << angle * 180 / Pi ;
     qDebug() << "半径：" << r ;
     return angle * r;
}


