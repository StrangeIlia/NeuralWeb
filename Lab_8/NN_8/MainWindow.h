#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSet>
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

#include "PrDefinitions.h"
#include "neural_networks/SimpleNeuralNetwork.h"

#include "Console.h"
#include "SwitchButton.h"

#include "RecoveryImage.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void messageSended(QString html);
    void closed(QObject *);

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
    void hemming(bool ignored = false);

private:
    Ui::MainWindow *ui;

    Cluster inputCluster;
    Cluster outputCluster;

    SimpleNeuralNetwork *neuralNetwork;

    static Signal convertToSignal(MatrixPtr matrix);

    QList<Signal> getAllSignals();

    void sendMessage(QString html);

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
