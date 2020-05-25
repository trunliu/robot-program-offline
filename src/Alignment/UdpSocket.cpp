#include "UdpSocket.h"
#include "CommonFun/CommonFun.h"
#include "CodeParse/NewScaraArgorithmn.h"
UdpSoket::UdpSoket(RobotType  type):OpenglWidget(type)
{
    parser = new Parser;
    socket = new QUdpSocket(this);
    widgetname = ALIGNMENT;
    initSocket();
    setWindowTitle("坐标机器人联调");
    isConnect = false;
}

//将解析的数据包数据封装，发送信号给analysisWidget窗口
void UdpSoket::sendJC()
{
    double joint[6] = {j1Rot,j2Rot,j3Rot,j4Rot,j5Rot,j6Rot};

    //发送6个关节8个坐标
    emit jointcoordSignal(joint,coord);
}

/***************************************
 * 1、创建UDP客户端对象
 * 2、绑定端口
 * 3、连接端口（当有数据过来的时候就对应执行槽函数）
****************************************/
void UdpSoket::initSocket()
{
    //UDP创建和绑定端口号
    socket = new QUdpSocket(this);
    socket->bind(8080);
}

//连接端口号
void UdpSoket::UdpConnect()
{
    //连接socket的readRead信号，当有数据过来的时候就执行槽函数。
    QObject::connect(socket,SIGNAL(readyRead()),this,SLOT(processPendingDatagram()));

    //connect(socket,&QUdpSocket::readyRead,this,&UdpSoket::processPendingDatagram);
    qDebug() << "绑定连接udp端口";

    connectServer();
}

//向服务器发送消息  请示连接
void UdpSoket::connectServer()
{
   //向此ip地址发送EE字符数据
   sendData("EE","192.168.1.250");

   //单singleShot，表示它只会触发一次，
   //2s发出一次信号，然后来执行槽函数，检查是否连接
   QTimer::singleShot(2000, this, SLOT(checkIsConnect()));
}

void UdpSoket::UdpDisConnect()
{
    disconnect(socket,SIGNAL(readyRead()),this,SLOT(processPendingDatagram()));
    isConnect = false;
}

UdpSoket::~UdpSoket()
{

}

void UdpSoket::processPendingDatagram()
{
    //用于存放接收的数据
    QByteArray datagram;

    //让datagram的大小为等待处理的数据报的大小，这样才能接收到完整的数据
    datagram.resize(socket->pendingDatagramSize());

    //读取数据包，将其存放在datagram中
    socket->readDatagram(datagram.data(),datagram.size());

    //解析数据包
    int ret = parser->parser(QString(datagram.data()));
    qDebug() << QString(datagram.data());

    //0表示解析成功
    if(ret == 0){
        //如果数据包是坐标和关节值
        if(parser->cmd == CoordJoint){
            if(glType == LanxunCoordRobot){
                j1Rot = parser->joint[0];   //大臂
                j2Rot = parser->joint[1];   //小臂
            }else{
                j1Rot = parser->joint[0] * 180 / Pi;   //大臂  转化为度
                j2Rot = parser->joint[1] * 180 / Pi;   //小臂  转化为度
            }

            if(glType == Lanxun6JointRobot){
                j3Rot = parser->joint[2] * 180 / Pi;
            }else{
                j3Rot = parser->joint[2];
            }

            j4Rot = parser->joint[3] * 180 / Pi;
            j5Rot = parser->joint[4] * 180 / Pi;
            j6Rot = parser->joint[5] * 180 / Pi;

            //变位机构
            c1Rot = parser->joint[6] * 180 / Pi;
            c2Rot = parser->joint[7] * 180 / Pi;

            //保存坐标
            for(int i = 0; i < 6;i++){
                coord[i] = parser->coord[i];
            }
            //坐标值包括两个变位机构
            coord[6] = c1Rot;
            coord[7] = c2Rot;

            //添加入轨迹坐标
            addTrialPoints();

            //重绘
            updateGL();

            //发送关节角度值
            sendJC();
            qDebug() << "j1Rot: " << j1Rot << " j2Rot: " << j2Rot << " j3Rot: " << j3Rot << " j4Rot: " << j4Rot << " j5Rot: " << j5Rot;
        }else if(parser->cmd == OpenFire){
            linecolor = Qt::red;
        }else if(parser->cmd == CloseFire){
            linecolor = Qt::white;
        }else if(parser->cmd == ConnectCmd){
            qDebug() << "连接成功";
            isConnect = true;   //为true
        }
     }
}


void UdpSoket::sendData(const QString& str,const QString& ipAddress){
    //发送数据函数必须发送数据到指定的对方的地址和端口
    //串口读取一般使用QByteArray数据，读取需要进行提取和解析，需要QByteArray转换为各类型数据
    //toLocal8Bit()返回的是一个字节数组类型QByteArray，使用[]、data()可读可写，at()只能读
    //string是基于16位unicode编码的字符数组
    socket->writeDatagram(str.toLocal8Bit().data(),str.size(),QHostAddress(ipAddress),8080);
}


 void UdpSoket::closeEvent(QCloseEvent *e)
{
     //关闭套接字的I / O设备，并调用DisconnectFromHost（）关闭套接字的连接。
     socket->close();
     return OpenglWidget::closeEvent(e);
}

 //这个还得测试去
void UdpSoket::keyPressEvent(QKeyEvent *e)
 {
     switch(e->key())
     {
      case Qt::Key_Escape:
      trailPoints.clear();
      updateGL();
      break;
     }
    return OpenglWidget::keyPressEvent(e);
}

//校验连接
void UdpSoket::checkIsConnect()
{
    if(!isConnect) // 连接失败
    {
      CommonFun::msgErr("连接失败，请检查！",this);
      UdpDisConnect();
      emit checkConnectSignal(false);
    }
    else
      emit checkConnectSignal(true);
}
