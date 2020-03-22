#ifndef ANALYSISWIDGET_H
#define ANALYSISWIDGET_H

#include <QWidget>
#include "qcustomplot.h"
#include <QTime>
namespace Ui {
class AnalysisWidget;
}

class AnalysisWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AnalysisWidget(QWidget *parent = 0);
    ~AnalysisWidget();
    void replot();
    void switchAna(bool isSpeedAna);
public slots:
    void setData(double *,double *);
private:
    Ui::AnalysisWidget *ui;
    bool isSpeedAna;
    QColor color[9];
    int axisNum;
    void initPlot();
    void init();
    void initReploy();
    QTime updatetime;
    bool isFirst;
    double lastJoint[6];
    double lastXyzrpw[6];
};

#endif // ANALYSISWIDGET_H
