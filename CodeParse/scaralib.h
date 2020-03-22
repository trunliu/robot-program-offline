/*
 * scaralib.h
 *
 *  Created on: 2016年11月21日
 *      Author: yin
 */

#ifndef SCARALIB_H_
#define SCARALIB_H_
#include "Eigen/Dense"
using namespace Eigen;
using namespace std;
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <QDebug>
#define ZERO_THRESH 	(0.000001)
#define PI 				M_PI



//==========================================关节结构体
typedef struct
{
    float j1; float j2; float j3;
    float j4; float j5; float j6;

	bool ISOK;

	//打印关节角度，单位度，3轴升降单位为mm
    void printJointDegrees(void)
    {
		printf("Joint[%5.2f,%5.2f,%5.2f,%5.2f,%5.2f,%5.2f]\n",
                j1*180.0/PI, j2*180.0/PI, j3,
                j4*180.0/PI, j5*180.0/PI, j6*180.0/PI);
	}

	//打印关节角度，单位弧度，3轴升降单位为mm
    void printJointRadians(void) const
    {
        qDebug() << j1 << "  " <<  j2 << "  " << j3 << "  " << j4 << "  " << j5 << "  " <<j6;

	}

}Joint;

//==========================================变位机构结构体
typedef struct
{
	float c1; float c2;

	//打印变位机构旋转角度，单位度
	void printCointDegrees(void){
        printf("Coint[%5.2f,%5.2f]\n",c1*180.0/PI, c2*180.0/PI);
	}

	//打印变位机构旋转角度，单位度
    void printCointRadians(void) const{
		printf("Coint[%5.2f,%5.2f]\n",c1, c2);
	}

}Coint;


typedef struct
{
	Vector3d Pc;		//圆心
	double R;		//半径

	//pu,pv为圆所在平面上相互垂直的任意两单位向量
	Vector3d pu;
	Vector3d pv;

	double alfa;		//弧AB对应的圆心角
	double theta;	//弧AC对应的圆心角

}Circle;


typedef struct
{
	Matrix4d m;
	Joint j;
	Coint c;
}MJCoint;


extern Coint targetC, currentC;


//具体定义参见《SCARA机器人DH模型》
//==========================关节机械手尺寸定义
extern float a2, a3, d3, dh;
//==========================变位机构尺寸定义
extern float ch_x0, ch_y0, ch_z0, ch_d1, ch_d2;
////变位机构上测试点)
extern float ch_a3, ch_d3;

//==========================关节机械手限制角度
extern float JA_min[6], JA_max[6];

//==========================变位机构限制角度
extern float CA_min[2], CA_max[2];

//==========================焊枪变换矩阵
extern Matrix4d TCF;

//==========================每1mm离散点个数，默认10，即轨迹精度为0.1mm
extern float RESOLUTION;
//==========================姿态1°离散点个数，默认10，即姿态精度为0.1°
extern float RESOLUTION_ATT;


////==========================关节机械手尺寸定义
////具体定义参见《SCARA机器人DH模型》
//float a2 = 700.000;
//float a3 = 600.000;
//float d3 = 703.200;
////float dh = 233.924;
//float dh = 160.000;
//
////==========================变位机构尺寸定义
////具体定义参见《变位机构DH模型》
//float ch_x0 =  700.00;		//变位机构摆放
//float ch_y0 = -600.00;		//变位机构摆放
//float ch_z0 =    0.00;
//float ch_d1 =  150.00;
//float ch_d2 =  100.00;		//小变位机构
////变位机构上测试点
//float ch_a3 =  100.00;
//float ch_d3 =   40.00;
//
////==========================关节机械手限制角度
//float JA_min[6] = {-170.0/180*pi, -170.0/180*pi, -500, -170.0/180*pi, -90.0/180*pi, -170.0/180*pi};
//float JA_max[6] = { 170.0/180*pi,  170.0/180*pi,   10,  170.0/180*pi,  90.0/180*pi,  170.0/180*pi};
//
////==========================变位机构限制角度
//float CA_min[2] = {-60.0/180*pi, -170.0/180*pi};
//float CA_max[2] = { 60.0/180*pi,  170.0/180*pi};



//===========================坐标式器人正逆解算法
Matrix4d coo_fk(Joint j);
Joint coo_ik(Matrix4d matrix);

//===========================关节式器人正逆解算法
Matrix4d fk(Joint j);
Joint ik(Matrix4d matrix, bool choose_j2);
Joint NewPositionJoints(Matrix4d matrix);

Matrix4d ch_Ofk(Coint c);
Matrix4d ch_iik(Matrix4d matrix, Coint c);


//位姿矩阵转化为==>xyz坐标和欧拉角
ArrayXd pose_2_xyzrpw(Matrix4d H);
//xyz坐标和欧拉角转换为==>位姿矩阵
Matrix4d xyzrpw_2_pose(ArrayXd xyzrpw);


//通过标定6点获取TCF
Matrix4d getTCFMatrix(Matrix4d Moe1, Matrix4d Moe2, Matrix4d Moe3, Matrix4d Moe4, Matrix4d Moe5, Matrix4d Moe6);
//通过标定6点获取变位机构
Matrix4d getCHPosAttitude(Matrix4d Moe1, Matrix4d Moe2, Matrix4d Moe3, Matrix4d Moe4, Matrix4d Moe5, Matrix4d Moe6);


Vector3d PositionLinearInterpolation(Vector3d p1, Vector3d p2, double t);
Quaterniond Lerp(Quaterniond qa, Quaterniond qb, double t);
Circle getCircleInfo(Vector3d p1, Vector3d p2, Vector3d p3);
Vector3d Position3DInterpolation(Vector3d p1, Vector3d p2, Vector3d p3, double t);
Quaterniond Slerp(Quaterniond qa, Quaterniond qb, double t);
Quaterniond pose_2_quaternion(Matrix4d m);
Matrix4d quaternion_2_pose(Quaterniond qin);

//变位机构在配合机械手臂圆弧示教中，根据弧AB、弧AC对应圆心角比例，序列化C1、C2、C3
list<Coint> InterpolationOf3C(Coint C1, Coint C2, Coint C3, double a, double b, int step);
//输入A,B两点位姿，得到AB两点间直线的位姿序列
list<MJCoint> PositionAttitudeLinearInterpolation(Matrix4d A, Matrix4d B);
//输入A,B,C三点位姿，得到经过ABC三点圆弧的位姿序列
list<MJCoint> PositionAttitude3DInterpolation(Matrix4d A, Matrix4d B, Matrix4d C);
//输入A,B两点位姿以及在A,B两点处变位机构角度，得到AB两点间直线的位姿序列和变位机构对应的角度序列
list<MJCoint> RobotAndCHPositionAttitudeLinearInterpolation(Coint C1, Coint C2, Matrix4d A, Matrix4d B);
//输入A,B,C三点位姿以及在A,B,C三点处变位机构角度，得到经过ABC三点圆弧的位姿序列和变位机构对应的角度序列
list<MJCoint> RobotAndCHPositionAttitude3DInterpolation(Coint C1, Coint C2, Coint C3, Matrix4d A, Matrix4d B, Matrix4d C);
//输入A,B,C三点位姿，得到有ABC三点确定的贝塞尔曲线位姿序列
list<MJCoint> PositionAttitudeBezierInterpolation(Matrix4d A, Matrix4d B, Matrix4d C);



#endif /* SCARALIB_H_ */
