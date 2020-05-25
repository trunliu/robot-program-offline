#ifndef TEACHGLWIDGET_H
#define TEACHGLWIDGET_H
#include "3DModel/OpenGLWidget.h"
#include "CodeParse/scaralib.h"
#include <QWidget>

class TeachGlWidget : public OpenglWidget
{
    Q_OBJECT
public:
    TeachGlWidget(RobotType type);
    void  keyPressEvent(QKeyEvent * e);
public slots:
    void homeSlot();
    void moto_SettingJSlot(char ch, double angle,double speed);  //单轴移动
    void moveLSlot(char ch,double distance,double speed);
    void getCoordSlot();
signals:
    void sendCoordSignal(ArrayXd);
private:
     void moto_runJ(Joint j,double speed);  //先把考虑变位机构
     char ch;
     double speed;
     double angle;
     double moveLineInter;
     double moveJointInter;
};
#endif // TEACHGLWIDGET_H
