#ifndef ROBOTARGORITHMN_H
#define ROBOTARGORITHMN_H
#include "Eigen/Dense"
#include "iostream"
using namespace std;
using namespace Eigen;

//关节参数
class Joint1
{
public:
    double j1;
    double j2;
    double j3;
    double j4;
    double j5;
    double j6;

    //弧度或者角度打印
    void printJointRadians() const;
    void printJointDegrees() const;
};

class RobotArgorithmn
{
public:
    RobotArgorithmn();

    //坐标式机器人正解,第二种效率更高
    static Matrix4d fk_Coord(const Joint1&);
    static Matrix4d fk_CoordNew(const Joint1&);
    //逆解并判断是否有解,并打印解
    static bool ik_Coord(const Matrix4d& m);

    static Matrix4d fk_5JointNew(const Joint1&);
    static Matrix4d fk_5Joint(const Joint1&);
    static bool ik_5Joint(const Matrix4d& m);
    static Matrix4d fk_6Joint(const Joint1&);

private:
    //五轴机器人参数
    static double a1_5;
    static double a2_5;
    static double d2_5;

    //焊枪矩阵
    static Matrix4d TCF_Coord;
    static Matrix4d TCF_5Joint;
    static Matrix4d TCF_6Joint;

    //六轴机器人参数
    static double a1_6;
    static double a2_6;
    static double a3_6;
    static double d1_6;
    static double d4_6;

    //获得每层joint转换矩阵，参数a，α，d，θ，
    static Matrix4d getT(double o,double d,double a,double i);

    //旋转矩阵
    static Matrix4d rotZ(double o);
    static Matrix4d rotX(double i);
    static Matrix4d rotY(double j);

    //移动矩阵
    static Matrix4d transZ(double d);
    static Matrix4d transX(double a); 

    //判断逆解Joint1是否正确
    static bool compare(const Joint1& jt,const Matrix4d &m,int i);
};

#endif // ROBOTARGORITHMN_H
