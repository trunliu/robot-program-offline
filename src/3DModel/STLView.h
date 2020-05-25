#ifndef STLPARSE_H
#define STLPARSE_H
#include <QString>
#include <QVector>
#include "CommonFun/Common.h"
#include <QThread>
#define COORDTYPE 1 //0表示正常  1表示另外一种

//三角形片类,主要由三个顶点坐标和一个法向量组成
class MyTriangle
{
 public:
    MyPoint normal,vertex1,vertex2,vertex3;
};

//STL解析类
class STLView : public QThread
{
public:
    //机器人模型库  一个模型对应一个库   这样就不会释放
    //解析类对象指针作为类的静态成员，只用初始化一次
    //单例模式
    static STLView* Artifacts;
    static STLView* lanxunCoordView;
    static STLView* lanxun5JointView;
    static STLView* lanxun6JointView;
    static void init(RobotType);
    static STLView* init();

    virtual void run();

    //读取stl文件并解析出来
    bool readSTL(const QString &fname);

    //保存解析出来的三角片数据
    QVector<MyTriangle> triangleVector;

private:
    static RobotType type;
    int n;
};
#endif // STLPARSE_H
