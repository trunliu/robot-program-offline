#ifndef PARSER_H
#define PARSER_H
#include <QStringList>
enum Command
{
    CoordJoint,
    OpenFire,
    CloseFire,
    ConnectCmd
};
class Parser
{
public:
     int getValue(const QStringList & list);
     int parser(const QString &data);
     double coord[6]; //坐标值值包括变位机构
     double joint[8]; //关节角的度数
     Command cmd;
};
#endif // PARSER_H
