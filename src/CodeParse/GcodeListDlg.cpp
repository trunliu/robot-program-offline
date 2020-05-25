#include "GCodeListDlg.h"
#include "ui_GcodeListDlg.h"
#include <QTextStream>
#include <QFile>
#include <CommonFun/CommonFun.h>
#include <QPalette>
GCodeListDlg::GCodeListDlg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GcodeListDlg)
{
    ui->setupUi(this);
    abs_list = NULL;
    rel_list = NULL;
    ui->tabWidget->clear();
    setWindowIcon(QIcon(":/images/glist.png"));
}
GCodeListDlg::~GCodeListDlg()
{
    if(abs_list)
    delete abs_list;
    if(rel_list)
    delete rel_list;
    delete ui;
}
void GCodeListDlg::loadlist(const QString &fileName)
{
    QString filename = fileName;
    if(!abs_list)
        abs_list = new QListWidget();
    if(!rel_list)
        rel_list = new QListWidget();
    abs_list->clear();
    rel_list->clear();
    ui->tabWidget->clear();
    QVector<QString> content;
    if(filename != "C:\\simulation.cnc")
    {
        if(filename.endsWith("_abs.cnc")) //表示是绝对的
        {
            QString filename1 = filename.remove("_abs.cnc");
            QFile file1(filename1 + ".cnc");
            QFile file2(filename1 + ".txt");
            if(file1.open(QIODevice::ReadOnly))  //相对的表示能够打开
            {
               content.clear();
               prerun(filename1 + ".cnc",content);  //预处理验证坐标
               foreach(QString line,content)
                   rel_list->addItem(new QListWidgetItem(line,rel_list));
               file1.close();
            }
            else if(file2.open(QIODevice::ReadOnly))  //相对的
            {
                content.clear();
                prerun(filename1 + ".txt",content);  //预处理验证坐标
                foreach(QString line,content)
                  rel_list->addItem(new QListWidgetItem(line,rel_list));
                file2.close();
            }
        }
        else
        {
           content.clear();
           if(!prerun(filename,content)) //相对的
           {
               foreach(QString line,content)
               rel_list->addItem(new QListWidgetItem(line,rel_list));
           }

        }
    }
    QFile file("C:\\simulation.cnc");
    if(file.open(QIODevice::ReadOnly))  //绝对的
    {
        QTextStream stream(&file);        //将文件入流
        QString line;
        while ( !stream.atEnd() )          //当流不到最后的时候  一直读取一行
        {
            line = stream.readLine(); // 不包括“\n”的一行文本
            abs_list->addItem(new QListWidgetItem(line,abs_list));
        }
        file.close();
    }
    if(rel_list->count() > 0)
    {
        ui->tabWidget->addTab(rel_list,"相对");
        rel_list->setCurrentRow(0);

    }
    if(abs_list->count() > 0)
    {
        ui->tabWidget->addTab(abs_list,"绝对");
        abs_list->setCurrentRow(0);
    }
    ui->tabWidget->setCurrentIndex(0);
}

bool GCodeListDlg::prerun(const QString&fname,QVector<QString>& content)
{
    readFile(fname,content);
    preProcess(content);
    confirmCoord(content);
    if(isAbsolute)  //绝对坐标
        return true;
    else
        return false; //相对坐标
}
//读取文件内容到content中
void GCodeListDlg::readFile(const QString &fname,QVector<QString>& content)
{
    CommonFun::readTextFile(fname,content);  //读取数据
}
void GCodeListDlg::closeEvent(QCloseEvent *e)
{
    emit closesignal();
    qDebug() << "关闭";
    return QWidget::closeEvent(e);
}
void GCodeListDlg::preProcess(QVector<QString>& content)
{
    QString line;
    for(int i = content.size() - 1;i > -1;i--)//m_content.GetSize()改m_content.size()都是获取数组中元素的个数
    {
        line = content[i].trimmed();//去掉前后的空格 .Trim(_T(" ")改.trimmed()
        line = line.toUpper();			//转换为大写.MakeUpper()改.toUpper(
        if(line.length() < 1) //这个CString对象中的字节计数。这个计数不包括结尾的空字符。.GetLength()改.length()
            content.remove(i);//.RemoveAt(i)改removeAt(i)
        else
            content[i] = line;
    }
}
void GCodeListDlg::confirmCoord(QVector<QString>& content)
{
    isAbsolute = false;        //缺省是相对坐标
    int relative_num = 0;
    int absolute_num = 0;
    for(int i=0;i < content.size();i++)
    {
        if(content[i] == "G91")
            relative_num++;
        else if(content[i] == "G90")
            absolute_num++;
    }
    if(absolute_num > 0)    //如果是绝对的
        isAbsolute = true;
}
void GCodeListDlg::setCurLine(int i)
{
    ui->curLine_label->setText(QString::number(i));
}
int GCodeListDlg::curtabchoose()
{
    if(ui->tabWidget->currentWidget() == abs_list)
        return 1;
    else
        return 0;
}

void GCodeListDlg::on_tabWidget_currentChanged(int index)
{
   emit tabchangeSignal(index);
}
