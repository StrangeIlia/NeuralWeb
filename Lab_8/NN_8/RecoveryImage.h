#ifndef RECOVERYIMAGE_H
#define RECOVERYIMAGE_H

#include <QWidget>
#include <QTableWidgetItem>

#include "SwitchButton.h"
#include "neural_networks/SimpleNeuralNetwork.h"

namespace Ui {
class RecoveryImage;
}

class RecoveryImage : public QWidget
{
    Q_OBJECT

public:
    explicit RecoveryImage(int rowCount, int columnCount,
                           Cluster inputCluster,
                           Cluster outputCluster,
                           SimpleNeuralNetwork *neuralNetwork,
                           QWidget *parent = nullptr);
    ~RecoveryImage();

signals:
    void closed(QObject *);

private slots:
    void distoredImageChanged(bool ignored);

private:
    int idOperation;

    Ui::RecoveryImage *ui;

    Cluster inputCluster;
    Cluster outputCluster;

    SimpleNeuralNetwork *neuralNetwork;

    void initTable();
    void closeEvent(QCloseEvent *ignored);

    Signal distoredSignal() const;
    void setRestoredSignal(const Signal &signal);

    static void activateSignal(Signal &currentSignal, const Signal &lastSignal);
};

#endif // RECOVERYIMAGE_H
