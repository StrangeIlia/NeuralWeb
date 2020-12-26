#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QHash>
#include <QVector>
#include <QDateTime>
#include <QTextStream>
#include <QMainWindow>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QStandardItemModel>

#include <QIntValidator>

#include <cmath>
#include <limits>

#include "Hemming.h"
#include "neural_networks/SimpleNeuralNetwork.h"

#include "Console.h"
#include "SwitchButton.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

typedef QSharedPointer<Matrix>                  MatrixPtr;
typedef QSharedPointer<QVariantHash>            QVariantHashPtr;
typedef QSharedPointer<QAbstractItemModel>      QAbstractItemModelPtr;

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
    void recognize(bool ignored = false);
    void printfInfo(bool ignored = false);

    void changeLearningFactor(double value = 1.0);

private:
    Ui::MainWindow *ui;

    double __e__ = 0.01;
    int summImageCount = 0;

    Cluster inputCluster;
    Cluster outputCluster;

    Hemming *hemming;

    SimpleNeuralNetwork *neuralNetwork;

    static Signal convertToSignal(MatrixPtr matrix);

    void sendMessage(QString html);
    /// Ищет наиболее близкую группу к данному изображению
    int closestImage(MatrixPtr matrix);

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

    double maxError();

    void initTable();
    void initImageSet();
    void initNeuralWebs();
    void initImageGroups();
    void deleteUnusedImages(int imageGroup);

    QVector<MatrixPtr> images(int groupIndex);

    void closeEvent(QCloseEvent* event) override;

    static QString Models;
    static QString MatrixData;
};



#endif // MAINWINDOW_H
