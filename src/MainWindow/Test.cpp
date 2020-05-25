#include "Test.h"
#include <QFile>
#include "CommonFun/CommonFun.h"
#include <QVector>
#include "math.h"
Test::Test()
{

}

bool Test::copy()
{
    QString ss = "H:\\hs\\";
    QVector<QString> all;
    QVector<QString> content;
    all.clear();
    for(int i = 1; i <=2; i++)
    {
       CommonFun::readTextFile(ss + QString("%1.STL").arg(i),content);
       all.append(content);
    }
    CommonFun::wirteTextFile(ss + "w.STL",all);
    return true;
}
Matrix4d Test::rotZ(double o)
{
    Matrix4d m = Matrix4d::Zero();;
    m << cos(o), -sin(o), 0, 0,
         sin(o),  cos(o), 0, 0,
           0,         0,  1, 0,
           0,         0,  0, 1;
    return m;
}
Matrix4d Test::rotX(double i)
{
    Matrix4d m = Matrix4d::Zero();
    m << 1,   0,      0,     0,
         0, cos(i), -sin(i), 0,
         0, sin(i), cos(i),  0,
         0,    0,     0,     1;
    return m;
}
Matrix4d Test::transZ(double d)
{
    Matrix4d m = Matrix4d::Zero();
    m << 1, 0, 0, 0,
         0, 1, 0, 0,
         0, 0, 1, 0,
         0, 0, 0, d;
    return m;
}
Matrix4d Test::transX(double a)
{
    Matrix4d m = Matrix4d::Zero();;
    m << 1, 0, 0, a,
         0, 1, 0, 0,
         0, 0, 1, 0,
         0, 0, 0, 0;
    return m;
}
Matrix4d Test::fk_Coord(double joint[6])
{
   Matrix4d T13 = Matrix4d::Zero();
   T13 << 1,0,0,joint[0],
          0,1,0,joint[1],
          0,0,1,joint[2],
          0,0,0,1;
   Matrix4d T4 = getT(joint[3],0,0,-90 / 180.0 * PI);
   Matrix4d Th = rotZ(joint[4]) * rotX(90 / 180.0 * PI);
   return T13 * T4 * Th;
}
 Matrix4d Test::getT(double o,double d,double a,double i)  //o是弧度值
 {
     Matrix4d m = Matrix4d::Zero();
     m << cos(o), -sin(o) * cos(i), sin(o) * sin(i), a * cos(o),
          sin(o), cos(o) * cos(i), -cos(o) * sin(i), a * sin(o),
            0,          sin(i),         cos(i),         d,
            0,            0,              0,            1;
     return m;
 }
