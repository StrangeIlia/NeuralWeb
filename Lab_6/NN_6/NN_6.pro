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

# OpenMP для параллельных вычислениях на процессоре
# Правда, для таких мелких расчетов создание и удаление нитей очень длительно
QMAKE_LFLAGS += -fopenmp
QMAKE_CXXFLAGS += -fopenmp

SOURCES += \
    Hemming.cpp \
    neural_networks/Signal.cpp \
    neural_networks/AbstractActivation.cpp \
    neural_networks/RelatationWeights.cpp \
    neural_networks/SimpleClusterOfNeurons.cpp \
    neural_networks/SimpleNeuralNetwork.cpp \
    neural_networks/SimpleNeuralNetworkTrainer.cpp \
    Binary.cpp \
    Console.cpp \
    SwitchButton.cpp \
    main.cpp \
    MainWindow.cpp


HEADERS += \
    Hemming.h \
    math/matrix/MatrixOnRow.hpp \
    neural_networks/Definitions.h \
    neural_networks/Signal.h \
    neural_networks/RelatationWeights.h \
    neural_networks/AbstractActivation.h \
    neural_networks/SimpleClusterOfNeurons.h \
    neural_networks/SimpleNeuralNetwork.h \
    neural_networks/SimpleNeuralNetworkTrainer.h \
    Binary.h \
    Console.h \
    MainWindow.h \
    SwitchButton.h

FORMS += \
    Console.ui \
    MainWindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
