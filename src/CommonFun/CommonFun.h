#ifndef COMMONFUN_H
#define COMMONFUN_H
#include <QString>
#include <string.h>
#include <QSettings>
#include <QVector>
#include <vector>
#include <QToolButton>
#include "CodeParse/scaralib.h"
using namespace std;
typedef long long LL;
enum E_TEXTSIZE
{
    E_NORMAL = 0,       //
    E_SMALL =  1 ,  //用枚举类型哈哈
    E_LITTER = 2,
    E_Mid = 3
};
class CommonFun
{
public:
    CommonFun();
    //分割字符串
    static Joint lastj;
    static QVector<int> listNum;
    static string joinstr;
    static QSettings settings;

    //读出设置：根据key值返回一个bool类型
    static bool ReadSettingsBool(const QString &dataItem);
    //写入设置（关键字：dataItem，值：bool类型）
    static void WriteSettingsBool(const QString &dataItem,bool data);
    //写入设置（关键字：dataItem，值：int类型）
    static void WriteSettingsInt(const QString &dataItem,int data);
    //读出设置：根据key值返回一个int类型
    static int ReadSettingsInt(const QString &dataItem);
    //读出设置：根据key值返回一个double类型
    static double ReadSettingsDouble(const QString &dataItem);
    //写入设置（关键字：dataItem，值：Double类型）
    static void WriteSettingsDouble(const QString &dataItem,double data);
    //读出设置：根据key值返回一个string类型
    static QString ReadSettingsStr(const QString &dataItem);
    //写入设置（关键字：dataItem，值：sting类型）
    static void WriteSettingsStr(const QString &dataItem,const QString& data);


    static int splitString(const QString &str, char split, QVector<QString>& strArray);
    //读取文本文件，每行读取为一个字符串
    static bool readTextFile(const QString &fname,QVector<QString>&content);
    static bool wirteTextFile(const QString &fname,const QVector<QString>&content);
    //拷贝文件
    static  bool copyFileToPath(QString sourceDir ,QString toDir, bool coverFileIfExist);
    static bool isThreePointInLine(double x1, double y1, double z1,double x2, double y2, double z2, double x3, double y3, double z3);
    //得到目录,不带最后的'\'
    static QString getExePath();
    static void msgErr(const QString &err,QWidget*wid);
    static double round(double number, unsigned int bits);
    static void sleep(unsigned int msec);
    static void setToolButtonStyle(QToolButton *tbn, const QString &text,
                                             int textSize,const QString iconName);
    static QString hextoInt(const QString &str);
    static int time_substract(struct timeval *result, struct timeval *begin,struct timeval *end);

};

#endif // COMMONFUN_H
