#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QHash>
#include <QVector>
#include <QTextStream>
#include <QMainWindow>
#include <QMessageBox>
#include <QStandardItemModel>

#include <cmath>
#include <limits>

#include "Console.h"
#include "Bipolar.h"
#include "Adaline.h"
#include "SwitchButton.h"
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

signals:
    void messageSended(QString html);
    void closed(QObject*);

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
    void printfInfo(bool ignored = false);

private:
    Ui::MainWindow *ui;

    /// Это входные кластеры
    Bipolar *inputCluster;

    /// Эти кластеры считают
    Adaline *outputCluster;

    /// Это нейронная сеть
    DebugNeuralNetwork *nueralNetwork;

    void __centring();

    void sendMessage(QString html);
    /// Ищет наиболее близкую группу к данному изображению
    int closestGroup(MatrixPtr matrix);

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

    void createNewGroup();
    void removeGroup(int group);

    void initTable();
    void initImageSet();
    void initNeuralWebs();
    void initImageGroups();
    void deleteUnusedImages(int imageGroup);

    QVector<MatrixPtr> images(int groupIndex);

    void calcError(int groupNumber, double &standardDev, double &maximumDev);

    void closeEvent(QCloseEvent* event) override;

    static QString Models;
    static QString MatrixData;
};



#endif // MAINWINDOW_H
