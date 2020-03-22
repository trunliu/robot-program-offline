#include "NewScaraArgorithmn.h"
#include "Setting/Parameter.h"
Matrix4d  NewScaraArgorithmn::coordTCFmatrix = Matrix4d::Zero();
Matrix4d  NewScaraArgorithmn::lanxun5JointTCFmatrix = Matrix4d::Zero();
Matrix4d  NewScaraArgorithmn::lanxun6JointTCFmatrix = Matrix4d::Zero();
void NewScaraArgorithmn::initTCF()  //这个TCF值还得测试出来  根据TCF来画杆子
{
    dh = 0;
    coordTCFmatrix << 1, 0, 0, 220,
                     0, 1, 0, -45,
                     0, 0, 1, -170,
                     0, 0, 0, 1;

    lanxun5JointTCFmatrix << 1, 0, 0,0,
                             0, 1, 0, 0,
                             0, 0, 1, -233.924,
                             0, 0, 0, 1;
    lanxun6JointTCFmatrix << 1, 0, 0,0,
                             0, 1, 0, 0,
                             0, 0, 1, 0,
                             0, 0, 0, 1;
      float JAAA_min[6] = {-17000000.0, -17000000.0, -500000, -3600.0 / 180 * Pi, -3600.0 / 180 * Pi, -3600.0 / 180 * Pi};
      float JAAA_max[6] = { 17000000.0,  17000000.0,   500000,  3600.0 / 180 * Pi, 3600.0 / 180 * Pi,  3600.0 / 180 * Pi};
      for(int i = 0; i < 6; i++)
      {
           JA_min[i] = JAAA_min[i];
           JA_max[i] = JAAA_max[i];
      }
}

// 判断机器人类型，进行正解，包含焊枪标定值
Matrix4d NewScaraArgorithmn::fksolution(Joint j)
{
    if(Parameter::robotType == LanxunCoordRobot)
    return coo_fk(j) * coordTCFmatrix;
    else if(Parameter::robotType == Lanxun5JointRobot)
    return fk(j) * lanxun5JointTCFmatrix;
}

//根据机器人类型，进行逆解
Joint NewScaraArgorithmn::NewPositionJointssolution(Matrix4d Matrix)
{
    Matrix4d temp = Matrix4d::Zero();
    //坐标机器人逆解
    if(Parameter::robotType == LanxunCoordRobot){
      temp = Matrix * coordTCFmatrix.inverse();
      return coo_ik(temp);
    //关节机器人逆解
    }else if(Parameter::robotType == Lanxun5JointRobot){
       temp = Matrix * lanxun5JointTCFmatrix.inverse();
      return NewPositionJoints(temp);
    }
}


