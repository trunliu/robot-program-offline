#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H
#include <QObject>
#include <QWidget>
#include <QtOpenGL>
#include <QOpenGLFunctions_2_1>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include "STLView.h"
#include <QList>
#include "CodeParse/scaralib.h"

enum Mode
{
    FireMode,   //焊接模式
    ShowMode,   //演示模式
    NoMode
};

enum WidgetName
{
    NONAME,
    TEACH,      //示教
    SIMULATION, //仿真
    ALIGNMENT   //联调
};
enum ViewMode
{
    NOHIDE,
    HIDEROBOT,
    HIDEROBOTANDTORCH
};
class MyPoint1
{
public:
    MyPoint1()
    {
      color = Qt::black;
    }
    double x,y,z;
    QColor color;
};

//必须继承QGLWidget和QOpenGLFunctions_2_1
class OpenglWidget : public QGLWidget, public QOpenGLFunctions_2_1
{
    Q_OBJECT
public:
    OpenglWidget(RobotType type);

    //重载paintGL()、initializeGL()、resizeGL()
    //调用initializeOpenGLFunctions() 环境的初始化工作
    void  initializeGL() ;

    //调用updateGL()触发重绘
    void  paintGL();

    //当界面的窗口的大小发生变化时，就会触发resizeGL()函数，并调用updateGL（）实现重绘
    void  resizeGL( int w, int h );

    //事件处理
    void  keyPressEvent(QKeyEvent * e);
    void  mousePressEvent(QMouseEvent *event);
    void  mouseMoveEvent(QMouseEvent *event);
    void  mouseDoubleClickEvent(QMouseEvent *event);
    void  wheelEvent(QWheelEvent *event);

    void  loadGLTextures();
    double callFrequency();
    double Fps;

    //用于保存关节数据和坐标
    GLfloat j1Rot,j2Rot,j4Rot,j3Rot,j5Rot,j6Rot,c1Rot,c2Rot;
    double coord[8];

    void addTrialPoints();
    void decTrialPoints();
//  double dx,dy,dz;
    RobotType glType;
    void init();
    void home();

    //根据数据包解析cmd指令获得颜色赋值给linecolor
    QColor linecolor;

    QList<MyPoint1> trailPoints;
    GLuint *gllist;
    Mode mode;
    ViewMode viewmode;
    WidgetName widgetname;
    int percent;
    int speedRate;
    bool amplification();
    bool NarrowDown();
    void reduce();
    void xRotateadd();
    void xRotatedec();
    void yRotateadd();
    void yRotatedec();
    bool isHideLine;
    void changeViewMode(ViewMode mode);
    bool changeIsHideLine();
    void getCurrentCoord();
    void loadArtifacts(const QString &fname,bool flag);
    void setJ(const Joint& j);
    void getCurrentJ(Joint& j);
    static STLView *artifacts;
    bool isShowWorkPiece;
private:
    double scaleNum;
    double d1,a2,a3,a4,d3,dh,d4,d6;
    void makeObject();
    STLView *view;
    GLfloat xRot,yRot,zRot,zoom;
    GLfloat wpxTrans,wpyTrans,wpzTrans;  //保证平移方面
    QPoint lastPos;
    double lastDis;
    QColor color[8];
    bool light;
    GLuint texture[1];
    int num;
    void setText();
    void drawLanxun5JointRobot();
    void drawLanxun6JointRobot();
    void drawLanxunCoordRobot();
    void drawFloor();

};
#endif // OPENGLWIDGET_H
