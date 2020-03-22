#ifndef GCODELISTDLG_H
#define GCODELISTDLG_H

#include <QWidget>
#include <QListWidget>
#include <QCloseEvent>
namespace Ui {
class GcodeListDlg;
}

class GCodeListDlg : public QWidget
{
    Q_OBJECT

public:
    explicit GCodeListDlg(QWidget *parent = 0);
    ~GCodeListDlg();
    QListWidget *abs_list;
    QListWidget *rel_list;
    void loadlist(const QString &fileName);
    void setCurLine(int i);
    int curtabchoose();
signals:
    void closesignal();
    void tabchangeSignal(int i);
private slots:
    void on_tabWidget_currentChanged(int index);

private:
    QString gFileName;
    QVector<QString> errs;
    bool isAbsolute; //是不是绝对的坐标
    bool isFireLine;
    void readFile(const QString &fname,QVector<QString>& content);
    void preProcess(QVector<QString>& content);    //预处理
    void confirmCoord(QVector<QString>& content);  //确定坐标形式是相对还是绝对坐标
    bool prerun(const QString&fname,QVector<QString>& content);
    Ui::GcodeListDlg *ui;
    void closeEvent(QCloseEvent *e);
};

#endif // GCODELISTDLG_H
