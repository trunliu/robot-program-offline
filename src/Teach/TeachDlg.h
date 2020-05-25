#ifndef TEACHDLG_H
#define TEACHDLG_H

#include <QWidget>
#include <QCloseEvent>
#include <QListWidget>
#include "CodeParse/scaralib.h"
#include <QStringList>
#include <QVector>
namespace Ui {
class TeachDlg;
}

class TeachDlg : public QWidget
{
    Q_OBJECT

public:
    explicit TeachDlg(QWidget *parent = 0);

    ~TeachDlg();

    void saveAsFileFun();

    void saveFileFun();

    void openFileFun();

    void newFileFun();

    QString teachFileName;
public slots:
signals:
    void closesignal();
    void getCoordSignal();
    void moto_SettingJSignal(char ch, double angle,double speed);
    void movelSignal(char ch,double distance,double speed);
private slots:

    void on_j1dec_button_pressed();

    void on_j1dec_button_released();

    void on_j1add_button_pressed();

    void on_j1add_button_released();

    void on_j2dec_button_pressed();

    void on_j2dec_button_released();

    void on_j2add_button_pressed();

    void on_j2add_button_released();

    void on_j3dec_button_pressed();

    void on_j3dec_button_released();

    void on_j3add_button_pressed();

    void on_j3add_button_released();

    void on_j4dec_button_pressed();

    void on_j4dec_button_released();

    void on_j4add_button_pressed();

    void on_j4add_button_released();

    void on_j5dec_button_pressed();

    void on_j5dec_button_released();

    void on_j5add_button_pressed();

    void on_j5add_button_released();

    void on_j6dec_button_pressed();

    void on_j6dec_button_released();

    void on_j6add_button_pressed();

    void on_j6add_button_released();
    void on_xdec_button_pressed();

    void on_xdec_button_released();

    void on_xadd_button_pressed();

    void on_xadd_button_released();

    void on_ydec_button_pressed();

    void on_ydec_button_released();

    void on_yadd_button_pressed();

    void on_yadd_button_released();

    void on_zdec_button_pressed();

    void on_zdec_button_released();

    void on_zadd_button_pressed();

    void on_zadd_button_released();

    void on_rdec_button_pressed();

    void on_rdec_button_released();

    void on_radd_button_pressed();

    void on_radd_button_released();

    void on_pdec_button_pressed();

    void on_pdec_button_released();

    void on_padd_button_pressed();

    void on_padd_button_released();

    void on_wdec_button_pressed();

    void on_wdec_button_released();

    void on_wadd_button_pressed();

    void on_wadd_button_released();

    void on_toolButton_clicked();

    void on_order_listWidget_itemClicked(QListWidgetItem *item);

    void receiveCoordSlot(ArrayXd);

    void customContextSlot(QPoint);

    void deleteSeedSlot();

    void clearSeedsSlot();

    void insertActSlot();

    void updateCurrentRowSlot(int);




private:
    void closeEvent(QCloseEvent *e);
     Ui::TeachDlg *ui;
     void moto_SettingJ12(char ch,bool isAdd);
     void moto_SettingJ3(bool isAdd);
     void moto_SettingJ456(char ch,bool isAdd);
     QListWidget *absG_list;
     QListWidget *teach_list;
     void setstyle();
     void initTeachlist();
     QStringList content;
     double weldSpeed;
     int arcNum;
     int circleNum;
     bool isMoveLine;
     double lastCoord[6];
     void setLastCoord(double coord[6]);
     bool isCoordChange(double coord[6]);
     void loadFile(const QString & fileName);
     double arcStartPoint[3];

};

#endif // TEACHDLG_H
