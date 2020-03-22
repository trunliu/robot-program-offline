#include "SimulationParse.h"
#include "CommonFun/CommonFun.h"
#include "scaralib.h"
#include <QDebug>

const  double moveJointInter = 6;
const  double moveLineinter = 5;
 SimulationParse::SimulationParse(RobotType type):OpenglWidget(type)
 {
     mode = ShowMode;
     NewScaraArgorithmn::initTCF();
     widgetname = SIMULATION;
     IsStop = false;
     isFireLine = false; //默认为false
     num = 0;
     trailPoints.clear();
     divideLength = 8;
     currentXyzrpw =  ArrayXd::Zero(6);
     home();
     for(int i = 0; i < 6;i++)
     currentXyzrpw[i] = coord[i];
     curC = {0,0};
     resize(800,700);
     setWindowIcon(QIcon(":images/drawable/run_1.png"));
     setWindowTitle("仿真");
     //开始时会清空
     lineIndex = 0;
     interIndex = -1;
     isFinished = false;
     isFireMode = false;
     for(int i = 0; i < TIMELIST;i++)
         timelist[i] = TIMELIST - i - 1;
     speedRate = CommonFun::ReadSettingsInt("Para/speedRate");
 }
 SimulationParse::~SimulationParse()
{
   CommonFun::WriteSettingsInt("Para/speedRate",speedRate);     //在析构时保存文件名到ini文件
}
void SimulationParse::closeEvent(QCloseEvent * e)
{
    IsStop = true; //关闭时能够从循环中跳出来
    return OpenglWidget::closeEvent(e);
}
void SimulationParse::readFile(const QString &fname)
{

    if(!CommonFun::readTextFile(fname,content))
        {
         qDebug() << "读取文件失败：" << fname;
         throw ERR_sfileopenfalse;
        }
}
//
void SimulationParse::moto_runJAbs(Joint j, Coint c,bool flag,double sp)
{
    double j1,j2,j3,j4,j5,j6,c1,c2;  //这个要走的角度
    //gl目前的角度和需要转动角度的差距
    if(glType == Lanxun5JointRobot)
    {
         j1 = j.j1 * 180 / PI - j1Rot;  //角度值
         j2 = j.j2 * 180 / PI - j2Rot;
    }
    else
    {
        j1 = j.j1 - j1Rot;
        j2 = j.j2 - j2Rot;
    }
    j3 = j.j3 - j3Rot;
    j4 = j.j4 * 180 / PI - j4Rot;
    j5 = j.j5 * 180 / PI - j5Rot;
    j6 = j.j6 * 180 / PI;

    c1 = c.c1 * 180 / PI - c1Rot;
    c2 = c.c2 * 180 / PI - c2Rot;
    double contia = sqrt(j1 * j1 + j2 * j2 + j3 * j3 + j4 * j4 + j5 * j5 + j6 * j6 + c1 * c1 + c2 * c2);
    if(contia == 0)
    {
        return;
    }
    double rate = 1.0 / contia * moveJointInter;
    for(double i = 0; i < contia; i+= moveJointInter) //每次转一度
    {
        j1Rot += j1 * rate;
        j2Rot += j2 * rate;
        j3Rot += j3 * rate;
        j4Rot += j4 * rate;
        j5Rot += j5 * rate;
        j6Rot += j6 * rate;
        c1Rot += c1 * rate;
        c2Rot += c2 * rate;
        ArrayXd xyzrpw = ArrayXd::Zero(6);
        Joint curJ;
        if(glType == Lanxun5JointRobot)
        {
             curJ.j1 = j1Rot * PI / 180.0;
             curJ.j2 = j2Rot * PI / 180.0;
        }
        else
        {
            curJ.j1 = j1Rot;
            curJ.j2 = j2Rot;
        }
        curJ.j3 = j3Rot;
        curJ.j4 = j4Rot * PI / 180;
        curJ.j5 = j5Rot * PI / 180;
        curJ.j6 = 0;
        xyzrpw = pose_2_xyzrpw(NewScaraArgorithmn::fksolution(curJ));
        for(int i = 0;i < 6;i++)
            coord[i] = CommonFun::round(xyzrpw[i],2);
       if(flag)
       addTrialPoints();
       else
       decTrialPoints();
       updateGL();
       sendJC();
       if(IsStop)
           return;
       CommonFun::sleep(timelist[speedRate / 10 -1]);
    }
    /***************为了保证精度，最后一个点要走到**************/

    if(glType == Lanxun5JointRobot)
    {
        j1Rot = j.j1 * 180 / PI;
        j2Rot = j.j2 * 180 / PI;
    }
    else
    {
        j1Rot = j.j1;
        j2Rot = j.j2;
    }
    j3Rot = j.j3;
    j4Rot = j.j4 * 180 / PI;
    j5Rot = j.j5 * 180 / PI;
    c1Rot = c.c1 * 180 / PI;
    c2Rot = c.c2 * 180 / PI;
    ArrayXd xyzrpw = ArrayXd::Zero(6);
    xyzrpw = pose_2_xyzrpw(NewScaraArgorithmn::fksolution(j));
    for(int i = 0;i < 6;i++)
        coord[i] = CommonFun::round(xyzrpw[i],2);
    if(flag)
    addTrialPoints();
    else
    decTrialPoints();
    updateGL();
}

//解析主入口,错误显示tbd
void SimulationParse::doParse(bool flag)   //true表示前进 flag表示后退
{
    QString msg; 
    if(flag)
    {
        if(isFinished )
        {
          isFinished = false;
          percent = 0;
          lineIndex = 0;
          updateGL();
        }
        for(int i = lineIndex;i < content.size();i++)
        {

            emit parseGrogress(i);
            CommonFun::sleep(4);
            if(i>0)
            {
                if(!ParserSentence(content[i],content[i-1]))
                {
                     msg = "第" + QString::number(i+1,10) + "行有错误";
                     qDebug() << msg;
                     errs.push_back(msg);//add
                }
                else
                {
                    if(IsStop)
                     {
                        lineIndex = i;
                        return;
                     }
                }

            }
            else
            {
                if(!ParserSentence(content[i],content[i]))
                {

                     msg = "第" + QString::number(i+1,10) + "行有错误";
                     qDebug() << msg;
                     errs.push_back(msg);//add
                }
                else
                {
                    if(IsStop)
                     {
                        lineIndex = i;
                        return;
                     }
                }

            }
         percent = (i + 1.0) / content.size() * 100;
        }
    emit runFinished();
    percent = 100;
    updateGL();
    QMessageBox::information(this,"提示","运行完成！");
    }
    else
    {
/****************************后退的代码***********************************/
        if(isFinished )
        {
          isFinished = false;
          lineIndex = content.size() -1;
          percent = 100;
        }
        for(int i = lineIndex;i > 0;i--)  //读取前面的一段
        {
            emit parseGrogress(i);
            CommonFun::sleep(4);
            if(!ParserSentenceback(content[i],content[i-1]))   //解析一段话
            {

                 msg = "第" + QString::number(i+1,10) + "行有错误";
                 qDebug() << msg;
                 errs.push_back(msg);//add
            }
            else
            {
                if(IsStop)
                 {
                    lineIndex = i;
                    return;
                 }
            }
             percent = (i + 1.0) / content.size() * 100;

        }
    }
    //表示整个循环已经结束了 对其初始化的过程
    isFinished = true;
}
//解析一行
bool SimulationParse::ParserSentence(const QString &sentence,const QString &sentence1)
{
    QVector<QString> words;
    int num = CommonFun::splitString(sentence,' ',words);
    if(num < 1)
        return false;
    QVector<QString> words1;
    num = CommonFun::splitString(sentence1,' ',words1);
    if(num < 1)
        return false;
    CodeTypeEnum code;
    if(!getCode(words[0],code))  //getCode 得到代码
        return false;
    switch(code)
    {
    case MOVE_LINE:      //直线类型
    case INTERPOLATION_LINE:
        if(!ParserLine(words,code,true))
            return false;
        break;
    case ARC_CODE:
    case CIRCLE_CODE:
        if(!ParserArc(words,words1,code))
            return false;
        break;
    default://全部缺省处理
        break;
    }
   return true;
}
//解析一行
bool SimulationParse::ParserSentenceback(const QString &sentence,const QString &sentence1)
{
    QVector<QString> words;
    int num = CommonFun::splitString(sentence,' ',words);
    if(num < 1)
        return false;
    QVector<QString> words1;
    num = CommonFun::splitString(sentence1,' ',words1);
    if(num < 1)
        return false;
    CodeTypeEnum code;
    if(!getCode(words[0],code))  //getCode 得到代码
        return false;
    switch(code)
    {
    case FIRE_ON_CODE:   //如果没有V就加上v来处理或单独做两个函数
    {
     isFireLine = true;
     break;
    }
    case FIRE_OFF_CODE:
    {
     isFireLine = false;
     break;
    }
    case MOVE_LINE:      //直线类型
    case INTERPOLATION_LINE:
        if(!ParserLine(words1,code,false))
            return false;
        break;
    case ARC_CODE:
    case CIRCLE_CODE:
        if(!ParserArcBack(words,words1,code))
            return false;
        break;
    default://全部缺省处理
        break;
    }
   return true;
}
//得到命令的代码
 bool SimulationParse::getCode(const QString &cmd,CodeTypeEnum &code)//cstring 改string
 {
    if(cmd=="G90")//绝对坐标
        code = ABSOLUTE_CODE;
    else if(cmd == "G00" || cmd == "G0")//直线切割
        code = MOVE_LINE;
     else if(cmd == "G01"|| cmd == "G1")//直线切割
         code = INTERPOLATION_LINE;
    else if(cmd == "G04" || cmd == "G4")//逆时针圆弧
        code=ARC_CODE;
    else if(cmd == "G05" || cmd=="G5")//逆时针圆弧
        code = CIRCLE_CODE;
    else{//处理其他情况
        code = NO_USE_CODE;
        string cmd1 = cmd.toStdString();//转化成std：string 临时变量去接
        switch(cmd1[0])
        {
        case '(':
        case '%':
            return true;
        }
        //不是注释，处理不了
        return false;
    }
    return true;
}

 void SimulationParse::preProcess()
 {
     QString line;
     for(int i = content.size() - 1;i > -1;i--)//m_content.GetSize()改m_content.size()都是获取数组中元素的个数
     {
         line = content[i].trimmed();//去掉前后的空格 .Trim(_T(" ")改.trimmed()
         line = line.toUpper();			//转换为大写.MakeUpper()改.toUpper()

         if(line.length() < 1) //这个CString对象中的字节计数。这个计数不包括结尾的空字符。.GetLength()改.length()
           {
             content.remove(i);//.RemoveAt(i)改removeAt(i)
           }
         else
         {
             bool removeFlag = false;
             content[i] = line;
             if(content[i].startsWith("M07"))
             {
                 content[i-1].append(" M0");
                 removeFlag = true;
             }
             if(content[i].startsWith("M08"))
             {
                 content[i-1].append(" N0");
                 removeFlag = true;
             }
             if(removeFlag)
             content.remove(i);
         }
     }
     //判断是否为空
     if(content.size() < 1)//.GetSize()改size();
     throw ERR_fileempty;
 }
//边解析边运动
 //解析直线要注意 1.插补直线去进行 保证xyz在一条直线上
  bool SimulationParse::ParserLine(const QVector<QString> &words,CodeTypeEnum code,bool flag)
  {
      wchar_t pre;
      double val;
      Coint c = {0,0};
      double speed = 3000;
      ArrayXd xyzrpw = ArrayXd::Zero(6);
      if(isFireMode)
      {
          if(isFireLine)  //就是插补去做
             linecolor = Qt::red;
          else
             linecolor = Qt::white;
      }
      else
      {
           if(code == INTERPOLATION_LINE)
               linecolor = Qt::green;
           else
               linecolor = Qt::white;
      }
      for(int i = 1; i < words.length(); i ++)
      {
          if(!ParserDouble(words[i],pre,val))
              return false;
          switch (pre)
          {
                case 'X':  xyzrpw[0] = val;  break;
                case 'Y':  xyzrpw[1] = val;  break;
                case 'Z':  xyzrpw[2] = val;  break;
                case 'R':  xyzrpw[3] = val;  break;
                case 'P':  xyzrpw[4] = val;  break;
                case 'W':  xyzrpw[5] = val;  break;
                case 'U':  c.c1  = val / 180.0 * PI;    break;
                case 'V':  c.c2  = val / 180.0 * PI;    break;
                case 'S':  speed = val;      break;
                case 'M':  if(flag) isFireLine = true;  break;  //表示下一次的线变红
                case 'N':  if(flag) isFireLine = false; break;
                default:
                break;
          }
      }

      if(code == INTERPOLATION_LINE)
      getLineDividePoint(xyzrpw,c,flag);
      else
      {
       Matrix4d mat = Matrix4d::Identity();
       mat = xyzrpw_2_pose(xyzrpw);
       Joint j = NewScaraArgorithmn::NewPositionJointssolution(mat);
       moto_runJAbs(j,c,flag,0);
       currentXyzrpw = xyzrpw;  //空移线这个是没问题的
      }
      return true;
}
bool SimulationParse::ParserArcBack(const QVector<QString>&words,const QVector<QString>&words1,const CodeTypeEnum &code)
{
    wchar_t pre;
    double val;
    double x1=0,y1=0,z1=0,r1=0,p1=0,w1=0;
    double x2=0,y2=0,z2=0,x3=0,y3=0,z3=0,r3=0,p3=0,w3=0,s=0;  //坐标值
    Coint c = {0,0};
    for(int index = 1;index < words.size();index++)
    {
        if(!ParserDouble(words[index],pre,val))
        return false;
        switch(pre)
        {
         case 'A':  x2 = val;     break;
         case 'B':  y2 = val;     break;
         case 'C':  z2 = val;     break;
         case 'X':  x3 = val;     break;
         case 'Y':  y3 = val;     break;
         case 'Z':  z3 = val;     break;
         case 'R':  r3 = val;     break;
         case 'P':  p3 = val;     break;
         case 'W':  w3 = val;     break;
         case 'U':  c.c1 = val;   break;
         case 'V':  c.c2 = val;   break;
         case 'S':  s = val;      break;
        }
    }
    for(int index = 1;index < words1.size();index++)
    {
        if(!ParserDouble(words1[index],pre,val))
        return false;
        switch(pre)
        {
         case 'X':  x1 = val;     break;
         case 'Y':  y1 = val;     break;
         case 'Z':  z1= val;     break;
         case 'R':  r1 = val;     break;
         case 'P':  p1 = val;     break;
         case 'W':  w1 = val;     break;
         case 'U':  c.c1 = val;   break;
         case 'V':  c.c2 = val;   break;
         case 'S':  s = val;      break;
        }
    }
     QList <QVector<double>> pointList;
     if(code == ARC_CODE)
    {
        if(!getArcDividePoint(x1,y1,z1,r1,p1,w1,x2,y2,z2,x3,y3,z3,r3,p3,w3,0,pointList))
        {
            qDebug() << "画圆弧三个点共线";
            return false;
        }
    }
    else
    {
        if(!getCircleDividePoint(x1,y1,z1,r1, p1,w1,x2,y2,z2,x3,y3,z3,r3,p3,w3,pointList))
        {
            qDebug() << "画整圆三个点共线";
             return false;
        }
    }
       if(interIndex < 0)
           interIndex = pointList.size();   //表示这行已经走完啦
       for(int i = interIndex-1;i >=0;i--)
       {
            ArrayXd xyzrpw = ArrayXd::Zero(6);
            for(int j = 0; j < 6; j++)
                xyzrpw[j] = pointList[i][j];
            Matrix4d mat = Matrix4d::Identity();
            mat = xyzrpw_2_pose(xyzrpw);
            Joint j = NewScaraArgorithmn::NewPositionJointssolution(mat);
             if(fabs((CommonFun::lastj.j4 - j.j4)) * 180 / Pi > 300)
             {
                 CommonFun::lastj = j;
                 j4Rot = j.j4 * 180 / Pi;
                 continue;
             }
            CommonFun::lastj = j;
            moto_runJAbs(j,c,false,0);
            if(IsStop)
            {
                interIndex = i+1;
                return true;
            }
       }
   interIndex = -1;
   currentXyzrpw[0] = x1;   //等于末点
   currentXyzrpw[1] = y1;   //
   currentXyzrpw[2] = z1;
   currentXyzrpw[3] = r1;
   currentXyzrpw[4] = p1;
   currentXyzrpw[5] = w1;
   curC = c;
   return true;
}
bool SimulationParse::ParserArc(const QVector<QString>&words,const QVector<QString>&words1,const CodeTypeEnum &code)
{
    wchar_t pre;
    double val;
    double x1=0,y1=0,z1=0,r1=0,p1=0,w1=0;
    double x2=0,y2=0,z2=0,x3=0,y3=0,z3=0,r3=0,p3=0,w3=0,s=0;  //坐标值
    Coint c = {0,0};
    if(isFireMode)
    {
    if(isFireLine)  //就是插补去做
       linecolor = Qt::red;
    else
       linecolor = Qt::white;
    }
    else
       linecolor = Qt::blue;
    for(int index = 1;index < words.size();index++)
    {
        if(!ParserDouble(words[index],pre,val))
        return false;
        switch(pre)
        {
         case 'A':  x2 = val;     break;
         case 'B':  y2 = val;     break;
         case 'C':  z2 = val;     break;
         case 'X':  x3 = val;     break;
         case 'Y':  y3 = val;     break;
         case 'Z':  z3 = val;     break;
         case 'R':  r3 = val;     break;
         case 'P':  p3 = val;     break;
         case 'W':  w3 = val;     break;
         case 'U':  c.c1 = val;   break;
         case 'V':  c.c2 = val;   break;
         case 'S':  s = val;      break;
         case 'M':  isFireLine = true; break;  //表示下一次的线变红
         case 'N':  isFireLine = false; break;
        }
    }
    for(int index = 1;index < words1.size();index++)
    {
        if(!ParserDouble(words1[index],pre,val))
        return false;
        switch(pre)
        {
         case 'X':  x1 = val;     break;
         case 'Y':  y1 = val;     break;
         case 'Z':  z1= val;     break;
         case 'R':  r1 = val;     break;
         case 'P':  p1 = val;     break;
         case 'W':  w1 = val;     break;
         case 'U':  c.c1 = val;   break;
         case 'V':  c.c2 = val;   break;
         case 'S':  s = val;      break;
        }
    }
     QList <QVector<double>> pointList;
    if(code == ARC_CODE)
    {
        if(!getArcDividePoint(x1,y1,z1,r1,p1,w1,x2,y2,z2,x3,y3,z3,r3,p3,w3,0,pointList))
        {
            qDebug() << "画图三个点共线";
            return false;
        }
    }
    else
    {
        if(!getCircleDividePoint(x1,y1,z1,r1,p1,w1,x2,y2,z2,x3,y3,z3,r3,p3,w3,pointList))
        {

            qDebug() << "圆画图三个点共线";
             return false;
        }
    }
      if(interIndex < 0)
          interIndex = 0;
       for(int i = interIndex;i < pointList.size(); i++)
       {

            ArrayXd xyzrpw = ArrayXd::Zero(6);
            for(int j = 0; j < 6; j++)
                xyzrpw[j] = pointList[i][j];
            Matrix4d mat = Matrix4d::Identity();
            mat = xyzrpw_2_pose(xyzrpw);
            Joint j = NewScaraArgorithmn::NewPositionJointssolution(mat);
             if(fabs((CommonFun::lastj.j4 - j.j4)) * 180 / Pi > 300)
             {
                 CommonFun::lastj = j;
                 j4Rot = j.j4 * 180 / Pi;
                 continue;
             }
            CommonFun::lastj = j;
            moto_runJAbs(j,c,true,0);
            if(IsStop)
            {
                interIndex = i;
                return true;
            }
    }
   interIndex = -1;
   currentXyzrpw[0] = x3;   //等于末点
   currentXyzrpw[1] = y3;
   currentXyzrpw[2] = z3;
   currentXyzrpw[3] = r3;
   currentXyzrpw[4] = p3;
   currentXyzrpw[5] = w3;
   curC = c;
   return true;
}
void SimulationParse::sendJC()
{
    double joint[6] = {j1Rot,j2Rot,j3Rot,j4Rot,j5Rot,j6Rot};
    switch (glType)
    {
    case LanxunCoordRobot:
        joint[0] = j1Rot;
        joint[1] = j2Rot;
        joint[2] = j3Rot;
        break;
    case Lanxun5JointRobot:
         joint[2] = j3Rot;
        break;
    default:
        break;
    }
  emit jointcoordSignal(joint,coord);
}
//插补直线是另外的一条线来做的  来处理的
bool SimulationParse::getLineDividePoint(ArrayXd &xyzrpw,Coint& c,bool flag)
{
     ArrayXd xyzrpwlen = ArrayXd::Zero(6);
     ArrayXd curxyzrpw = ArrayXd::Zero(6);
     curxyzrpw = currentXyzrpw;
     for(int i = 0;i < 6; i++)
     {
         xyzrpwlen[i] = xyzrpw[i] - curxyzrpw[i];
     }
     if(glType == Lanxun5JointRobot)
     {
       if(fabs(xyzrpwlen[0]) < PI / 18000.0&& fabs(xyzrpwlen[1]) < PI / 18000.0 && fabs(xyzrpwlen[2]) < 0.01 && fabs(xyzrpwlen[3]) < PI / 18000.0 && fabs(xyzrpwlen[4]) < PI / 18000.0 && fabs(xyzrpwlen[5]) < PI / 18000.0)
              return false;
     }
     else
     {
         if(fabs(xyzrpwlen[0]) < 0.01 && fabs(xyzrpwlen[1]) < 0.01 && fabs(xyzrpwlen[2]) < 0.01 && fabs(xyzrpwlen[3]) < PI / 18000.0 && fabs(xyzrpwlen[4]) < PI / 18000.0 && fabs(xyzrpwlen[5]) < PI / 18000.0)
          return false;
     }

     double contia = sqrt(xyzrpwlen[0] * xyzrpwlen[0] + xyzrpwlen[1] * xyzrpwlen[1] + xyzrpwlen[2] * xyzrpwlen[2]
             + xyzrpwlen[3] * xyzrpwlen[3] + xyzrpwlen[4] * xyzrpwlen[4] + xyzrpwlen[5] * xyzrpwlen[5]);

     double rate =  moveLineinter / contia ;
     for(double i = 0; i < contia; i += moveLineinter)
     {
         for(int j= 0; j < 6; j++)
         {
             curxyzrpw[j] += xyzrpwlen[j] * rate;
         }
         Matrix4d mat = Matrix4d::Identity();
         mat = xyzrpw_2_pose(curxyzrpw);
         Joint j = NewScaraArgorithmn::NewPositionJointssolution(mat);
         if(glType == Lanxun5JointRobot)
         {
             j1Rot = j.j1 * 180 / PI;
             j2Rot = j.j2 * 180 / PI;
         }
         else
         {
             j1Rot = j.j1;
             j2Rot = j.j2;
         }
         j3Rot = j.j3;
         j4Rot = j.j4 * 180 / PI;
         j5Rot = j.j5 * 180 / PI;
         currentXyzrpw = curxyzrpw;
         for(int i = 0;i < 6;i++)
            coord[i] = CommonFun::round(curxyzrpw[i],2);

         if(flag)
         addTrialPoints();    //加入点
         else
         decTrialPoints();
         updateGL();
         sendJC();
         if(IsStop)
         {
           currentXyzrpw = curxyzrpw;
          return false;
         }
         CommonFun::sleep(timelist[speedRate / 10 -1]);
     }
     //把最后一个点加入进去
     Matrix4d mat = Matrix4d::Identity();
     mat = xyzrpw_2_pose(xyzrpw);
     Joint j = NewScaraArgorithmn::NewPositionJointssolution(mat);
     if(glType == Lanxun5JointRobot)
     {
         j1Rot = j.j1 * 180 / PI;
         j2Rot = j.j2 * 180 / PI;
     }
     else
     {
         j1Rot = j.j1;
         j2Rot = j.j2;
     }
     j3Rot = j.j3;
     j4Rot = j.j4 * 180 / PI;
     j5Rot = j.j5 * 180 / PI;
     for(int i = 0;i < 6;i++)
        coord[i] = CommonFun::round(curxyzrpw[i],2);
     if(flag)
     addTrialPoints();    //加入点
     else
     decTrialPoints();
     updateGL();
     currentXyzrpw =xyzrpw;
     return true;
}
//解析浮点数值,tbd 判断是否为合法浮点数
bool SimulationParse::ParserDouble(const QString &word,wchar_t &pre,double &val)
{
    if(word.length() < 2)//getlength改length  //分割的字符
    {
        return false;
    }
    string word1 = word.toStdString();//不知道这种转换可以不，语法可以通过
    pre = word1[0];
    QString tmp = word.right(word.length() - 1);
    val = tmp.toDouble();
    return true;
}
//运行主函数
bool SimulationParse::prerun(const QString &fname)
{
    try
    {
        content.clear();
        errs.clear();
        num = 0;
        readFile(fname);
        preProcess();
    }
    catch(...)
    {
        return false;
    }
}
bool SimulationParse::run(bool flag)
{

    try
    {
         IsStop = false;
        doParse(flag);
        if(IsStop)
            IsStop = false;
        if(errs.size() > 0)
            return false;
        return true;
    }
    catch(...)
    {
        return false;
    }
}
 //***************************************圆/圆弧等分算法**************************************//
//判断最后一点是否是第三点
bool SimulationParse::isEndPoint(double x1, double y1, double z1, double x2, double y2, double z2)
{
    return fabs(x1 - x2) < 0.000001 && fabs(y1 - y2) < 0.000001 && fabs(z1 - z2) < 0.000001;
}
//圆弧等分得到等分点的坐标
//num是等分点序号的基数,在等分圆时用，等分圆弧为0
bool SimulationParse::getArcDividePoint(double x1, double y1, double z1, double r1, double p1,double w1, double x2,
                                             double y2, double z2, double x3, double y3, double z3, double r3,
                                             double p3,double w3, int num,QList<QVector<double>>& pointList)
{

    QVector<double> point;
    if (CommonFun::isThreePointInLine(x1, y1, z1, x2, y2, z2, x3, y3, z3))
               return false;
    //不共线表示可以构成三角形，用平面基向量 AB AC 来表示 AO， AO = aAB + bAC; AO·AB = 1/2 * AB^2, AO·AC = 1/2 * AC^2;
           double ABAB, ABAC, ACAC, a, b;
           ABAB = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) + (z2 - z1) * (z2 - z1);
           ABAC = (x2 - x1) * (x3 - x1) + (y2 - y1) * (y3 - y1) + (z2 - z1) * (z3 - z1);
           ACAC = (x3 - x1) * (x3 - x1) + (y3 - y1) * (y3 - y1) + (z3 - z1) * (z3 - z1);
           a = ACAC * (ABAB - ABAC) / (ACAC * ABAB - ABAC * ABAC) / 2.0;
           b = (ABAB * ABAC - ABAB * ACAC) / (ABAC * ABAC - ACAC * ABAB) / 2.0;
           //圆心坐标和半径
           double x0, y0, z0, r;
           x0 = x1 + a * (x2 - x1) + b * (x3 - x1);
           y0 = y1 + a * (y2 - y1) + b * (y3 - y1);
           z0 = z1 + a * (z2 - z1) + b * (z3 - z1);
           r = sqrt((x0 - x1) * (x0 - x1) + (y0 - y1) * (y0 - y1) + (z0 - z1) * (z0 - z1));
           //考虑到起点、末点之间的圆心角可能大于180度，把角度分成起点与中点，中点与末点对应的圆心角之和
           //使用向量积来求，cos在0-pi内是单调的，选用点积
           double OAOB = (x1 - x0) * (x2 - x0) + (y1 - y0) * (y2 - y0) + (z1 - z0) * (z2 - z0);
           double OBOC = (x2 - x0) * (x3 - x0) + (y2 - y0) * (y3 - y0) + (z2 - z0) * (z3 - z0);
           double angle1 = acos(OAOB / (r * r));
           double angle2 = acos(OBOC / (r * r));
           //每一等分对应的圆心角
           double angle = divideLength / r;
   //        angle = 0.15707963267948966192313216916398;       //测试用
           //等分数
           int divideNum = (int) ((angle1 + angle2) / angle);
           if(divideNum < 1)
           {
               QVector<double> point;
               point.append(x1);
               point.append(y1);
               point.append(z1);
               point.append(r1);
               point.append(p1);
               point.append(w1);
               point.append(1 + num);
               pointList.append(point);
               point.clear();
               point.append(x2);
               point.append(y2);
               point.append(z2);
               point.append((r1 + r3) / 2.0);
               point.append((p1 + p3) / 2.0);
               point.append((w1 + p3) / 2.0);
               point.append(2 + num);
               pointList.append(point);
               point.clear();
               point.append(x3);
               point.append(y3);
               point.append(z3);
               point.append(r3);
               point.append(p3);
               point.append(w3);
               point.append(3 + num);
               pointList.append(point);
               return true;
           }
           //姿态的等分
           double rDivide = (r3 - r1) / divideNum;
           double pDivide = (p3 - p1) / divideNum;
           double wDivide = (w3 - w1) / divideNum;

           //同样根据向量积来求等分点的坐标(x, y, z)
           //先判断A、O、B是否共线，共线就用A、O、C来求解，因为A、O、B和A、O、C不可能同时共线
           //A、O、B不共线，用平面基向量 OA OB 来表示 OD， OD = aOA + bOB; OA·OD = r*r*cos(angle), OB·OD = r*r*cos(angle1);
           //A、O、B共线，用平面基向量 OA OC 来表示 OD， OD = aOA + bOC; OA·OD = r*r*cos(angle), OC·OD = r*r*cos(angle1 + angle2);
           //A、O、B不共线解得 a = sin(angle)/sin(angle1), b = sin(angle1 - angle)/sin(angle1);
           //A、O、B共线解得 a = sin(angle)/sin(angle1 + angle2), b = sin(angle1 + angle2 - angle)/sin(angle1 + angle2);
           //判断A、O、B是否共线，可以用之前的判断共线
           double x, y, z;     //等分点的坐标
           if (fabs(angle1 - PI) < 0.01 || fabs(angle1 - 0.0) < 0.01)
           {            //共线,
               // 不一定用isThreePointInLine来判断，用 angle1 与 pi 的关系也可以判断的
               for (int i = 0; i < divideNum; i++)
               {
                   //等分点的属性，x, y, z和点的序号, 必须每次都初始化对象，不然之前point的也会跟着变化
                   QVector<double> point;
                   point.clear();
                   b = sin(angle * (i + 1)) / sin(angle1 + angle2);
                   a = sin(angle1 + angle2 - angle * (i + 1)) / sin(angle1 + angle2);
                   point.append(x0 + a * (x1 - x0) + b * (x3 - x0));        //x
                   point.append(y0 + a * (y1 - y0) + b * (y3 - y0));        //y
                   point.append(z0 + a * (z1 - z0) + b * (z3 - z0));        //z
                   point.append(r1 + rDivide * (i + 1));                    //r
                   point.append(p1 + pDivide * (i + 1));                    //p
                   point.append(w1 + wDivide * (i + 1));                    //w
                   point.append(i + 1 + num);              //序号，从 1 开始，到时候获取的时候直接强制转换成int就可以了
                   pointList.append(point);
               }
           }
           else {
               for (int i = 0; i < divideNum; i++)
               {
                   QVector<double> point;
                   point.clear();
                   b = sin(angle * (i + 1)) / sin(angle1);
                   a = sin(angle1 - angle * (i + 1)) / sin(angle1);
                   point.append(x0 + a * (x1 - x0) + b * (x2 - x0));
                   point.append(y0 + a * (y1 - y0) + b * (y2 - y0));
                   point.append(z0 + a * (z1 - z0) + b * (z2 - z0));
                   point.append( r1 + rDivide * (i + 1));                    //r
                   point.append( p1 + pDivide * (i + 1));                    //p
                   point.append( w1 + wDivide * (i + 1));                    //w
                   point.append(i + 1 + num);
                   pointList.append(point);
               }
           }
           point = pointList.at(divideNum - 1);
           if (!isEndPoint(point[0], point[1], point[2], x3, y3, z3))
           {    //判断是否最后一点
              QVector<double> point;
               point.clear();
               point.append(x3);        //x
               point.append(y3);        //y
               point.append(z3);        //z
               point.append(r3);                    //r
               point.append(p3);                    //p
               point.append(w3);                    //w
               point.append(divideNum + 1 + num);            //序号，从 1 开始
               pointList.append(point);
           }
           return true;
}


//得到圆弧的等分点 通过三个点得到
bool SimulationParse::getCircleDividePoint(double x1, double y1, double z1, double r1, double p1,double w1, double x2,
                                    double y2, double z2, double x3, double y3, double z3, double r3,
                                    double p3,double w3,QList<QVector<double>>& arcDividePoint )
{

   QList<QVector<double>>newArcDivide;     //新的圆的等分函数
   newArcDivide.clear();                   //圆的清零
   if(!getArcDividePoint(x1, y1, z1, r1, p1,w1, x2, y2, z2, x3, y3, z3, r3, p3,w3,0,arcDividePoint))   //
    return false;
   int num = arcDividePoint.size();
          //倒数第一个点就是x3, y3, z3,这里是取倒数第三个点，然后以这个点为起点，x3, y3, z3为中点，x1, y1, z1为终点再次等分圆弧
          QVector<double> preLast = arcDividePoint.at(num - 2);
          //计算新的圆弧等分
          if(!getArcDividePoint(preLast[0], preLast[1], preLast[2], preLast[3],
                  preLast[4], preLast[5],x3, y3, z3, x1, y1, z1, r1, p1,w1, num - 1,newArcDivide))
              return false;

          arcDividePoint.removeAt(num - 1);        //原来的去掉末尾
          arcDividePoint += newArcDivide;
          return true;
}
void SimulationParse::modeSwitch()
{
    isFireMode = !isFireMode;
    if(isFireMode)
        mode = FireMode;
    else
        mode = ShowMode;
     updateGL();
}
void SimulationParse::addspeed()
{
    speedRate += 10;
    if(speedRate > TIMELIST  * 10)
        speedRate = TIMELIST * 10;
    updateGL();
}
void SimulationParse::decspeed()
{
    speedRate -=10;
    if(speedRate < 10)
        speedRate = 10;
    updateGL();
}
