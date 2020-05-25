#include "STLView.h"
#include <QFile>
#include <QTextStream>
#include <math.h>
#include "CommonFun/CommonFun.h"
#include <QMessageBox>
 STLView* STLView::lanxunCoordView = NULL;
 STLView* STLView::lanxun5JointView = NULL;
 STLView* STLView::lanxun6JointView = NULL;
 STLView* STLView::Artifacts=NULL;
 RobotType STLView::type = LanxunCoordRobot;

//解析STL文件生成triangleVector数组
bool STLView::readSTL(const QString &fname){
    QFile file(fname);
    if ( file.open(QFile::ReadOnly | QFile::Text))  //仅读和文字
    {
            QTextStream in( &file );
            MyTriangle triangle;
            QString line;
            int trianglePartNums = 0;

            while ( !in.atEnd() )
            {
                line = in.readLine();
                QVector<QString> words;

                //分割一行，生成words字符串数组返回字符个数
                int num = CommonFun::splitString(line,' ',words);

                //解析一行
                for(int i = 0; i < num; i++)
                {
                    //集齐4个参数,添加一个三角片
                    if(trianglePartNums == 4)
                    {
                        triangleVector.push_back(triangle);
                        trianglePartNums = 0;
                    }
                    if(words[i] == "normal")
                    {
                        triangle.normal.x = words[i+1].toDouble();
                        triangle.normal.y = words[i+2].toDouble();
                        triangle.normal.z = words[i+3].toDouble();
                        trianglePartNums++;
                        break;
                    }
                    if(words[i] == "vertex")
                    {
                        if(trianglePartNums == 1)
                        {
                        triangle.vertex1.x = words[i+1].toDouble();
                        triangle.vertex1.y = words[i+2].toDouble();
                        triangle.vertex1.z = words[i+3].toDouble();
                        }
                        if(trianglePartNums == 2)
                        {
                        triangle.vertex2.x = words[i+1].toDouble();
                        triangle.vertex2.y = words[i+2].toDouble();
                        triangle.vertex2.z = words[i+3].toDouble();

                        }
                        if(trianglePartNums == 3)
                        {
                        triangle.vertex3.x = words[i+1].toDouble();
                        triangle.vertex3.y = words[i+2].toDouble();
                        triangle.vertex3.z = words[i+3].toDouble();
                        }
                        trianglePartNums++;
                        break;
                    }
                }
            }
            file.close();
            return true;
    }
    else
    {
        qWarning() << fname << ": STL 文件打开失败";
        return false;
    }

}

//初始化工件
STLView* STLView::init(){
    if(!Artifacts){
       Artifacts=new STLView();
    }
    return Artifacts;
}

//初始化工作，根据机器人类型，创建多线程分别解析不同部位的stl文件
void STLView::init(RobotType rtype)
{
    type = rtype;
    switch (type)
    {
    case LanxunCoordRobot:
    {
        //为空的情况，没有初始化 就让他初始化一次
        if(!lanxunCoordView)
        {
            //创建STLView[5]数组，开始5个线程
            //run()函数的开始运行是通过start()函数执行的，
            //停止是通过stop()函数,等待线程完成是通过wait()函数
            lanxunCoordView = new STLView[lanxunCoordRobotPartNum];
            for(int i = 0; i < lanxunCoordRobotPartNum;i++)
            {
              lanxunCoordView[i].n = i;
              lanxunCoordView[i].start();
            }
            for(int i = 0; i < lanxunCoordRobotPartNum;i++)
             lanxunCoordView[i].wait();
        }
        break;
    }
    case Lanxun5JointRobot:   //5关节式的机器人
        if(!lanxun5JointView) //为空的情况
        {
            lanxun5JointView = new STLView[lanxun5JointRobotPartNum];   //读取文件
            for(int i = 0; i < lanxun5JointRobotPartNum;i++)
            {
                lanxun5JointView[i].n = i;
                lanxun5JointView[i].start();
            }
              for(int i = 0; i < lanxun5JointRobotPartNum;i++)
              lanxun5JointView[i].wait();
        }
        break;
    case Lanxun6JointRobot:
        if(!lanxun6JointView) //为空的情况
        {
            lanxun6JointView = new STLView[lanxun6JointRobotPartNum];   //读取文件
            for(int i = 0; i < lanxun6JointRobotPartNum;i++)
            {
                lanxun6JointView[i].n = i;
                lanxun6JointView[i].start();
            }
          for(int i = 0; i < lanxun6JointRobotPartNum;i++)
              lanxun6JointView[i].wait();
        }
        break;
    default:
        break;
    }
}

 void STLView::run()
 {
    switch(type)
    {
        case LanxunCoordRobot:
        lanxunCoordView[n].readSTL(QString(":/stl/lanxunCoordRobot/%1.STL").arg(n+1));
        break;
        case Lanxun5JointRobot:
        lanxun5JointView[n].readSTL(QString(":/stl/lanxun5JointRobot/%1.STL").arg(n+1));
        break;
        case Lanxun6JointRobot:
        lanxun6JointView[n].readSTL(QString(":/stl/lanxun6JointRobot/%1.STL").arg(n));
        break;
    }
 }
