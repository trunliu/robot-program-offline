#ifndef GCODECONVERSION_H
#define GCODECONVERSION_H
#include <QVector>
#include <iostream>
#include "Setting/Parameter.h"
using namespace std;
const int ERR_sfileopenfalse = -2;
const int ERR_fileempty = -3;
const int ERR_G9091 = - 4;

//G代码类型
enum CodeTypeEnum
    {
        START_POINT_CODE,METER_CODE,INCH_CODE,       //起点
        NO_USE_CODE,G99_CODE,
        ABSOLUTE_CODE,RELATIVE_CODE,
        INTERPOLATION_LINE,MOVE_LINE,
        CW_ARC_CODE,ACW_ARC_CODE,
        FIRE_ON_CODE,FIRE_OFF_CODE,FINISH_CODE,ARC_CODE,
        CIRCLE_CODE
    };

class GCodeConversion
{
public:
    GCodeConversion();

    //创建最终焊接文件入口函数
    int CreateConvertedFile(const QString& filename);
    void dspMsg(const QString &preMsg,QWidget*wid);

    //bool CreateConvertedFile(const QVector<QString>& content,QString &filename);

private:
   Posture point;
   int curNum;
   QVector<QString> content;//G代码内容（经过预处理)
   QVector<QString> converedContent;//G代码内容（经过预处理)
   QVector<QString> errs;
   bool isAbsolute; //是不是绝对的坐标
   bool isFireLine;
   QString currentCode;
   bool IsCam;

private:
   //运行主函数
   bool run(const QString &fname);
   void readFile(const QString &fname);
   void preProcess();    //预处理
   void confirmCoord();  //确定坐标形式是相对还是绝对坐标
   //解析主入口,错误显示tbd
   void doParse();
   bool ParserSentence(const QString &sentence);
   bool ParserLine(const QVector<QString> &words);
   bool ParserArc(const QVector<QString> &words,const CodeTypeEnum &currentCode);
   void tansvertArcNew(double x1,double y1,double i,double j,bool IsCw,bool isTurn);

private:
   //解析浮点数值,tbd 判断是否为合法浮点数
   bool ParserDouble(const QString &word,wchar_t &pre,double &val);
   bool getCode(const QString &cmd,CodeTypeEnum &currentCode);
   void writeConveredGcodes(const QString &cmd);








   //double  getAngle(double x,double y);
   //void tansvertArc(double x1,double y1,double i,double j,bool IsCw);

};

#endif // GCODECONVERSION_H
