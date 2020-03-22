LIBS    += -L$$PWD/  -llibscara  #添加静态库


HEADERS += \
    $$PWD/GCodeConversion.h \
    $$PWD/ArcParse.h \
    $$PWD/NewScaraArgorithmn.h \
    $$PWD/scaralib.h \
    $$PWD/CodeEditDlg.h \
    $$PWD/GCodeListDlg.h \
    $$PWD/SimulationParse.h \
    $$PWD/RobotArgorithmn.h

SOURCES += \
    $$PWD/GCodeConversion.cpp \
    $$PWD/ArcParse.cpp \
    $$PWD/NewScaraArgorithmn.cpp \
    $$PWD/CodeEditDlg.cpp \
    $$PWD/GCodeListDlg.cpp \
    $$PWD/SimulationParse.cpp \
    $$PWD/RobotArgorithmn.cpp

FORMS += \
    $$PWD/CodeEditDlg.ui \
    $$PWD/GCodeListDlg.ui

