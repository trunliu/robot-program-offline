#include "AnalysisWidget.h"
#include "ui_AnalysisWidget.h"
#include <QDateTime>
#include "Setting/Parameter.h"
#define Plot_Count 10
#define TextColor1 QColor(255,255,255)         //黄色
#define TextColor QColor(255,255,0)         //黄色
#define Plot_NoColor QColor(0,0,0,0)        //黑色

#define Plot1_DotColor QColor(5,189,251)
#define Plot1_LineColor QColor(41,138,220)
#define Plot1_BGColor QColor(41,138,220,80)

#define Plot2_DotColor QColor(236,110,0)
#define Plot2_LineColor QColor(246,98,0)
#define Plot2_BGColor QColor(246,98,0,80)

#define Plot3_DotColor QColor(204,0,0)
#define Plot3_LineColor QColor(246,0,0,200)
#define Plot3_BGColor QColor(246,98,0,80)
#define TextWidth 2
#define TextWidth1 1
#define LineWidth 2
#define DotWidth  0
AnalysisWidget::AnalysisWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AnalysisWidget)
{
    ui->setupUi(this);
    isSpeedAna = false;
    isFirst = true;
    color[0] = Qt::cyan;
    color[1] = Qt::red;
    color[2] = Qt::blue;
    color[3] = Qt::yellow;
    color[4] = Qt::green;
    color[5] = Qt::magenta;
    color[6] = Qt::white;
    color[7] = Qt::gray;
    color[8] = Qt::lightGray;
    for(int i = 0; i < 6;i++)
    {
        lastJoint[i] = 0;
        lastXyzrpw[i] = 0;
    }
    initPlot();
}

AnalysisWidget::~AnalysisWidget()
{
    delete ui;
}
void AnalysisWidget::init()
{
    ui->xyzrpw_plot->legend->setVisible(true); //设置图例可以显示
    ui->joint_plot->legend->setVisible(true);  //设置关节显示

    ui->xyzrpw_plot->xAxis->setTickLabelFont(QFont(QFont().family(),5)); //将显示时间可以显示小一点
    ui->joint_plot->xAxis->setTickLabelFont(QFont(QFont().family(),5));  //显示时间显示小一点

    ui->joint_plot->yAxis->setLabelColor(TextColor);
    ui->joint_plot->yAxis->setTickLabelColor(TextColor);  //Y轴标签颜色
    ui->joint_plot->yAxis2->setLabelColor(TextColor);
    ui->joint_plot->yAxis2->setTickLabelColor(TextColor);  //Y轴标签颜色

    ui->joint_plot->xAxis->setTickLabelColor(TextColor);
    ui->joint_plot->xAxis->setBasePen(QPen(TextColor,TextWidth));
    ui->joint_plot->yAxis->setBasePen(QPen(TextColor,TextWidth));
    ui->joint_plot->xAxis->setBasePen(QPen(TextColor,TextWidth));
    ui->joint_plot->yAxis2->setBasePen(QPen(TextColor,TextWidth));


    ui->joint_plot->xAxis->setTickPen(QPen(TextColor,TextWidth));
    ui->joint_plot->yAxis->setTickPen(QPen(TextColor,TextWidth1));
    ui->joint_plot->yAxis2->setTickPen(QPen(TextColor,TextWidth1));


    ui->joint_plot->xAxis->setSubTickPen(QPen(TextColor,TextWidth));
    ui->joint_plot->yAxis->setSubTickPen(QPen(TextColor,TextWidth));
    ui->joint_plot->yAxis2->setSubTickPen(QPen(TextColor,TextWidth));



    ui->xyzrpw_plot->yAxis->setLabelColor(TextColor);
    ui->xyzrpw_plot->yAxis->setTickLabelColor(TextColor);  //Y轴标签颜色

    ui->xyzrpw_plot->yAxis2->setLabelColor(TextColor);
    ui->xyzrpw_plot->yAxis2->setTickLabelColor(TextColor);  //Y轴标签颜色
    ui->xyzrpw_plot->xAxis->setTickLabelColor(TextColor);

    ui->xyzrpw_plot->xAxis->setBasePen(QPen(TextColor,TextWidth));
    ui->xyzrpw_plot->yAxis->setBasePen(QPen(TextColor,TextWidth));
    ui->xyzrpw_plot->yAxis2->setBasePen(QPen(TextColor,TextWidth));


    ui->xyzrpw_plot->xAxis->setTickPen(QPen(TextColor,TextWidth));
    ui->xyzrpw_plot->yAxis->setTickPen(QPen(TextColor,TextWidth));
    ui->xyzrpw_plot->yAxis2->setTickPen(QPen(TextColor,TextWidth));

    ui->xyzrpw_plot->xAxis->setSubTickPen(QPen(TextColor,TextWidth));
    ui->xyzrpw_plot->yAxis->setSubTickPen(QPen(TextColor,TextWidth));
    ui->xyzrpw_plot->yAxis2->setSubTickPen(QPen(TextColor,TextWidth));

    //设置画布背景
    QLinearGradient plotGradient;
    plotGradient.setStart(0,0);
    plotGradient.setFinalStop(0,350);
    plotGradient.setColorAt(0,QColor(80,80,80));
    plotGradient.setColorAt(1,QColor(50,50,50));
    ui->joint_plot->setBackground(plotGradient);
    ui->xyzrpw_plot->setBackground(plotGradient);
    //设置坐标背景色
    QLinearGradient axisRectGradient;
    axisRectGradient.setStart(0,0);
    axisRectGradient.setFinalStop(0,350);
    axisRectGradient.setColorAt(0,QColor(80,80,80));
    axisRectGradient.setColorAt(1,QColor(30,30,30));
    ui->joint_plot->axisRect()->setBackground(axisRectGradient);
    ui->xyzrpw_plot->axisRect()->setBackground(axisRectGradient);

    //设置图例提示位置以及背景色
    ui->joint_plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignRight);
    ui->joint_plot->legend->setBrush(QColor(255, 255, 255, 200));
    ui->xyzrpw_plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignRight);
    ui->xyzrpw_plot->legend->setBrush(QColor(255, 255, 255, 200));
    //设置动态曲线的横坐标格式及范围
    ui->joint_plot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    ui->joint_plot->xAxis->setDateTimeFormat("hh:mm:ss");
    ui->joint_plot->xAxis->setAutoTickStep(false);
    ui->joint_plot->xAxis->setTickStep(1);
    ui->joint_plot->xAxis->setRange(0, Plot_Count, Qt::AlignRight);
    ui->xyzrpw_plot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    ui->xyzrpw_plot->xAxis->setDateTimeFormat("hh:mm:ss");
    ui->xyzrpw_plot->xAxis->setAutoTickStep(false);
    ui->xyzrpw_plot->xAxis->setTickStep(1);
    ui->xyzrpw_plot->xAxis->setRange(0, Plot_Count, Qt::AlignRight);

    //  ui->joint_plot->setInteractions(QCP::iRangeDrag |QCP::iRangeZoom);
    //  ui->xyzrpw_plot->setInteractions(QCP::iRangeDrag |QCP::iRangeZoom);
}

void AnalysisWidget::initReploy()
{
    ui->xyzrpw_plot->clearGraphs();   //清除数据
    ui->joint_plot->clearGraphs();
    if(!isSpeedAna)
    {
     ui->joint_plot->yAxis2->setVisible(true); //第二个轴可见
     ui->xyzrpw_plot->yAxis2->setVisible(true); //第二个轴可见
    }
    else
    {
       ui->joint_plot->yAxis2->setVisible(false); //第二个轴可见
       ui->xyzrpw_plot->yAxis2->setVisible(false); //第二个轴可见
    }
    QStringList  xyzrpwName,jointName;
    double limit;
    int num;
    switch(Parameter::robotType)
    {
       case LanxunCoordRobot: //p w a b
       xyzrpwName << "X" << "Y" << "Z" << "P" << "W";
       jointName << "X轴" << "Y轴" << "Z轴" << "A轴" << "B轴";
       num = 2;
       limit = 1400;
        if(!isSpeedAna)
        {
         for(int i = 0; i < 3; i++)
          ui->joint_plot->addGraph(ui->joint_plot->xAxis,ui->joint_plot->yAxis2);
          for(int i = 0; i < 2; i++)
          ui->joint_plot->addGraph(ui->joint_plot->xAxis,ui->joint_plot->yAxis);
        }
       break;
       case Lanxun5JointRobot: //p w j1 j2 j4 j5
       xyzrpwName << "X" << "Y" << "Z" << "P" << "W";
       jointName << "大臂" << "小臂" << "升降" << "旋转" << "摆枪";
       limit = 1400;
       num = 2;
       if(!isSpeedAna)
       {
          for(int i = 0; i < 2; i++)
          ui->joint_plot->addGraph(ui->joint_plot->xAxis,ui->joint_plot->yAxis);
          ui->joint_plot->addGraph(ui->joint_plot->xAxis,ui->joint_plot->yAxis2);
          for(int i = 0; i < 2; i++)
          ui->joint_plot->addGraph(ui->joint_plot->xAxis,ui->joint_plot->yAxis);
       }
       break;
       case Lanxun6JointRobot: //r p w j1 - j6
       xyzrpwName << "X" << "Y" << "Z" << "R" << "P" << "W";
       jointName <<"J1" << "J2" << "J3" << "J4" << "J5" << "J6";
       limit = 1400;
       num = 3;
       if(!isSpeedAna)
       {
          ui->joint_plot->yAxis2->setVisible(false);
          for(int i = 0; i < 6; i++)
          ui->joint_plot->addGraph(ui->joint_plot->xAxis,ui->joint_plot->yAxis);
       }
    }
    axisNum = 3 + num;
    if(!isSpeedAna)
    {
    for(int i = 0; i < 3; i++)
        ui->xyzrpw_plot->addGraph(ui->xyzrpw_plot->xAxis,ui->xyzrpw_plot->yAxis2);
    for(int i = 0; i < num;i++)
        ui->xyzrpw_plot->addGraph(ui->xyzrpw_plot->xAxis,ui->xyzrpw_plot->yAxis);
    }
    for(int i = 0; i <  axisNum;i++)
    {
      if(isSpeedAna)
      {
         ui->joint_plot->addGraph();
         ui->xyzrpw_plot->addGraph();
      }
      ui->xyzrpw_plot->graph(i)->setName(xyzrpwName[i]);
      ui->xyzrpw_plot->graph(i)->setPen(QPen(color[i],LineWidth));
      ui->xyzrpw_plot->graph(i)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,QPen(color[i], LineWidth),QBrush(color[i]), DotWidth));
      ui->joint_plot->graph(i)->setName(jointName[i]);
      ui->joint_plot->graph(i)->setPen(QPen(color[i],LineWidth));
      ui->joint_plot->graph(i)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,QPen(color[i], LineWidth),QBrush(color[i]), DotWidth));
    }
    if(isSpeedAna)
    {
        ui->joint_plot->yAxis->setLabel("关节速度(单位:度(mm)/min)");
        ui->joint_plot->yAxis->setRange(-10000,10000);
        //长度永远只是xyz
        ui->xyzrpw_plot->yAxis->setLabel("位姿速度(单位:度(mm)/min)");
//        ui->xyzrpw_plot->yAxis->setAutoTicks(false);
//        ui->xyzrpw_plot->yAxis->setAutoTickLabels(false);
        ui->xyzrpw_plot->yAxis->setRange(-10000,10000);
    }
    else
     {
         ui->joint_plot->yAxis->setLabel("关节角度(单位:°)");
         ui->joint_plot->yAxis->setRange(-180,180);
         ui->joint_plot->yAxis2->setLabel("移动距离(单位:mm)");
         ui->joint_plot->yAxis2->setRange(-limit,limit);
         //长度永远只是xyz
         ui->xyzrpw_plot->yAxis->setLabel("姿态角度(单位:°)");
//         ui->xyzrpw_plot->yAxis->setAutoTicks(false);
//         ui->xyzrpw_plot->yAxis->setAutoTickLabels(false);
         ui->xyzrpw_plot->yAxis->setRange(-180,180);
         ui->xyzrpw_plot->yAxis2->setLabel("坐标值(单位:mm)");
         ui->xyzrpw_plot->yAxis2->setRange(-limit,limit);
     }
}
void AnalysisWidget::initPlot()
{
    init();
    initReploy();
}
void AnalysisWidget::setData(double *joint,double *xyzrpw)
{
//    int time = updatetime.elapsed();
//    qDebug() << time;
//    updatetime.restart();
    int time = 50;
    double key = QDateTime::currentDateTime().toMSecsSinceEpoch() / 1000.0;
    for(int i  = 0; i <  axisNum;i++)
    {
       if(axisNum == 5)
       {
           for(int i = 0; i < 3;i++)
           {
            if(isSpeedAna)
            {
              if(!isFirst)
              ui->xyzrpw_plot->graph(i)->addData(key,(xyzrpw[i] - lastXyzrpw[i]) / (time / 1000.0 / 60));
            }
            else
            ui->xyzrpw_plot->graph(i)->addData(key,xyzrpw[i]);

            ui->xyzrpw_plot->graph(i)->removeDataBefore(key - Plot_Count - 1);
           }
           for(int i = 4; i < 6;i++)
           {
           if(isSpeedAna)
           {
             if(!isFirst)
             ui->xyzrpw_plot->graph(i-1)->addData(key,(xyzrpw[i] - lastXyzrpw[i]) / (time / 1000.0 / 60));
           }
           else
            ui->xyzrpw_plot->graph(i-1)->addData(key,xyzrpw[i]);
            ui->xyzrpw_plot->graph(i-1)->removeDataBefore(key - Plot_Count - 1);
           }
       }
       else
       {
           for(int i = 0; i < 6;i++)
           {
               if(isSpeedAna)
               {
                 if(!isFirst)
                 ui->xyzrpw_plot->graph(i)->addData(key,(xyzrpw[i] - lastXyzrpw[i]) / (time / 1000.0 / 60));
               }
               else
               ui->xyzrpw_plot->graph(i)->addData(key,xyzrpw[i]);
               ui->xyzrpw_plot->graph(i)->removeDataBefore(key - Plot_Count - 1);
           }
       }
       for(int i = 0; i < axisNum;i++)
       {
           if(isSpeedAna)
           {
             if(!isFirst)
             ui->joint_plot->graph(i)->addData(key,(joint[i] - lastJoint[i]) / (time / 1000.0 / 60));
           }
           else
           ui->joint_plot->graph(i)->addData(key,joint[i]);
           ui->joint_plot->graph(i)->removeDataBefore(key - Plot_Count - 1);
       }
    }
     ui->joint_plot->xAxis->setRange(key, Plot_Count , Qt::AlignRight);
     ui->xyzrpw_plot->xAxis->setRange(key, Plot_Count , Qt::AlignRight);
     ui->joint_plot->replot();
     ui->xyzrpw_plot->replot();
     isFirst = false;
     for(int i = 0; i < 6;i++)
     {
         lastJoint[i] = joint[i];
         lastXyzrpw[i] = xyzrpw[i];
     }
}

void AnalysisWidget::replot()
{
    initReploy();
    ui->joint_plot->replot();
    ui->xyzrpw_plot->replot();
}
void AnalysisWidget::switchAna(bool isSpeedAna)
{
    this->isSpeedAna = isSpeedAna;
    replot();
}
