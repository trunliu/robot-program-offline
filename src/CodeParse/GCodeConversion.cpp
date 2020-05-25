#include "GCodeConversion.h"
#include "CommonFun/CommonFun.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include "math.h"
#include <QVector2D>
#include <QDir>
#include "ArcParse.h"
GCodeConversion::GCodeConversion()
{
  converedContent.clear();
  currentCode.clear();
  IsCam = false;
}

//读取文件内容到content数组中
void GCodeConversion::readFile(const QString &fname)
{
    if(!CommonFun::readTextFile(fname,content))
        throw ERR_sfileopenfalse;             //读取失败就抛出异常
}

void GCodeConversion::preProcess()
{
    QString line;
    //遍历字符串数组
    for(int i = content.size() - 1;i >=0;i--)
    {
        //去掉前后的空格
        line = content[i].trimmed();
        //转换为大写
        line = line.toUpper();
        //去掉空行
        if(line.length() < 1)
            content.remove(i);
        else
            content[i] = line;
    }
    //若字符串数组为空，抛出空文件异常
    if(content.size() < 1)
        throw ERR_fileempty;
}

void GCodeConversion::confirmCoord()
{
    isAbsolute = false; //缺省是相对坐标
    int relative_num = 0;
    int absolute_num = 0;

    //遍历content数组
    for(int i=0;i < content.size();i++)
    {
        if(content[i] == "G91")
            relative_num++;
        else if(content[i] == "G90")
            absolute_num++;
    }

    //相对坐标G91,绝对坐标G90不能同时存在!   如果相对绝对同时存在  则抛出异常
    if(absolute_num > 0 && relative_num > 0)
        throw ERR_G9091;

    //如果是绝对的
    if(absolute_num > 0)
        isAbsolute = true;
}

//解析浮点数值,tbd 判断是否为合法浮点数,根据word返回pre和数值
bool GCodeConversion::ParserDouble(const QString &word,wchar_t &pre,double &val)
{
    if(word.length() < 2)
    {
        return false;
    }
    string word1 = word.toStdString();//不知道这种转换可以不，语法可以通过
    pre = word1[0];
    QString tmp = word.right(word.length() - 1);//
    val = tmp.toDouble();
    return true;
}

//得到命令类型
 bool GCodeConversion::getCode(const QString &cmd,CodeTypeEnum &code)
 {
    if(cmd=="G99")//部件选项
        code=G99_CODE;
    else if(cmd=="G91")//相对坐标
        code=RELATIVE_CODE;                 //相对
    else if(cmd=="G90")//绝对坐标
        code=ABSOLUTE_CODE;
//    else if(cmd=="G92")//起点坐标
//        code=START_POINT_CODE;
    else if(cmd=="G00" || cmd=="G0") //空移线
         code = MOVE_LINE;
    else if(cmd=="G01" || cmd=="G1") //插补线
        code = INTERPOLATION_LINE;
    else if((cmd=="G02") || cmd=="G2")//顺时针圆弧
        code=CW_ARC_CODE;
    else if(cmd=="G20")//英制
        code=INCH_CODE;
    else if(cmd=="G21")//米制
        code=METER_CODE;
    else if(cmd=="G03" || cmd=="G3")//逆时针圆弧
        code=ACW_ARC_CODE;
    else if(cmd=="M02")
        code=FINISH_CODE;
    else if(cmd=="G28")
           code=NO_USE_CODE;
    else if(cmd=="M07" || cmd=="M17")
    {
       if(!IsCam)
       code=FIRE_ON_CODE;
       else
        code=NO_USE_CODE;
    }
    else if(cmd=="M08" || cmd=="M18")
    {
        if(!IsCam)
         code=FIRE_OFF_CODE;
        else
         code=NO_USE_CODE;
    }
    else{//处理其他情况
        code=NO_USE_CODE;
        string cmd1=cmd.toStdString();//转化成std：string 临时变量去接
        switch(cmd1[0])
        {
        case '(':
        case '%':    IsCam = true;   //根据代码去判断有没有问题  这些东西都可以忽略 其他情况无法处理
        case 'O':    IsCam = true;
        case 'G':
        case 'M':
        case 'T':
        return true;
        }
        //不是注释，处理不了
        return false;
    }
    return true;
}

 //解析一行直线，获得各参数数据，并将结果写入converedContent数组中
 bool GCodeConversion::ParserLine(const QVector<QString> &words)
 {
     wchar_t pre;
     double val;
     for(int i = 1; i < words.length(); i ++)
     {
         if(!ParserDouble(words[i],pre,val))
             return false;
         switch (pre)
         {
             case 'X':  point.x += val;  break;
             case 'Y':  point.y += val;  break;
             case 'Z':  point.z += val;  break;
             case 'R':  point.r =  val;  break;
             case 'P':  point.p =  val;  break;
             case 'W':  point.w =  val;  break;
             case 'U':  point.u =  val;  break;
             case 'V':  point.v =  val;  break;
             case 'S':  point.s =  val;  break;
             case 'F':  point.s =  val;  break;
             default:
                 break;
         }
     }

     //并将结果写入converedContent字符串数组中
     writeConveredGcodes(words[0]);
     return true;
 }

 //解析一行弧
 bool GCodeConversion::ParserArc(const QVector<QString> &words,const CodeTypeEnum &code)
 {
     //姿态默认无变化
     bool isTurn = false;

     if(words.size() > 6)
         return false;
     double x = 0,y = 0,i = 0,j = 0;//参数省略，默认为0
     int xnum = 0,ynum = 0,inum = 0,jnum = 0;//每个参数出现的数目
     wchar_t pre;
     double val;
     for(int index=1;index < words.size();index++)
     {
         if(!ParserDouble(words[index],pre,val)) return false;
         switch(pre)
         {
         case 'X':
             x=val;
             xnum++;
             break;
         case 'Y':
             y=val;
             ynum++;
             break;
         case 'I':
             i=val;
             inum++;
             break;
         case 'J':
             j=val;
             jnum++;
             break;
         case 'S':  point.s =  val;  break;
         case 'F':  point.s =  val;  break;
         case 'T':  isTurn = true;   break;
         default://不能识别的，退出
             {

             }
         }
     }

     //判断错误情况，不能出现存在重复参数
     if(xnum>1 || ynum>1 || inum>1 || jnum>1)
         return false;

     //默认为顺圆
     bool bCW=true;
     if(code==ACW_ARC_CODE)   //逆圆
         bCW=false;

     tansvertArcNew(x,y,i,j,bCW,isTurn);
     return true;

 }

 void GCodeConversion::tansvertArcNew(double x1,double y1,double i,double j,bool IsCw,bool isTurn)
 {
     bool IsCircle = false;     //默认不是整圆
     bool IsMidCircle = false;  //默认不是半圆
     double w;

     //画弧，起点与终点重合判断是
     if(x1 == 0 && y1 == 0)
     {
         IsCircle = true;
         qDebug() << "整圆";
     }

     //上一个点得终点为起点 绝对坐标
     double ax0 = point.x;
     double ay0 = point.y;
     //圆心
     double ai =  point.x + i;
     double aj =  point.y + j;
     //末点
     double ax1 = point.x + x1;
     double ay1 = point.y + y1;
     //起点向量、末点向量
     QVector2D v0(ax0 - ai,ay0 - aj);
     QVector2D v1(ax1 - ai,ay1 - aj);
     //半径
     double  r = v0.length();

     //若不是整圆
     if(!IsCircle)
     {
        //判断是不是半圆，两向量共线/平行/线性相关
        if((v0.x() * v1.y() )- (v0.y() * v1.x()) == 0)
        {
            qDebug() << "半圆";
            IsMidCircle = true;
        }
     }

     //即不是整圆也不是半圆的情况
     if(!IsCircle && !IsMidCircle)
     {
     //标准化为单位向量
     v0.normalize();
     v1.normalize();

     //建立一个位于起点向量和终点向量之间得中间向量v，长度为r
     QVector2D v = (v0 + v1).normalized() * r;
     //中间向量v的终点坐标绝对坐标
     double x = v.x() + ai;
     double y = v.y() + aj;

     //1、已知圆弧的起点、终点、圆心以及圆弧方向，判断圆弧是优弧还是劣弧
     //2、优弧：大于半圆的弧
     //  a·b=|a||b|cosθ  |a×b|=|a||b|sinθ
     int n = (x-ax0)*(ay1-y)-(y-ay0)*(ax1-x);

     //说明v的终点是在起点到终点逆时针路线上
     if(n > 0 && IsCw){
        //若此时弧线是顺时针的，则说明v方向反了
        v = -v;
        x = v.x() + ai;
        y = v.y() + aj;
     }

     if(n < 0 && !IsCw){
        v = -v;
        x = v.x() + ai;
        y = v.y() + aj;
     }

     QString line;

    //起点，中间点，终点 在空间中是否共线，如果共线说明是直线
    if(CommonFun::isThreePointInLine(point.x,point.y,point.z,x,y,point.z,ax1,ay1,point.z))
    {
        point.x = ax1;
        point.y = ay1;
        qDebug() << "圆弧三点共线";   //共线问题我来解决
        line = QString("G01 X%4 Y%5 Z%6 R%7 P%8 W%9 U%10 V%11 S%12\n")
       .arg(point.x).arg(point.y).arg(point.z).arg(point.r).arg(point.p).arg(point.w).arg(point.u).arg(point.v).arg(point.s);

    //不共线时
    }else{
        point.x = ax1;
        point.y = ay1;

        //如果姿态发生变化，A轴跟着转动
        if(isTurn)
        {
            //求夹角，(oa.ob)/|oa|*|ob|=cos(θ) acos(θ)是弧度 还需再*180/Pi
            double OAOB = (ax0 - ai) * (x - ai) + (ay0 - aj) * (y- aj);
            double OBOC = (x - ai) * (ax1 - ai) + (y - aj) * (ay1 - aj);
            double angle1 = acos(OAOB / (r * r));
            double angle2 = acos(OBOC / (r * r));
            double angle = (angle1 + angle2) * 180 / Pi; 
            qDebug() << "圆心角：" << angle << endl;

            //逆正顺反
            if(IsCw)
               angle = -angle;

            //中间点v向量的姿态，当画弧到v向量终点时，让A轴刚好转动半角
            w = point.w + angle / 2.0;

            //终点姿态
            point.w = point.w + angle;

            //ABC为中间向量的坐标，DEF为中间向量的姿态，XYZ终点坐标，RPW终点姿态
            line = QString("G04 A%1 B%2 C%3 D%4 E%5 F%6 X%7 Y%8 Z%9 R%10 P%11 W%12 U%13 V%14 S%15\n").arg(x).arg(y).arg(point.z)
         .arg(point.r).arg(point.p).arg(w).arg(point.x).arg(point.y).arg(point.z).arg(point.r).arg(point.p).arg(point.w).arg(point.u).arg(point.v).arg(point.s);
        }else{
       //如果画圆弧过程中姿态不变化，A轴不转动
       line = QString("G04 A%1 B%2 C%3 D%4 E%5 F%6 X%7 Y%8 Z%9 R%10 P%11 W%12 U%13 V%14 S%15\n").arg(x).arg(y).arg(point.z)
    .arg(point.r).arg(point.p).arg(point.w).arg(point.x).arg(point.y).arg(point.z).arg(point.r).arg(point.p).arg(point.w).arg(point.u).arg(point.v).arg(point.s);
       }
   }

    converedContent.push_back(line);
    CommonFun::listNum.append(curNum);
 }
    else
    {
        if(IsCircle)
        {
           ax1 = 2 * ai - ax1;
           ay1 = 2 * aj - ay1;
        }
           double xx1 = ai + r * (ay1 - ay0) / (sqrt((ay1 - ay0) * (ay1 - ay0) + (ax0 - ax1) * (ax0 - ax1)));
           double yy1 = aj + ((ax0 - ax1) * (xx1 - ai) / (ay1 - ay0));

           double xx2 = ai -     r * (ay1 - ay0) / (sqrt((ay1 - ay0) * (ay1 - ay0) + (ax0 - ax1) * (ax0 - ax1)));
           double yy2 = aj + ((ax0 - ax1) * (xx2 - ai) / (ay1 - ay0));

           int n = (xx1-ax0)*(ay1-yy1)-(yy1-ay0)*(ax1-xx1);
           double xx = 0,yy = 0;
           if(n > 0)
           {
               if(IsCw) //则不是需要的点
               {
                  xx = xx1;
                  yy = yy1;
                  xx1 = xx2;
                  yy1 = yy2;

               }
               else
               {
                   xx = xx2;
                   yy = yy2;
               }
           }
           if(n < 0) //顺时针
           {
               if(!IsCw) //则不是需要的点
               {
                   xx = xx1;
                   yy = yy1;
                   xx1 = xx2;
                   yy1 = yy2;
               }
               else
               {
                   xx = xx2;
                   yy = yy2;
               }
           }
           QString line;
           if(CommonFun::isThreePointInLine(point.x,point.y,point.z,xx1,yy1,point.z,ax1,ay1,point.z))
           {
               point.x = ax1;
               point.y = ay1;
               qDebug() << "圆弧三点共线";   //共线问题我来解决
               line = QString("G01 X%4 Y%5 Z%6 R%7 P%8 W%9 U%10 V%11 S%12\n")
              .arg(point.x).arg(point.y).arg(point.z).arg(point.r).arg(point.p).arg(point.w).arg(point.u).arg(point.v).arg(point.s);
                converedContent.push_back(line);
               CommonFun::listNum.append(curNum);
           }
           else
          {
             point.x = ax1;
             point.y = ay1;
             if(isTurn)
             {
                 double OAOB = (ax0 - ai) * (xx1 - ai) + (ay0 - aj) * (yy1- aj);
                 double OBOC = (xx1 - ai) * (ax1 - ai) + (yy1 - aj) * (ay1 - aj);
                 double angle1 = acos(OAOB / (r * r));
                 double angle2 = acos(OBOC / (r * r));
                 double angle = (angle1 + angle2) * 180 / Pi;
                 qDebug() << "圆心角：" << angle << endl;
                if(IsCw)  //顺时针
                angle = -angle;
                w = point.w + angle / 2.0;
                point.w = point.w + angle;
                line = QString("G04 A%1 B%2 C%3 D%4 E%5 F%6 X%7 Y%8 Z%9 R%10 P%11 W%12 U%13 V%14 S%15\n").arg(xx1).arg(yy1).arg(point.z)
              .arg(point.r).arg(point.p).arg(w).arg(point.x).arg(point.y).arg(point.z).arg(point.r).arg(point.p).arg(point.w).arg(point.u).arg(point.v).arg(point.s);
             }
            else
             line = QString("G04 A%1 B%2 C%3 D%4 E%5 F%6 X%7 Y%8 Z%9 R%10 P%11 W%12 U%13 V%14 S%15\n").arg(xx1).arg(yy1).arg(point.z)
           .arg(point.r).arg(point.p).arg(point.w).arg(point.x).arg(point.y).arg(point.z).arg(point.r).arg(point.p).arg(point.w).arg(point.u).arg(point.v).arg(point.s);
             converedContent.push_back(line);
              CommonFun::listNum.append(curNum);
             if(IsCircle)
             {
                 point.x = ax0;
                 point.y = ay0;
                 if(isTurn)
                 {
                     double OAOB = (ax0 - ai) * (xx - ai) + (ay0 - aj) * (yy- aj);
                     double OBOC = (xx - ai) * (ax1 - ai) + (yy - aj) * (ay1 - aj);
                     double angle1 = acos(OAOB / (r * r));
                     double angle2 = acos(OBOC / (r * r));
                     double angle = (angle1 + angle2) * 180 / Pi;
                     qDebug() << "圆心角：" << angle;
                    if(IsCw)  //顺时针
                    angle = -angle;
                    w = point.w + angle / 2.0;
                    point.w = point.w + angle;
                    line = QString("G04 A%1 B%2 C%3 D%4 E%5 F%6 X%7 Y%8 Z%9 R%10 P%11 W%12 U%13 V%14 S%15\n").arg(xx).arg(yy).arg(point.z)
                  .arg(point.r).arg(point.p).arg(w).arg(point.x).arg(point.y).arg(point.z).arg(point.r).arg(point.p).arg(point.w).arg(point.u).arg(point.v).arg(point.s);
                 }
                else
                 line = QString("G04 A%1 B%2 C%3 D%4 E%5 F%6 X%7 Y%8 Z%9 R%10 P%11 W%12 U%13 V%14 S%15\n").arg(xx).arg(yy).arg(point.z)
               .arg(point.r).arg(point.p).arg(point.w).arg(point.x).arg(point.y).arg(point.z).arg(point.r).arg(point.p).arg(point.w).arg(point.u).arg(point.v).arg(point.s);
                converedContent.push_back(line);
                 CommonFun::listNum.append(curNum);
             }
          }

    }
 }


/*
void GCodeConversion::tansvertArc(double x1,double y1,double i,double j,bool IsCw)
{

     bool IsCircle = false; //默认不是整圆
     bool IsMidCircle = false;  //默认不是半圆
     double ax0 = point.x; //起点
     double ay0 = point.y;


     double ai =  point.x + i;  //圆心
     double aj =  point.y + j;
     double ax1 = point.x + x1;  //末点
     double ay1 = point.y + y1;

     QVector2D op0(ax0 - ai,ay0 - aj);  //起点向量
     QVector2D op1(ax1 - ai,ay1 - aj);  //末点向量
     double r = op0.length();  //半径
     if(x1 == 0 && y1 == 0)
     {
         IsCircle = true;
         qDebug() << "整圆";
     }
     if(!IsCircle)   //不是整圆
     {
        if((op0.x() * op1.y() )- (op0.y() * op1.x()) == 0)  //向量共线
        {
            qDebug() << "半圆";
            IsMidCircle = true;
        }
     }

       if(!IsCircle && !IsMidCircle)  //不是整圆的情况
       {
         QVector2D op = op0.normalized() + op1.normalized();  //单位画
         op.normalize(); //单位矩阵
         QVector2D opr;   //中点坐标
         double angle0 = getAngle(ax0 - ai,ay0 - aj);
         double angle1 = getAngle(ax1 - ai,ay1 - aj);
         double angle = angle1 - angle0;
         if(angle0 != 0 && angle1 != 0)   //没有一个点在0度位置上
         {
             if(IsCw)  //顺时针
             {
                 if(angle >= 0) //说明在线上
                  opr = op * r;
                 else
                  opr = op * (-r);
             }
             else       //逆时针
             {
                 if(angle >= 0)  //在线外
                   opr = op * (-r);
                 else
                   opr = op * r;  //在线内
             }


         }
         else
           {
              if(angle0 == 0)   //起点在0度位置
              {
                 if(ax1 - ai > 0)
                 {
                      if(IsCw)
                      {
                           opr = op * r;
                      }
                      else
                      {
                           opr = op * -r;
                      }
                 }
                 else
                 {
                     if(IsCw)
                     {
                          opr = op * -r;
                     }
                     else
                     {
                          opr = op * r;
                     }

                 }

              }
              if(angle1 == 0)
              {

                  if(ax0 -ai > 0)
                  {
                      if(IsCw)
                      {
                           opr = op * -r;
                      }
                      else
                      {
                           opr = op * r;
                      }

                  }
                  else
                  {
                      if(IsCw)
                      {
                           opr = op * r;
                      }
                      else
                      {
                           opr = op * -r;
                      }

                  }

              }


           }
         double midX = opr.x() + ai;
         double midY = opr.y() + aj;
          QString line;

         if(CommonFun::isThreePointInLine(point.x,point.y,point.z,midX,midY,point.z,ax1,ay1,point.z))
         {
             point.x = ax1;
             point.y = ay1;
             qDebug() << "圆弧三点共线";   //共线问题我来解决
             line = QString("G01 X%4 Y%5 Z%6 R%7 P%8 W%9 U%10 V%11 S%12\n")
            .arg(point.x).arg(point.y).arg(point.z).arg(point.r).arg(point.p).arg(point.w).arg(point.u).arg(point.v).arg(point.s);
         }
         else
        {
           point.x = ax1;
           point.y = ay1;
           line = QString("G04 A%1 B%2 C%3 D%4 E%5 F%6 X%7 Y%8 Z%9 R%10 P%11 W%12 U%13 V%14 S%15\n").arg(midX).arg(midY).arg(point.z)
         .arg(point.r).arg(point.p).arg(point.w).arg(point.x).arg(point.y).arg(point.z).arg(point.r).arg(point.p).arg(point.w).arg(point.u).arg(point.v).arg(point.s);
        }
         converedContent.push_back(line);
          CommonFun::listNum.append(curNum);
     }

     else
       {
           double xm = (-op0).x() + ai;   //zhong
           double ym = (-op0).y() + aj;
           double xx1 = ax0 - ai;
           double yy1 = ay0 - aj;

           double xx2 = xm - ai;
           double yy2 = ym - aj;

           double xx3,yy3;
           QVector2D op3;
          if(yy1 > 0) //一三象限
           {
              double y = - op0.x() / op0.y();
              QVector2D ov(1,y);
              op3 = ov.normalized() * r;
              if(!IsCw) //逆圆
              op3 = -op3;
              xx3 = op3.x() + ai;
              yy3 = op3.y() + aj;

           }
           else if(yy1 < 0)
          {
              double y = op0.x() / op0.y();
              QVector2D ov(-1,y);
              op3 = ov.normalized() * r;
              if(!IsCw) //逆圆
              op3 = -op3;
              xx3 = op3.x() + ai;
              yy3 = op3.y() + aj;
          }

          else if(yy1 == 0) //x轴
          {
              if(xx1 > 0)
              {
                  if(IsCw)
                  {
                      xx3 = 0 + ai;
                      yy3 = aj - r;
                  }
                  else
                  {
                      xx3 = ai;
                      yy3 = aj + r;
                  }
              }

             else
              {
                  if(IsCw)
                  {
                      xx3 = ai;
                      yy3 = aj + r;
                  }
                  else
                  {
                      xx3 = 0 + ai;
                      yy3 = aj - r;
                  }
              }

          }
          point.x = ax1;
          point.y = ay1;
          QString line;
          if(IsMidCircle)
          line = QString("G04 A%1 B%2 C%3 D%4 E%5 F%6 X%7 Y%8 Z%9 R%10 P%11 W%12 U%13 V%14 S%15\n").arg(xx3).arg(yy3).arg(point.z).arg(point.r).arg(point.p).arg(point.w)
                  .arg(point.x).arg(point.y).arg(point.z).arg(point.r).arg(point.p).arg(point.w).arg(point.u).arg(point.v).arg(point.s);
          if(IsCircle)
          line = QString("G05 A%1 B%2 C%3 D%4 E%5 F%6 X%7 Y%8 Z%9 R%10 P%11 W%12 U%13 V%14 S%15\n").arg(xx3).arg(yy3).arg(point.z).arg(point.r).arg(point.p).arg(point.w)
                      .arg(xx2).arg(yy2).arg(point.z).arg(point.r).arg(point.p).arg(point.w).arg(point.u).arg(point.v).arg(point.s);
          converedContent.push_back(line);
           CommonFun::listNum.append(curNum);
       }


}
*/

/*
double  GCodeConversion::getAngle(double x,double y)
{
    double angle = 0;
    if(x > 0 && y > 0) //第一象限中的点
        angle = atan(x / y) * 180 / Pi;
    else if(x > 0 && y < 0) //第二象限中的点
        angle = (atan( -y / x) * 180 / Pi) + 90;
    else if(x < 0 && y < 0)
        angle = (atan( x / y) * 180 / Pi) + 180;
    else if(x < 0 && y > 0)
        angle = (atan(y / -x) * 180 / Pi) + 270;
    else if(x > 0 && y == 0)   //x+
        angle = 90;
    else if(x == 0 && y > 0)  //y+
        angle = 0;
    else if(x == 0 && y < 0)
        angle = 180;         //y-
    else if(x < 0 && y == 0)
        angle = 270;
    angle = CommonFun::round(angle,2);  //四舍勿入
    if(angle == 360)
        angle = 0;
    return angle;

}
*/

//将当前点写入到converedContent中
 void GCodeConversion::writeConveredGcodes(const QString &cmd)
 {
     //转换绝对路径得文本
     QString line = QString(" X%1 Y%2 Z%3 R%4 P%5 W%6 U%7 V%8 S%9\n").arg(point.x).arg(point.y).arg(point.z)
             .arg(point.r).arg(point.p).arg(point.w).arg(point.u).arg(point.v).arg(point.s);

     //判断是代码是空移还是直线
     if(cmd == "G0")
     line.push_front("G00");
     else if(cmd == "G1")
     line.push_front("G01");
     else
     line.push_front(cmd);

     converedContent.push_back(line);

     //当前行数加入到int数组
     CommonFun::listNum.append(curNum);
 }

 //解析主入口,错误显示tbd
 void GCodeConversion::doParse()
 {

     CommonFun::listNum.clear();
     curNum = 0;

     //添加一个起点，坐标姿态速度都是0
     writeConveredGcodes("G00");

     QString msg;     //错误信息
     for(int i = 0;i < content.size();i++)
     {
         if(!ParserSentence(content[i]))
         {
             //QString::number(int num,int base=10),将int型num转换为十进制string
             msg = "第" + QString::number(i+1,10) + "行有错误";

             //add  加入错误信息
             errs.push_back(msg);
         }
         //表示相对到第几行
         ++curNum;
     }
 }

 //解析一行的数据
 bool GCodeConversion::ParserSentence(const QString &sentence)
 {
     //根据空格分割一行，放入words容器中，返回words容器的大小
     QVector<QString> words;
     int num = CommonFun::splitString(sentence,' ',words);

     //words容器的大小小于1，说明分割不出来
     if(num < 1)
         return false;

     //首先过滤N代码(20110623)，若为n代码，删除第一个words[0]
     if(words[0].at(0)== 'N' ||  words[0].at(0) == 'n')
     {
         words.remove(0);
     }

     /***********黄石专用***************/
     if(words.size() == 1 && words[0].left(3) == "G92")  //做一个标志的判断位来判断  //移除G92这条数据点
          words.remove(0);

     CodeTypeEnum code;   //代码类型
     if(words.size() == 0)
     {//由于过滤N代码
         code = NO_USE_CODE;
         return false;
     }

     //若没写指令类型，则默认还是上条指令类型，头插words
     if(words[0].at(0) == 'X' || words[0].at(0) == 'Y'|| words[0].at(0) == 'Z'|| words[0].at(0) == 'R'|| words[0].at(0) == 'P'|| words[0].at(0) == 'W'
             || words[0].at(0) == 'U'|| words[0].at(0) == 'V')
     {
         if(!currentCode.isEmpty())
         words.push_front(currentCode);

     }

     //必须获取指令类型，否则无法解析
     if(!getCode(words[0],code))
         return false;

     //判断是不是cam下来的 如果是cam下来的 ,空移线
     if(IsCam){

         //如果上条指令还是开火线，当前指令为空移线，就先添加一条关火指令
         if(isFireLine){
             if(words[0] == "G0" || words[0] == "G00"){
                 isFireLine = false;
                 converedContent +="M08 V1\n";
                 CommonFun::listNum.append(curNum);
             }
         }else{
             //如果上条指令还是关火线，当前指令为开火，添加一条开火指令
             if(words[0] == "G1" || words[0] == "G01" || words[0] == "G2" || words[0] == "G02"
                     || words[0] == "G3" || words[0] == "G03")
             {
                 if(!(words[1] == "G91" || words[1].at(0) == 'Z'))
                 {
                     isFireLine = true;
                     converedContent +="M07 V1\n";
                     CommonFun::listNum.append(curNum);
                 }
                 else
                 {
                    words[0] = "G00";
                 }
             }
         }
     }

     //保存当前指令类型，除了开火点火命令
     if(words[0] != "M07" && words[0] != "M08")
        currentCode = words[0];

     switch(code)
     {
     case FIRE_ON_CODE:   //如果没有V就加上v来处理或单独做两个函数
         converedContent += sentence;
         if(words.size() == 1)
         converedContent +=" V1";
         converedContent +="\n";
         CommonFun::listNum.append(curNum);
         break;
     case FIRE_OFF_CODE:
         converedContent += sentence;
         if(words.size() == 1)
         converedContent +=" V1";
         converedContent +="\n";
         CommonFun::listNum.append(curNum);
         break;
     case MOVE_LINE:             //空移线
     case INTERPOLATION_LINE:    //插补线
         if(!ParserLine(words))  //解析一行
             return false;
         break;
     case CW_ARC_CODE:           //解析圆弧
     case ACW_ARC_CODE:
         if(!ParserArc(words,code))
             return false;
         break;
     default://全部缺省处理
         break;
     }
    return true;
 }

 //运行主函数
 bool GCodeConversion::run(const QString &fname)
 {
     try
     {
         converedContent.clear();        //转换的内容
         content.clear();
         errs.clear();
         currentCode.clear();             //目前的G代码清空
         IsCam = false;                   //不是cam生成的
         isFireLine = false;               //默认是不开火的
         readFile(fname);                 //读取文件内存到content
         preProcess();                    //预处理  去掉空格和空行
         confirmCoord();                  //验证相对还是绝对的
         if(isAbsolute)                    //cad生成的是相对的，一般不会执行这条  如果是绝对的就不做处理
             return true;
         doParse();                       //开始做解析
         if(errs.size() > 0)
             return false;
         return true;
     }
     catch(...)   //如果捕获到异常就只是处理就是return false
     {
         return false;
     }
 }

 //错误信息显示
 void GCodeConversion::dspMsg(const QString &preMsg,QWidget*wid)
 {
     QString msg;
      msg = preMsg;
     for(int i=0;i < errs.size();i++)
     {
         msg += errs[i];
         msg += "\n";
     }
     CommonFun::msgErr(msg,wid);
 }

//先以五关节式的来做处理
 int GCodeConversion::CreateConvertedFile(const QString& filename)
 {
     //得到起点数据 也就是回零后的数据
     point.x = Parameter::startPosture.x;
     point.y = Parameter::startPosture.y;
     point.z = Parameter::startPosture.z;
     point.r = Parameter::startPosture.r;
     point.p = Parameter::startPosture.p;
     point.w = Parameter::startPosture.w;
     point.u = Parameter::startPosture.u;
     point.v = Parameter::startPosture.v;
     point.s = Parameter::startPosture.s;

     //这个才是解析的入口
     if(run(filename))
     {
         //绝对的不处理
         if(isAbsolute)
         {
             qDebug() << "绝对不处理并复制过来";
             //直接复制过来看能用不
             CommonFun::copyFileToPath(filename,"C:\\simulation.cnc",true);
             return 1;
         }

         //获得后缀，删除后缀，更改为_abs.cnc
         QString filename_rel;
         QString str = filename.right(4);
         QString filename1 = filename;
         if( str == ".txt" || str == ".cnc")
           filename_rel =  filename1.remove(str);
         QFile *file = new QFile();
         file->setFileName(filename_rel + "_abs.cnc");

         bool ok = file->open(QIODevice::WriteOnly | QIODevice::Truncate | QFile::Text);
         if (ok)
         {
             QTextStream out(file);
             converedContent.push_front("G90\n");  //转换成绝对的
             //输出转换后的内容
             for(int i = 0; i < converedContent.size();i++)
             out << converedContent[i];
             file->close();
             delete file;

             CommonFun::copyFileToPath(filename_rel + "_abs.cnc","C:\\simulation.cnc",true);
             return 0;
         }
         else
             return -1;
     }
     else
             return -2;
 }
