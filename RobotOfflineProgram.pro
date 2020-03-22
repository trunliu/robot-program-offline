QT       += core gui  opengl network printsupport
RC_FILE = icon.rc
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = RobotOfflineProgram
TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS
include (./MainWindow/MainWindow.pri)  #导入pri文件
include (./3DModel/3DModel.pri)        #导入pri文件
include (./CommonFun/CommonFun.pri)    #导入pri文件
include (./Alignment/Alignment.pri)    #导入pri文件
include (./CodeParse/CodeParse.pri)    #导入pri文件
include (./Gallery/Gallery.pri)        #导入pri文件
include (./Setting/Setting.pri)        #导入pri文件
include (./Teach/Teach.pri)            #导入pri文件
RESOURCES += \
    res.qrc

HEADERS +=

SOURCES +=

INCLUDEPATH += C:\openLIB\eigen
