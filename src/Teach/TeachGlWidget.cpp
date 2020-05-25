#include "TeachGlWidget.h"
#include "CodeParse/NewScaraArgorithmn.h"
#include "CommonFun/CommonFun.h"
#include "Setting/Parameter.h"
#include "math.h"
#include <QThread>
TeachGlWidget::TeachGlWidget(RobotType type) : OpenglWidget(type)
{

   widgetname = TEACH;
   moveLineInter = 1;
   moveJointInter = 1;
   isShowWorkPiece = true;
   NewScaraArgorithmn::initTCF();
   home();
   qDebug()<<"TeachGlWidget构造函数";
}

void  TeachGlWidget::keyPressEvent(QKeyEvent * e)
{
    return OpenglWidget::keyPressEvent(e);
}

void TeachGlWidget::homeSlot()
{
    home();
}
//单轴移动函数
void TeachGlWidget::moto_runJ(Joint j,double speed)
{
     moveJointInter =  speed / 4000;
     if(moveJointInter > 1)
         moveJointInter = 1;
    double j1,j2,j3,j4,j5,j6;  //这个要走的角度
    //gl目前的角度和需要转动角度的差距
     Joint lastJ;
     getCurrentJ(lastJ);
     lastJ.j1 += j.j1;
     lastJ.j2 += j.j2;
     lastJ.j3 += j.j3;
     lastJ.j4 += j.j4;
     lastJ.j5 += j.j5;
     lastJ.j6 += j.j6;
    if(glType == Lanxun5JointRobot)
    {
         j1 = j.j1 * 180 / PI ;  //角度值
         j2 = j.j2 * 180 / PI;
    }
    else
    {
        j1 = j.j1;
        j2 = j.j2;
    }
    if(glType == Lanxun6JointRobot)
        j3 = j.j3 * 180 / PI;
    else
        j3 = j.j3;
    j4 = j.j4 * 180 / PI;
    j5 = j.j5 * 180 / PI ;
    j6 = j.j6 * 180 / PI;
    double contia = sqrt(j1 * j1 + j2 * j2 + j3 * j3 + j4 * j4 + j5 * j5 + j6 * j6);
    if(contia == 0)
    {
        return;
    }
    double rate = 1.0 / contia * moveJointInter;
    double vs = 1000;  //初始速度
    Parameter::teachStop = false;
    for(double i = 0; i < contia; i+= moveJointInter) //每次转一度 10度的加减速
    { 
        QElapsedTimer et;
        et.start();
        if(!Parameter::teachStop)
        vs += 200;
        if(vs > speed)
            vs = speed;
        QCoreApplication::processEvents();
        j1Rot += j1 * rate;
        j2Rot += j2 * rate;
        j3Rot += j3 * rate;
        j4Rot += j4 * rate;
        j5Rot += j5 * rate;
        j6Rot += j6 * rate;
        getCurrentCoord();
       if(Parameter::teachStop)
       { 
             Parameter::teachStop = false;
             return;  
       }
       while(et.elapsed() < (60.0 * 1000.0 * moveJointInter / vs))
           QCoreApplication::processEvents();
    }
    /***************为了保证精度，最后一个点要走到**************/
    setJ(lastJ);
    Parameter::teachStop = false;
}
//单轴移动指令
void TeachGlWidget::moto_SettingJSlot(char ch, double angle,double speed) //单轴移动
{
    Joint j = {0,0,0,0,0,0};
    switch(ch)
    {
    case 1:
    if(glType == LanxunCoordRobot)
    j.j1 = angle;
    else
    j.j1 = angle / 180.0 * Pi;
    break;
    case 2:
    if(glType == LanxunCoordRobot)
    j.j2 = angle ;
    else
    j.j2 = angle / 180.0 * Pi;
    break;
    case 3:
    if(glType == Lanxun6JointRobot)
    j.j3 = angle / 180.0 * Pi;
    else
    j.j3 = angle ;
    break;
    case 4: j.j4 = angle / 180.0 * Pi; break;
    case 5: j.j5 = angle / 180.0 * Pi; break;
    case 6: j.j6 = angle / 180.0 * Pi; break;
    default:break;
    }
    moto_runJ(j,speed);//旋转一个角度
}
//还是做一个梯形的加减速的过程
void TeachGlWidget::moveLSlot(char ch,double distance,double speed)
{
    moveLineInter =  speed / 4000;
    if(moveLineInter > 1)
        moveLineInter = 1;
    double d[6] = {0};
    if(distance > 0)
        d[ch-1] = moveLineInter;
    else
        d[ch-1] = -moveLineInter;
    ArrayXd xyzrpw(6);
    ArrayXd lastxyzrpw(6);
    for(int i = 0; i < 6;i++)
     lastxyzrpw[i] = coord[i];
    double vs = 1000;
    double ss = fabs(distance);
    Parameter::teachStop = false;
    for(double i = 0; i < ss; i += fabs(moveLineInter))
    {
        QElapsedTimer et;
        et.start();
        if(!Parameter::teachStop)
        vs += 500;
        if(vs > speed)
            vs = speed;
        QCoreApplication::processEvents();
        for(int i = 0; i < 6;i++)
        {
         if(i != ch -1)
         coord[i] = lastxyzrpw[i];
          xyzrpw[i] = coord[i] + d[i];
        }
         Joint j = NewScaraArgorithmn::NewPositionJointssolution(xyzrpw_2_pose(xyzrpw));
         setJ(j);
         if(Parameter::teachStop)
         {
              Parameter::teachStop = false;
              return;
         }
         while(et.elapsed()< 60.0 * 1000.0 * moveLineInter / speed)
             QCoreApplication::processEvents();
    }
    d[ch-1] = distance;
    for(int i = 0; i < 6;i++)
     xyzrpw[i] = lastxyzrpw[i] + d[i];
    Joint j = NewScaraArgorithmn::NewPositionJointssolution(xyzrpw_2_pose(xyzrpw));
    setJ(j);
    for(int i = 0;i < 6;i++)
        coord[i] = CommonFun::round(xyzrpw[i],2);
    updateGL();
    Parameter::teachStop = false;
}
void TeachGlWidget::getCoordSlot()
{
    ArrayXd xyzrpw(6);
    for(int i = 0; i < 6;i++)
        xyzrpw[i] = coord[i];
    emit sendCoordSignal(xyzrpw);
}
