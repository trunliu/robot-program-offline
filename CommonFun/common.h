#ifndef COMMON_H
#define COMMON_H
#include <QDebug>
/*******************常量定义区******************/
#define robotNum  3
#define Pi 3.14159265358979323846
const int lanxunCoordRobotPartNum = 5;
const int lanxun5JointRobotPartNum = 6;
const int lanxun6JointRobotPartNum = 7;
//机器人类型
enum RobotType
{
  LanxunCoordRobot = 0,      //蓝讯坐标式机器人
  Lanxun5JointRobot = 1,     //蓝讯五关节式机器人
  Lanxun6JointRobot = 2     //蓝讯六关节式机器人
};
class MyPoint
{
public:
    double x = 0,y = 0,z = 0;
};

class Posture
{
public:
    double x = 0;
    double y = 0;
    double z = 0;
    double r = 0;
    double p = 0;
    double w = 0;
    double u = 0;
    double v = 0;
    double s = 3000;
};

#endif // COMMON_H
