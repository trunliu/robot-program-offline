#include "CodeEditDlg.h"
#include "ui_CodeEditDlg.h"
#include "CommonFun/CommonFun.h"
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QColorDialog>
#include <QFontDialog>
#include <QPrinter>
#include <QPrintDialog>
#include <QFileInfo>
CodeEditDlg::CodeEditDlg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CodeEditDlg)
{
    ui->setupUi(this);
    init();

    //读取上次保存的xyzrpw参数以及机器人类型
    Parameter::readParameter();
}

CodeEditDlg::~CodeEditDlg()
{
    //记录最后保存的文件名到QSettings对象中便于初始化，key："Para/saveFileName"，value：saveFileName
    CommonFun::WriteSettingsStr("Para/saveFileName",saveFileName);
    //记录文件打开时的路径
    CommonFun::WriteSettingsStr("Para/lastFilePath",lastFilePath);

    Parameter::writeParameter();
    delete ui;
}

//从ini文件中获取上次保存的文件名，并初始化到textEdit中
void CodeEditDlg::init()
{
    //获取上次保存的文件名
    saveFileName = CommonFun::ReadSettingsStr("Para/saveFileName");
    //获取上次打开文件的路径
    lastFilePath = CommonFun::ReadSettingsStr("Para/lastFilePath");
    //如果文件路径为空，则设置为应用程序当前目录的绝对路径。
    if(lastFilePath.isEmpty())
        lastFilePath = QDir::currentPath();

    ui->filename_label->setText(saveFileName);

    if(saveFileName.isEmpty())
        return;

    //1、获取上次保存文件名 2、打开先前的保存文件 3、读取到代码编辑区
    QFile *file = new QFile(saveFileName);
    if (file->open(QIODevice::ReadOnly))
    {
        QTextStream in(file);
        ui->textEdit->setText(in.readAll());
        file->close();
        delete file;
    }
}

//新建槽函数，并询问是否需要保存文本？
void CodeEditDlg::newCreteFile()
{
    if (ui->textEdit->document()->isModified())
    {
        QMessageBox::StandardButton button = QMessageBox::information(this, "尚未保存", "是否要保存?",
                                             QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        switch (button)
        {
        case QMessageBox::Save:
            saveFile();
            if (ui->textEdit->document()->isModified())
                return;
            break;
        case QMessageBox::Discard:
            break;
        case QMessageBox::Cancel:
            return;
            break;
       default:
            break;
        }
    }

    //text清空
    ui->textEdit->clear();
    //文件名清空
    saveFileName.clear();
    //设置为无名
    ui->filename_label->setText("untitled.txt.....");
    //设置为字体
    ui->textEdit->setCurrentFont(QFont("宋体", 13, QFont::Normal));

}

 void CodeEditDlg::openFile()
 {
     if (ui->textEdit->document()->isModified())
     {
         QMessageBox::StandardButton button = QMessageBox::information(this, "尚未保存", "是否要保存?",
                                              QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
         switch (button)
         {
         case QMessageBox::Save:
             saveFile();
             if (ui->textEdit->document()->isModified())
                 return;
             break;
         case QMessageBox::Discard:
             break;
         case QMessageBox::Cancel:
             return;
             break;
         }
     }
     QString Name = QFileDialog::getOpenFileName(this, "打开",lastFilePath, "焊接离线文件 (*.txt *.cnc)");
     if (Name.isEmpty())
         return;
     saveFileName = Name;
     QFile file(saveFileName);
     ui->textEdit->clear();
     ui->textEdit->setCurrentFont(QFont("宋体", 13, QFont::Normal));
     if (!file.open(QIODevice::ReadOnly))
     {
         QMessageBox::warning(this, "错误", "不能打开文件", QMessageBox::Yes);
         return;
     }
     QTextStream in(&file);
     ui->textEdit->setText(in.readAll());
     file.close();

     //设置打开文件的label
     ui->filename_label->setText(saveFileName);

     //构建一个QFileInfo，包含文件的信息：绝对路径和相对路径
     lastFilePath = QFileInfo(saveFileName).absolutePath();
 }


 void CodeEditDlg::saveFile()
 {
     //如果不是通过打开的文件（例如：新建文件），没有saveFileName值，则调用saveAsFile另存为函数
     if (saveFileName.isEmpty())
         saveAsFile();
     else
     {
         //打开saveFileName文件，写入数据
         QFile file(saveFileName);
         if(!file.open(QIODevice::WriteOnly))
         {
             QMessageBox::warning(this, "ERROR", "Can't save the file", QMessageBox::Yes);
             return;
         }
         QTextStream out(&file);
         out << ui->textEdit->toPlainText();
         //保存后重置 已修改 状态
         ui->textEdit->document()->setModified(false);
         file.close(); 
     }
 }

 //解析代码
 void CodeEditDlg::transFile()
 {
     saveFile();
     int n = codeconversion.CreateConvertedFile(saveFileName);
     if(n == -2) //表示解析有错误
     {
         codeconversion.dspMsg("解析错误:\n",this);
     }
     else if(n == 0)
     {
         QMessageBox::information(this,"提示","文本G代码转换及编译成功！");
     }
     else if(n == 1)
     {
         QMessageBox::information(this,"提示","文本G代码编译成功！");
     }
 }

 //解析代码
 void CodeEditDlg::transFile(const QString & fileName)
 {
     int n = codeconversion.CreateConvertedFile(fileName);
     if(n == -2) //表示解析有错误
     {
         codeconversion.dspMsg("解析错误:\n",this);
     }
     else if(n == 1)
     {
         QMessageBox::information(this,"提示","示教G代码编译成功！");
     }
 }

 //另存为
  void CodeEditDlg::saveAsFile()
  {
      //新获得保存文件的名称
      QString Name = QFileDialog::getSaveFileName(this, "保存",lastFilePath, "焊接离线文件 (*.cnc)");
      if(Name.isEmpty())
          return;
      saveFileName = Name;

      //若文件不是.txt或者.cnc文件例如(新建的文件另存为时）
      //把保存文件都变为.cnc，默认Qt::CaseSensitive区分大小写
      //Qt::CaseInsensitive不区分大小写
      if(!saveFileName.endsWith(".txt", Qt::CaseInsensitive) && !saveFileName.endsWith(".cnc", Qt::CaseInsensitive))
          saveFileName.append(".cnc");

      QFile file(saveFileName);
      if(!file.open(QIODevice::WriteOnly))
      {
          QMessageBox::warning(this, "错误", "不能打开文件", QMessageBox::Yes);
          return;
      }
      QTextStream out(&file);
      out << ui->textEdit->toPlainText();
      ui->textEdit->document()->setModified(false);
      file.close();

      ui->filename_label->setText(saveFileName);
      //记录本次文件的绝对路径
      lastFilePath = QFileInfo(saveFileName).absolutePath();
  }

  void CodeEditDlg::colorSlot()
  {
      QColor color = QColorDialog::getColor(Qt::green, this, "颜色");
      if (color.isValid())
      {
          ui->textEdit->setTextColor(color);
          ui->textEdit->document()->setModified(true);
      }
  }
  void CodeEditDlg::fontSlot()
  {
      bool ok;
      QFont font = QFontDialog::getFont(&ok, ui->textEdit->font(), this, "字体");
      if (ok)
      {
          ui->textEdit->setCurrentFont(font);
          ui->textEdit->document()->setModified(true);
      }
  }

  //重写关闭事件
  void CodeEditDlg::closeEvent(QCloseEvent *event)
  {
      //判断修改后是否保存
      if (ui->textEdit->document()->isModified())
      {
          QMessageBox::StandardButton button = QMessageBox::information(this, "尚未保存", "是否要保存?",
                                               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
          switch (button)
          {
          case QMessageBox::Save:
              saveFile();
              //如果还存在修改，忽略退出信号，程序继续进行
              if (ui->textEdit->document()->isModified())
                  event->ignore();
              else
                  event->accept();// 接受退出信号，程序退出
              break;
          case QMessageBox::Discard:
              event->accept();
              break;
          case QMessageBox::Cancel:
              event->ignore();//忽略退出信号，程序继续进行
              break;
          }
      }
  }

//切换代码，通过QFile切换文件输出textEdit上,所以前提必须先编译代码生成_abs.cnc文件才能切换
bool CodeEditDlg::switchSlot()
{
    QString filename = saveFileName;
    //判断是否是机器人能使识别的绝对路径焊接文件_abs.cnc
    //若是最终生成的焊接文件，则切换打开.cnc，或.txt文件
    if(filename.endsWith("_abs.cnc"))
    {
        QString filename1 = filename.remove("_abs.cnc");
        qDebug() << filename1;
        QString filename2 = filename1 + ".cnc";
        QString filename3 = filename1 + ".txt";
         QFile file2(filename2);
         QFile file3(filename3);
        if(file2.open(QIODevice::ReadOnly))
        {
            saveFileName = filename2;
            ui->textEdit->clear();
            ui->textEdit->setCurrentFont(QFont("宋体", 13, QFont::Normal));
            QTextStream in(&file2);
            ui->textEdit->setText(in.readAll());
            file2.close();
            ui->filename_label->setText(saveFileName);
            return true;
        }
        else if(file3.open(QIODevice::ReadOnly))
        {
            saveFileName = filename3;
            ui->textEdit->clear();
            ui->textEdit->setCurrentFont(QFont("宋体", 13, QFont::Normal));
            QTextStream in(&file3);
            ui->textEdit->setText(in.readAll());
            file3.close();
            ui->filename_label->setText(saveFileName);
            return true;
        }else
            return false;
    }else{
        //若当前切换文件是.cnc或.txt文件，则切换为最终的焊接文件_abs.cnc输出
        if(filename.endsWith(".cnc"))
            filename.remove(".cnc");
        else if(filename.endsWith(".txt"))
            filename.remove(".txt");
        else
            return false;
        filename += "_abs.cnc";
        qDebug() << filename;
         QFile file(filename);
        if(file.open(QIODevice::ReadOnly))
        {
            saveFileName = filename;
            ui->textEdit->clear();
            ui->textEdit->setCurrentFont(QFont("宋体", 13, QFont::Normal));
            QTextStream in(&file);
            ui->textEdit->setText(in.readAll());
            file.close();
            ui->filename_label->setText(saveFileName);
            return true;
        }
        else
        return false;
    }
}

void CodeEditDlg::boldSlot()
{
    if (QFont::Bold == ui->textEdit->fontWeight())
        ui->textEdit->setFontWeight(QFont::Normal);
    else
        ui->textEdit->setFontWeight(QFont::Bold);
}

void CodeEditDlg::printSlot()
{
    qDebug() << "打印";
    QPrinter printer;
    QString printer_name = printer.printerName();
   if( printer_name.size() == 0)
   {
       qDebug() << "没有打印";
       return;
   }
   QPrintDialog dlg(&printer, this);
   if (ui->textEdit->textCursor().hasSelection())
   {
       dlg.addEnabledOption(QAbstractPrintDialog::PrintSelection);
   }
   if(dlg.exec() == QDialog::Accepted)
   {
       ui->textEdit->print(&printer);
   }
}

//检查切换的状态 这个要做预处理来检查是否有问题
bool CodeEditDlg::checkSwitch()
{
   QString filename = saveFileName;
   if(filename.endsWith("_abs.cnc"))
   {
       QString filename1 = filename.remove("_abs.cnc");
       qDebug() << filename1;
       QString filename2 = filename + ".cnc";
       QString filename3 = filename + ".txt";
        QFile file2(filename2);
        QFile file3(filename3);
       if(file2.open(QIODevice::ReadOnly))
           return true;
       else if(file3.open(QIODevice::ReadOnly))
           return true;
       else
           return false;
   }
   else
   {
       if(filename.endsWith(".cnc"))
           filename.remove(".cnc");
       else if(filename.endsWith(".txt"))
           filename.remove(".txt");
       else
           return false;
       filename += "_abs.cnc";
       qDebug() << filename;
        QFile file(filename);
       if(file.open(QIODevice::ReadOnly))
           return true;
       else
           return false;
   }
}
