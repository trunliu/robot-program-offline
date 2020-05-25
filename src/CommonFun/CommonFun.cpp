#include "CommonFun.h"
#include <QVector>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QTime>
#include <QCoreApplication>
#include <QDebug>
#include <fstream>
#include <time.h>
Joint CommonFun::lastj = {0,0,0,0,0,0};
QSettings CommonFun::settings("Statics.ini");
QVector<int> CommonFun::listNum;
CommonFun::CommonFun()
{

}
bool CommonFun::ReadSettingsBool(const QString &dataItem)
{
    return settings.value(dataItem).toBool();
}
void CommonFun::WriteSettingsBool(const QString &dataItem,bool data)
{
     settings.setValue(dataItem, data);
}

QString CommonFun::ReadSettingsStr(const QString &dataItem)
{
    return  settings.value(dataItem).toString();
}

double CommonFun::ReadSettingsDouble(const QString &dataItem)
{
    return settings.value(dataItem).toDouble();
}

void CommonFun::WriteSettingsStr(const QString &dataItem,const QString& data)
{
     settings.setValue(dataItem, data);

}
int CommonFun::ReadSettingsInt(const QString &dataItem)
{
    return settings.value(dataItem).toInt();
}

void CommonFun::WriteSettingsInt(const QString &dataItem,int data)
{
     settings.setValue(dataItem, data);

}

void CommonFun::WriteSettingsDouble(const QString &dataItem,double data)
{
     settings.setValue(dataItem, data);
}
bool CommonFun::copyFileToPath(QString sourceDir ,QString toDir, bool coverFileIfExist)
{
    //toDir.replace("\\","/");
    if (sourceDir == toDir)
    {
        return true;
    }
    if (!QFile::exists(sourceDir))
    {
        return false;
    }
    QDir *createfile = new QDir;
    bool exist = createfile->exists(toDir);
    if (exist)
    {
        if(coverFileIfExist)
        {
            createfile->remove(toDir);
        }
    }
      if(!QFile::copy(sourceDir, toDir))
        {
            return false;
        }
        return true;
}

int CommonFun::splitString(const QString &str, char split, QVector<QString>& strArray)
{
    strArray.clear();
    QString strTemp = str; //此赋值不能少
    int nIndex = 0;
    while( 1 ){
        nIndex = strTemp.indexOf(split); //QString 采用  x.indexOf(y)获取字符在字符中的位置;strTemp.Find( split )改.indexOf(split);
        if(nIndex==0){//合并分割符
            if(strTemp.length()<1)
                break;
            strTemp=strTemp.right(strTemp.length()-1);
        }
        else if( nIndex > 0 ) {
            strArray.push_back( strTemp.left( nIndex ) );
            strTemp = strTemp.right( strTemp.length() - nIndex - 1 );
        }
        else break;
    }

    //必须加判断
    if(strTemp.length() > 0)
        strArray.push_back(strTemp);//add 改pushback 好像改append也可以
    return strArray.size();
}

//每行读取文件出来
bool CommonFun::readTextFile(const QString &fname,QVector<QString>&content)
{
      //清空content
      content.clear();
      QFile file(fname);
      //读取文件内容到content
      if (file.open(QFile::ReadOnly | QFile::Text))
      {
              QTextStream in(&file);        //将文件入流
              QString line;
              while ( !in.atEnd() )          //当流不到最后的时候  一直读取一行
              {
                  line = in.readLine();     // 不包括“\n”的一行文本
                  content.push_back(line);         //加入内容到content中
              }
              file.close();
              return true;
      }
      else
      {
          qWarning() << fname << ": 读文件打开失败";
          return false;
      }
}

bool CommonFun::wirteTextFile(const QString &fname,const QVector<QString> &content)
{
    QFile file(fname);
    if(file.open(QFile::WriteOnly | QIODevice::Truncate | QFile::Text))
    {
         QTextStream out(&file);
         QString str;
         foreach (str, content)
         {
             out << str << endl;
         }
         file.close();
         return true;
    }
    else
    {
        qWarning() << fname << ": 写文件打开失败";
        return false;
    }
}

//判断是否共线
bool CommonFun::isThreePointInLine(double x1, double y1, double z1,
                                             double x2, double y2, double z2, double x3, double y3, double z3)
{
      //法向量的值 n = AB x AC  (叉乘)
      double i, j, k;
      i = (y2 - y1) * (z3 - z1) - (z2 - z1) * (y3 - y1);
      j = (z2 - z1) * (x3 - x1) - (x2 - x1) * (z3 - z1);
      k = (x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1);
      //先判断是否共线，根据三角形面积，S = 1/2|AB x AC| ,向量叉乘的模
      return i == 0.0 && j == 0.0 && k == 0.0;
}

//得到目录,不带最后的'\'
QString CommonFun::getExePath()
{
    QString path;
    QDir dir;
    path=dir.absolutePath();
    reverse(path.begin(),path.end());
    int  pos=path.indexOf("/");
    path=path.right(path.length()-1-pos);
    reverse(path.begin(),path.end());
    return path;
}

void CommonFun::msgErr(const QString &err,QWidget*wid)
{
    QMessageBox::warning(wid,"出错",err);

}
double CommonFun::round(double number, unsigned int bits)
{
    LL integerPart = number;
    number -= integerPart;
    for (unsigned int i = 0; i < bits; ++i)
        number *= 10;
    number = (LL) (number + 0.5);
    for (unsigned int i = 0; i < bits; ++i)
        number /= 10;
    return integerPart + number;
}
void CommonFun::sleep(unsigned int msec)
{

    QTime dieTime = QTime::currentTime().addMSecs(msec);

    while( QTime::currentTime() < dieTime )

    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}
void CommonFun::setToolButtonStyle(QToolButton *tbn, const QString &text,int textSize,const QString iconName)  //设置统一按钮样式的形式
{
     tbn->setStyleSheet("selection-background-color: rgb(0, 0, 0);color: rgb(255, 255, 255);border-radius:5px;border-width:0px");
    //设置显示的文本
    tbn->setText(text);
    if (textSize == E_SMALL)//大尺寸
    {
        tbn->setFont(QFont("文泉驿雅黑",8,QFont::Bold));
         tbn->setIconSize(QSize(40,38));//设置图标尺寸    哈哈根据图片的尺寸去设置图标的尺寸
    }
    else if (textSize == E_NORMAL)//正常尺寸做法
    {
        tbn->setFont(QFont("文泉驿雅黑",12,QFont::Bold));//设置字体颜色哈哈
        tbn->setIconSize(QSize(60,60));//设置图标尺寸    哈哈根据图片的尺寸去设置图标的尺寸
    }
    else if (textSize == E_Mid)//正常尺寸做法
    {
        tbn->setFont(QFont("文泉驿雅黑",9,QFont::Bold));//设置字体颜色哈哈
        tbn->setIconSize(QSize(52,52));//设置图标尺寸    哈哈根据图片的尺寸去设置图标的尺寸
    }
    tbn->setAutoRaise(true);//自动适应变化
    //设置按钮图标
    tbn->setIcon(QPixmap(QString("%1").arg(iconName)));//设置图标从资源文件的位置上去加载哈哈
    //设置文本在图标下边
    tbn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);//懂啦哈哈
}
QString  CommonFun::hextoInt(const QString &str)
{
    string cmd=str.toStdString();
    int n1,n2;
    if((cmd[0] >= 'a' && cmd[0] <= 'f')|| (cmd[0] >= 'A' && cmd[0]<= 'F'))
            n1 = (cmd[0] & 0x0f) + 9;
        else
            n1 = cmd[0] & 0x0f;
        if((cmd[1] >= 'a' && cmd[1] <= 'f')|| (cmd[1] >= 'A' && cmd[1]<= 'F'))
            n2 = (cmd[1] & 0x0f) + 9;
        else
            n2 = cmd[1] & 0x0f;
        int n=16*n1+n2;
        QString ss=QString("%1").arg(n);
        return ss;
}
//求出分割时间的函数
int CommonFun::time_substract(struct timeval *result, struct timeval *begin,struct timeval *end)
{
    if(begin->tv_sec > end->tv_sec)
        return -1;
    if((begin->tv_sec == end->tv_sec) && (begin->tv_usec > end->tv_usec))
        return -2;
    result->tv_sec = (end->tv_sec - begin->tv_sec);
    result->tv_usec = (end->tv_usec - begin->tv_usec);
    if(result->tv_usec < 0)
    {
    result->tv_sec--;
    result->tv_usec += 1000000;
    }
    return 0;
}
