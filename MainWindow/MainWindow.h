#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "CodeParse/CodeEditDlg.h"
#include "CodeParse/GCodeListDlg.h"
#include "Alignment/UdpSocket.h"
#include <QCloseEvent>
#include "CodeParse/SimulationParse.h"
#include <QLabel>
#include "Teach/TeachGlWidget.h"
#include "Teach/TeachDlg.h"
#include <QCloseEvent>
#include "Alignment/AnalysisWidget.h"
class ParaDlg;
namespace Ui
{
  class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void closeEvent(QCloseEvent *e);
private slots:

    void on_editText_button_clicked();

    void on_alignment_button_clicked();

    void on_simulation_button_clicked();
    
    void on_playAct_triggered();

    void on_pauseAct_triggered();

    void on_backAct_triggered();

    void on_resetAct_triggered();

    void on_modelSwitchAct_triggered();

    void on_newAct_triggered();

    void on_openAct_triggered();

    void on_saveAct_triggered();

    void on_saveAsAct_triggered();

    void changelistIndex(int i);

    void runFinishedSlot();

    void on_amplificationAct_triggered();

    void on_narrowdownAct_triggered();

    void on_reductionAct_triggered();

    void on_rotate_Xa_triggered();

    void on_rotate_Xd_triggered();

    void on_rotate_Ya_triggered();

    void on_rotate_Yd_triggered();

    void on_hideRobotAct_triggered();

    void on_paraAct_triggered();

    void typeChooseSlot();

    void on_connectAct_triggered();

    void checkConnectSlot(bool flag);

    void on_clearTrials_triggered();

    void on_hideLineAct_triggered();

    void on_teach_button_clicked();

    void on_load_workPiece_triggered();

    void tabChangeSlot(int);

    void on_compile_button_clicked();
    

    void on_analyAct_triggered();

private:
    void  keyPressEvent(QKeyEvent * e);
    Ui::MainWindow *ui;
    bool isCompile;
    CodeEditDlg * codeEditDlg;
    SimulationParse *parse;
    TeachGlWidget * teachGLWidget;
    UdpSoket*robot_udp;
    GCodeListDlg *listdlg;
    TeachDlg * teachDlg;
    AnalysisWidget *analysisWdg;
    QLabel * statusLabel;
    void init();
    void setButtonStyle();
    void connectSlot();
    bool isfirstSimulation;
    void setSimulationStatus(bool flag);
    void setSimulationBtnBool(bool flag);
    void checkSwitchStatus();
    void initWorkpiece();
    void createTeachConnect();
    void setSimulation_buttonTrue();
    int firei;
    int curi;
    int lasti;
    bool isback;
    QString lastStlFile;
    RobotType lastSimultype;
    RobotType lastAlitype;
    RobotType lastTeachtype;
    ParaDlg *paradlg;
    int last_cur_i;
    bool isFinished;
    bool isFirstCompile;
    QString compileFileName;
    bool playBool;
    bool pauseBool;
    bool backBool;
    bool resetBool;
    bool connectflag;
};

#endif // MAINWINDOW_H
