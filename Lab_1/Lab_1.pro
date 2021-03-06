QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    neural_networks/SimpleNeuralNetwork.cpp \
    neural_networks/AbstractClusterOfNeurons.cpp \
    neural_networks/AbstractSignalConverter.cpp \
    neural_networks/DebugNeuralNetwork.cpp \
    neural_networks/NeuralNetworkTrainer.cpp \
    Binary.cpp \
    BinaryConverter.cpp \
    Bipolar.cpp \
    BipolarConverter.cpp \
    Info.cpp \
    LinerBipolar.cpp \
    SwitchButton.cpp \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    math/matrix/MatrixOnRow.hpp \
    neural_networks/AbstractClusterOfNeurons.h \
    neural_networks/AbstractSignalConverter.h \
    neural_networks/DebugNeuralNetwork.h \
    neural_networks/NeuralNetworkTrainer.h \
    neural_networks/SimpleNeuralNetwork.h \
    Binary.h \
    BinaryConverter.h \
    Bipolar.h \
    BipolarConverter.h \
    Info.h \
    LinearBipolar.h \
    MainWindow.h \
    SwitchButton.h

FORMS += \
    Info.ui \
    MainWindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
