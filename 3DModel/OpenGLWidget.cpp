#include <QImage>
#include <time.h>
#include "OpenGLWidget.h"
#include <math.h>
#include "CodeParse/NewScaraArgorithmn.h"
#include "CommonFun/CommonFun.h"

//设置光照参数 使其更加立体
GLfloat lightAmbient[4] = {0.0,0.0,0.0,1.0};  //环境光
GLfloat lightDiffuse[4] = {1.0,1.0,1.0,1.0};  //漫射光
GLfloat lightPosition[4] = {1.5,1.5,2,2.0};   //光的位置
GLfloat lightSpecular[4] = {1.0f, 1.0f, 1.0f, 1.0f};

GLfloat matAmbient[4] = {0.0,0.0,0.0,1.0f};
GLfloat matDiffuse[4] = {0.0,0.0,0.0,1.0f};
GLfloat matSpecular[4] = {0.0,0.0,0.0,1.0f};
GLfloat matEmission[4] = {0.0,0.0,0.0,1.0f};
GLfloat matShininess = 50.0f;

//六关节的模型
//const double a2  = 700 / scaleNum;
//const double a3  = 600 / scaleNum;
//const double d3  = 703.20 / scaleNum;
//const double dh  = 233.924 / scaleNum;

//下面是变位机构的值
//const double ch_x0  = 700 / scaleNum;
//const double ch_y0  = -600 / scaleNum;
//const double ch_z0  = 0 / scaleNum;
//const double ch_d1  = 150 / scaleNum;
//const double ch_d2  = 100 / scaleNum;
//const double ch_a3  = 100 / scaleNum;
//const double ch_d3  = 40 /  scaleNum;
//const double base_length = 1000 / scaleNum;
STLView* OpenglWidget::artifacts = NULL;

OpenglWidget::OpenglWidget(RobotType type)
{
     isShowWorkPiece = false;
     STLView::init(type);
     switch(type)
     {
       case LanxunCoordRobot:  view = STLView::lanxunCoordView;  break;
       case Lanxun5JointRobot: view = STLView::lanxun5JointView; break;
       case Lanxun6JointRobot: view = STLView::lanxun6JointView; break;
     }
     percent = 0;
     speedRate = 100;
     mode = NoMode;
     glType = type;   //通过构造函数获取需要的所需的类型
     isHideLine = false;
     for(int i = 0; i < 8; i++)  //当前坐标清理
         coord[i] = 0;
     viewmode = NOHIDE;
     widgetname = NONAME;    //当前的名字
     Fps  =  0;
     xRot = 0;    //坐标的移动
     yRot = 0;
     zRot = 0.0;
     wpxTrans = 0;
     wpyTrans = 0;
     wpzTrans = 0;
     j1Rot = j2Rot = j4Rot = j3Rot = j5Rot = j6Rot = 0;
     c1Rot = c2Rot = 0;      //变位机构
     light = true;           //光照开启
     zoom = -10;             //zoom透过深度为10
     linecolor = Qt::white;  //空移线的颜色
     color[0] = Qt::cyan;
     color[1] = Qt::red;
     color[2] = Qt::blue;
     color[3] = Qt::yellow;
     color[4] = Qt::green;
     color[5] = Qt::magenta;
     color[6] = Qt::cyan;
     color[7] = Qt::red;
//     color[8] = Qt::blue;
//     color[9] = Qt::yellow;
//     color[10] = Qt::white;
//     color[11] = Qt::white;
//     color[12] = Qt::white;
     switch (type)
     {
     case LanxunCoordRobot:
          scaleNum = 700;
//          a2  = 700;
//          a3  = 600;
//          d3  = 703.20;
//          dh  = 233.924;
          num = lanxunCoordRobotPartNum;
          break;
     case Lanxun5JointRobot:
          scaleNum = 700;
          a2  = 700;
          a3  = 600;
          d3  = 703.20;
          dh  = 233.924;
          num = lanxun5JointRobotPartNum;
          break;
     case Lanxun6JointRobot:
          scaleNum = 400;
          d1  = 215;
          a2  = 109;
          a3  = 250.5;
          a4  = 115.5;
          d4  = 239.5;
          d6  = 270.2;
          num = lanxun6JointRobotPartNum;
          break;
     default:
         break;
     }
      gllist = new GLuint[num];   //获取造型的list列表
}

/***********************************************************************
    qglClearColor(QColor(“#61adc8”))        清理背景颜色，将背景颜色设置以“#61adc8”十六进制代表的颜色。
    glShadeModel(GL_SMOOTH)                 设置阴影平滑
    glClearDepth(1.0)                       深度缓存的设置
    glEnable(GL_DEPTH_TEST)                 深度缓存的启用，使物体显示更加具有层次感
    glDepthFunc( GL_LEQUAL )                设置深度缓存的类型，表示当深度较小和相等时也渲染
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)      清除颜色缓存和深度缓存


    glViewport(0,0,(GLint)w,(GLint)h)       视口大小的设置，一般和窗口大小一样
    glMatrixMode(GL_PROJECTION)             选择透视投影为投影矩阵
    glLoadIdentity()                        把当前矩阵重置为单位矩阵
    glFrustum(-x,+x,-1.0,+1.0,5.0,15.0)     定义视锥体，参数表示截面大小
    glMatrixMode(GL_MODELVIEW)              加载当前投影矩阵
    gluLookAt(x, y, z, cx, cy, cz, ux, uy, uz);       视图变化，设置相机或眼睛的位置和焦点
************************************************************************/
void  OpenglWidget::initializeGL()
{
    //环境的初始化
    initializeOpenGLFunctions();

    loadGLTextures();
    //glEnable( GL_TEXTURE_2D );  //没有打开纹理

    // 初始化的时候就已经读取了stl中的数据
    makeObject();

    qglClearColor(QColor("#61adc8"));                     //设置背景颜色
    glShadeModel( GL_SMOOTH );                            //阴影平滑

/********************************************************************************
    1、glHint 透视修正
    2、GL_PERSPECTIVE_CORRECTION_HINT
       表示颜色和纹理坐标的差值质量。 如果OpenGL不能有效的支持透视修正参数差值，
       则 GL_DONT_CARE 或 GL_FASTEST 可以执行颜色或纹理坐标的简单线性插值计算。
    3、GL_NICEST
       应选择最正确或最高质量的选项。
********************************************************************************/
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

/********************************************************************************
1、glClearDepth, 它给深度缓冲指定了一个初始值，缓冲中的每个像素的深度值都是这个。
   每个物体的每个像素都有一个深度缓存的值（在0到1之间），越靠近人的深度值小例如：glClearDepth(1.0);默认深度最远。
2、glDepthFunc( GL_LEQUAL )：
   指定“目标像素与当前像素在z方向上值大小比较”的函数，符合该函数关系的目标像素才进行绘制，否则对目标像素不予绘制。
   GL_LEQUAL——输入的深度值小于或等于参考值
3、该函数只有启用“深度测试”时才有效，参考glEnable(GL_DEPTH_TEST)
********************************************************************************/
    glClearDepth(1.0);                                    //设置深度缓存
    glDepthFunc( GL_LEQUAL );                             //所做深度缓存类型
    glEnable(GL_DEPTH_TEST);                              //启用深度缓存


/********************************************************************************
**  glLightfv（光源编号，光源特性，强度参数数据）来设置光源,可以设置８个光源GL_LIGHT0~7
**  光源特性主要可取：GL_AMBIENT（设置光源的环境光属性，默认值(0,0,0,1)）
**  GL_DIFFUSE（设置光源的散射光属性，默认值(1,1,1,1)）
**  GL_SPECULAR（设置光源的镜面反射光属性，默认值(1,1,1,1)）
**  GL_POSITION（设置光源的位置，默认值(0,0,1,0)）
**  参数数据格式要求为数组形式，即数学上的向量形式
**  GL_POSITION数组(x，y，z，w)定义了光源在空间中的位置，当w＝0时，根据齐次坐标的性质，它表示光源位于无穷远处，所有光线几乎是相互平等的，如太阳
********************************************************************************/
    glLightfv(GL_LIGHT0,GL_AMBIENT,lightAmbient);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,lightDiffuse);
    glLightfv(GL_LIGHT0,GL_SPECULAR,lightSpecular);
    glLightfv(GL_LIGHT0,GL_POSITION,lightPosition);

    //光源的性质设置完毕，还要调用glEnable(GL_LIGHT0)来打开0号光源
    //glEnable(GL_LIGHTING)启动光照
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);

/********************************************************************************
    1、通常情况下设置一个物体的材质需要以下这么5个属性
    2、除了采用glMaterialfv()设置材质外，还可以使用glColorMaterial()设置材质属性
    3、使用这个函数时，要启用glEnable(GL_COLOR_MATERIAL)
    4、第一个参数决定该材质运用于图元的正面还是反面。可以取GL_FRONT(正面)，GL_BACK(反面)，GL_FRONT_AND_BACK(正反两面)。
       第2个值表示对何种光进行设置，GL_DIFFUSE表示对漫射光反射率进行设置，
       可以取GL_AMBIENT(环境光)、GL_DIFFUSE(漫射光)、GL_AMBIENT_AND_DIFFUSE(环境光和漫射光)、GL_SPECULAR(平行/镜面光)。
       而第三个参数是一个四维数组，这个数组描述了反光率的RGBA值，每一项取值都为0-1之间
    5、当光源中含有镜面光成分，且镜面光较强时，一些光滑的物体便会出现一些高亮的焦点我们可以通过设置材质的镜面指数来确定光斑的大小和聚焦程度。调用
       glMateriali(GL_FRONT,GL_SHININESS,N );可以对镜面指数进行设定。如果N值越大，则光斑尺寸越小，物体越有光泽，反之越大。N值可取1-128之间的任意整数。
    6、有些物体，本身会发光。我们可以设置材质的Emission成分来使物体看起来有发光效果
       glMaterialfv(GL_FRONT,GL_EMISSION,LightEmission);LightEmission表示物体所发光的颜色的RGBA值
*********************************************************************************/
    glMaterialfv(GL_FRONT,GL_AMBIENT,matAmbient);
    glMaterialfv(GL_FRONT,GL_DIFFUSE,matDiffuse);
    glMaterialfv(GL_FRONT,GL_SPECULAR,matSpecular);
    glMaterialfv(GL_FRONT,GL_EMISSION,matEmission);
    glMaterialf(GL_FRONT,GL_SHININESS,matShininess);

    //设置材料函数工作函数
    glEnable(GL_COLOR_MATERIAL);
}

//重绘一帧
void  OpenglWidget::paintGL()
{
     qDebug()<<"paintGL";
    //清理屏幕和深度缓存
    //使用当前缓冲区清除值，也就是glClearColor或者glClearDepth等函数所指定的值来清除指定的缓冲区。
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //允许建立一个绑定到目标纹理的有名称的纹理
    glBindTexture( GL_TEXTURE_2D, texture[0] );

    //将当前矩阵保存入堆栈顶(保存当前矩阵)。
    glPushMatrix();

    //把矩阵堆栈中的在栈顶的那个矩阵置为单位矩阵
    glLoadIdentity();

    //将模型的坐标绕z轴旋转zRot角度(这行代码没用）
    //????zRot未赋值????????????????
    glRotatef(zRot, 0.0, 0.0, 1.0 );

    switch (glType)
    {
        case LanxunCoordRobot:
            drawLanxunCoordRobot();
            break;
        case Lanxun5JointRobot:
            drawLanxun5JointRobot();
            break;
        case Lanxun6JointRobot:
            drawLanxun6JointRobot();
            break;
        default:
            break;
    }

    //glPushMatrix()和glPopMatrix()的配对使用可以消除上一次的变换对本次变换的影响
    //将当前的用户坐标系的原点移到原来的位置：类似于一个复位操作
    glPopMatrix();

    setText();

    //强制刷新指令缓冲区送到显卡并清空指令队列，保证绘图命令将被执行，发送完立即返回
    glFlush();
    //显卡完成这些命令（也就是画完了）后才返回，GPU画图时CPU闲置，降低了性能，所以一般少用finish
    //glFinish();

    Fps = callFrequency();
    qDebug()<<"paintGL";
}

void  OpenglWidget::resizeGL( int w, int h )
{
    if(h == 0)
        h = 1;

    //窗口大小变化时，重新视口设置宽高
    //其中前两个参数定义了视口的左下脚（0,0表示最左下方），后两个参数分别是宽度和高度。
    //绘制到窗口区域，通常情况下，默认是完整的填充整个窗口
    glViewport(0,0,(GLint)w,(GLint)h);

    //设置当前操作矩阵模式为投影矩阵
    glMatrixMode(GL_PROJECTION);
    //在进行变换前把当前矩阵设置为单位矩阵。
    glLoadIdentity();

    GLfloat x = GLfloat(w) / h;

    //投影变换就是定义一个可视空间，可视空间以外的物体不会被绘制到屏幕上。（景深）
    //重新定义可视空间体
    //注意：坐标默认是-1.0到1.0，但可以更改
    //参数为left,  Right,  bottom,  top,  near,  far
    //x轴最小坐标和最大坐标,中间两个是y轴,最后两个是z轴值
    glFrustum(-x,+x,-1.0,+1.0,5.0,15.0);

    //选择当前为模型观察矩阵
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}

void OpenglWidget::drawLanxun5JointRobot()
{
    //模型变换函数 移动
    glTranslatef( -0.4, -1.0, 0 );    //调整到中间位置
    glTranslatef( 0, 0, zoom );       //调整与屏幕z轴的距离

    //模型变换函数  旋转 angle表示旋转的角度（注意单位不是弧度），(x,y,z)表示转轴
    glRotatef( -90,  1.0, 0.0, 0.0 );
    glRotatef( xRot, 1.0, 0.0, 0.0 );
    glRotatef( yRot, 0.0, 1.0, 0.0 );

    if(isShowWorkPiece && artifacts)
    {
      glTranslatef(wpxTrans / scaleNum,wpyTrans / scaleNum,wpzTrans / scaleNum);
      glCallList(gllist[num]);
      glTranslatef(-wpxTrans / scaleNum,-wpyTrans / scaleNum,-wpzTrans / scaleNum);
    }
    drawFloor();
    glBegin(GL_LINE_STRIP);
    glLineWidth(20);                  //这个就涉及到tcf值得标定问题

    for(int i = 0; i < trailPoints.size(); i++)
    {
         qglColor(trailPoints[i].color);
         if(isHideLine)
          {
             if(trailPoints[i].color != Qt::white)
             {
              if(i > 0 && trailPoints[i-1].color == Qt::white)
              {
                  glEnd();
                  glBegin(GL_LINE_STRIP); //平移线
                  glVertex3f(trailPoints[i].x,trailPoints[i].y,trailPoints[i].z);
              }
              else
              glVertex3f(trailPoints[i].x,trailPoints[i].y,trailPoints[i].z);
             }
          }
         else
         glVertex3f(trailPoints[i].x,trailPoints[i].y,trailPoints[i].z);
    }
    glEnd();
    for(int i = 0; i < num; i++)       //5的关节类型调整到stl文件中
    {
     switch(i)
     {
        case 1: glRotatef(j1Rot,0.0,0.0,1.0); break;
        case 2:
                glTranslatef(a2 / scaleNum,0.0,0.0);
                glRotatef(j2Rot, 0.0, 0.0, 1.0);
                glTranslatef(-a2 / scaleNum,0.0,0.0);
                break;
        case 3:
                 glTranslatef((a2 + a3) / scaleNum,0.0,0.0);
                 glRotatef( j4Rot, 0.0, 0.0, 1.0 );
                 glTranslatef(-(a2 + a3) / scaleNum,0.0,0.0);
                 glTranslatef(0,0,j3Rot / scaleNum);
                break;
        case 5:
               glTranslatef((a2 + a3) / scaleNum,0.0,d3 / scaleNum);
               glRotatef( j5Rot, 0.0, 1.0, 0.0 );
               glTranslatef(-(a2 + a3) / scaleNum,0.0,-d3 / scaleNum);
               break;
     }
     switch (viewmode)
     {
     case NOHIDE:
         glCallList(gllist[i]);
         break;
     case HIDEROBOT:
         if(i == num - 1)
         glCallList(gllist[num-1]);
         break;
     case HIDEROBOTANDTORCH:
         break;
     default:
         break;
     }
    }
}

//导入工件(加载stl文件)
void OpenglWidget::loadArtifacts(const QString& ArtifactsName,bool flag)
{
    qDebug()<<"loadArtifacts";
   //加载文件时,重新读取数据的过程
   //if(!artifacts)
   if(artifacts)
   {
       delete artifacts;
       artifacts = NULL;
   }

   //artifacts = new STLView;
   artifacts=STLView::init();
   artifacts->readSTL(ArtifactsName);

   //flag是否为导入工件，还是初始化上一次的stl文件
   if(flag){
    initializeGL();
    updateGL();
   }
}

void OpenglWidget::drawLanxun6JointRobot()
{
    glTranslatef( -0.4, -1.0, 0 );    //调整到中间位置
    glTranslatef( 0, 0, zoom );       //调整与屏幕z轴的距离
    glRotatef( -90,  1.0, 0.0, 0.0 );
    glRotatef( xRot, 1.0, 0.0, 0.0 );
    glRotatef( yRot, 0.0, 0.0, 1.0 );
    if(isShowWorkPiece && artifacts)
    {
      glTranslatef(wpxTrans / scaleNum,wpyTrans / scaleNum,wpzTrans / scaleNum);
      glCallList(gllist[num]);
      glTranslatef(-wpxTrans / scaleNum,-wpyTrans / scaleNum,-wpzTrans / scaleNum);
    }
    drawFloor();
    glBegin(GL_LINE_STRIP);
    glLineWidth(20);                  //这个就涉及到tcf值得标定问题
    for(int i = 0; i < trailPoints.size(); i++)
    {
         qglColor(trailPoints[i].color);
         if(isHideLine)
          {
             if(trailPoints[i].color != Qt::white)
             {
              if(i > 0 && trailPoints[i-1].color == Qt::white)
              {
                  glEnd();
                  glBegin(GL_LINE_STRIP);
                  glVertex3f(trailPoints[i].x,trailPoints[i].y,trailPoints[i].z);
              }
              else
              glVertex3f(trailPoints[i].x,trailPoints[i].y,trailPoints[i].z);
             }
          }
         else
         glVertex3f(trailPoints[i].x,trailPoints[i].y,trailPoints[i].z);
    }
    glEnd();
    for(int i = 0; i < 7; i++)       //5的关节类型调整到stl文件中
    {
     switch(i)
     {
        case 1:
                glTranslatef(0,0,d1 / scaleNum);
                glRotatef( j1Rot, 0.0, 0.0, 1.0 );
                glTranslatef(0,0,-d1 / scaleNum);
                break;
        case 2:
                glTranslatef(a2 / scaleNum,0,d1 / scaleNum);
                glRotatef( j2Rot, 0.0, 1.0, 0.0 );
                glTranslatef(-a2 / scaleNum,0,-d1 / scaleNum);
                break;
        case 3:
                glTranslatef(a2 / scaleNum,0,(d1 + a3) / scaleNum);
                glRotatef( j3Rot, 0.0, 1.0, 0.0 );
                glTranslatef(-a2 / scaleNum,0,-(d1 + a3) / scaleNum);
                break;
        case 4:
                glTranslatef(a2 / scaleNum,0,(d1 + a3 + a4) / scaleNum);
                glRotatef( j4Rot, 1.0, 0.0, 0.0 );
                glTranslatef(-a2 / scaleNum,0,-(d1 + a3 + a4) / scaleNum);
                break;
        case 5:
                glTranslatef((a2 + d4) / scaleNum,0,(d1 + a3 + a4) / scaleNum);
                glRotatef( j5Rot - 90, 0.0, 1.0, 0.0 );
                glTranslatef(-(a2 + d4) / scaleNum,0,-(d1 + a3 + a4) / scaleNum);
                break;
        case 6:
                glTranslatef((a2 + d4) / scaleNum,0,(d1 + a3 + a4) / scaleNum);
                glRotatef( j6Rot, 0.0, 0.0, 1.0 );
                glTranslatef(-(a2 + d4) / scaleNum,0,-(d1 + a3 + a4) / scaleNum);
                break;
     }
     switch (viewmode)
     {
     case NOHIDE:
         glCallList(gllist[i]);
         break;
     case HIDEROBOT:
         if(i == num - 1)
         glCallList(gllist[num-1]);
         break;
     case HIDEROBOTANDTORCH:
         break;
     default:
         break;
     }
    }
    qDebug()<<"loadArtifacts";
}

void OpenglWidget::drawFloor()
{
    int ss = 0;
    const int NN= 11;
    double x1,y1,x2,y2;
    switch(glType)
    {
       case LanxunCoordRobot:
       x1 = - 0.4;y1 = -2.5; x2 = 3.2; y2 = 2.5;
       break;
       case Lanxun5JointRobot:
       x1 = - 2.0;y1 = -2.5; x2 = 2.2; y2 = 2.5;
       break;
       case Lanxun6JointRobot:
       x1 = - 1.8;y1 = -2.5; x2 = 2.2; y2 = 2.5;
       break;
    }
     for(int i = 0; i < NN;i++)
         for(int j = 0; j < NN;j++)
         {
             ss++;
             if(ss % 2 == 0)
             glColor3f(204 / 255.0 ,104 / 255.0,57 / 255.0);
             else
             glColor3f(204 / 255.0 ,204 / 255.0,157 / 255.0);
             glRectf(x1 + (x2 - x1) / NN * i,y1 + (y2 - y1) / NN * j,x1 + (x2 - x1) / NN * (i+1),y1 + (y2 - y1)  / NN * (j + 1));
         }
}

void OpenglWidget::drawLanxunCoordRobot()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //因为坐标式模型的基座坐标定义在模型的左下角，所以先让坐标往左下角移动，才能整个模型显示在窗口中心
    glTranslatef( -1.4, -1.4, zoom );

    //定义视窗观看的角度，向下20°观看模型
    glRotatef( -70,  1.0, 0.0, 0.0 );
    //glRotatef( 30,  0.0, 0.0, 1.0 );

    //xRot用于记录用户的操作角度
    glRotatef( xRot, 1.0, 0.0, 0.0 );
    glRotatef( yRot, 0.0, 0.0, 1.0 );

    //判断是否显示工件 && 是否已经生成工件文件解析类
    if(isShowWorkPiece && artifacts)
    {
      glTranslatef(wpxTrans / scaleNum,wpyTrans / scaleNum,wpzTrans / scaleNum);
      glCallList(gllist[num]);
      glTranslatef(-wpxTrans / scaleNum,-wpyTrans / scaleNum,-wpzTrans / scaleNum);
    }
    drawFloor();
     for(int i = 0; i < num; i++)
     {
      switch(i)
      {
           case 0: glTranslatef( 0.0, j2Rot / scaleNum, 0.0 ); break; //往Z轴方向移动吧
           case 1: glTranslatef( j1Rot / scaleNum,0.0,  0.0 ); break;
           case 2: glTranslatef( 0.0,  0.0, j3Rot / scaleNum);
                   glTranslatef(1045 / scaleNum,-105 / scaleNum,0.0);
                   glRotatef( j4Rot, 0.0, 0.0, 1.0 );
                   glTranslatef(-1045 / scaleNum,105 / scaleNum,0.0);
                   break;
           case 4:
                glTranslatef(1045 / scaleNum,0.0,400 / scaleNum);
                glRotatef( j5Rot, 0.0, 1.0, 0.0 );
                glTranslatef(-(1045 / scaleNum),0.0,-400 / scaleNum);
                break;
      }
      switch (viewmode)
      {
      case NOHIDE:
          glCallList(gllist[i]);
          break;
      case HIDEROBOT:
          if(i == num - 1)
          glCallList(gllist[num-1]);
          break;
      case HIDEROBOTANDTORCH:
          break;
      default:
          break;
      }
    }
     glLoadIdentity();
     glTranslatef( -1.4, -1.4, 0 );
     glTranslatef( 0, 0, zoom );   //调整与屏幕z轴的距离
     glRotatef( -70,  1.0, 0.0, 0.0 );
 //    glRotatef( 30,  0.0, 0.0, 1.0 );
     glRotatef( xRot, 1.0, 0.0, 0.0 );     //
//     glRotatef( yRot, 0.0, 0.0, -1.0 );
      glTranslatef(1045 / scaleNum,0.0,400 / scaleNum);
//     glTranslatef(150 / scaleNum,0.0,400 / scaleNum);
      glBegin(GL_LINE_STRIP);
      glLineWidth(20);
      for(int i = 0; i < trailPoints.size(); i++)
      {
           qglColor(trailPoints[i].color);
           if(isHideLine)
            {
                if(trailPoints[i].color != Qt::white)
                {
                 if(i > 0 && trailPoints[i-1].color == Qt::white)
                 {
                     glEnd();
                     glBegin(GL_LINE_STRIP);
                     glVertex3f(trailPoints[i].x,trailPoints[i].y,trailPoints[i].z);
                 }
                 else
                 glVertex3f(trailPoints[i].x,trailPoints[i].y,trailPoints[i].z);
                }
           }
           else
            glVertex3f(trailPoints[i].x,trailPoints[i].y,trailPoints[i].z);
      }
      glEnd();

}

void OpenglWidget::makeObject()
{
    qDebug()<<"makeObject";
   double div = scaleNum;
   int listNum = 0;

   //如果没有工件，就少一个显示列表
   /*if(artifacts == NULL){
      listNum = num;
   }else{
      listNum = num + 1;
   }*/
   listNum=(!artifacts)?num:++num;

/**************************************************************************
   OpenGL中用正整数来区分不同的显示列表，为防止重复定义已经存在的显示列表号，
   使用glGenLists函数来自动分配一个没有被使用过的显示列表编号。
   参数指定要分配几个显示列表，返回值是被分配的显示列表中的最小编号，若返回0表示分配失败
**************************************************************************/
   gllist[0] = glGenLists(listNum);

   for(int i = 1; i < listNum; i++){
       gllist[i] = gllist[0] + i;
   }

   for(int n = 0; n < num; n++){
       //开始装入：第一个参数标示当前正在操作的显示列表号
       //第二个参数有两种取值GL_COMPILE和GL COMPILE_AND_EXECUTE，
       //前者声明当前显示列表只是装入相应OpenGL语句，不执行；后者表示在装入的同时，执行一遍当前显示列表。
       glNewList(gllist[n], GL_COMPILE );

       qglColor(color[n]);
       glBegin(GL_TRIANGLES);
       for(int i = 0; i < (view[n]).triangleVector.size();i++) {
           glNormal3f((view[n]).triangleVector[i].normal.x  , (view[n]).triangleVector[i].normal.y ,(view[n]).triangleVector[i].normal.z);
           glTexCoord2f(0, 0);
           glVertex3f((view[n]).triangleVector[i].vertex1.x / div ,(view[n]).triangleVector[i].vertex1.y/ div ,
                      (view[n]).triangleVector[i].vertex1.z / div );
           glTexCoord2f( 0.5, 1.0 );
           glVertex3f((view[n]).triangleVector[i].vertex2.x / div ,(view[n]).triangleVector[i].vertex2.y / div,
                      (view[n]).triangleVector[i].vertex2.z / div );
           glTexCoord2f( 1.0, 0 );
           glVertex3f((view[n]).triangleVector[i].vertex3.x / div,(view[n]).triangleVector[i].vertex3.y / div ,
                      (view[n]).triangleVector[i].vertex3.z / div );
       }
       glEnd();
       glEndList();
   }

   if(artifacts){
       //创建显示列表并付给list
       glNewList(gllist[num], GL_COMPILE );
       qglColor(color[num]);
/**************************************************************************
* glNormal3f设置当前法线数组
* glTexCoord2f绘制图形时指定纹理的坐标，为了将纹理正确的映射到四边形上，必须将纹理的四个角映射到四边形的四个角
* glVertex3f指定3D顶点，一般与glTexCoord2f同时使用，先纹理后顶点
* glBegin(GL_TRIANGLES)多组独立填充三角形
**************************************************************************/
       glBegin(GL_TRIANGLES);

       for(MyTriangle it: artifacts->triangleVector) {
           glNormal3f(it.normal.x,it.normal.y ,it.normal.z);
           //左下角的纹理与顶点映射
           glTexCoord2f(0, 0);
           glVertex3f(it.vertex1.x / div ,it.vertex1.y/ div ,it.vertex1.z / div );
           //中间点
           glTexCoord2f( 0.5, 1.0 );
           glVertex3f(it.vertex2.x / div ,it.vertex2.y / div,it.vertex2.z / div );
           //右下角
           glTexCoord2f( 1.0, 0 );
           glVertex3f(it.vertex3.x / div,it.vertex3.y / div ,it.vertex3.z / div );
       /*
       for(int i = 0; i < artifacts->triangleVector.size();i++) {
           glNormal3f(artifacts->triangleVector[i].normal.x  , artifacts->triangleVector[i].normal.y ,artifacts->triangleVector[i].normal.z);
           //左下角的纹理与顶点映射
           glTexCoord2f(0, 0);
           glVertex3f(artifacts->triangleVector[i].vertex1.x / div ,artifacts->triangleVector[i].vertex1.y/ div ,
                      artifacts->triangleVector[i].vertex1.z / div );
           //中间点
           glTexCoord2f( 0.5, 1.0 );
           glVertex3f(artifacts->triangleVector[i].vertex2.x / div ,artifacts->triangleVector[i].vertex2.y / div,
                      artifacts->triangleVector[i].vertex2.z / div );
           //右下角
           glTexCoord2f( 1.0, 0 );
           glVertex3f(artifacts->triangleVector[i].vertex3.x / div,artifacts->triangleVector[i].vertex3.y / div ,
                      artifacts->triangleVector[i].vertex3.z / div );
       */
       }

       glEnd();
       glEndList();
   }
   qDebug()<<"makeObject";

}

void OpenglWidget::setText()
{
  QFont ft("文泉驿雅白",12);
  int w = width() / 5.0;
  int h = height() / 9.0;
  int h1 = height() - 20;
  int l = width() - 130;
  int lr = width() / 60;
  int hr = height() / 25;
  qglColor(Qt::cyan);
  switch(glType)
  {
    case LanxunCoordRobot:
    if(widgetname == NONAME)
    renderText(lr, hr,"坐标式焊接机器人",ft);
    else if(widgetname == TEACH)
    renderText(lr, hr,"坐标式焊接机器人(示教)",ft);
    else if(widgetname == SIMULATION)
    renderText(lr, hr,"坐标式焊接机器人(仿真)",ft);
    else if(widgetname == ALIGNMENT)
    renderText(lr, hr,"坐标式焊接机器人(联调)",ft);
    break;
   case Lanxun5JointRobot:
   if(widgetname == NONAME)
   renderText(lr, hr,"五关节焊接机器人",ft);
   else if(widgetname == TEACH)
   renderText(lr, hr,"五关节焊接机器人(示教)",ft);
   else if(widgetname == SIMULATION)
   renderText(lr, hr,"五关节焊接机器人(仿真)",ft);
   else if(widgetname == ALIGNMENT)
   renderText(lr, hr,"五关节焊接机器人(联调)",ft);
   break;
  case Lanxun6JointRobot:
  if(widgetname == NONAME)
  renderText(lr, hr,"六关节焊接机器人",ft);
  else if(widgetname == TEACH)
  renderText(lr, hr,"六关节焊接机器人(示教)",ft);
  else if(widgetname == SIMULATION)
  renderText(lr, hr,"六关节焊接机器人(仿真)",ft);
  else if(widgetname == ALIGNMENT)
  renderText(lr, hr,"六关节焊接机器人(联调)",ft);
  break;
  }
  qglColor(Qt::white);
  switch(mode)
  {
   case ShowMode:
        renderText(l, height() / 12.0,"演示模式",ft);
        break;
   case FireMode:
        renderText(l, height() / 12.0,"焊接模式",ft);
        break;
  }
  if(mode != NoMode)
  {
       renderText(l, height() / 8.0,QString("进度：%1%").arg(percent),ft);
       renderText(l, height() / 6.0,QString("倍率：%1%").arg(speedRate),ft);
  }
  renderText(w,h,"X:",ft);
  renderText(w,h + 20,"Y:",ft);
  renderText(w,h + 40,"Z:",ft);
  int ss = 60;
  if(glType == Lanxun6JointRobot)
  {
   renderText(w,h+60,"R:",ft);
   ss = 80;
  }
  renderText(w,h + ss,"P:",ft);
  renderText(w,h + ss+20,"W:",ft);
//  renderText(w,h + ss+40,"U:",ft);
//  renderText(w,h + ss+60,"V:",ft);
  int wc = w + 25;
  renderText(wc,h,QString::number(coord[0]),ft);
  renderText(wc,h + 20,QString::number(coord[1]),ft);
  renderText(wc,h + 40,QString::number(coord[2]),ft);
  ss = 60;
  if(glType == Lanxun6JointRobot)
  {
   renderText(wc,h+60,QString::number(coord[3]),ft);
   ss = 80;
  }
  renderText(wc,h + ss,QString::number(coord[4]),ft);
  renderText(wc,h + ss+20,QString::number(coord[5]),ft);
// renderText(wc,h + ss+40,QString::number(coord[6]),ft);
// renderText(wc,h + ss+60,QString::number(coord[7]),ft);
  QFont ft1("文泉驿雅白",8);
  renderText(w / 9.0,h1,"FPS:",ft1);
  renderText(w / 9.0 + 30,h1,QString::number(Fps),ft1);
}

//后面阶段用按键实现
void  OpenglWidget::keyPressEvent(QKeyEvent * e)
{
    switch(e->key())
    {
     case Qt::Key_L:
      light =! light;
      if(!light)
         glDisable(GL_LIGHTING);
      else
         glEnable(GL_LIGHTING);
      updateGL();
        break;
    case Qt::Key_Escape:
        trailPoints.clear();
        updateGL();
        break;
    case Qt::Key_1:
        j1Rot += 1;
        updateGL();
        break;
    case Qt::Key_2:
        j1Rot -= 1;
        updateGL();
        break;
    case Qt::Key_3:
        j2Rot += 1;
        updateGL();
        break;
    case Qt::Key_4:
        j2Rot -= 1;
        updateGL();
        break;
    case Qt::Key_5:
        j3Rot += 1;
        updateGL();
        break;
    case Qt::Key_6:
        j3Rot -= 1;
        updateGL();
        break;
    case Qt::Key_7:
        j4Rot += 1;
        updateGL();
        break;
    case Qt::Key_8:
        j4Rot -= 1;
        updateGL();
        break;
    case Qt::Key_9:
        j5Rot += 1;
        updateGL();
        break;
    case Qt::Key_0:
        j5Rot -= 1;
        updateGL();
        break;
    case Qt::Key_W:
        c1Rot += 1;
        updateGL();
        break;
    case Qt::Key_X:
        c1Rot -= 1;
        updateGL();
        break;
    case Qt::Key_A:
        c2Rot += 1;
        updateGL();
        break;
    case Qt::Key_D:
        c2Rot -= 1;
        updateGL();
        break;
    case Qt::Key_Up:
        if(e->modifiers() == Qt::ControlModifier)
        wpyTrans += 100;
        else if(e->modifiers() == Qt::AltModifier)
        wpyTrans += 1;
        else
        wpyTrans += 10;
        updateGL();
        break;
    case Qt::Key_Down:
        if(e->modifiers() == Qt::ControlModifier)
        wpyTrans -= 100;
        else if(e->modifiers() == Qt::AltModifier)
        wpyTrans -= 1;
        else
        wpyTrans -= 10;
        updateGL();
        break;
    case Qt::Key_Left:
        if(e->modifiers() == Qt::ControlModifier)
        wpxTrans -= 100;
        else if(e->modifiers() == Qt::AltModifier)
        wpxTrans -= 1;
        else
        wpxTrans -= 10;
        updateGL();
        break;
    case Qt::Key_Right:
        if(e->modifiers() == Qt::ControlModifier)
        wpxTrans += 100;
        else if(e->modifiers() == Qt::AltModifier)
        wpxTrans += 1;
        else
        wpxTrans += 10;
        updateGL();
        break;
    }
    return QGLWidget::keyPressEvent(e);
}


void OpenglWidget::mousePressEvent(QMouseEvent *event)
{
    //储存按下鼠标的位置
    lastPos = event->pos();
}


void OpenglWidget::mouseMoveEvent(QMouseEvent *event)
{
     //计算鼠标移动后的坐标与之前按下鼠标的坐标的相对位置
     GLfloat dx = GLfloat(event->x() - lastPos.x()) / width();
     GLfloat dy = GLfloat(event->y() - lastPos.y()) / height();
     if(event->buttons() & Qt::LeftButton)
     {

         xRot += 180 * dy;
         //yRot += 180 * dx;
         updateGL();
     }
     else if(event->buttons() & Qt::RightButton)
     {
         //xRot += 180 * dy;
         yRot += 180 * dx;
         updateGL();
     }
     lastPos = event->pos();

}


void OpenglWidget::mouseDoubleClickEvent(QMouseEvent *event)
{

}

void OpenglWidget::wheelEvent(QWheelEvent *event)
 {
        if((zoom < -14 && event->delta() < 0) || (zoom >-7 && event->delta() > 0))
            return;
        zoom += event->delta() / 300.0;
        updateGL();
 }

void OpenglWidget::loadGLTextures()
{
  QImage tex,buf;
  if(!buf.load("D:\\bricks.jpg"))
  {
      qWarning("Could not read image file, using single-color instead.");
      QImage dummy( 128, 128, QImage::Format_RGB32 );
      dummy.fill(Qt::red);
      buf = dummy;
  }
 tex = QGLWidget::convertToGLFormat(buf);
 glGenTextures(1, &texture[0]);
 glBindTexture(GL_TEXTURE_2D, texture[0]);
 glTexImage2D(GL_TEXTURE_2D, 0, 3, tex.width(),tex.height(),0,GL_RGBA,GL_UNSIGNED_BYTE, tex.bits());
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

}
double  OpenglWidget::callFrequency()
{
    static int count;
    static double save;
    static clock_t last,current;
    double timegap;
    ++count;
    if(count <= 50)
        return save;
    count = 0;
    last = current;
    current = clock();
    timegap = (current - last) / (double)CLK_TCK;
    save = 50.0 / timegap;
    return save;
}


void OpenglWidget::addTrialPoints()
{
    MyPoint1 point;
    point.x = coord[0] / scaleNum;
    point.y = coord[1] / scaleNum;
    point.z = coord[2] / scaleNum;
    point.color = linecolor;
    trailPoints.push_back(point);
}
void OpenglWidget::decTrialPoints()
{
    if(!trailPoints.isEmpty())   //如果不为空就减少一个
    trailPoints.pop_back();
}
void OpenglWidget::init()
{
    xRot = 0;
    yRot = 0;
    zRot = 0.0;
    j1Rot = j2Rot = j4Rot = j3Rot = j5Rot = j6Rot = 0;
    c1Rot = c2Rot = 0;
    zoom = -10;
    updateGL();
}
bool OpenglWidget::amplification()
{
    zoom += 0.4;
    updateGL();
    if(zoom > -6.4)
        return false;
    else
        return true;
}
bool OpenglWidget::NarrowDown()
{
    zoom -= 0.4;
    updateGL();
    if(zoom < -14)
        return false;
    else
        return true;
}
void OpenglWidget::reduce()
{
    zoom = -10;
    xRot = 0;
    yRot = 0;
    zRot = 0.0;
    updateGL();
}
void OpenglWidget::xRotateadd()
{
     xRot += 10;
     updateGL();
}
void OpenglWidget::xRotatedec()
{
    xRot -= 10;
    updateGL();
}
void OpenglWidget::yRotateadd()
{
    yRot += 10;
    updateGL();
}
void OpenglWidget::yRotatedec()
{
    yRot -= 10;
    updateGL();
}

void OpenglWidget::changeViewMode(ViewMode mode)
{
    viewmode = mode;
    updateGL();
}
bool OpenglWidget::changeIsHideLine()
{
    isHideLine = !isHideLine;
    updateGL();
    return isHideLine;
}
void OpenglWidget::getCurrentCoord()
{
    Joint j;
    if(glType == LanxunCoordRobot)
    {
       j.j1 = j1Rot;
       j.j2 = j2Rot;
    }
    else
    {
        j.j1 = j1Rot * Pi / 180.0;
        j.j2 = j2Rot * Pi / 180.0;
    }
    if(glType == Lanxun6JointRobot)
    j.j3 = j3Rot * Pi / 180;
    else
    j.j3 = j3Rot;
    j.j4 = j4Rot * Pi / 180.0;
    j.j5 = j5Rot * Pi / 180.0;
    j.j6 = j6Rot * Pi / 180.0;
    ArrayXd xyzrpw = pose_2_xyzrpw(NewScaraArgorithmn::fksolution(j));
    for(int i = 0;i < 6;i++)
        coord[i] = CommonFun::round(xyzrpw[i],2);
   //addTrialPoints();
     updateGL();
}
void OpenglWidget::home()
{
     j1Rot = j2Rot = j4Rot = j3Rot = j5Rot = j6Rot = 0;
     trailPoints.clear();
     getCurrentCoord();

}
void OpenglWidget::getCurrentJ(Joint& j)
{
    if(glType == LanxunCoordRobot)
    {
       j.j1 = j1Rot;
       j.j2 = j2Rot;
    }
    else
    {
        j.j1 = j1Rot * Pi / 180.0;
        j.j2 = j2Rot * Pi / 180.0;
    }
    if(glType == Lanxun6JointRobot)
    j.j3 = j3Rot * Pi / 180;
    else
    j.j3 = j3Rot;
    j.j4 = j4Rot * Pi / 180.0;
    j.j5 = j5Rot * Pi / 180.0;
    j.j6 = j6Rot * Pi / 180.0;
}
void OpenglWidget::setJ(const Joint& j)
{
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
    if(glType == Lanxun6JointRobot)
        j3Rot = j.j3 * 180 / PI;
    else
        j3Rot = j.j3;
    j4Rot = j.j4 * 180 / PI;
    j5Rot = j.j5 * 180 / PI;
    j6Rot = j.j6 * 180 / PI;
    ArrayXd xyzrpw = ArrayXd::Zero(6);
    xyzrpw = pose_2_xyzrpw(NewScaraArgorithmn::fksolution(j));
    for(int i = 0;i < 6;i++)
        coord[i] = CommonFun::round(xyzrpw[i],2);
//  addTrialPoints();
    updateGL();
}
