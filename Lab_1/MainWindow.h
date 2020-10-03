#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QStandardItemModel>

#include <limits>

#include "SwitchButton.h"

#include "Binary.hpp"
#include "Bipolar.hpp"
#include "MyActivation.hpp"

#include <QVector>

#include "neural_networks/DebugNeuralNetwork.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void changeImage(int index);
    void changeImageSet(int index);
    void addImage(bool);
    void removeImage(bool);
    void saveImage(bool);

private:
    Ui::MainWindow *ui;
    Bipolar<double> *bipolar;
    Binary<double> *binary;
    MyActivation<double> *my;
    Bipolar<double> *inputBipolar;
    Binary<double> *inputBinary;
    DebugNeuralNetwork<double> *binaryNet;
    DebugNeuralNetwork<double> *bipolarNet;


    void showMessage();
    void addGroup(bool isValid);
    void removeGroup(bool isValid);

    void initImageSet();
    void initImageGroups();
    void initTable();
    void initNeuralWebs();

    void setImageData(int index, const QVector<bool> &vec);
    void setTableData(const QVector<bool> &vec);
    void readTableData(QVector<bool> &vec);
    void clearTableData();

    typedef QVector<MatrixOnRow<double>> Singals;

    Singals getOutputSignals() const;
    Singals getInputsSignals(int group) const;
    bool reqStop(const QVector<Singals> &inputs, const Singals& outputs) const;
    void training(bool ignored);
    void recognize(bool ignored);

    QAbstractItemModel *modelForNewGroup() const;

    static QString Images;
    static QString Matrix;
};
#endif // MAINWINDOW_H
