#include "MainWindow.h"
#include "ui_MainWindow.h"

QString MainWindow::Images = "Images";
QString MainWindow::Matrix = "Matrix";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initImageSet();
    initTable();
    initImageGroups();
    initNeuralWebs();
}

MainWindow::~MainWindow() {
    delete ui;
    delete bipolar;
    delete binary;
    delete my;
    delete inputBipolar;
    delete inputBinary;
    delete binaryNet;
    delete bipolarNet;
}

void MainWindow::changeImage(int index) {
    ui->imageInfo->setText("");
    auto variant = ui->image->itemData(index);
    if(!variant.isValid() || !variant.canConvert<QVariantHash>()) {
        QMessageBox messageBox(this);
        messageBox.setWindowTitle(tr("Внутренняя ошибка"));
        messageBox.setText(tr("У изображения нет множества пользовательских данных"));
        messageBox.exec();
        this->close();
        return;
    }
    auto hash = variant.value<QVariantHash>();
    if(hash.find(Matrix) == hash.end()) {
        QMessageBox messageBox(this);
        messageBox.setWindowTitle(tr("Внутренняя ошибка"));
        messageBox.setText(tr("У изображения нет матрицы для данных"));
        messageBox.exec();
        this->close();
        return;
    }
    variant = hash[Matrix];
    if(!variant.isValid() || !variant.canConvert<QVector<bool>>()) {
        QMessageBox messageBox(this);
        messageBox.setWindowTitle(tr("Внутренняя ошибка"));
        messageBox.setText(tr("Изображение имеет неверный формат данных"));
        messageBox.exec();
        this->close();
    }
    auto matrix = variant.value<QVector<bool>>();
    if(matrix.size() != ui->table->rowCount() * ui->table->columnCount()) {
        ui->imageInfo->setText(tr("В данном изображении не записаны данные!!!"));
        clearTableData();
    } else setTableData(matrix);
}

void MainWindow::changeImageSet(int index) {
    auto variant = ui->imageGroup->itemData(index);
    if(!variant.isValid() || ! variant.canConvert<QVariantHash>()) {
        ui->image->setEnabled(false);
        return;
    }
    auto hash = variant.value<QVariantHash>();
    ui->image->setModel(hash[Images].value<QAbstractItemModel*>());
    ui->image->setEnabled(true);
}

void MainWindow::showMessage() {
    QMessageBox messageBox(this);
    messageBox.setWindowTitle("Подайте на пропитание");
    QString message = tr("Данная функция доступна лишь в premium версии");
    message += tr("\nПриобретите данную функцию всего за 9.99$");
    messageBox.setText(message);
    messageBox.exec();
    return;
}

void MainWindow::addGroup(bool isValid) {
    if(!isValid) {
        showMessage();
        return;
    }

    QString name = tr("Новая группа");
    if(ui->imageGroup->findText(name) != -1) {
        int index = 2;
        QString tmpName = name + " " + QString::number(index);
        while(ui->imageGroup->findText(tmpName) != -1) {
            ++index;
            tmpName = name + " " + QString::number(index);
        }
        name = tmpName;
    }
    QVariantHash hash;
    hash[Images] = QVariant::fromValue(modelForNewGroup());
    ui->imageGroup->addItem(name, hash);
    ui->imageGroup->setCurrentIndex(ui->imageGroup->count() - 1);
}

void MainWindow::removeGroup(bool isValid) {
    if(!isValid) {
        showMessage();
        return;
    }

    if(ui->imageGroup->count() <= 2) {
        QMessageBox messageBox(this);
        messageBox.setWindowTitle(tr("Ошибка ввода данных"));
        messageBox.setText(tr("Для работы должно быть не меньше двух групп"));
        messageBox.exec();
        return;
    }

    auto variant = ui->imageGroup->currentData();
    if(variant.isValid() && variant.canConvert<QVariantHash>()) {
        auto hash = variant.value<QVariantHash>();
        if(hash.find(Images) != hash.end()) {
            variant = hash.find(Images).value();
            if(variant.isValid() && variant.canConvert<QAbstractItemModel*>()){
                variant.value<QAbstractItemModel*>()->deleteLater();
            }
        }
    }

    auto model = ui->imageGroup->model();
    model->removeRow(ui->imageGroup->currentIndex());
}

void MainWindow::addImage(bool /*value*/) {
    QString name = tr("Новое изображение");
    if(ui->image->findText(name) != -1) {
        int index = 2;
        QString tmpName = name + " " + QString::number(index);
        while(ui->image->findText(tmpName) != -1) {
            ++index;
            tmpName = name + " " + QString::number(index);
        }
        name = tmpName;
    }
    auto model = ui->image->model();
    model->insertRow(model->rowCount());
    auto index = model->index(model->rowCount() - 1, 0);
    model->setData(index, name);
    model->setData(index, QVariant::fromValue(QVector<bool>()), Qt::UserRole);
    ui->image->setCurrentIndex(model->rowCount() - 1);
}

void MainWindow::removeImage(bool /*value*/) {
    if(ui->image->count() <= 1) {
        QMessageBox messageBox(this);
        messageBox.setWindowTitle(tr("Ошибка ввода данных"));
        messageBox.setText(tr("Для работы должно быть не меньше одного изображения в группе"));
        messageBox.exec();
        return;
    }
    auto model = ui->image->model();
    model->removeRow(ui->image->currentIndex());
}

void MainWindow::saveImage(bool /*value*/) {
    QVector<bool> readedMatrix;
    readTableData(readedMatrix);
    setImageData(ui->image->currentIndex(), readedMatrix);
    ui->imageInfo->setText("");
}

void MainWindow::initImageSet() {
    connect(ui->addImage, SIGNAL(clicked(bool)), this, SLOT(addImage(bool)));
    connect(ui->saveImage, SIGNAL(clicked(bool)), this, SLOT(saveImage(bool)));
    connect(ui->removeImage, SIGNAL(clicked(bool)), this, SLOT(removeImage(bool)));
    connect(ui->image, SIGNAL(currentIndexChanged(int)), this, SLOT(changeImage(int)));
}

void MainWindow::initImageGroups() {
    addGroup(true);
    addGroup(true);
    connect(ui->addGroup, &QAbstractButton::clicked, [this] (bool) { this->addGroup(false); });
    connect(ui->removeGroup, &QAbstractButton::clicked, [this] (bool) { this->removeGroup(false); });
    connect(ui->imageGroup, SIGNAL(currentIndexChanged(int)), this, SLOT(changeImageSet(int)));
    ui->imageGroup->setCurrentIndex(0);
}

void MainWindow::initTable() {
    ui->table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
    ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
    for(int i = 0; i != ui->table->rowCount(); ++i) {
        for (int j = 0; j != ui->table->columnCount(); ++j) {
            ui->table->setCellWidget(i, j, new SwitchButton(Qt::white, Qt::black));
        }
    }
    connect(ui->clear, &QAbstractButton::clicked, [this] (bool) { this->clearTableData(); });
}

void MainWindow::initNeuralWebs() {
    bipolar = new Bipolar<double>(1);
    binary = new Binary<double>(1);
    my = new MyActivation<double>(1, -1, 1);

    int inputCount = ui->table->rowCount() * ui->table->columnCount();
    inputBipolar = new Bipolar<double>(inputCount);
    inputBinary = new Binary<double>(inputCount);

    inputBipolar->addOutput(bipolar);
    inputBipolar->addOutput(my);

    inputBinary->addOutput(binary);

    binaryNet = new DebugNeuralNetwork<double>(true, false);
    bipolarNet = new DebugNeuralNetwork<double>(true, false);

    binaryNet->addCluster(binary);
    binaryNet->addCluster(inputBinary);

    bipolarNet->addCluster(my);
    bipolarNet->addCluster(bipolar);
    bipolarNet->addCluster(inputBipolar);

    binaryNet->updateClusterSequence();
    bipolarNet->updateClusterSequence();

    connect(ui->learning, &QPushButton::clicked, this, &MainWindow::training);
    connect(ui->recognizeImage, &QPushButton::clicked, this, &MainWindow::recognize);
    connect(ui->info, &QPushButton::clicked, this, &MainWindow::printfInfo);
}

void MainWindow::setImageData(int index, const QVector<bool> &vec) {
    if(vec.size() != ui->table->rowCount() * ui->table->columnCount()) {
        throw std::exception();
        return;
    }
    auto variant = ui->image->itemData(index);
    if(!variant.isValid() || !variant.canConvert<QVariantHash>()) {
        throw std::exception();
        return;
    }
    auto hash = variant.value<QVariantHash>();
    if(hash.find(Matrix) == hash.end()) {
        throw std::exception();
        return;
    }
    hash[Matrix] = QVariant::fromValue(vec);
    ui->image->setItemData(index, hash);
}

void MainWindow::setTableData(const QVector<bool> &vec) {
    for(int i = 0; i != ui->table->rowCount(); ++i) {
        for (int j = 0; j != ui->table->columnCount(); ++j) {
            SwitchButton *button = (SwitchButton*)ui->table->cellWidget(i, j);
            button->setSelected(vec[i * ui->table->columnCount() + j]);
        }
    }
}

void MainWindow::readTableData(QVector<bool> &vec) {
    vec.resize(ui->table->rowCount() * ui->table->columnCount());
    for(int i = 0; i != ui->table->rowCount(); ++i) {
        for (int j = 0; j != ui->table->columnCount(); ++j) {
            SwitchButton *button = (SwitchButton*)ui->table->cellWidget(i, j);
            vec[i * ui->table->columnCount() + j] = button->isSelected();
        }
    }
}

void MainWindow::clearTableData() {
    for(int i = 0; i != ui->table->rowCount(); ++i) {
        for (int j = 0; j != ui->table->columnCount(); ++j) {
            SwitchButton *button = (SwitchButton*)ui->table->cellWidget(i, j);
            button->setSelected(false);
        }
    }
}

MainWindow::Singals MainWindow::getOutputSignals() const {
    int imageGroupCount = ui->imageGroup->count();
    QVector<MatrixOnRow<double>> outputSingal(imageGroupCount);
    for(int i = 0; i != outputSingal.size(); ++i) {
        auto& matrix = outputSingal[i];
        matrix.setSize(imageGroupCount - 1, 1);
        for(int j = 0; j != matrix.rows(); ++j) {
            matrix(j, 0) = (i == j) ? 0 : 1;
        }
    }
    return outputSingal;
}

MainWindow::Singals MainWindow::getInputsSignals(int group) const {
    auto groupHash = ui->imageGroup->itemData(group).value<QVariantHash>();
    QAbstractItemModel* model = groupHash[Images].value<QAbstractItemModel*>();
    QVector<MatrixOnRow<double>> inputSingal(model->rowCount());
    for(int i = 0; i != model->rowCount(); ++i) {
        auto imageHash = model->data(model->index(i, 0), Qt::UserRole).value<QVariantHash>();
        QVector<bool> vector = imageHash[Matrix].value<QVector<bool>>();
        auto& matrix = inputSingal[i];
        matrix.setSize(vector.size(), 1);
        for(int i = 0; i != vector.size(); ++i) {
            matrix(i, 0) = vector[i] ? 1 : 0;
        }
    }
    return inputSingal;
}

bool MainWindow::reqStop(const QVector<Singals> &inputs, const Singals& outputs) const {
    double eps = std::numeric_limits<double>::epsilon();
    for(int i = 0; i != outputs.size(); ++i ){
        const auto& outputMatrix = outputs[i];
        const auto& vectorInputs = inputs[i];
        for(int j = 0; j != vectorInputs.size(); ++j) {
            inputBinary->writeOutputSignalsWithNorm(vectorInputs[j], 0, 1);
            inputBipolar->writeOutputSignalsWithNorm(vectorInputs[j], 0, 1);

            binaryNet->updateNetwork();
            bipolarNet->updateNetwork();

            MatrixOnRow<double> binaryMatrix;
            binary->readOutputSignalsWithNorm(binaryMatrix, 0, 1);

            MatrixOnRow<double> bipolarMatrix;
            bipolar->readOutputSignalsWithNorm(bipolarMatrix, 0, 1);

            for(int j = 0; j != outputMatrix.rows(); ++j) {
                double binaryDiff = binaryMatrix(j, 0) - outputMatrix(j, 0);
                double bipolarDiff = bipolarMatrix(j, 0) - outputMatrix(j, 0);
                if(std::abs(binaryDiff) > eps) return false;
                if(std::abs(bipolarDiff) > eps) return false;
            }
        }
    }

    return true;
}

void MainWindow::initMy(const QVector<Singals> &inputs) {
    if(inputs.count() > 2 || bipolar->neuronsCount() != 1) return;

    double avgS1 = 0, avgS2 = 0;
    for(int j = 0; j != inputs[0].size(); ++j) {
        inputBipolar->writeOutputSignalsWithNorm(inputs[0][j], 0, 1);

        bipolarNet->updateNetwork();

        MatrixOnRow<double> summSignal = bipolarNet->summSignal()[bipolar];
        for(int ii = 0; ii != summSignal.rows(); ++ii)
            avgS1 += summSignal(ii, 0) / inputs[0].size();
    }

    for(int j = 0; j != inputs[1].size(); ++j) {
        inputBipolar->writeOutputSignalsWithNorm(inputs[1][j], 0, 1);

        bipolarNet->updateNetwork();

        MatrixOnRow<double> summSignal = bipolarNet->summSignal()[bipolar];
        for(int ii = 0; ii != summSignal.rows(); ++ii)
            avgS2 += summSignal(ii, 0) / inputs[1].size();
    }

    double avg = (avgS1 + avgS2) / 2;
    double shift = std::abs(avgS1 - avgS2) / 6;

    my->q1(avg - shift);
    my->q2(avg + shift);
}

void MainWindow::training(bool /*ignored*/) {
    Singals outputs = getOutputSignals();
    QVector<Singals> inputs;
    for(int i = 0; i != outputs.size(); ++i)
        inputs.push_back(getInputsSignals(i));

    while(!reqStop(inputs, outputs)) {
        for(int i = 0; i != outputs.size(); ++i ){
            const auto& vectorInputs = inputs[i];
            binary->writeOutputSignalsWithNorm(outputs[i], 0, 1);
            bipolar->writeOutputSignalsWithNorm(outputs[i], 0, 1);

            for(int j = 0; j != vectorInputs.size(); ++j) {
                inputBinary->writeOutputSignalsWithNorm(vectorInputs[j], 0, 1);
                inputBipolar->writeOutputSignalsWithNorm(vectorInputs[j], 0, 1);

                binaryNet->training();
                bipolarNet->training();
            }
        }
        printfInfo(false);
    }

    my->writeShiftWeight(bipolar->getWeightingShift());
    my->writeWeightingFactors(bipolar->getWeightingFactors());
    initMy(inputs);

    ui->result->setText(tr("Нейронные сети обучены"));
}

void MainWindow::recognize(bool /*ignored*/) {
    QVector<bool> vector;
    readTableData(vector);
    MatrixOnRow<double> input(vector.size(), 1);
    for(int i = 0; i != vector.size(); ++i)
        input(i, 0) = vector[i] ? 1 : 0;
    inputBinary->writeOutputSignalsWithNorm(input, 0, 1);
    inputBipolar->writeOutputSignalsWithNorm(input, 0, 1);
    binaryNet->updateNetwork();
    bipolarNet->updateNetwork();

    QString resultStr;
    MatrixOnRow<double> output;
    double eps = std::numeric_limits<double>::epsilon();
    auto functor = [&output, eps] () {
        int shiftedIndex = 0;
        for(int i = 0; i != output.rows(); ++i) {
            if(output(i, 0) > eps) {
                shiftedIndex = i + 1;
                break;
            }
        }
        return shiftedIndex;
    };


    binary->readOutputSignalsWithNorm(output, 0, 1);
    resultStr += tr("Бинарная сеть считает, что это группа \"");
    resultStr += ui->imageGroup->itemText(functor()) + "\"\n";

    bipolar->readOutputSignalsWithNorm(output, 0, 1);
    resultStr += tr("Биполярная сеть считает, что это группа \"");
    resultStr += ui->imageGroup->itemText(functor()) + "\"\n";

    my->readOutputSignalsWithNorm(output, 0, 1);
    resultStr += tr("Линейная биполярная сеть считает, что это группа \"");
    resultStr += ui->imageGroup->itemText(functor()) + "\"\n";

    ui->result->setText(resultStr);
}

void MainWindow::printfInfo(bool /*ignored*/) {
    QString str;
    QTextStream out(&str);

    int rowsCount = ui->table->rowCount();
    int columnsCount = ui->table->columnCount();

    auto functor = [&out, rowsCount, columnsCount] (AbstractClusterOfNeurons<double> *cluster){
        for(int k = 0; k != cluster->neuronsCount(); ++k) {
            out << tr("Весовые коэффициенты для ");
            out << QString::number(k + 1) << tr(" нейрона");
            const auto &shift = cluster->getWeightingShift();
            const auto &weight = cluster->getWeightingFactors();
            for(int i = 0; i != rowsCount; ++i) {
                out << "\n";
                out.setFieldWidth(8);
                for(int j = 0; j != columnsCount; ++j)
                    out << round(weight(i, j) * 1e+5) * 1e-5 << " ";
                out.setFieldWidth(0);
            }
            out.setFieldWidth(8);
            out << shift(k, 0) << " ";
            out.setFieldWidth(0);
            out << "\n";
        }
    };

    out << tr("Информация по биполярной функции:\n");
    functor(bipolar); out << "\n\n\n\n";

    out << tr("Информация по бинарной функции:\n");
    functor(binary); out << "\n\n\n\n";

    out << tr("Информация по линейной биполярной функции:\n");
    out << "Q1 = " << QString::number(my->q1()) << "\n";
    out << "Q2 = " << QString::number(my->q2()) << "\n";
    out << "k = " << QString::number(2 / (my->q2() - my->q1())) << "\n";
    functor(binary);

    Info info(this);
    info.doc()->setPlainText(str);
    info.exec();
}

QAbstractItemModel *MainWindow::modelForNewGroup() const {
    QStandardItemModel *model = new QStandardItemModel();
    model->insertRows(0, 1);
    model->insertColumns(0, 1);
    auto index = model->index(0, 0);
    model->setData(index, tr("Новое изображение"));
    QVariantHash hash;
    hash[Matrix] = QVariant::fromValue(QVector<bool>());
    model->setData(index, hash, Qt::UserRole);
    return model;
}
