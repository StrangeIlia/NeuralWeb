#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QHash>
#include <QDebug>
#include <QVector>
#include <QTextStream>
#include <QMainWindow>
#include <QMessageBox>
#include <QStandardItemModel>

#include <cmath>
#include <limits>

#include "Info.h"
#include "Binary.h"
#include "Bipolar.h"
#include "SwitchButton.h"
#include "LinearBipolar.h"
#include "BinaryConverter.h"
#include "BipolarConverter.h"
#include "neural_networks/DebugNeuralNetwork.h"
#include "neural_networks/NeuralNetworkTrainer.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

typedef QSharedPointer<Matrix>                  MatrixPtr;
typedef QSharedPointer<QVariantHash>            QVariantHashPtr;
typedef QSharedPointer<QAbstractItemModel>      QAbstractItemModelPtr;
typedef QSharedPointer<AbstractSignalConverter> SignalConverterPtr;

Q_DECLARE_METATYPE(MatrixPtr);
Q_DECLARE_METATYPE(QVariantHashPtr);
Q_DECLARE_METATYPE(QAbstractItemModelPtr);

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void changeImage(int index);
    void changeImageSet(int index);
    void addImage(bool ignored = false);
    void removeImage(bool ignored = false);
    void saveImage(bool ignored = false);
    void addGroup(bool ignored = false);
    void removeGroup(bool ignored = false);
    /// Чтобы можно было использовать без указания значения ignored
    void clearTableData(bool ignored = false);
    void training(bool ignored = false);
    void recognize(bool ignored = false);
    void centring(bool ignored = false);

private:
    Ui::MainWindow *ui;

    /// Эти кластеры считают
    Binary *binary;
    Bipolar *bipolar;
    LinearBipolar *linearBipolar;

    /// Это входные кластеры
    Binary *inputBinary;
    Bipolar *inputBipolar;

    /// Это нейронные сети
    DebugNeuralNetwork *binaryNet;
    DebugNeuralNetwork *bipolarNet;
    SimpleNeuralNetwork *linearBipolarNet;

    template<class T>
    T dataInHash(QVariantHashPtr hash, QString field);

    QAbstractItemModelPtr model(int groupIndex);
    void setModel(int groupIndex, QAbstractItemModelPtr model);

    MatrixPtr table();
    void setTable(MatrixPtr matrix);

    MatrixPtr matrix(int groupIndex, int imageIndex);
    void setMatrix(int groupIndex, int imageIndex, MatrixPtr matrix);

    void addImage(int group);
    void removeImage(int group, int image);

    void showMessage();

    void initTable();
    void initImageSet();
    void initNeuralWebs();
    void initImageGroups();
    void updateLinearBipolar();
    void deleteUnusedImages(int imageGroup);

    QVector<MatrixPtr> images(int groupIndex);

    void printfInfo(bool ignored);

    static QString Models;
    static QString MatrixData;
};



#endif // MAINWINDOW_H
