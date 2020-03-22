#ifndef NEWSCARAARGORITHMN_H
#define NEWSCARAARGORITHMN_H
#include "scaralib.h"
class NewScaraArgorithmn
{
public:
   static Matrix4d  coordTCFmatrix;
   static Matrix4d  lanxun5JointTCFmatrix;
   static Matrix4d  lanxun6JointTCFmatrix;
static   void initTCF();
// 正解： 包含 焊枪标定值
static Matrix4d fksolution(Joint j);
// 逆解： 包含 焊枪标定值
static Joint NewPositionJointssolution(Matrix4d Matrix);
};


#endif // NEWSCARAARGORITHMN_H
