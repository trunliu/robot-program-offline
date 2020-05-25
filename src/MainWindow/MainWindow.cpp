#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "CommonFun/CommonFun.h"
#include "Setting/ParaDlg.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();  //初始化
    setButtonStyle();   //设置按钮样式
    isfirstSimulation = false;   //第一次仿真的表现
    ui->simulation_button->setEnabled(false);
    checkSwitchStatus();
     curi = 0;
     lasti = -1;
     firei = 0;
     isback = false;
     lastStlFile.clear();
     lastStlFile = CommonFun::ReadSettingsStr("Para/lastStlFile");
     compileFileName = CommonFun::ReadSettingsStr("Para/compileFileName");
     last_cur_i = 1;
     isFinished = false;
     isCompile = false;
     isFirstCompile = false;
     connectflag = false;
     setGeometry(150,50,1600,900);
}
 void MainWindow::checkSwitchStatus()
 {
 if(codeEditDlg->checkSwitch())
     ui->fileSwitchAct->setEnabled(true);
 else
     ui->fileSwitchAct->setEnabled(false);
 }
MainWindow::~MainWindow()
{
    CommonFun::WriteSettingsStr("Para/lastStlFile",lastStlFile);
    CommonFun::WriteSettingsStr("Para/compileFileName",compileFileName);
    if(codeEditDlg)
        delete codeEditDlg;
    if(robot_udp)
        delete robot_udp;
    if(parse)
        delete parse;
    if(paradlg)
        delete paradlg;
    if(teachDlg)                //示教信息
        delete teachDlg;
    if(listdlg)
        delete listdlg;         //显示信息
    if(teachGLWidget)
        delete teachGLWidget;
    if(analysisWdg)
        delete analysisWdg;
    delete ui;
}

void MainWindow::init()
{
    robot_udp = NULL;
    listdlg = NULL;
    teachDlg = NULL;
    parse =  NULL;
    paradlg = NULL;
    teachGLWidget = NULL;
    analysisWdg = NULL;
    statusLabel =  new QLabel;
    ui->statusbar->addWidget(statusLabel);
    QFont font;
    font.setPointSize(8); // 设置字号
    statusLabel->setFont(font);  // 设置字体
    statusLabel->setText("就绪");
//    statusLabel->setStyleSheet("color:red");
    codeEditDlg = new CodeEditDlg;
    ui->stackedWidget->addWidget(codeEditDlg);
    ui->stackedWidget->setCurrentWidget(codeEditDlg);
    ui->dockWidget->setVisible(false);
    connectSlot();
    setSimulationStatus(false);
    ui->editText_button->setEnabled(false);
    ui->alignment_button->setEnabled(true);
    lastSimultype = Parameter::robotType;
}
//设置button颜色
 void MainWindow::setButtonStyle()
 {
     CommonFun::setToolButtonStyle(ui->alignment_button,"联调",1,":images/alignment.png");
     CommonFun::setToolButtonStyle(ui->editText_button,"编辑",1,":images/edit.png");
     CommonFun::setToolButtonStyle(ui->teach_button,"示教",1,":images/teach.png");
     CommonFun::setToolButtonStyle(ui->simulation_button,"运行",1,":images/simulation.png");
     CommonFun::setToolButtonStyle(ui->graph_button,"图库",1,":images/shape.png");
     CommonFun::setToolButtonStyle(ui->compile_button,"编译",1,":images/compile.png");
 }

//连接槽函数
 void MainWindow::connectSlot()
 {
     connect(ui->printAct,SIGNAL(triggered()),codeEditDlg,SLOT(printSlot()));
     connect(ui->exitAct,SIGNAL(triggered()),this,SLOT(close()));
     connect(ui->boldAct,SIGNAL(triggered()),codeEditDlg,SLOT(boldSlot()));
     connect(ui->colorAct,SIGNAL(triggered()),codeEditDlg,SLOT(colorSlot()));
     connect(ui->fontAct,SIGNAL(triggered()),codeEditDlg,SLOT(fontSlot()));
     connect(ui->fileSwitchAct,SIGNAL(triggered()),codeEditDlg,SLOT(switchSlot()));
 }
void  MainWindow::keyPressEvent(QKeyEvent * e)
{
    if(ui->stackedWidget->currentWidget() == teachGLWidget)
        return teachGLWidget->keyPressEvent(e);

}
 void MainWindow::closeEvent(QCloseEvent * e)
 {
    if(parse)
    parse->close();
    if(teachGLWidget)
        teachGLWidget->close();
    if(robot_udp)
        robot_udp->close();
   if(teachDlg)
     teachDlg->close();
   if(listdlg)
     listdlg->close();
    return codeEditDlg->closeEvent(e);
 }
void MainWindow::on_editText_button_clicked()
{
    ui->editText_button->setEnabled(false);
    setSimulation_buttonTrue();
    ui->alignment_button->setEnabled(true);
    if(!ui->teach_button->isEnabled())
        ui->teach_button->setEnabled(true);
    ui->stackedWidget->setCurrentWidget(codeEditDlg);
    ui->compile_button->setEnabled(true);
    ui->dockWidget->setWidget(NULL);
    ui->dockWidget->setVisible(false);
    setSimulationStatus(false);
    setSimulationBtnBool(false);
    statusLabel->setText("就绪");
}
void MainWindow::setSimulationBtnBool(bool flag)
{
    ui->hideLineAct->setEnabled(flag);
    ui->hideRobotAct->setEnabled(flag);
    ui->addAct->setEnabled(flag);
    ui->reduceAct->setEnabled(flag);
    ui->modelSwitchAct->setEnabled(flag);
}
void MainWindow::setSimulationStatus(bool flag)
{
    ui->hideLineAct->setEnabled(flag);
    ui->playAct->setEnabled(flag);
    ui->resetAct->setEnabled(flag);
    ui->pauseAct->setEnabled(flag);
    ui->backAct->setEnabled(flag);
    ui->addAct->setEnabled(flag);
    ui->reduceAct->setEnabled(flag);
    ui->amplificationAct->setEnabled(flag);
    ui->narrowdownAct->setEnabled(flag);
    ui->reductionAct->setEnabled(flag);
    ui->modelSwitchAct->setEnabled(flag);
    ui->hideRobotAct->setEnabled(flag);
    ui->connectAct->setEnabled(flag);
}
void MainWindow::on_alignment_button_clicked()
{
    QString name;
    switch(Parameter::robotType)  //机器人型号
    {
       case LanxunCoordRobot:
       name = "坐标式";
       break;
       case Lanxun5JointRobot:
       name = "五关节";
       break;
       case Lanxun6JointRobot:
       name = "六关节";
       break;
    }
    statusLabel->setText(name + "焊接机器人模型加载中...");
    if(!ui->teach_button->isEnabled())
        ui->teach_button->setEnabled(true);
    ui->alignment_button->setEnabled(false);
    ui->editText_button->setEnabled(true);
    setSimulationBtnBool(false);
    setSimulation_buttonTrue();
    ui->connectAct->setEnabled(true);
    ui->compile_button->setEnabled(false);
    if(!analysisWdg)
        analysisWdg = new AnalysisWidget();
    if(!robot_udp)  //如果为空
    {
        robot_udp = new UdpSoket(Parameter::robotType);
        connect(robot_udp,SIGNAL(checkConnectSignal(bool)),this,SLOT(checkConnectSlot(bool)));
        connect(robot_udp,SIGNAL(jointcoordSignal(double*,double*)),analysisWdg,SLOT(setData(double*,double*)));
        lastAlitype = Parameter::robotType;
        ui->stackedWidget->addWidget(robot_udp);
    }

    if( lastAlitype != Parameter::robotType && robot_udp)
    {
        ui->stackedWidget->removeWidget(robot_udp);
        disconnect(robot_udp,SIGNAL(checkConnectSignal(bool)),this,SLOT(checkConnectSlot(bool)));
        disconnect(robot_udp,SIGNAL(jointcoordSignal(double*,double*)),analysisWdg,SLOT(setData(double*,double*)));
        delete robot_udp;
        robot_udp = NULL;
        robot_udp = new UdpSoket(Parameter::robotType);
        connect(robot_udp,SIGNAL(checkConnectSignal(bool)),this,SLOT(checkConnectSlot(bool)));
        connect(robot_udp,SIGNAL(jointcoordSignal(double*,double*)),analysisWdg,SLOT(setData(double*,double*)));
        lastAlitype = Parameter::robotType;
        ui->stackedWidget->addWidget(robot_udp);
        analysisWdg->replot();
    }

    ui->dockWidget->setWidget(analysisWdg);
    ui->dockWidget->setVisible(true);
    ui->stackedWidget->setCurrentWidget(robot_udp);
    statusLabel->setText(name + "焊接机器人模型加载完成");
}

void MainWindow::typeChooseSlot()
{
if(lastSimultype != Parameter::robotType)
{
    if(ui->stackedWidget->currentWidget() == robot_udp)
    {
        QString name;
        switch(Parameter::robotType)  //机器人型号
        {
           case LanxunCoordRobot:
           name = "坐标式";
           break;
           case Lanxun5JointRobot:
           name = "五关节";
           break;
           case Lanxun6JointRobot:
           name = "六关节";
           break;
        }
        statusLabel->setText(name + "焊接机器人模型加载中...");
        UdpSoket * tmp = robot_udp;
        disconnect(robot_udp,SIGNAL(checkConnectSignal(bool)),this,SLOT(checkConnectSlot(bool)));
        disconnect(robot_udp,SIGNAL(jointcoordSignal(double*,double*)),analysisWdg,SLOT(setData(double*,double*)));
        robot_udp = new UdpSoket(Parameter::robotType);
        connect(robot_udp,SIGNAL(checkConnectSignal(bool)),this,SLOT(checkConnectSlot(bool)));
        connect(robot_udp,SIGNAL(jointcoordSignal(double*,double*)),analysisWdg,SLOT(setData(double*,double*)));
        lastSimultype = Parameter::robotType;
        ui->stackedWidget->addWidget(robot_udp);
        ui->stackedWidget->setCurrentWidget(robot_udp);
        ui->stackedWidget->removeWidget(tmp);
        delete tmp;
        tmp = NULL;
        analysisWdg->replot();
        statusLabel->setText(name + "焊接机器人模型加载完成");
    }
    else if(ui->stackedWidget->currentWidget() == parse)
    {
        QString name;
        switch(Parameter::robotType)  //机器人型号
        {
           case LanxunCoordRobot:
           name = "坐标式";
           break;
           case Lanxun5JointRobot:
           name = "五关节";
           break;
           case Lanxun6JointRobot:
           name = "六关节";
           break;
        }
        statusLabel->setText(name + "焊接机器人模型加载中...");
        SimulationParse * tmp = parse;
        parse = new SimulationParse(Parameter::robotType);
        lastSimultype = Parameter::robotType;
        ui->stackedWidget->addWidget(parse);
        ui->stackedWidget->setCurrentWidget(parse);
        ui->stackedWidget->removeWidget(tmp);
        delete tmp;
        tmp = NULL;
        parse->prerun("C:\\simulation.cnc");
        connect(parse,SIGNAL(parseGrogress(int)),this,SLOT(changelistIndex(int)));
        connect(parse,SIGNAL(runFinished()),this,SLOT(runFinishedSlot()));
        connect(ui->addAct,SIGNAL(triggered()),parse,SLOT(addspeed()));
        connect(ui->reduceAct,SIGNAL(triggered()),parse,SLOT(decspeed()));
        statusLabel->setText(name + "焊接机器人模型加载完成");
    }
}
}
//转到槽函数 这个按钮只是互换到那个界面上去调节
void MainWindow::on_simulation_button_clicked()   //跳转这个界面要做的事 1.转换成绝对到c 2.读取c文件做预处理  content
{

    QString name;
    switch(Parameter::robotType)  //机器人型号
    {
       case LanxunCoordRobot:
       name = "坐标式";
       break;
       case Lanxun5JointRobot:
       name = "五关节";
       break;
       case Lanxun6JointRobot:
       name = "六关节";
       break;
    }
    setSimulationBtnBool(true);
    statusLabel->setText(name + "焊接机器人模型加载中...");
    ui->playAct->setEnabled(playBool);
    ui->backAct->setEnabled(backBool);
    ui->pauseAct->setEnabled(pauseBool);
    ui->resetAct->setEnabled(resetBool);
    if(!ui->teach_button->isEnabled())
        ui->teach_button->setEnabled(true);
    ui->simulation_button->setEnabled(false);
    ui->alignment_button->setEnabled(true);
    ui->editText_button->setEnabled(true);
    ui->compile_button->setEnabled(false);
    if(!isfirstSimulation)   //第一次仿真的时候
    {
        isfirstSimulation = true;
        parse = new SimulationParse(Parameter::robotType);
        lastSimultype = Parameter::robotType;
        ui->stackedWidget->addWidget(parse);
        connect(parse,SIGNAL(parseGrogress(int)),this,SLOT(changelistIndex(int)));
        connect(parse,SIGNAL(runFinished()),this,SLOT(runFinishedSlot()));
        connect(ui->addAct,SIGNAL(triggered()),parse,SLOT(addspeed()));
        connect(ui->reduceAct,SIGNAL(triggered()),parse,SLOT(decspeed()));
    }
    if(lastSimultype != Parameter::robotType && parse) //表示已经创建了 重新再来
    {
        ui->stackedWidget->removeWidget(parse);
        delete parse;
        parse = NULL;
        parse = new SimulationParse(Parameter::robotType);
        lastSimultype = Parameter::robotType;
        parse->home();
        parse->lineIndex = 0;
        parse->interIndex = -1;
        firei = 0;
        ui->stackedWidget->addWidget(parse);
        connect(parse,SIGNAL(parseGrogress(int)),this,SLOT(changelistIndex(int)));
        connect(parse,SIGNAL(runFinished()),this,SLOT(runFinishedSlot()));
        connect(ui->addAct,SIGNAL(triggered()),parse,SLOT(addspeed()));
        connect(ui->reduceAct,SIGNAL(triggered()),parse,SLOT(decspeed()));
    }
        if(!ui->addAct->isEnabled())
        ui->addAct->setEnabled(true);
        if(!ui->reduceAct->isEnabled())
        ui->reduceAct->setEnabled(true);
        if(!ui->amplificationAct->isEnabled())
        ui->amplificationAct->setEnabled(true);
        if(!ui->reductionAct->isEnabled())
        ui->reductionAct->setEnabled(true);
        if(!ui->narrowdownAct->isEnabled())
        ui->narrowdownAct->setEnabled(true);
        ui->hideRobotAct->setEnabled(true);
//        if(!analysisWdg)
//        {
//            analysisWdg = new AnalysisWidget();
//            analysisWdg->show();
//            connect(parse,SIGNAL(jointcoordSignal(double*,double*)),analysisWdg,SLOT(setData(double*,double*)),Qt::QueuedConnection);
//        }
        if(!listdlg)   //第一次创建的时候
        {
            listdlg = new GCodeListDlg();
            ui->playAct->setEnabled(true);
            ui->backAct->setEnabled(false);
            ui->pauseAct->setEnabled(false);
            playBool = true;
            backBool = false;
            pauseBool = false;
            ui->modelSwitchAct->setEnabled(true);
            //第一次的时候不需要编译也是可以加的
            parse->prerun("C:\\simulation.cnc"); //仿真就是看的这个 这个G反正就是这个无所谓的
            listdlg->loadlist(compileFileName);   //编译的文件夹我要记住
            connect(listdlg,SIGNAL(tabchangeSignal(int)),this,SLOT(tabChangeSlot(int)));
        }
       else  //如果不是第一次就看是否编译过没有
       {
            if(isCompile)  //这个是重新编译过的文件
            {
                ui->playAct->setEnabled(true);
                ui->backAct->setEnabled(false);
                ui->pauseAct->setEnabled(false);
                playBool = true;
                backBool = false;
                pauseBool = false;
                ui->modelSwitchAct->setEnabled(true);
                qDebug() << "重新编译";
                parse->home();
                parse->lineIndex = 0;
                parse->interIndex = -1;
                firei = 0;
                parse->trailPoints.clear();
                parse->prerun("C:\\simulation.cnc"); //仿真就是看的这个 这个G反正就是这个无所谓的
                listdlg->loadlist(compileFileName);   //编译的文件夹我要记住
                changelistIndex(1);
            }
        }

    ui->dockWidget->setWidget(listdlg);
    ui->dockWidget->setVisible(true);
    isCompile = false;
    ui->stackedWidget->setCurrentWidget(parse);
    statusLabel->setText(name + "焊接机器人模型加载完成");
}
void MainWindow::on_playAct_triggered()
{
    statusLabel->setText("正在焊接...");
    isFinished = false;
    ui->playAct->setEnabled(false);
    playBool = false;
    ui->backAct->setEnabled(false);
    backBool = false;
    ui->pauseAct->setEnabled(true);
    pauseBool = true;
    ui->resetAct->setEnabled(false);
    resetBool = false;
    ui->modelSwitchAct->setEnabled(false);
    isback = false;
    if(parse->isFinished)
       firei = 0;
    parse->run(true);
    ui->playAct->setEnabled(true);
    playBool = true;
    ui->backAct->setEnabled(true);
    backBool = true;
    ui->pauseAct->setEnabled(false);
    pauseBool = false;
    ui->resetAct->setEnabled(true);
    resetBool = true;
    statusLabel->setText("运动停止");
    qDebug() << "运动停止";
}
void MainWindow::on_pauseAct_triggered()
{
    parse->IsStop = true;
    ui->pauseAct->setEnabled(false);
    pauseBool = false;
    ui->playAct->setEnabled(true);
    playBool = true;
    ui->backAct->setEnabled(true);
    backBool = true;
    ui->resetAct->setEnabled(true);
    resetBool = true;
}
void MainWindow::on_backAct_triggered()
{
    statusLabel->setText("正在后退...");
    isback = true;
    ui->playAct->setEnabled(false);
    playBool = false;
    ui->backAct->setEnabled(false);
    backBool = false;
    ui->pauseAct->setEnabled(true);
    pauseBool = true;
    ui->resetAct->setEnabled(false);
    resetBool = false;
    parse->run(false);
    ui->playAct->setEnabled(true);
    playBool = true;
    ui->pauseAct->setEnabled(false);
    pauseBool = false;
    ui->modelSwitchAct->setEnabled(true);
    statusLabel->setText("运动停止");
}

void MainWindow::on_resetAct_triggered()
{
    ui->resetAct->setEnabled(false);
    resetBool = true;
    qDebug() << "重置";
    parse->home();
    parse->lineIndex = 0;
    parse->interIndex = -1;
    firei = 0;
    if(listdlg->rel_list->count() > 0)
    {
       listdlg->abs_list->setCurrentRow(0);
    }
    if(listdlg->abs_list->count() > 0)
    {
        listdlg->abs_list->setCurrentRow(0);
    }
    ui->modelSwitchAct->setEnabled(true);
    statusLabel->setText("重置成功");
}

void MainWindow::on_modelSwitchAct_triggered()
{
    parse->modeSwitch();
    QString str;
    if(parse->mode == FireMode)
        str = "焊接模式";
    else if(parse->mode == ShowMode)
        str = "演示模式";
    statusLabel->setText("已切换"+str);
}

void MainWindow::on_newAct_triggered()
{
    if(ui->stackedWidget->currentWidget() == codeEditDlg)
    {
      codeEditDlg->newCreteFile();
      checkSwitchStatus();
    }
    else if(ui->stackedWidget->currentWidget() == teachGLWidget)
    {
        if(teachDlg)
            teachDlg->newFileFun();
    }
}

void MainWindow::on_openAct_triggered()
{
    //如果当前堆栈窗口是代码编辑区，调用open()函数
    if(ui->stackedWidget->currentWidget() == codeEditDlg)
    {
       codeEditDlg->openFile();
       checkSwitchStatus();
    }
    //如果当前堆栈窗口是示教区
    else if(ui->stackedWidget->currentWidget() == teachGLWidget)
    {
        if(teachDlg)
        teachDlg->openFileFun();
    }
}

void MainWindow::on_saveAct_triggered()
{
    if(ui->stackedWidget->currentWidget() == codeEditDlg)
    {
       codeEditDlg->saveFile();
       checkSwitchStatus();
    }
    else if(ui->stackedWidget->currentWidget() == teachGLWidget)
    {
        if(teachDlg)
        teachDlg->saveFileFun();
    }
}
void MainWindow::on_saveAsAct_triggered()
{
    if(ui->stackedWidget->currentWidget() == codeEditDlg)
    {
        codeEditDlg->saveAsFile();
        checkSwitchStatus();
    }
    else if(ui->stackedWidget->currentWidget() == teachGLWidget)
    {
        if(teachDlg)
         teachDlg->saveAsFileFun();
    }
}
void MainWindow::changelistIndex(int i)
{
    if(i < 1)
        i = 1;
    last_cur_i = i;
    if(listdlg->abs_list->count() > 0)   //绝对的
    {
        if((listdlg->abs_list->item(i+firei)->text()).startsWith("M07") || (listdlg->abs_list->item(i+firei)->text()).startsWith("M08"))
        {
            if(lasti != i)
            {
                lasti = i;
                if(isback)
                --firei;
                else
                ++firei;
            }

        }
        if( listdlg->curtabchoose() == 1)
        {
        listdlg->abs_list->setCurrentRow(i + firei);
        listdlg->setCurLine(i + firei + 1);
        }
    }
    if(listdlg->rel_list->count() > 0 && CommonFun::listNum.size() > 0 )  //相对的
    {
       if(listdlg->curtabchoose() == 0)
       {
          listdlg->rel_list->setCurrentRow(CommonFun::listNum[i + firei-1]);
          listdlg->setCurLine(CommonFun::listNum[i + firei-1] + 1);
       }
    }
}

//槽函数
void MainWindow::tabChangeSlot(int index)
{
    if(listdlg->rel_list->count() > 0 && listdlg->abs_list->count() > 0)
    {
        if(index == 1 )  //表示是绝对的的
         {
            listdlg->abs_list->setCurrentRow(last_cur_i + firei);
            listdlg->setCurLine(last_cur_i + firei + 1);
         }
        else
        {
            if(isFinished)
            {
                listdlg->rel_list->setCurrentRow(listdlg->rel_list->count()-1);
                listdlg->setCurLine(listdlg->rel_list->count());
            }
            else
            {
            listdlg->rel_list->setCurrentRow(CommonFun::listNum[last_cur_i + firei-1]);
            listdlg->setCurLine(CommonFun::listNum[last_cur_i + firei-1] + 1);
            }
        }
     }
    if(listdlg->rel_list->count() < 1 && listdlg->abs_list->count() > 0)
    {
        listdlg->abs_list->setCurrentRow(last_cur_i + firei);
        listdlg->setCurLine(last_cur_i + firei + 1);
    }
}
void MainWindow::runFinishedSlot()
{
    if(listdlg->rel_list->count() > 0)   //相对的
    {
       listdlg->rel_list->setCurrentRow(listdlg->rel_list->count()-1);
       listdlg->setCurLine(listdlg->rel_list->count());
       isFinished = true;
    }
}

void MainWindow::on_amplificationAct_triggered()
{
    if(!parse->amplification())
        ui->amplificationAct->setEnabled(false);
    if(!ui->narrowdownAct->isEnabled())
        ui->narrowdownAct->setEnabled(true);
}

void MainWindow::on_narrowdownAct_triggered()
{
    if(!parse->NarrowDown())
        ui->narrowdownAct->setEnabled(false);
    if(!ui->amplificationAct->isEnabled())
        ui->amplificationAct->setEnabled(true);
}

void MainWindow::on_reductionAct_triggered()
{
    parse->reduce();
    if(!ui->amplificationAct->isEnabled())
        ui->amplificationAct->setEnabled(true);
    if(!ui->narrowdownAct->isEnabled())
        ui->narrowdownAct->setEnabled(true);
}



void MainWindow::on_rotate_Xa_triggered()
{
    parse->xRotateadd();
}

void MainWindow::on_rotate_Xd_triggered()
{
    parse->xRotatedec();
}

void MainWindow::on_rotate_Ya_triggered()
{
    parse->yRotateadd();
}

void MainWindow::on_rotate_Yd_triggered()
{
    parse->yRotatedec();
}

void MainWindow::on_hideRobotAct_triggered()
{
    static int hidnum = 0;
    hidnum++;
    if(hidnum == 3)
        hidnum = 0;
    switch(hidnum)
    {
     case 0:
     parse->changeViewMode(NOHIDE);
     ui->hideRobotAct->setText("隐藏机器人");
     ui->hideRobotAct->setIcon(QIcon(":images/hideRobot.png"));
     break;
     case 1:
      parse->changeViewMode(HIDEROBOT);
      ui->hideRobotAct->setText("隐藏机器人和焊枪");
      ui->hideRobotAct->setIcon(QIcon(":images/hideRobotAndTorch.png"));
      break;
     case 2:
     parse->changeViewMode(HIDEROBOTANDTORCH);
     ui->hideRobotAct->setText("无隐藏");
     ui->hideRobotAct->setIcon(QIcon(":images/noHide.png"));
     break;
    }
}


void MainWindow::on_hideLineAct_triggered()
{
    if(parse->changeIsHideLine())  //已经隐藏了
    {
        ui->hideLineAct->setText("显示空移线");
    }
    else
        ui->hideLineAct->setText("隐藏空移线");
}
void MainWindow::on_paraAct_triggered()
{
    if(!paradlg)
    {
        paradlg = new ParaDlg;
        connect(paradlg,SIGNAL(typechoosesignal()),this,SLOT(typeChooseSlot()));
    }
    paradlg->exec();
}
//连接之后
void MainWindow::on_connectAct_triggered()
{
    ui->connectAct->setEnabled(false);
    if(!connectflag)
    {
     robot_udp->UdpConnect();
    }
    else
    {
     robot_udp->UdpDisConnect();
     ui->connectAct->setText("连接");
     ui->connectAct->setIcon(QIcon(":images/connect.png"));
     ui->connectAct->setEnabled(true);
     connectflag = false;
    }
}

 void MainWindow::checkConnectSlot(bool flag)
 {
     if(flag)
     {
        qDebug() << "连接成功";
         connectflag = true;
        ui->connectAct->setText("断开");
        ui->connectAct->setIcon(QIcon(":images/disConnect.png"));
     }
     else   //连接失败
     {
         qDebug() << "连接失败";
         connectflag = false;
         ui->connectAct->setText("连接");
         ui->connectAct->setIcon(QIcon(":images/connect.png"));
     }
     ui->connectAct->setEnabled(true);
 }
void MainWindow::on_clearTrials_triggered()  //清楚
{
    robot_udp->trailPoints.clear();
    robot_udp->updateGL();
}

void MainWindow::on_teach_button_clicked()
{

    ui->teach_button->setEnabled(false);
    ui->alignment_button->setEnabled(true);
    setSimulationBtnBool(false);
    setSimulation_buttonTrue();
    ui->editText_button->setEnabled(true);
    ui->compile_button->setEnabled(true);
    if(!teachGLWidget) //只是创建了一次
    {
        teachGLWidget = new TeachGlWidget(Parameter::robotType);
        ui->stackedWidget->addWidget(teachGLWidget);
        lastTeachtype = Parameter::robotType;   //关节式机器人
        initWorkpiece();
    }
    if(lastTeachtype != Parameter::robotType && teachGLWidget)
    {
        ui->stackedWidget->removeWidget(teachGLWidget);
        delete teachGLWidget;
        teachGLWidget = new TeachGlWidget(Parameter::robotType);
        lastTeachtype = Parameter::robotType;   //关节式机器人
        ui->stackedWidget->addWidget(teachGLWidget);
        createTeachConnect();
        qDebug() << "加载示教";
    }

    if(ui->stackedWidget->currentWidget() != teachGLWidget){
        qDebug()<<"on_teach_button_clicked"<<endl;
        ui->stackedWidget->setCurrentWidget(teachGLWidget);
        qDebug()<<"on_teach_button_clicked"<<endl;
    }
    teachGLWidget->setFocus();

    if(!teachDlg)
    {
        teachDlg = new TeachDlg;
        createTeachConnect();
    }
       qDebug()<<"标记5"<<endl;
    ui->dockWidget->setWidget(teachDlg);
    ui->dockWidget->setVisible(true);
}

//导入工件
void MainWindow::on_load_workPiece_triggered()
{
    QString path;
    if(lastStlFile.isEmpty())
        path = QDir::currentPath();
    else
        path = QFileInfo(lastStlFile).absolutePath();
    QString Name = QFileDialog::getOpenFileName(this,"打开",path,"工件模型文件 (*.stl *.STL");
    if (Name.isEmpty())
        return;
    teachGLWidget->loadArtifacts(Name,true);  //导入工件
    lastStlFile = Name;
}

//初始化为上一次导入的工件
void MainWindow::initWorkpiece()
{
    QString Name = lastStlFile;
    if (Name.isEmpty())
        return;
    teachGLWidget->loadArtifacts(Name,false);  //导入工件
}

//创建信号和槽
void MainWindow::createTeachConnect()
{
    connect(teachDlg,SIGNAL(moto_SettingJSignal(char,double,double)),teachGLWidget,SLOT(moto_SettingJSlot(char,double,double)));
    connect(teachDlg,SIGNAL(movelSignal(char,double,double)),teachGLWidget,SLOT(moveLSlot(char,double,double)));
    connect(teachDlg,SIGNAL(getCoordSignal()),teachGLWidget,SLOT(getCoordSlot()));
    connect(teachGLWidget,SIGNAL(sendCoordSignal(ArrayXd)),teachDlg,SLOT(receiveCoordSlot(ArrayXd)));
}
void MainWindow::setSimulation_buttonTrue()
{
    if(isFirstCompile && !ui->simulation_button->isEnabled())
    ui->simulation_button->setEnabled(true);
}

//编译
void MainWindow::on_compile_button_clicked()
{
    if(!isCompile)
        isCompile = true;  //更新编译状态
    if(parse)
    {
      parse->IsStop = true;
    }
    if(ui->stackedWidget->currentWidget() == codeEditDlg)
    {
        codeEditDlg->transFile();
        checkSwitchStatus();
        compileFileName = codeEditDlg->saveFileName;
    }

    else if(ui->stackedWidget->currentWidget() == teachGLWidget)
    {

        codeEditDlg->transFile(teachDlg->teachFileName);
        compileFileName = "C:\\simulation.cnc";
    }

     if(!isFirstCompile)
         isFirstCompile = true;
     ui->simulation_button->setEnabled(true);
}
void MainWindow::on_analyAct_triggered()
{
    static bool flag = false;   //true 为速度  false 为轨迹
    flag = !flag;
    if(flag)
    {
      ui->analyAct->setText("轨迹分析");
      ui->analyAct->setIcon(QIcon(":images/trailAna.png"));
    }
    else
    {
       ui->analyAct->setText("速度分析");
       ui->analyAct->setIcon(QIcon(":images/speedAna.png"));
    }
    if(analysisWdg)
    analysisWdg->switchAna(flag);
}
