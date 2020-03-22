#include "MainWindow.h"
#include <QApplication>
#include "3DModel/STLView.h"
#include "CommonFun/common.h"
#include "CommonFun/CommonFun.h"
#include "3DModel/OpenGLWidget.h"
#include "Test.h"
#include "Alignment/UdpSocket.h"
#include "CodeParse/GCodeConversion.h"
#include "CodeParse/CodeEditDlg.h"
#include "CodeParse/GCodeListDlg.h"
#include "Teach/TeachDlg.h"
#include "Alignment/AnalysisWidget.h"
#include <iostream>
#include "CodeParse/RobotArgorithmn.h"
#include "math.h"
using namespace  std;
Coint currentC = { 0, 0 };
int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    Test::copy();
//    //OpenglWidget w(Lanxun6JointRobot,STLView::lanxun6JointView);
//   // GCodeConversion c;
//   // c.CreateConvertedFile("H:\\1.cnc",true);
//   // UdpSoket w(Lanxun5JointRobot,STLView::lanxun5JointView);
//   // w.show();
//   //CodeEditDlg c;
//   //c.show();
//    STLView::init(LanxunCoordRobot);
//    SimulationParser * parse = new SimulationParser(LanxunCoordRobot,STLView::lanxunCoordView);
//    OpenglWidget w(LanxunCoordRobot,STLView::lanxunCoordView);
//    w.show();
//    parse->show();
//    parse->run("C:\\simulation.cnc");
//     QTime time,time1,time2;
//     time.start();
//     STLView::init(LanxunCoordRobot);
//     qDebug()<<time.elapsed()/1000.0<<"s";
//     time1.start();
//     STLView::init(Lanxun5JointRobot);
//     qDebug()<<time1.elapsed()/1000.0<<"s";
//     time2.start();
//     STLView::init(Lanxun6JointRobot);
//     qDebug()<<time2.elapsed()/1000.0<<"s";
       MainWindow w;
       w.show();
//     AnalysisWidget s;
//     s.show();
//     SimulationParse p(Lanxun5JointRobot);
//     p.show();
//       OpenglWidget o(Lanxun5JointRobot);
//       o.show();
//     qDebug() << "加载";
//     CommonFun::sleep(3000);
//     o.loadArtifacts("H:\\hs\\w.STL");
//       NewScaraArgorithmn::TCFmatrix << 1, 0, 0, 0,
//                 0, 1, 0, 0,
//                 0, 0, 1, 0,
//                 0, 0, 0, 1;
      //Joint1 joint = {0,0,0,0,0,0};
//      dh = 0;
//    cout << acos(sqrt(2)/2.0) << endl;
//      Joint1 joint = {100,200,500,-30 / 180.0 * Pi,52 / 180.0 * Pi,0};
//    Matrix4d m = coo_fk(j1);
//    cout << m << endl;
//      Joint1  joint = {132 / 180.0 * PI,-31 / 180.0 * PI,320,99 / 180.0 * PI,-45 / 180.0 * PI,0};
//      Matrix4d m1 = RobotArgorithmn::fk_5JointNew(joint);
//      cout << m1 << endl;
//      cout << endl;
//      Joint j = {joint.j1,joint.j2,joint.j3,joint.j4,joint.j5,joint.j6};
//      Matrix4d m2 = fk(j);
//      cout << m2 << endl;
//      cout << endl;
//      RobotArgorithmn::ik_5Joint(m1);
      //Matrix4d m2 = RobotArgorithmn::fk_5Joint(joint);
      //cout << m2 << endl;
//      ArrayXd xyzrpw(6);
//      xyzrpw = pose_2_xyzrpw(m1);
//      cout << xyzrpw[0] << " "<< xyzrpw[1] << " "<< xyzrpw[2] << " "<< xyzrpw[3] << " "<< xyzrpw[4] << " "<< xyzrpw[5] << endl;
//      xyzrpw = pose_2_xyzrpw(m2);
//      cout << xyzrpw[0] << " "<< xyzrpw[1] << " "<< xyzrpw[2] << " "<< xyzrpw[3] << " "<< xyzrpw[4] << " "<< xyzrpw[5] << endl;
//    QTime time;
//    STLView::init(LanxunCoordRobot);
//    SimulationParser s(LanxunCoordRobot,STLView::lanxunCoordView);
//    s.show();
    return a.exec();
}
