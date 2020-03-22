#ifndef SIMULATIONPARSER_H
#define SIMULATIONPARSER_H

#include <QWidget>
#include "GCodeConversion.h"
#include "3DModel/OpenGLWidget.h"
#include "NewScaraArgorithmn.h"
#include <QList>
#include <QVector>
#include <QPushButton>
#include <QWidget>
#include <QObject>
#include <QCloseEvent>
#include "Alignment/AnalysisWidget.h"
#define TIMELIST 40

class SimulationParse : public OpenglWidget
{
    Q_OBJECT
    public:
        SimulationParse(RobotType type);
        ~SimulationParse();
        bool run(bool flag);
        bool prerun(const QString &fname);
        void closeEvent(QCloseEvent * e);
        bool IsStop;
        int lineIndex;   //每一条线段的索引值
        int interIndex;  //表是直线插补或则圆弧的插补点
        bool isFireMode;
        bool isFinished;
    public:
        void modeSwitch();
    private:

        double divideLength;     //等分的长度
        QVector<QString> content;//G代码内容（经过预处理)
        QVector<QString> errs;
        void preProcess();    //预处理
        void readFile(const QString &fname);
        //解析主入口,错误显示tbd
        void doParse(bool flag);   //true表示前进  表示后退
        bool ParserSentence(const QString &sentence,const QString &sentence1);
        bool ParserSentenceback(const QString &sentence,const QString &sentence1);
        bool getCode(const QString &cmd,CodeTypeEnum &code);
        bool isFireLine;
        bool ParserDouble(const QString &word,wchar_t &pre,double &val);
        //解析直线，用比较固定的方法直接解析
        bool ParserLine(const QVector<QString> &words,CodeTypeEnum code,bool flag);
        bool ParserArcBack(const QVector<QString> &words,const QVector<QString>&words1,const CodeTypeEnum &code);
        bool ParserArc(const QVector<QString>&words,const QVector<QString>&words1,const CodeTypeEnum &code);
        ArrayXd currentXyzrpw;
        Coint curC;
        int num;
        void moto_runJAbs(Joint j, Coint c,bool flag,double sp);
        void sendJC();

        /**************************圆/圆弧等分算法************************************/
        bool isEndPoint(double x1, double y1, double z1, double x2, double y2, double z2);
        //圆弧等分得到等分点的坐标
        //num是等分点序号的基数,在等分圆时用，等分圆弧为0
        bool getArcDividePoint(double x1, double y1, double z1, double r1, double p1, double w1,double x2,
                                                     double y2, double z2, double x3, double y3, double z3, double r3,
                                                     double p3,double w3, int num,QList<QVector<double>>& pointList);
        bool getCircleDividePoint(double x1, double y1, double z1, double r1, double p1,double w1, double x2,
                                            double y2, double z2, double x3, double y3, double z3, double r3,
                                            double p3,double w3,QList<QVector<double>>& arcDividePoint);
        bool  getLineDividePoint(ArrayXd &xyzrpw,Coint& c,bool flag);
        unsigned int timelist[TIMELIST];
signals:
        void parseGrogress(int n);
        void jointcoordSignal(double *j,double *c);
        void runFinished();
public slots:
        void addspeed();
        void decspeed();
};

#endif
