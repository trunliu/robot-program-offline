#include "Parser.h"
#include <QDebug>

//解析数据包内容
int Parser::parser(const QString &data)
{
     QStringList list = data.split(",");
     return getValue(list);
}

//提取数据,数据包结构包括，(指令，指令长度，6个坐标，8关节值)
int Parser::getValue(const QStringList & list)
{
    if(list.size() < 2)
        return -1;
    if(list.size() != list[1].toInt())
        return -1;

    if(list[0] == "UC"){
        cmd = CoordJoint;
        for(int i=2;i<16;++i){
            if(i<8){
                coord[i-2] = list[i].toDouble();
            }else{
                joint[i-8] = list[i].toDouble();
            }
        }
//        for(int i = 2;i < 8;i++){
//           coord[i - 2] = list[i].toDouble();
//        }
//        for(int i = 8; i < 16; i++){
//           joint[i - 8] = list[i].toDouble();
//        }
         return 0;
    }

    if(list[0] == "M7"){
        qDebug() << "开火";
        cmd = OpenFire;
        return 0;
    }

    if(list[0] == "M8"){
        qDebug() << "关火";
        cmd = CloseFire;
        return 0;
    }

    if(list[0] == "EE"){
        qDebug() << "连接";
        cmd = ConnectCmd;
        return 0;
    }
    else
    return -1;
}
