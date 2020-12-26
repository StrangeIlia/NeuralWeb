#include "RecoveryImage.h"
#include "ui_RecoveryImage.h"

RecoveryImage::RecoveryImage(int rowCount,
                             int columnCount,
                             Cluster inputCluster,
                             Cluster outputCluster,
                             SimpleNeuralNetwork *neuralNetwork,
                             QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecoveryImage)
{
    ui->setupUi(this);

    ui->restore->setVisible(false);
    ui->restoredImage->setEnabled(false);

    ui->distoredImage->setRowCount(rowCount);
    ui->distoredImage->setColumnCount(columnCount);

    ui->restoredImage->setRowCount(rowCount);
    ui->restoredImage->setColumnCount(columnCount);

    initTable();

    this->inputCluster = inputCluster;
    this->outputCluster = outputCluster;
    this->neuralNetwork = neuralNetwork;
}

RecoveryImage::~RecoveryImage() {
    delete ui;
}

void RecoveryImage::distoredImageChanged(bool /*ignored*/) {
    idOperation += 1;
    int currentOperation = idOperation;

    bool hasChanged = true;
    auto lastSignal = distoredSignal();

    ui->restore->setVisible(true);
    while(hasChanged) {
        QApplication::processEvents();

        hasChanged = false;

        inputCluster->outputSignal() = lastSignal;
        neuralNetwork->updateNetwork();
        activateSignal(outputCluster->outputSignal(), lastSignal);

        for(int i = 0; i != outputCluster->outputSignal().size(); ++i) {
            auto first = lastSignal.signal(i);
            auto second = outputCluster->outputSignal().signal(i);
            if(std::abs(first - second) > 1e-7) {
                hasChanged = true;
                break;
            }
        }

        lastSignal = outputCluster->outputSignal();

        if(idOperation != currentOperation) {
            return;
        }
    }

    ui->restore->setVisible(false);
    setRestoredSignal(lastSignal);
}


void RecoveryImage::initTable() {
    ui->restoredImage->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
    ui->restoredImage->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);

    ui->distoredImage->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
    ui->distoredImage->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
    for(int i = 0; i != ui->distoredImage->rowCount(); ++i) {
        for (int j = 0; j != ui->distoredImage->columnCount(); ++j) {
            auto button = new SwitchButton(Qt::white, Qt::black, false, this);
            ui->distoredImage->setCellWidget(i, j, button);
            connect(button, &QPushButton::clicked, this, &RecoveryImage::distoredImageChanged);
            ui->restoredImage->setCellWidget(i, j, new SwitchButton(Qt::white, Qt::black, false, this));
        }
    }
}

void RecoveryImage::closeEvent(QCloseEvent */*ignored*/){
    emit closed(this);
    deleteLater();
}

Signal RecoveryImage::distoredSignal() const {
    Signal result(ui->distoredImage->rowCount() * ui->distoredImage->columnCount());
    for(int i = 0; i != ui->distoredImage->rowCount(); ++i) {
        for(int j = 0; j != ui->distoredImage->columnCount(); ++j) {
            SwitchButton *button = (SwitchButton*)ui->distoredImage->cellWidget(i, j);
            result.setSignal(button->isSelected() ? 1 : -1, i * ui->distoredImage->columnCount() + j);
        }
    }
    return std::move(result);
}

void RecoveryImage::setRestoredSignal(const Signal &signal) {
    for(int i = 0; i != ui->restoredImage->rowCount(); ++i) {
        for(int j = 0; j != ui->restoredImage->columnCount(); ++j) {
            SwitchButton *button = (SwitchButton*)ui->restoredImage->cellWidget(i, j);
            button->setSelected(signal.signal(i * ui->restoredImage->columnCount() + j) > 0);
        }
    }
}

void RecoveryImage::activateSignal(Signal &currentSignal, const Signal &lastSignal)  {
    for(int i = 0; i != currentSignal.size(); ++i) {
        auto signal = currentSignal.signal(i);
        if(std::abs(signal) < 1e-7) {
            currentSignal.setSignal(lastSignal.signal(i), i);
        } else {
            currentSignal.setSignal(signal > 0 ? 1 : -1, i);
        }
    }
}
