#ifndef PARAMETER_H
#define PARAMETER_H
#include "CommonFun/common.h"
#include <QThread>
class Parameter
{
public:
    static Posture startPosture;  //起始点
    static bool teachStop;
    static double raiseHeight;
    static RobotType robotType;
    static void readParameter();
    static void writeParameter();
    static QThread thread;

};
#endif // PARAMETER_H
