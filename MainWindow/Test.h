#ifndef TEST_H
#define TEST_H
#include "Eigen/Dense"
using namespace Eigen;
class Test
{
public:
    Test();
    static bool copy();
    static Matrix4d fk_Coord(double joint[6]);
private:
    static Matrix4d getT(double o,double d,double a,double i);
    static Matrix4d rotZ(double o);
    static Matrix4d rotX(double i);
    static Matrix4d transZ(double d);
    static Matrix4d transX(double a);
};

#endif // TEST_H
