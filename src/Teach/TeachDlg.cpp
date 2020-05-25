#include "TeachDlg.h"
#include "ui_TeachDlg.h"
#include "Setting/Parameter.h"
#include <QListWidgetItem>
#include <QDateTime>
#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include "CommonFun/CommonFun.h"
#include <QTextStream>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
TeachDlg::TeachDlg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TeachDlg)
{
    ui->setupUi(this);
    initTeachlist();
    arcNum = 0;
    circleNum = 0;
    weldSpeed = 3000;
    isMoveLine = false;
//    setstyle();
    teachFileName = CommonFun::ReadSettingsStr("Para/teachFileName");
    if(!teachFileName.isEmpty())
    loadFile(teachFileName);

}
void TeachDlg::initTeachlist()
{
    ui->tabWidget->clear();
    teach_list = new QListWidget();
    absG_list = new QListWidget();      //没有意义的做
    absG_list->setContextMenuPolicy(Qt::CustomContextMenu);
    teach_list->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(absG_list,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(customContextSlot(QPoint)));
    connect(teach_list,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(customContextSlot(QPoint)));
    ui->tabWidget->addTab(teach_list,"列表");
    ui->tabWidget->addTab(absG_list,"G代码");
    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(updateCurrentRowSlot(int)));
}
//传统槽函数
 void TeachDlg::customContextSlot(QPoint pos)
 {
     QListWidgetItem * curItem;
     if(ui->tabWidget->currentWidget() == absG_list)
         curItem = absG_list->itemAt(pos);
     else
         curItem = teach_list->itemAt(pos);
     if(!curItem)
         return;
    QMenu *popMenu = new QMenu(ui->tabWidget->currentWidget());
    QAction *deleteSeed = new QAction("删除", this);
    QAction *clearSeeds = new QAction("清空", this);
    QAction *insertAct = new QAction("插入", this);
    popMenu->addAction(deleteSeed);
    popMenu->addAction(clearSeeds);
    popMenu->setStyleSheet("background-color: rgb(244, 244, 244);selection-background-color: rgb(0, 146, 255);");
//  popMenu->addAction(insertAct);
    connect( deleteSeed, SIGNAL(triggered()), this, SLOT(deleteSeedSlot()));
    connect( clearSeeds, SIGNAL(triggered()), this, SLOT(clearSeedsSlot()));
    connect( insertAct, SIGNAL(triggered()), this, SLOT(insertActSlot()));
    popMenu->exec( QCursor::pos() );
    delete popMenu;
    delete deleteSeed;
    delete clearSeeds;
 }
 void TeachDlg::insertActSlot()
 {
     if(ui->tabWidget->currentWidget() == absG_list)
     {
         if(absG_list->currentRow() < 2)
             return;
         int n = absG_list->currentRow();
         absG_list->insertItem(n,"");
         teach_list->insertItem(n - 2,"");
         absG_list->setCurrentRow(n);
         teach_list->setCurrentRow(n - 2);
     }
     else
     {
         int n = teach_list->currentRow();
         teach_list->insertItem(n,"");
         absG_list->insertItem(n + 2,"");
         teach_list->setCurrentRow(n);
         absG_list->setCurrentRow(n + 2);
     }

 }

 //右键删除槽操作
 void TeachDlg::deleteSeedSlot()
 {

     int ch = QMessageBox::warning(ui->tabWidget, "警告",
                                      "你真的要删除吗?",
                                      QMessageBox::Yes | QMessageBox::No,
                                      QMessageBox::No);
        if ( ch != QMessageBox::Yes )
            return;
         int index;
         if(ui->tabWidget->currentWidget() == teach_list)   //根据当前行来判断
         {
             index = teach_list->currentRow();
             teach_list->takeItem(index);
             absG_list->takeItem(index);
         }
         else
         {
             index = absG_list->currentRow();
             absG_list->takeItem(index);
             teach_list->takeItem(index);
         }
 }
 void TeachDlg::clearSeedsSlot()
 {

     int ch = QMessageBox::warning(ui->tabWidget, "警告",
                                       "你确定清除示教文件?",
                                       QMessageBox::Yes | QMessageBox::No,
                                       QMessageBox::No);
         if ( ch != QMessageBox::Yes )
             return;
     absG_list->clear();
     teach_list->clear();
 }
 void TeachDlg::setstyle()
 {
     CommonFun::setToolButtonStyle(ui->j1add_button,"",E_NORMAL,":images/add1.png");
     CommonFun::setToolButtonStyle(ui->j1dec_button,"",E_NORMAL,":images/dec.png");
//     CommonFun::setToolButtonStyle(ui->teach_button,"示教",1,":images/teach.png");
//     CommonFun::setToolButtonStyle(ui->simulation_button,"运行",1,":images/simulation.png");
//     CommonFun::setToolButtonStyle(ui->graph_button,"图库",1,":images/shape.png");
 }

TeachDlg::~TeachDlg()
{

  CommonFun::WriteSettingsStr("Para/teachFileName",teachFileName);
  delete ui;
}

void TeachDlg::moto_SettingJ12(char ch,bool isAdd)
{
    double angle;
    if(Parameter::robotType == LanxunCoordRobot)
        angle = 999;
    else
        angle = 720;
    if(!isAdd)
        angle = -angle;
    emit moto_SettingJSignal(ch,angle,ui->speedEdit->text().toDouble());
}
 void TeachDlg::moto_SettingJ3(bool isAdd)
 {
     double angle;
     if(Parameter::robotType == Lanxun6JointRobot)
         angle = 720;
     else
         angle = 9999;
     if(!isAdd)
         angle = -angle;
     emit moto_SettingJSignal(3,angle,ui->speedEdit->text().toDouble());
 }
 void TeachDlg::moto_SettingJ456(char ch,bool isAdd)
 {
     double angle = 720;
     if(!isAdd)
         angle = -angle;
     emit moto_SettingJSignal(ch,angle,ui->speedEdit->text().toDouble());
 }
void TeachDlg::closeEvent(QCloseEvent *e)
{
    emit closesignal();  //释放关闭信号
    return QWidget::closeEvent(e);
}

void TeachDlg::on_j1dec_button_pressed()
{
   if(ui->point_move_radio->isChecked())
   moto_SettingJ12(1,false);
   else
   emit moto_SettingJSignal(1,-ui->distanceEdit->text().toDouble(),ui->speedEdit->text().toDouble());
}
void TeachDlg::on_j1dec_button_released()
{
    if(ui->point_move_radio->isChecked())
    Parameter::teachStop = true;
}
void TeachDlg::on_j1add_button_pressed()
{
    if(ui->point_move_radio->isChecked())
     moto_SettingJ12(1,true);
    else
    emit moto_SettingJSignal(1,ui->distanceEdit->text().toDouble(),ui->speedEdit->text().toDouble());
}
void TeachDlg::on_j1add_button_released()
{
    if(ui->point_move_radio->isChecked())
    Parameter::teachStop = true;
}

void TeachDlg::on_j2dec_button_pressed()
{
    if(ui->point_move_radio->isChecked())
     moto_SettingJ12(2,false);
    else
    emit moto_SettingJSignal(2,-ui->distanceEdit->text().toDouble(),ui->speedEdit->text().toDouble());
}


void TeachDlg::on_j2dec_button_released()
{
    if(ui->point_move_radio->isChecked())
    Parameter::teachStop = true;
}

void TeachDlg::on_j2add_button_pressed()
{
    if(ui->point_move_radio->isChecked())
     moto_SettingJ12(2,true);
    else
    emit moto_SettingJSignal(2,ui->distanceEdit->text().toDouble(),ui->speedEdit->text().toDouble());
}

void TeachDlg::on_j2add_button_released()
{
    if(ui->point_move_radio->isChecked())
    Parameter::teachStop = true;
}

void TeachDlg::on_j3dec_button_pressed()
{
    if(ui->point_move_radio->isChecked())
    moto_SettingJ3(false);
    else
    emit moto_SettingJSignal(3,-ui->distanceEdit->text().toDouble(),ui->speedEdit->text().toDouble());
}

void TeachDlg::on_j3dec_button_released()
{
    if(ui->point_move_radio->isChecked())
    Parameter::teachStop = true;
}

void TeachDlg::on_j3add_button_pressed()
{
    if(ui->point_move_radio->isChecked())
    moto_SettingJ3(true);
    else
    emit moto_SettingJSignal(3,ui->distanceEdit->text().toDouble(),ui->speedEdit->text().toDouble());
}

void TeachDlg::on_j3add_button_released()
{
    if(ui->point_move_radio->isChecked())
    Parameter::teachStop = true;
}

void TeachDlg::on_j4dec_button_pressed()
{
    if(ui->point_move_radio->isChecked())
    moto_SettingJ456(4,false);
    else
    emit moto_SettingJSignal(4,-ui->distanceEdit->text().toDouble(),ui->speedEdit->text().toDouble());
}

void TeachDlg::on_j4dec_button_released()
{
    if(ui->point_move_radio->isChecked())
     Parameter::teachStop = true;
}
void TeachDlg::on_j4add_button_pressed()
{
    if(ui->point_move_radio->isChecked())
    moto_SettingJ456(4,true);
    else
    emit moto_SettingJSignal(4,ui->distanceEdit->text().toDouble(),ui->speedEdit->text().toDouble());
}

void TeachDlg::on_j4add_button_released()
{
    if(ui->point_move_radio->isChecked())
     Parameter::teachStop = true;
}

void TeachDlg::on_j5dec_button_pressed()
{
    if(ui->point_move_radio->isChecked())
    moto_SettingJ456(5,false);
    else
    emit moto_SettingJSignal(5,-ui->distanceEdit->text().toDouble(),ui->speedEdit->text().toDouble());
}

void TeachDlg::on_j5dec_button_released()
{
    if(ui->point_move_radio->isChecked())
     Parameter::teachStop = true;
}

void TeachDlg::on_j5add_button_pressed()
{
    if(ui->point_move_radio->isChecked())
    moto_SettingJ456(5,true);
    else
    emit moto_SettingJSignal(5,ui->distanceEdit->text().toDouble(),ui->speedEdit->text().toDouble());
}

void TeachDlg::on_j5add_button_released()
{
    if(ui->point_move_radio->isChecked())
     Parameter::teachStop = true;
}

void TeachDlg::on_j6dec_button_pressed()
{
    if(ui->point_move_radio->isChecked())
    moto_SettingJ456(6,false);
    else
    emit moto_SettingJSignal(6,-ui->distanceEdit->text().toDouble(),ui->speedEdit->text().toDouble());
}

void TeachDlg::on_j6dec_button_released()
{
    if(ui->point_move_radio->isChecked())
    Parameter::teachStop = true;
}

void TeachDlg::on_j6add_button_pressed()
{
    if(ui->point_move_radio->isChecked())
    moto_SettingJ456(6,true);
    else
    emit moto_SettingJSignal(6,ui->distanceEdit->text().toDouble(),ui->speedEdit->text().toDouble());
}

void TeachDlg::on_j6add_button_released()
{
    if(ui->point_move_radio->isChecked())
     Parameter::teachStop = true;
}
void TeachDlg::on_xdec_button_pressed()
{
    if(ui->point_move_radio->isChecked())
    emit movelSignal(1,-9999,ui->speedEdit->text().toDouble());
    else
    emit movelSignal(1,-ui->distanceEdit->text().toDouble(),ui->speedEdit->text().toDouble());
}

void TeachDlg::on_xdec_button_released()
{
    if(ui->point_move_radio->isChecked())
    Parameter::teachStop = true;
}

void TeachDlg::on_xadd_button_pressed()
{
    if(ui->point_move_radio->isChecked())
    emit movelSignal(1,9999,ui->speedEdit->text().toDouble());
    else
    emit movelSignal(1,ui->distanceEdit->text().toDouble(),ui->speedEdit->text().toDouble());
}

void TeachDlg::on_xadd_button_released()
{
    if(ui->point_move_radio->isChecked())
    Parameter::teachStop = true;
}

void TeachDlg::on_ydec_button_pressed()
{
    if(ui->point_move_radio->isChecked())
    emit movelSignal(2,-9999,ui->speedEdit->text().toDouble());
    else
    emit movelSignal(2,-ui->distanceEdit->text().toDouble(),ui->speedEdit->text().toDouble());
}

void TeachDlg::on_ydec_button_released()
{
    if(ui->point_move_radio->isChecked())
    Parameter::teachStop = true;
}

void TeachDlg::on_yadd_button_pressed()
{
    if(ui->point_move_radio->isChecked())
    emit movelSignal(2,9999,ui->speedEdit->text().toDouble());
    else
    emit movelSignal(2,ui->distanceEdit->text().toDouble(),ui->speedEdit->text().toDouble());
}

void TeachDlg::on_yadd_button_released()
{
    if(ui->point_move_radio->isChecked())
    Parameter::teachStop = true;
}

void TeachDlg::on_zdec_button_pressed()
{
    if(ui->point_move_radio->isChecked())
    emit movelSignal(3,-9999,ui->speedEdit->text().toDouble());
    else
    emit movelSignal(3,-ui->distanceEdit->text().toDouble(),ui->speedEdit->text().toDouble());
}

void TeachDlg::on_zdec_button_released()
{
    if(ui->point_move_radio->isChecked())
    Parameter::teachStop = true;
}

void TeachDlg::on_zadd_button_pressed()
{
    if(ui->point_move_radio->isChecked())
    emit movelSignal(3,360,ui->speedEdit->text().toDouble());
    else
    emit movelSignal(3,ui->distanceEdit->text().toDouble(),ui->speedEdit->text().toDouble());
}

void TeachDlg::on_zadd_button_released()
{
    if(ui->point_move_radio->isChecked())
    Parameter::teachStop = true;
}

void TeachDlg::on_rdec_button_pressed()
{
    if(Parameter::robotType == Lanxun6JointRobot)
    {
     if(ui->point_move_radio->isChecked())
     emit movelSignal(4,-360,ui->speedEdit->text().toDouble());
     else
     emit movelSignal(4,-ui->distanceEdit->text().toDouble(),ui->speedEdit->text().toDouble());
    }
}

void TeachDlg::on_rdec_button_released()
{
    if(Parameter::robotType == Lanxun6JointRobot && ui->point_move_radio->isChecked())
    Parameter::teachStop = true;
}

void TeachDlg::on_radd_button_pressed()
{
    if(Parameter::robotType == Lanxun6JointRobot)
    {
     if(ui->point_move_radio->isChecked())
     emit movelSignal(4,360,ui->speedEdit->text().toDouble());
     else
     emit movelSignal(4,ui->distanceEdit->text().toDouble(),ui->speedEdit->text().toDouble());
    }
}

void TeachDlg::on_radd_button_released()
{
    if(Parameter::robotType == Lanxun6JointRobot && ui->point_move_radio->isChecked())
    Parameter::teachStop = true;
}

void TeachDlg::on_pdec_button_pressed()
{
    if(ui->point_move_radio->isChecked())
    emit movelSignal(5,-360,ui->speedEdit->text().toDouble());
    else
    emit movelSignal(5,-ui->distanceEdit->text().toDouble(),ui->speedEdit->text().toDouble());
}

void TeachDlg::on_pdec_button_released()
{
    if(ui->point_move_radio->isChecked())
    Parameter::teachStop = true;
}

void TeachDlg::on_padd_button_pressed()
{
    if(ui->point_move_radio->isChecked())
    emit movelSignal(5,360,ui->speedEdit->text().toDouble());
    else
    emit movelSignal(5,ui->distanceEdit->text().toDouble(),ui->speedEdit->text().toDouble());
}

void TeachDlg::on_padd_button_released()
{
    if(ui->point_move_radio->isChecked())
    Parameter::teachStop = true;
}

void TeachDlg::on_wdec_button_pressed()
{
    if(ui->point_move_radio->isChecked())
    emit movelSignal(6,-360,ui->speedEdit->text().toDouble());
    else
    emit movelSignal(6,-ui->distanceEdit->text().toDouble(),ui->speedEdit->text().toDouble());
}

void TeachDlg::on_wdec_button_released()
{
    if(ui->point_move_radio->isChecked())
    Parameter::teachStop = true;
}

void TeachDlg::on_wadd_button_pressed()
{
    if(ui->point_move_radio->isChecked())
    emit movelSignal(6,360,ui->speedEdit->text().toDouble());
    else
    emit movelSignal(6,ui->distanceEdit->text().toDouble(),ui->speedEdit->text().toDouble());
}

void TeachDlg::on_wadd_button_released()
{
    if(ui->point_move_radio->isChecked())
    Parameter::teachStop = true;
}

void TeachDlg::on_toolButton_clicked()
{
    Parameter::teachStop = true;
}
void TeachDlg::on_order_listWidget_itemClicked(QListWidgetItem *item)
{
    if(item->text() != "空移" && teach_list->count() < 1)
     {
        QMessageBox::warning(this,"警告","示教第一点必须为空移线");
        return;
     }
    if(item->text() != "圆弧" && arcNum == 1)
       QMessageBox::warning(this,"警告","示教下一点为圆弧的末点");
    else if(item->text() != "圆" && circleNum == 1)
        QMessageBox::warning(this,"警告","示教下一点为圆的末点");
        emit getCoordSignal();
}

void TeachDlg::receiveCoordSlot(ArrayXd xyzrpw)
{
      double coord[6] = {0};
      for(int i = 0 ; i < 6;i++)
          coord[i] =  xyzrpw[i];
      QString gCode;
      if(ui->order_listWidget->currentItem()->text() == "空移")
      {
          if(!isMoveLine)   //第一次加载
          {
             isMoveLine = true;
          }
          else
          {
            if(!isCoordChange(coord))  //如果没有改变
               return;
          }
          setLastCoord(coord);
          QListWidgetItem * lst = new QListWidgetItem("空移");
          lst->setTextAlignment(Qt::AlignCenter);
          teach_list->addItem(lst);
          gCode = QString("G00 X%1 Y%2 Z%3 R%4 P%5 W%6 S%7").arg(coord[0]).arg(coord[1]).arg(coord[2]).arg(coord[3]).arg(coord[4]).arg(coord[5]).arg(weldSpeed);
          absG_list->addItem(gCode);
          content.append(gCode);
          if(ui->tabWidget->currentWidget() == teach_list)
          {
              if(teach_list->count() > 1)
              teach_list->setCurrentRow(teach_list->count() - 1);
          }
          else
          {
              if(absG_list->count() > 1)
              absG_list->setCurrentRow(absG_list->count() - 1);
          }
      }
      else if(ui->order_listWidget->currentItem()->text() == "插补")
      {
          if(!isCoordChange(coord))  //如果没有改变
          return;
          setLastCoord(coord);
          QListWidgetItem * lst = new QListWidgetItem("插补");
          lst->setTextAlignment(Qt::AlignCenter);
          teach_list->addItem(lst);
          gCode = QString("G01 X%1 Y%2 Z%3 R%4 P%5 W%6 S%7").arg(coord[0]).arg(coord[1]).arg(coord[2]).arg(coord[3]).arg(coord[4]).arg(coord[5]).arg(weldSpeed);
          absG_list->addItem(gCode);
          content.append(gCode);
          if(ui->tabWidget->currentWidget() == teach_list)
              teach_list->setCurrentRow(teach_list->count() - 1);
          else
              absG_list->setCurrentRow(absG_list->count() - 1);
      }
      else if(ui->order_listWidget->currentItem()->text() == "圆弧")
      {
           if(!isCoordChange(coord))  //如果没有改变
           return;
           gCode = QString("G04 A%1 B%2 C%3 D%4 E%5 F%6").arg(coord[0]).arg(coord[1]).arg(coord[2]).arg(coord[3]).arg(coord[4]).arg(coord[5]);
            ++arcNum;
            if(arcNum == 1)
            {
                for(int i = 0; i < 3; i++)
                arcStartPoint[i] = lastCoord[i]; //上一个点
                content.append(gCode);
                ui->order_listWidget->currentItem()->setBackgroundColor(QColor("#19649F"));
            }
            else if(arcNum == 2)
            {
                if(CommonFun::isThreePointInLine(arcStartPoint[0],arcStartPoint[1],arcStartPoint[2],
                        lastCoord[0],lastCoord[1],lastCoord[2],coord[0],coord[1],coord[2]))
                {
                    QMessageBox::warning(this,"警告","圆弧三点共线");
                    content.pop_back();
                    arcNum = 0;
                    ui->order_listWidget->currentItem()->setBackgroundColor(QColor(240,240,240,255));
                    return;
                }
                QListWidgetItem * lst = new QListWidgetItem("圆弧");
                lst->setTextAlignment(Qt::AlignCenter);
                teach_list->addItem(lst);
                gCode = content[content.size()-1];
                content.pop_back();
                gCode += QString(" X%1 Y%2 Z%3 R%4 P%5 W%6 S%7").arg(coord[0]).arg(coord[1]).arg(coord[2]).arg(coord[3]).arg(coord[4]).arg(coord[5]).arg(weldSpeed);
                absG_list->addItem(gCode);
                content.append(gCode);
                arcNum = 0;
                ui->order_listWidget->currentItem()->setBackgroundColor(QColor(240,240,240,255));
                if(ui->tabWidget->currentWidget() == teach_list)
                    teach_list->setCurrentRow(teach_list->count() - 1);
                else
                    absG_list->setCurrentRow(absG_list->count() - 1);

            }
             setLastCoord(coord);
      }
      else if(ui->order_listWidget->currentItem()->text() == "圆")
      {
            if(!isCoordChange(coord))  //如果没有改变
            return;
            gCode = QString("G05 A%1 B%2 C%3 D%4 E%5 F%6").arg(coord[0]).arg(coord[1]).arg(coord[2]).arg(coord[3]).arg(coord[4]).arg(coord[5]);
            ++circleNum;
            if(circleNum == 1)
            {
                for(int i = 0; i < 3; i++)
                arcStartPoint[i] = lastCoord[i]; //上一个点
                content.append(gCode);
                ui->order_listWidget->currentItem()->setBackgroundColor(QColor("#19649F"));
            }
            else if(circleNum == 2)
            {
                if(CommonFun::isThreePointInLine(arcStartPoint[0],arcStartPoint[1],arcStartPoint[2],
                        lastCoord[0],lastCoord[1],lastCoord[2],coord[0],coord[1],coord[2]))
                {
                    QMessageBox::warning(this,"警告","圆三点共线");
                    content.pop_back();
                    arcNum = 0;
                    ui->order_listWidget->currentItem()->setBackgroundColor(QColor(240,240,240,255));
                    return;
                }
                QListWidgetItem * lst = new QListWidgetItem("圆");
                lst->setTextAlignment(Qt::AlignCenter);
                teach_list->addItem(lst);
                gCode = content[content.size()-1];
                content.pop_back();
                gCode += QString(" X%1 Y%2 Z%3 R%4 P%5 W%6 S%7").arg(coord[0]).arg(coord[1]).arg(coord[2]).arg(coord[3]).arg(coord[4]).arg(coord[5]).arg(weldSpeed);
                absG_list->addItem(gCode);
                content.append(gCode);
                circleNum = 0;
                ui->order_listWidget->currentItem()->setBackgroundColor(QColor(240,240,240,255));
                if(ui->tabWidget->currentWidget() == teach_list)
                    teach_list->setCurrentRow(teach_list->count() - 1);
                else
                    absG_list->setCurrentRow(absG_list->count() - 1);
            }
             setLastCoord(coord);
      }
      else if(ui->order_listWidget->currentItem()->text() == "开火")
      {
          gCode = "M07 V1";
          absG_list->addItem(gCode);
          content.append(gCode);
          QListWidgetItem * lst = new QListWidgetItem("开火");
          lst->setTextAlignment(Qt::AlignCenter);
          teach_list->addItem(lst);
          if(ui->tabWidget->currentWidget() == teach_list)
              teach_list->setCurrentRow(teach_list->count() - 1);
          else
              absG_list->setCurrentRow(absG_list->count() - 1);
      }
      else if(ui->order_listWidget->currentItem()->text() == "关火")
      {
          gCode = "M08 V1";
          absG_list->addItem(gCode);
          content.append(gCode);
          QListWidgetItem * lst = new QListWidgetItem("关火");
          lst->setTextAlignment(Qt::AlignCenter);
          teach_list->addItem(lst);
          if(ui->tabWidget->currentWidget() == teach_list)
              teach_list->setCurrentRow(teach_list->count() - 1);
          else
              absG_list->setCurrentRow(absG_list->count() - 1);
      }

}
void TeachDlg::updateCurrentRowSlot(int index)
{
    if(teach_list->count() < 1)
        return;
    if(index == 0)
        teach_list->setCurrentRow(absG_list->currentRow());
    else
        absG_list->setCurrentRow(teach_list->currentRow());
}

void TeachDlg::setLastCoord(double coord[6])
{
    for(int i = 0; i < 6; i++)
        lastCoord[i] = coord[i];
}

bool TeachDlg::isCoordChange(double coord[6])
{
    for(int i = 0; i < 6; i++)
    {
        if(fabs(coord[i] - lastCoord[i]) > 0.000001)
            return true;
    }
    QMessageBox::warning(this,"警告","当前坐标没有改变");
    qDebug() << "没有变化";
    return false;
}

void TeachDlg::saveFileFun()
{
    if (teachFileName.isEmpty())
        saveAsFileFun();
    else
    {
        QFile file(teachFileName);
        if(!file.open(QIODevice::WriteOnly))
        {
            QMessageBox::warning(this, "ERROR", "Can't save the file", QMessageBox::Yes);
            return;
        }
      QTextStream out(&file);
      QDateTime time = QDateTime::currentDateTime();
      QString str = time.toString("yyyy-MM-dd hh:mm:ss dddd");
      QString robotText;
      switch(Parameter::robotType)
      {
         case LanxunCoordRobot:
         robotText = "LanxunCoordRobot teach_Gfile";
         break;
         case Lanxun5JointRobot:
         robotText = "Lanxun5JointRobot teach_Gfile";
         break;
         case Lanxun6JointRobot:
         robotText = "Lanxun6JointRobot teach_Gfile";
         break;
      }
      str = "(" + str + " " + robotText + ")";
      out << str << endl;
      out << "G90" << endl;
    foreach (QString str, content)
    {
         out << str << endl;
    }
    file.close();
    ui->fileName_label->setText(teachFileName);
    }
}
void TeachDlg::saveAsFileFun()
{
    QString Name = QFileDialog::getSaveFileName(this, "保存",QFileInfo(teachFileName).absolutePath(), "焊接离线文件 (*.cnc)");
    if(Name.isEmpty())
        return;
    QFile file(Name);
    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(this, "错误", "不能打开文件", QMessageBox::Yes);
        return;
    }
    teachFileName = Name;
    QTextStream out(&file);
    QDateTime time = QDateTime::currentDateTime();
    QString str = time.toString("yyyy-MM-dd hh:mm:ss dddd");
    QString robotText;
    switch(Parameter::robotType)
    {
       case LanxunCoordRobot:
       robotText = "LanxunCoordRobot teach_Gfile";
       break;
       case Lanxun5JointRobot:
       robotText = "Lanxun5JointRobot teach_Gfile";
       break;
       case Lanxun6JointRobot:
       robotText = "Lanxun6JointRobot teach_Gfile";
       break;
    }
    str = "(" + str + " " + robotText + ")";
    out << str << endl;
    out << "G90" << endl;
    foreach (QString str, content)
    {
         out << str << endl;
    }
    file.close();
    ui->fileName_label->setText(teachFileName);
}

void TeachDlg::loadFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, "错误", "不能打开文件", QMessageBox::Yes);
        return;
    }
    content.clear();
    absG_list->clear();
    teach_list->clear();
    QTextStream in(&file);
    QString line;
    while ( !in.atEnd() )          //当流不到最后的时候  一直读取一行
    {
        QString teachLine;
        line = in.readLine(); // 不包括“\n”的一行文本
        if(line.startsWith("G00"))
            teachLine.append("空移");
        else if(line.startsWith("G01") )
            teachLine.append("插补");
        else if(line.startsWith("G04"))
            teachLine.append("圆弧");
        else if(line.startsWith("G05"))
            teachLine.append("圆");
        else if(line.startsWith("M07"))
            teachLine.append("开火");
        else if(line.startsWith("M08"))
            teachLine.append("关火");
        if(!teachLine.isEmpty())
        {
          QListWidgetItem * lst = new QListWidgetItem(teachLine);
          lst->setTextAlignment(Qt::AlignCenter);
          teach_list->addItem(lst);
          absG_list->addItem(line);
          content.push_back(line);         //加入内容到content中
        }
    }
    ui->fileName_label->setText(fileName);
    file.close();
}
void TeachDlg::openFileFun()
{
    QString Name = QFileDialog::getOpenFileName(this, "打开示教文件",QFileInfo(teachFileName).absolutePath(), "示教焊接文件 (*.txt *.cnc)");
    if (Name.isEmpty())
        return;
    teachFileName = Name;//这是文件的绝对路径
    loadFile(teachFileName);
}
void TeachDlg::newFileFun()   //新建文件
{
    absG_list->clear();
    teach_list->clear();
    teachFileName.clear();
    ui->fileName_label->setText("untitled.cnc.....");
}
