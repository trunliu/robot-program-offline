#include "Parameter.h"
Posture Parameter::startPosture;
double Parameter::raiseHeight = 50;
RobotType Parameter::robotType = LanxunCoordRobot;
bool Parameter::teachStop = false;
QThread Parameter::thread;
#include "CommonFun/CommonFun.h"
void Parameter::readParameter()
{
    startPosture.x = CommonFun::ReadSettingsDouble("Para/startX");
    startPosture.y = CommonFun::ReadSettingsDouble("Para/startY");
    startPosture.z = CommonFun::ReadSettingsDouble("Para/startZ");
    startPosture.r = CommonFun::ReadSettingsDouble("Para/startR");
    startPosture.p = CommonFun::ReadSettingsDouble("Para/startP");
    startPosture.w = CommonFun::ReadSettingsDouble("Para/startW");
    robotType = (RobotType)CommonFun::ReadSettingsInt("Para/robotType");
}
void Parameter::writeParameter()
{
    CommonFun::WriteSettingsDouble("Para/startX",startPosture.x);
    CommonFun::WriteSettingsDouble("Para/startY",startPosture.y);
    CommonFun::WriteSettingsDouble("Para/startZ",startPosture.z);
    CommonFun::WriteSettingsDouble("Para/startR",startPosture.r);
    CommonFun::WriteSettingsDouble("Para/startP",startPosture.p);
    CommonFun::WriteSettingsDouble("Para/startW",startPosture.w);
    CommonFun::WriteSettingsInt("Para/robotType",robotType);
}
