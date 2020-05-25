#include "ParaDlg.h"
#include "ui_ParaDlg.h"
#include "Parameter.h"
#include <QDebug>
ParaDlg::ParaDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ParaDlg)
{
    ui->setupUi(this);
    ui->X_edit->setText(QString::number(Parameter::startPosture.x));
    ui->Y_edit->setText(QString::number(Parameter::startPosture.y));
    ui->Z_edit->setText(QString::number(Parameter::startPosture.z));
    ui->R_edit->setText(QString::number(Parameter::startPosture.r));
    ui->P_edit->setText(QString::number(Parameter::startPosture.p));
    ui->W_edit->setText(QString::number(Parameter::startPosture.w));
    ui->robotTypeBox->setCurrentIndex(Parameter::robotType);
    setInput();
    connect(ui->ok_buttton,SIGNAL(clicked()),this,SLOT(okclikedslot()));
    connect(ui->cancel_button,SIGNAL(clicked()),this,SLOT(close()));
}
ParaDlg::~ParaDlg()
{
    delete ui;
}
void ParaDlg::setInput()
{
    QRegExp rx("^(-?[0]|-?[1-9][0-9]{0,5})(?:\\.\\d{1,4})?$|(^\\t?$)");
    QRegExpValidator *pReg = new QRegExpValidator(rx, this);
    ui->X_edit->setValidator(pReg);
    ui->Y_edit->setValidator(pReg);
    ui->Z_edit->setValidator(pReg);
    ui->R_edit->setValidator(pReg);
    ui->P_edit->setValidator(pReg);
    ui->W_edit->setValidator(pReg);
}
void ParaDlg::okclikedslot()
{
    Parameter::startPosture.x = (ui->X_edit->text()).toDouble();
    Parameter::startPosture.y = (ui->Y_edit->text()).toDouble();
    Parameter::startPosture.z = (ui->Z_edit->text()).toDouble();
    Parameter::startPosture.r = (ui->R_edit->text()).toDouble();
    Parameter::startPosture.p = (ui->P_edit->text()).toDouble();
    Parameter::startPosture.w = (ui->W_edit->text()).toDouble();
    Parameter::robotType = (RobotType)(ui->robotTypeBox->currentIndex());
    close();
    emit typechoosesignal(); //类型选择
}


