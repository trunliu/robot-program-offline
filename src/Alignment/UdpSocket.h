#ifndef UDPSOCKET_H
#define UDPSOCKET_H
#include <QWidget>
#include <QUdpSocket>
#include "3DModel/OpenGLWidget.h"
#include "Parser.h"
#include <QKeyEvent>
#include <QCloseEvent>

//继承OpenglWidget，为了使用渲染
class UdpSoket : public OpenglWidget
{
    Q_OBJECT

public:
     UdpSoket(RobotType type);
    ~UdpSoket();

     //连接与断开端口
     void UdpConnect();
     void UdpDisConnect();

public slots:
     void checkIsConnect();

     //串口数据传入处理槽
     void processPendingDatagram();

signals:
     void checkConnectSignal(bool flag);

     //输出数据
     void jointcoordSignal(double *,double *);

private:
    //创建客户端，绑定端口
    void initSocket();

    //连接服务器向服务器ip地址发送请求连接信号
    void connectServer();
    void sendData(const QString& str,const QString& ipAddress);

    //封装数据包解析出来的数据，发送数据给柱形图分析窗口
    void sendJC();

    void closeEvent(QCloseEvent *e);
    void keyPressEvent(QKeyEvent *e);

private:
    //解析句柄
    Parser *parser;
    bool isConnect;

    //套接字客户端
    QUdpSocket *socket;
};

#endif // UDPSOCKET_H
