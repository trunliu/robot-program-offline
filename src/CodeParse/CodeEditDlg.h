#ifndef CODEEDITDLG_H
#define CODEEDITDLG_H

#include <QWidget>
#include "GCodeConversion.h"
#include "SimulationParse.h"
#include <QCloseEvent>
namespace Ui {
class CodeEditDlg;
}
class CodeEditDlg : public QWidget
{
    Q_OBJECT
public:
    explicit CodeEditDlg(QWidget *parent = 0);
    ~CodeEditDlg();

    //解析g代码的句柄
    GCodeConversion codeconversion;
    QString saveFileName;
//    QString originFileName;
//    QString transFileName;

public:
    void closeEvent(QCloseEvent *event);

    //检查切换状态，true为最终焊接文件，
    bool checkSwitch();

    //打开，新建，保存，另存为函数
    void openFile();
    void newCreteFile();
    void saveFile();
    void saveAsFile();

    //开始解析当前文件
    void transFile();
    void transFile(const QString & fileName);

public slots:
     //颜色
     void colorSlot();
     //字体
     void fontSlot();
     //粗体
     void boldSlot(); 
     //打印
     void printSlot();
     //切换代码
     bool switchSlot();

private:
    Ui::CodeEditDlg *ui;
    void init();
    //void saveFileAsFun();
    //SimulationParse *simulationParse;
    QString lastFilePath;
};

#endif // CODEEDITDLG_H
