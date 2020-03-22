#include "RobotArgorithmn.h"
#include <math.h>
#include <vector>
#define PI 				M_PI
#define ZERO_THRESH 	(2)
#define EPSINON 0.00000001
 double  RobotArgorithmn::a1_5 = 700;
 double  RobotArgorithmn::a2_5 = 600;
 double  RobotArgorithmn::d2_5 = 703.2;
 double  RobotArgorithmn::a1_6 = 109;
 double  RobotArgorithmn::a2_6 = 250.5;
 double  RobotArgorithmn::a3_6 = 115.5;
 double  RobotArgorithmn::d1_6 = 215;
 double  RobotArgorithmn::d4_6 = 239.5;

 //Identity(4x4)的单位阵
 Matrix4d RobotArgorithmn::TCF_Coord = Matrix4d::Identity(4,4);
 Matrix4d RobotArgorithmn::TCF_5Joint = Matrix4d::Identity(4,4);
 Matrix4d RobotArgorithmn::TCF_6Joint= Matrix4d::Identity(4,4);

//弧度单位打印
void Joint1::printJointRadians() const{
    cout << j1 << " "<< j2 << " "<< j3 << " "<< j4 << " "<< j5 << " "<< j6 << endl;
}

//角度单位打印
void Joint1::printJointDegrees() const{
    cout << j1 * 180.0 / PI << " "<< j2 * 180.0 / PI<< " "<< j3 << " " << j4 * 180.0 / PI << " "<< j5 * 180.0 / PI << " "<< j6 << endl;
}

RobotArgorithmn::RobotArgorithmn()
{

}

//绕z轴的旋转矩阵
Matrix4d RobotArgorithmn::rotZ(double o)
{
    //初始化
    Matrix4d m = Matrix4d::Zero();//4x4的零矩阵
    m << cos(o), -sin(o), 0, 0,
         sin(o),  cos(o), 0, 0,
           0,         0,  1, 0,
           0,         0,  0, 1;
    return m;
}

//绕y轴的旋转矩阵
Matrix4d RobotArgorithmn::rotY(double j)
{
    Matrix4d m = Matrix4d::Zero();;
    m << cos(j), 0, sin(j), 0,
         0,      1,  0,     0,
         -sin(j),0,cos(j),  0,
         0,      0,  0,     1;
    return m;
}

//绕x轴的旋转矩阵
Matrix4d RobotArgorithmn::rotX(double i)
{
    Matrix4d m = Matrix4d::Zero();
    m << 1,   0,      0,     0,
         0, cos(i), -sin(i), 0,
         0, sin(i), cos(i),  0,
         0,    0,     0,     1;
    return m;
}

//z轴移动d距离
Matrix4d RobotArgorithmn::transZ(double d)
{
    Matrix4d m = Matrix4d::Zero();
    m << 1, 0, 0, 0,
         0, 1, 0, 0,
         0, 0, 1, d,
         0, 0, 0, 1;
    return m;
}

//x轴移动a距离
Matrix4d RobotArgorithmn::transX(double a)
{
    Matrix4d m = Matrix4d::Zero();
    m << 1, 0, 0, a,
         0, 1, 0, 0,
         0, 0, 1, 0,
         0, 0, 0, 1;
    return m;
}

//坐标机器人的正解，缺点每次都要进行矩阵的乘法，浪费资源
Matrix4d RobotArgorithmn::fk_Coord(const Joint1& joint)
{

   Matrix4d T1_3 = Matrix4d::Zero();
   //T1~3只是移动
   T1_3 << 1,0,0,joint.j1,
          0,1,0,joint.j2,
          0,0,1,joint.j3,
          0,0,0,1;
   //参数都是弧度
   Matrix4d T4 = getT(joint.j4,0,0,-90 / 180.0 * PI);
   Matrix4d T5=  getT(joint.j5,0,0,90/180*PI);
   //Matrix4d T5 = rotZ(joint.j5) * rotX(90 / 180.0 * PI);
   return T1_3 * T4 * T5 * TCF_Coord;
}

//坐标式机器人正解，输入关节输出矩阵
Matrix4d RobotArgorithmn::fk_CoordNew(const Joint1& joint)
{
    Matrix4d m;
    m << cos(joint.j4) * cos(joint.j5), -sin(joint.j4), cos(joint.j4) * sin(joint.j5), joint.j1,
         sin(joint.j4) * cos(joint.j5), cos(joint.j4),  sin(joint.j4) * sin(joint.j5), joint.j2,
         -sin(joint.j5),0, cos(joint.j5), joint.j3,
         0, 0, 0, 1;
    return m * TCF_Coord;
}

//坐标式机器人逆解，输入矩阵输出关节
bool RobotArgorithmn::ik_Coord(const Matrix4d& m)
{
   //右乘焊枪矩阵的逆才是1~5关节的T矩阵
   const Matrix4d T1_5 = m * TCF_Coord.inverse();
   Joint1 tmpJoint;
   vector<Joint1> res;

   //提取矩阵中的位移值dx,dy,dz
   tmpJoint.j1 = T1_5(0,3);
   tmpJoint.j2 = T1_5(1,3);
   tmpJoint.j3 = T1_5(2,3);
   //无第六关节
   tmpJoint.j6 = 0;

   //θ5  根据公式中的  nz=-sin(θ5)  az=cos(θ5)
   double nz=T1_5(2,0);
   double az=T1_5(2,2);
   tmpJoint.j5=-atan2(nz,az);

   //θ4可能有两个解 az=cos(θ4)
   tmpJoint.j4=acos(T1_5(1,1));
   //0代表是坐标机器人，将tmpJoint正解后与T1_5比较
   if(compare(tmpJoint,T1_5,0)){
     res.push_back(tmpJoint);
   }

   tmpJoint.j4=-acos(T1_5(1,1));
   if(compare(tmpJoint,T1_5,0)){
     res.push_back(tmpJoint);
   }


/****************************
   //θ4，θ5都可能有两个
   v4.push_back(acos(t(1,1)));
   v5.push_back(acos(t(2,2)));
   //判断是否大于1
   if(fabs(t(1,1) - 1) > EPSINON )
       v4.push_back(-acos(t(1,1)));

   if(fabs(t(2,2) - 1) > EPSINON)
       v5.push_back(-acos(t(2,2)));

   //暴力解法，测试关节4，5值带入后的正解是否与传入参数矩阵相等
   //可能有多个解
   for(int i = 0; i < v4.size();i++){
       for(int j = 0; j < v5.size();j++){
           tmpJoint.j4 = v4[i];
           tmpJoint.j5 = v5[j];
           //判断解对不对
           if(compare(tmpJoint,t,0))
             res.push_back(tmpJoint);
       }
    }
*************/

    if(res.size() < 1){
        cout << "无解" << endl;
        return false;
    }else{
        //打印所有解
        cout << "解为:"<< endl;
        for(int i = 0; i < res.size();++i){
          res[i].printJointDegrees();
        }
    }
    return true;
}

//求5关节机器人逆解
bool RobotArgorithmn::ik_5Joint(const Matrix4d& m)
{
    Matrix4d t = m * TCF_Coord.inverse(); //终点坐标乘以TCF_Coord矩阵的
    Joint1 jt;
    jt.j6 = 0;
    //第一个解
    jt.j3 = t(2,3) - d2_5;  //0
    vector<Joint1> v;
    vector<double> v5; //角5的集合 有两个解
    vector<double> v2; //两个解
    //第二个解 可能有两组
    v5.push_back(acos(t(2,2)));
    if(fabs(t(2,2) - 1) > EPSINON)
    v5.push_back(-acos(t(2,2)));

    //角2的解
    double sv2 = (t(0,3) * t(0,3) + t(1,3) * t(1,3) - a1_5 * a1_5 - a2_5 * a2_5) / (2.0 * a1_5 * a2_5);
    v2.push_back(acos(sv2));
    if(fabs(sv2 - 1) > EPSINON)
    v2.push_back(-acos(sv2));
    for(unsigned int i = 0; i < v5.size();i++)
    {
        for(unsigned int j = 0; j < v2.size();j++) //从角2出发来看情况 可能有四组解
        {
            //角1的解
            jt.j2 = v2[j];
            jt.j5 = v5[i];
            cout << ":: " << (((a1_5 + a2_5 * sv2) * t(1,3) - a2_5 * sin(v2[j]) * t(0,3)) / ((a1_5 + a2_5 * sv2) * t(0,3) +  a2_5 * sin(v2[j]) * t(1,3))) <<endl;
            double sv1 = atan((((a1_5 + a2_5 * sv2) * t(1,3) - a2_5 * sin(v2[j]) * t(0,3)) / ((a1_5 + a2_5 * sv2) * t(0,3) +  a2_5 * sin(v2[j]) * t(1,3))));
            jt.j1 = sv1;
            cout << sv1 << endl;
            /**************************************/
            jt.j4 = -sv1 - v2[j] + acos(t(1,1));
            if(compare(jt,t,1))
            v.push_back(jt);
            if(fabs(t(1,1) - 1) > EPSINON)
            {
                jt.j4 = -sv1 - v2[j] - acos(t(1,1));
                if(compare(jt,t,1))
                v.push_back(jt);
            }



            /**************************************/
           if(sv1 > 0)
           {
             jt.j1 = sv1 - PI;
             cout << "++: " << jt.j1 << endl;
             /**************************************/
             jt.j4 = -sv1 - v2[j] + acos(t(1,1));
             if(compare(jt,t,1))
             v.push_back(jt);
             if(fabs(t(1,1) - 1) > EPSINON)
             {
                 jt.j4 = -sv1 - v2[j] - acos(t(1,1));
                 if(compare(jt,t,1))
                 v.push_back(jt);
             }
             /**************************************/
           }
           if(sv1 < 0)
           {
               jt.j1 = sv1 + PI;
               cout << "--: " << jt.j1 << endl;
               /**************************************/
               jt.j4 = -sv1 - v2[j] + acos(t(1,1));
               if(compare(jt,t,1))
               v.push_back(jt);
               if(fabs(t(1,1) - 1) > EPSINON)
               {
                   jt.j4 = -sv1 - v2[j] - acos(t(1,1));
                   if(compare(jt,t,1))
                   v.push_back(jt);
               }
               /**************************************/
           }

        }
    }
    if(v.size() < 1)
    {
        cout << "wujie" << endl;
        return false;
    }
    else
    {
        cout << "jiewei:"<< endl;
        for(unsigned int i = 0; i < v.size();i++)
          v[i].printJointDegrees();
    }
}


//通过关节值正解得到的矩阵与矩阵m比较
 bool RobotArgorithmn::compare(const Joint1& jt,const Matrix4d &m,int i)
 {
        Matrix4d res;
        switch(i){
          case 0: res = fk_CoordNew(jt);
          break;
          case 1: res = fk_5Joint(jt);
          break;
          case 2: //res = fk_6JointNew(jt);
          break;
        }

        //比较两个矩阵，让每个元素相减，fabs()返回绝对值
        for(int i = 0; i < 4;i++){
            for(int j = 0; j < 4;j++){
               if(fabs(res(i,j) - m(i,j)) > ZERO_THRESH)
                 return false;
            }
        }
        return true;
}

//返回正解5关节矩阵位字旋转矩阵
Matrix4d RobotArgorithmn::fk_5Joint(const Joint1& joint)
{
    Matrix4d T1 = getT(joint.j1,0,a1_5,0);
    Matrix4d T2 = getT(joint.j2,d2_5,a2_5,0);
    Matrix4d T3 = getT(0,joint.j3,0,0);
    Matrix4d T4 = getT(joint.j4,0,0,-90 / 180.0 * PI);
    Matrix4d T5 = getT(joint.j5,0,0,90 / 180.0 * PI);
    return T1 * T2 * T3 * T4 * T5 * TCF_5Joint;
}

//返回正解5关节矩阵位字旋转矩阵（提前算好）效率更高
Matrix4d RobotArgorithmn::fk_5JointNew(const Joint1& joint)
{
    Matrix4d m;
    m << cos(joint.j1 + joint.j2 + joint.j4) * cos(joint.j5), -sin(joint.j1 + joint.j2 + joint.j4),  cos(joint.j1 + joint.j2 + joint.j4) * sin(joint.j5), a1_5 * cos(joint.j1) + a2_5 * cos(joint.j1 + joint.j2),
         sin(joint.j1 + joint.j2 + joint.j4) * cos(joint.j5),  cos(joint.j1 + joint.j2 + joint.j4),  sin(joint.j1 + joint.j2 + joint.j4) * sin(joint.j5), a1_5 * sin(joint.j1) + a2_5 * sin(joint.j1 + joint.j2),
         -sin(joint.j5),0, cos(joint.j5), d2_5 + joint.j3,
         0, 0, 0, 1;
    return m * TCF_5Joint;
}

//返回正解6关节矩阵位字旋转矩阵
Matrix4d RobotArgorithmn::fk_6Joint(const Joint1& joint)
{
  Matrix4d T1 = getT(joint.j1,d1_6,a1_6,-90 / 180.0 * PI);
  Matrix4d T2 = getT(joint.j2 - (90 / 180.0 * PI),0,a2_6,0);
  Matrix4d T3 = getT(joint.j3,0,a3_6,-90 / 180.0 * PI);
  Matrix4d T4 = getT(joint.j4,d4_6,0,90 / 180.0 * PI);
  Matrix4d T5 = getT(joint.j5,0,0,-90 / 180.0 * PI);
  Matrix4d T6 = getT(joint.j6,0,0,0);
  Matrix4d Ts = rotZ(180 / 180.0 * PI);
  return T1 * T2 * T3 * T4 * T5 * T6 * Ts * TCF_6Joint;
}

//求每个关节的转换矩阵，参数分别是a,α,d,Θ (单位弧度) Standard DH法
 Matrix4d RobotArgorithmn::getT(double o,double d,double a,double i)
 {
     Matrix4d m = Matrix4d::Zero();
     m << cos(o), -sin(o) * cos(i), sin(o) * sin(i), a * cos(o),
          sin(o), cos(o) * cos(i), -cos(o) * sin(i), a * sin(o),
            0,          sin(i),         cos(i),         d,
            0,            0,              0,            1;
     return m;
 }

