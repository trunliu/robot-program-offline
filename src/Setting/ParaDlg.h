#ifndef PARADLG_H
#define PARADLG_H

#include <QDialog>

namespace Ui {
class ParaDlg;
}

class ParaDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ParaDlg(QWidget *parent = 0);
    ~ParaDlg();
public slots:
    void okclikedslot();
     signals:
    void typechoosesignal();
private:
    Ui::ParaDlg *ui;
    void setInput();
};

#endif // PARADLG_H
