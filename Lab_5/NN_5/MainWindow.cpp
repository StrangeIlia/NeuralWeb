#include "MainWindow.h"
#include "ui_MainWindow.h"

QString MainWindow::Models      = "Images";
QString MainWindow::MatrixData  = "Matrix";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initNeuralWebs();

    initImageSet();
    initTable();
    initImageGroups();

//    test();
}

void MainWindow::test() {
    auto inputCluster = new SimpleClusterOfNeurons(2);
    auto hidden_1 = new SimpleClusterOfNeurons(2);
    auto hidden_2 = new SimpleClusterOfNeurons(2);
    auto outputCluster = new SimpleClusterOfNeurons(2);

    inputCluster->addOutput(hidden_1);
    hidden_1->addOutput(hidden_2);
    hidden_2->addOutput(outputCluster);

    inputCluster->setActivationFunction(sigmoid);
    hidden_1->setActivationFunction(sigmoid);
    hidden_2->setActivationFunction(sigmoid);
    outputCluster->setActivationFunction(sigmoid);

    auto lr = sigmoid->learningRate();
    sigmoid->setLearningRate(0.1);

    hidden_1->weights().setShift(0, 0);
    hidden_1->weights().setShift(0, 1);
    hidden_1->weights().setWeight( 0.2, 0, 0);
    hidden_1->weights().setWeight(-0.3, 0, 1);
    hidden_1->weights().setWeight( 0.3, 1, 0);
    hidden_1->weights().setWeight(-0.2, 1, 1);

    hidden_2->weights().setShift(0, 0);
    hidden_2->weights().setShift(0, 1);
    hidden_2->weights().setWeight(-0.1, 0, 0);
    hidden_2->weights().setWeight(-0.1, 0, 1);
    hidden_2->weights().setWeight( 0.2, 1, 0);
    hidden_2->weights().setWeight(-0.3, 1, 1);

    outputCluster->weights().setShift(0, 0);
    outputCluster->weights().setWeight( 0.4, 0, 0);
    outputCluster->weights().setWeight(-0.2, 0, 1);

    inputCluster->outputSignal().setSignal(0.5, 0);
    inputCluster->outputSignal().setSignal(0.3, 1);

    hidden_1->summation();          hidden_1->activation();
    hidden_2->summation();          hidden_2->activation();
    outputCluster->summation();     outputCluster->activation();

    Signal error(2);
    for(int i = 0; i != 1000; ++i) {
        error.setSignal(1 - outputCluster->outputSignal().signal(0), 0);
        error.setSignal(0 - outputCluster->outputSignal().signal(1), 1);
        error = sigmoid->correct(hidden_2->outputSignal(), outputCluster->outputSignal(), outputCluster->weights(), error);
        error = sigmoid->correct(hidden_1->outputSignal(), hidden_2->outputSignal(), hidden_2->weights(), error);
        sigmoid->correct(inputCluster->outputSignal(), hidden_1->outputSignal(), hidden_1->weights(), error);

        /// Лол, сигнал сдвига влияет на скорость сходимости
        /// В плохую сторону, хотя логично, больше влияющих факторов -> дольше обучения
        hidden_1->weights().setShift(0, 0);
        hidden_1->weights().setShift(0, 1);
        hidden_2->weights().setShift(0, 0);
        hidden_2->weights().setShift(0, 1);
        outputCluster->weights().setShift(0, 0);

        hidden_1->summation();          hidden_1->activation();
        hidden_2->summation();          hidden_2->activation();
        outputCluster->summation();     outputCluster->activation();
    }

    delete inputCluster;
    delete outputCluster;
    delete hidden_1;
    delete hidden_2;

    sigmoid->setLearningRate(lr);
}

MainWindow::~MainWindow() {
    delete ui;
    delete outputCluster;
    delete inputCluster;
    for(auto cluster : hiddenClusters)
        delete cluster;

    delete binary;
    delete sigmoid;
    delete neuralNetwork;
}

void MainWindow::changeImage(int imageIndex) {
    int groupIndex = ui->imageGroup->currentIndex();
    setTable(matrix(groupIndex, imageIndex));
}

void MainWindow::changeImageSet(int groupIndex) {
    deleteUnusedImages(groupIndex);
    ui->image->setModel(model(groupIndex).data());
}

void MainWindow::addImage(bool /*ignored*/) {
    addImage(ui->imageGroup->currentIndex());
    ui->image->setCurrentIndex(ui->image->count() - 1);
}

void MainWindow::removeImage(bool /*ignored*/) {
    if(ui->image->count() <= 1) {
        QMessageBox messageBox(this);
        messageBox.setWindowTitle(tr("Ошибка ввода данных"));
        messageBox.setText(tr("Для работы должно быть не меньше одного изображения в группе"));
        messageBox.exec();
        return;
    }
    removeImage(ui->imageGroup->currentIndex(), ui->image->currentIndex());
}

void MainWindow::saveImage(bool /*value*/) {
    ui->imageInfo->setText("");
    int imageIndex = ui->image->currentIndex();
    int groupIndex = ui->imageGroup->currentIndex();
    setMatrix(groupIndex, imageIndex, table());
}

void MainWindow::addGroup(bool /*ignored*/) {
    createNewGroup();
    ui->imageGroup->setCurrentIndex(ui->imageGroup->count() - 1);
}

void MainWindow::removeGroup(bool /*ignored*/) {
    if(ui->imageGroup->count() <= 2) {
        QMessageBox messageBox(this);
        messageBox.setWindowTitle(tr("Ошибка ввода данных"));
        messageBox.setText(tr("Для работы должно быть не меньше двух групп"));
        messageBox.exec();
        return;
    }
    removeGroup(ui->imageGroup->currentIndex());
}

void MainWindow::clearTableData(bool /*ignored*/) {
    for(int i = 0; i != ui->table->rowCount(); ++i) {
        for (int j = 0; j != ui->table->columnCount(); ++j) {
            SwitchButton *button = (SwitchButton*)ui->table->cellWidget(i, j);
            button->setSelected(false);
        }
    }
}

void MainWindow::training(bool /*ignored*/) {
    initWeights();
    deleteUnusedImages(ui->imageGroup->currentIndex());

   // sigmoid->__learningRate__ = 10;

    SimpleNeuralNetworkTrainer trainer(neuralNetwork, true);

    for(int i = 0; i != ui->imageGroup->count(); ++i) {
        SimpleNeuralNetworkTrainer::Values outputSignal;

        Signal out(ui->imageGroup->count());
        for(int k = 0; k != out.size(); ++k)
            out.setSignal(k == i ? 1 : 0, k);

        outputSignal[outputCluster] = out;

        SimpleNeuralNetworkTrainer::Values inputSignal;
        QVector<MatrixPtr> images = this->images(i);
        if(images.empty()) {
            QString message = tr("Ошибка: группа \"");
            message += ui->imageGroup->itemData(i, Qt::DisplayRole).toString() + "\"";
            message += tr(" не содержит изображений");
            sendMessage(message);
            return;
        }
        for(MatrixPtr matrix : images) {
            Signal in = convertToSignal(matrix);

            inputSignal[inputCluster] = in;

            trainer.addTrainingSet(
                new SimpleNeuralNetworkTrainer::TrainingSet{
                    inputSignal,
                    outputSignal,
                }
            );
        }
    }

    int maxIterCount = 100000;
    int iterCount = trainer.training(0.05, 100000);
    if(iterCount == maxIterCount) {
        sendMessage(tr("Нейронную сеть не удалось обучить"));
    } else {
        sendMessage(tr("Нейронную сеть обучены (") + QString::number(iterCount) + ")");
    }
}

void MainWindow::batchTraining(bool /*ignored*/) {
    initWeights();
    deleteUnusedImages(ui->imageGroup->currentIndex());

    SimpleNeuralNetworkTrainer trainer(neuralNetwork, true);

    SimpleNeuralNetworkTrainer::Values trainingInputSignal;
    SimpleNeuralNetworkTrainer::Values trainingOutputSignal;

    QList<QVector<MatrixPtr>> allImages;

    int summImageCount = 0;
    for(int i = 0; i != ui->imageGroup->count(); ++i) {
        auto images = this->images(i);
        if(images.empty()) {
            QString message = tr("Ошибка: группа \"");
            message += ui->imageGroup->itemData(i, Qt::DisplayRole).toString() + "\"";
            message += tr(" не содержит изображений");
            sendMessage(message);
            return;
        }
        allImages.append(images);
        summImageCount += images.size();
    }

    int imageSize = ui->table->rowCount();
    imageSize *= ui->table->columnCount();
    Signal inputSignal(imageSize, summImageCount);
    Signal outputSignal(ui->imageGroup->count(), summImageCount);

    int imageNumber = 0;
    for(int k = 0; k != allImages.size(); ++k) {
        const auto &images = allImages[k];
        for(MatrixPtr matrix : images) {
            for(int n = 0; n != outputSignal.size(); ++n)
                outputSignal.setSignal(n == k ? 1 : 0, n, imageNumber);
            for(int i = 0; i != matrix->rows(); ++i)
                for(int j = 0; j != matrix->columns(); ++j)
                    inputSignal.setSignal(matrix->cell(i, j), i * matrix->columns() + j, imageNumber);
            ++imageNumber;
        }
    }


    trainingInputSignal[inputCluster] = inputSignal;
    trainingOutputSignal[outputCluster] = outputSignal;

    trainer.addTrainingSet(
        new SimpleNeuralNetworkTrainer::TrainingSet{
            trainingInputSignal,
            trainingOutputSignal,
        }
    );

    int maxIterCount = 100000;
    int iterCount = trainer.training(0.05, maxIterCount);
    if(iterCount == maxIterCount) {
        sendMessage(tr("Нейронную сеть не удалось обучить"));
    } else {
        sendMessage(tr("Нейронную сеть обучены (") + QString::number(iterCount) + ")");
    }
}

void MainWindow::recognize(bool /*ignored*/) {
    MatrixPtr matrixPtr = table();

    inputCluster->outputSignal() = convertToSignal(matrixPtr);

    neuralNetwork->updateNetwork();

    auto outputSignal = outputCluster->outputSignal();

    QList<int> groups;
    for(int i = 0; i != outputSignal.size(); ++i){
        int s = round(outputSignal.signal(i));
        if (s != 0) groups.append(i);
    }

    QString message;
    if(groups.size() != 1) {
        if(groups.size() == 0) {
            message = tr("Нейронная сеть не смогла определить группу данного изображения<br>");
        } else {
            message = tr("Нейронная сеть считает, что это изображение может относится к группам:<br>");
            for(int i = 0; i != groups.size(); ++i) {
                int groupNumber = groups[i];
                message += QString::number(i + 1) + ") \"";
                message += ui->imageGroup->itemData(groupNumber, Qt::DisplayRole).toString();
                message += "\"<br>";
            }
        }
    } else {
        message = tr("Нейронная сеть считает, что это изображение относится к группе:");
        message += "\"" + ui->imageGroup->itemText(groups.front()) + "\"<br>";
    }
    message += "Вероятнее всего данное изображение относится к группе \"";
    int bestGroup = closestGroup(matrixPtr);
    message += ui->imageGroup->itemData(bestGroup, Qt::DisplayRole).toString();
    message += "\"<br>";
    sendMessage(message);
}

void MainWindow::printfInfo(bool /*ignored*/) {

}

void MainWindow::changeNeuronsCount(int index) {
    int newNeuronsCount = ui->layers->itemData(index, Qt::DisplayRole).toInt();
    hiddenClusters[index]->setNueronsCount(newNeuronsCount);
}

void MainWindow::changeRange(double /*value*/) {
    double lowerBound = ui->lowerBound->value();
    double uppedBound = ui->uppedBound->value();
    ui->lowerBound->setMaximum(uppedBound - 0.01);
    ui->uppedBound->setMinimum(lowerBound + 0.01);
}

void MainWindow::changeLearningFactor(double value) {
    sigmoid->__learningRate__ = value;
}

void MainWindow::initWeights() {
    initRandomWeight(inputCluster->weights());
    initRandomWeight(outputCluster->weights());
    for(auto cluster : hiddenClusters)
        initRandomWeight(cluster->weights());
}

void MainWindow::initRandomWeight(RelatationWeights &weight) {
    QRandomGenerator *random = QRandomGenerator::system();
    double range = ui->uppedBound->value() - ui->lowerBound->value();
    for(int i = 0; i != weight.neuronsCount(); ++i) {
        for(int j = 0; j != weight.inputsCount(); ++j) {
            auto value = random->generateDouble();
            value *= range;
            value += ui->lowerBound->value();
            weight.setWeight(value, i, j);
        }
        auto value = random->generateDouble();
        value *= range;
        value += ui->lowerBound->value();
        weight.setShift(value, i);
    }
}

Signal MainWindow::convertToSignal(MatrixPtr matrix) {
    Signal signal(matrix->rows() * matrix->columns());

    for(int i = 0; i != matrix->rows(); ++i) {
        for(int j = 0; j != matrix->columns(); ++j) {
            signal.setSignal(matrix->cell(i, j), i * matrix->columns() + j);
        }
    }
    return signal;
}

void MainWindow::sendMessage(QString html) {
    if(receivers(SIGNAL(messageSended(QString))) == 0) {
        Console *info = new Console();
        connect(this, &MainWindow::messageSended, info, &Console::printHtml);
        auto connection = connect(this, &MainWindow::closed, [info] (QObject*) {
            info->close();
        });
        /// Почему - то deleteLater не обрубил все связи
        /// Поэтому приходится из обрубать вручную
        connect(info, &Console::closed, [connection] (QObject* obj){
            disconnect(connection);
            obj->deleteLater();
        });

        info->show();
    }
    emit messageSended(html);
}

int MainWindow::closestGroup(MatrixPtr matrix) {
    inputCluster->outputSignal() = convertToSignal(matrix);
    neuralNetwork->updateNetwork();
    outputCluster->summation();

    int bestIndex = -1;
    double bestValue = -std::numeric_limits<double>::max();
    auto outputSignal = outputCluster->outputSignal();
    for(int i = 0; i != outputSignal.size(); ++i) {
        if(outputSignal.signal(i) > bestValue) {
            bestValue = outputSignal.signal(i);
            bestIndex = i;
        }
    }

    return bestIndex;
}

template<class T>
T MainWindow::dataInHash(QVariantHashPtr hash, QString field) {
    if(!hash->contains(field))           return T();

    auto variantData = hash->value(field);
    if(!variantData.canConvert<T>())    return T();

    return variantData.value<T>();
}

QAbstractItemModelPtr MainWindow::model(int groupIndex) {
    if(groupIndex > ui->imageGroup->count())            throw std::invalid_argument("MainWindow::model");

    auto variantUserData = ui->imageGroup->itemData(groupIndex);
    if(!variantUserData.canConvert<QVariantHashPtr>())  {
        QAbstractItemModelPtr result(new QStandardItemModel());
        result->insertColumn(0);
        setModel(groupIndex, result);
        addImage(groupIndex);
        return result;
    } else {
        auto hash = variantUserData.value<QVariantHashPtr>();
        return dataInHash<QAbstractItemModelPtr>(hash, Models);
    }
}

void MainWindow::setModel(int groupIndex, QAbstractItemModelPtr model) {
    if(groupIndex > ui->imageGroup->count())            throw std::error_code();
    auto variantUserData = ui->imageGroup->itemData(groupIndex);

    QVariantHashPtr hash;
    if(!variantUserData.isValid() || !variantUserData.canConvert<QVariantHash>()) {
        hash.reset(new QVariantHash());
        ui->imageGroup->setItemData(groupIndex, QVariant::fromValue(hash));
    } else hash = variantUserData.value<QVariantHashPtr>();

    hash->insert(Models, QVariant::fromValue(model));
}

MatrixPtr MainWindow::table() {
    Matrix *result = new Matrix(ui->table->rowCount(), ui->table->columnCount());
    for(int i = 0; i != ui->table->rowCount(); ++i) {
        for(int j = 0; j != ui->table->columnCount(); ++j) {
            SwitchButton *button = (SwitchButton*)ui->table->cellWidget(i, j);
            result->setCell(i, j, button->isSelected() ? 1 : 0);
        }
    }
    return MatrixPtr(result);
}

void MainWindow::setTable(MatrixPtr matrix) {
    if(matrix == nullptr)   {
        ui->imageInfo->setText(tr("В данном изображении не записаны данные!!!"));
        for(int i = 0; i != ui->table->rowCount(); ++i) {
            for(int j = 0; j != ui->table->columnCount(); ++j) {
                SwitchButton *button = (SwitchButton*)ui->table->cellWidget(i, j);
                button->setSelected(false);
            }
        }
    } else {
        ui->imageInfo->clear();
        for(int i = 0; i != ui->table->rowCount(); ++i) {
            for(int j = 0; j != ui->table->columnCount(); ++j) {
                SwitchButton *button = (SwitchButton*)ui->table->cellWidget(i, j);
                button->setSelected(matrix->cell(i, j) > 0);
            }
        }
    }
}

MatrixPtr MainWindow::matrix(int groupIndex, int imageIndex) {
    auto model = this->model(groupIndex);
    if(model == nullptr)                                    return nullptr;

    auto index = model->index(imageIndex, 0);
    auto variantUserData = model->data(index, Qt::UserRole);
    if(!variantUserData.canConvert<QVariantHashPtr>())      return nullptr;

    auto hash = variantUserData.value<QVariantHashPtr>();
    return dataInHash<MatrixPtr>(hash, MatrixData);
}

void MainWindow::setMatrix(int groupIndex, int imageIndex, MatrixPtr matrix) {
    auto model = this->model(groupIndex);
    if(model == nullptr)                                throw std::error_code();

    auto index = model->index(imageIndex, 0);
    auto variantUserData = model->data(index, Qt::UserRole);

    QVariantHashPtr hash;
    if(!variantUserData.canConvert<QVariantHashPtr>())    {
        hash.reset(new QVariantHash());
        model->setData(index, QVariant::fromValue(hash), Qt::UserRole);
    } else hash = variantUserData.value<QVariantHashPtr>();

    hash->insert(MatrixData, QVariant::fromValue(matrix));
}

void MainWindow::addImage(int group) {
    QString name = tr("Новое изображение");
    auto model = this->model(group);

    int index = 1;
    bool hasEquals = true;
    QString tmpName = name;
    while(true) {
        hasEquals = false;
        for (int i = 0; i != model->rowCount(); ++i) {
            if(tmpName == model->data(model->index(i, 0)).toString()) {
                hasEquals = true;
                ++index;
                break;
            }
        }
        if(!hasEquals) break;
        tmpName = name + " " + QString::number(index);
    }
    if(index != 1) name = tmpName;

    int imageIndex = model->rowCount();
    model->insertRow(imageIndex);
    model->setData(model->index(imageIndex, 0), name, Qt::DisplayRole);
    setMatrix(group, imageIndex, nullptr);
}

void MainWindow::removeImage(int group, int image) {
    auto model = this->model(group);
    model->removeRow(image);
}

void MainWindow::addHiddenLayer(int neuronsCount) {
    auto newCluster = new SimpleClusterOfNeurons(neuronsCount);

    Cluster lastCluster = hiddenClusters.empty() ? inputCluster : hiddenClusters.last();

    lastCluster->addOutput(newCluster);
    outputCluster->addInput(newCluster);
    lastCluster->removeOutput(outputCluster);
    newCluster->setActivationFunction(lastCluster->activationFunction());

    hiddenClusters.append(newCluster);
    neuralNetwork->addCluster(newCluster);
//    neuralNetwork->updateClusterRoles();

    ui->layers->addItem(QString::number(neuronsCount));
}

void MainWindow::removeHiddenLayer(int index) {
    auto cluster = hiddenClusters[index];

    hiddenClusters.removeAt(index);
    neuralNetwork->removeCluster(cluster);
//    neuralNetwork->updateClusterRoles();

    ui->layers->removeItem(index);

    delete cluster;
}

void MainWindow::createNewGroup() {
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
    ui->imageGroup->insertItem(ui->imageGroup->count(), name);
    outputCluster->insertNeurons(outputCluster->neuronsCount(), 1);
}

void MainWindow::removeGroup(int group) {
    auto variant =  ui->imageGroup->itemData(group);
    if(variant.isValid() && variant.canConvert<QVariantHash>()) {
        auto hash = variant.value<QVariantHash>();
        if(hash.find(Models) != hash.end()) {
            variant = hash.find(Models).value();
            if(variant.isValid() && variant.canConvert<QAbstractItemModel*>()){
                variant.value<QAbstractItemModel*>()->deleteLater();
            }
        }
    }

    auto model = ui->imageGroup->model();
    model->removeRow(ui->imageGroup->currentIndex());
    outputCluster->removeNeurons(group, group);
}

void MainWindow::initTable() {
    ui->table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
    ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
    for(int i = 0; i != ui->table->rowCount(); ++i) {
        for (int j = 0; j != ui->table->columnCount(); ++j) {
            ui->table->setCellWidget(i, j, new SwitchButton(Qt::white, Qt::black));
        }
    }
    connect(ui->clear, &QAbstractButton::clicked, this, &MainWindow::clearTableData);
}

void MainWindow::initImageSet() {
    connect(ui->addImage, SIGNAL(clicked(bool)), this, SLOT(addImage(bool)));
    connect(ui->saveImage, SIGNAL(clicked(bool)), this, SLOT(saveImage(bool)));
    connect(ui->removeImage, SIGNAL(clicked(bool)), this, SLOT(removeImage(bool)));
    connect(ui->image, SIGNAL(currentIndexChanged(int)), this, SLOT(changeImage(int)));
}

void MainWindow::initNeuralWebs() {
    connect(ui->info, &QPushButton::clicked, this, &MainWindow::printfInfo);
    connect(ui->learning, &QPushButton::clicked, this, &MainWindow::training);
    connect(ui->batchTraining, &QPushButton::clicked, this, &MainWindow::batchTraining);
    connect(ui->recognizeImage, &QPushButton::clicked, this, &MainWindow::recognize);

    connect(ui->lowerBound, SIGNAL(valueChanged(double)), this, SLOT(changeRange(double)));
    connect(ui->uppedBound, SIGNAL(valueChanged(double)), this, SLOT(changeRange(double)));
    connect(ui->smallTeta, SIGNAL(valueChanged(double)), this, SLOT(changeLearningFactor(double)));

    ui->lowerBound->setMinimum(-1.0);
    ui->uppedBound->setMaximum( 1.0);

    ui->lowerBound->setValue(-0.1);
    ui->uppedBound->setValue( 0.1);

    inputCluster = new SimpleClusterOfNeurons(ui->table->rowCount() * ui->table->columnCount());
    outputCluster = new SimpleClusterOfNeurons();

    binary = new Binary();
    sigmoid = new Sigmoid();
    outputCluster->setActivationFunction(sigmoid);

    inputCluster->addOutput(outputCluster);

    neuralNetwork = new SimpleNeuralNetwork();

    neuralNetwork->addCluster(outputCluster);
    neuralNetwork->addCluster(inputCluster);

    addHiddenLayer(12);
    addHiddenLayer(6);

    neuralNetwork->updateClusterSequence();
}

void MainWindow::initImageGroups() {
    addGroup();
    addGroup();

    connect(ui->addGroup, &QAbstractButton::clicked, this, &MainWindow::addGroup);
    connect(ui->removeGroup, SIGNAL(clicked(bool)), this, SLOT(removeGroup(bool)));
    connect(ui->imageGroup, SIGNAL(currentIndexChanged(int)), this, SLOT(changeImageSet(int)));

    ui->imageGroup->setCurrentIndex(1);
    ui->imageGroup->setCurrentIndex(0);
}

void MainWindow::deleteUnusedImages(int imageGroup) {
    auto imageModel = model(imageGroup);
    int countImage = imageModel->rowCount();
    if(countImage == 1) return;
    bool hasNotNull = false;
    for(int i = 0; i != countImage; ++i) {
        if(!matrix(imageGroup, i).isNull()) {
            hasNotNull = true;
            break;
        }
    }
    if(hasNotNull) {
        for(int i = 0; i != imageModel->rowCount(); ) {
            if(matrix(imageGroup, i).isNull()) {
                imageModel->removeRow(i);
            } else ++i;
        }
    } else {
        imageModel->removeRows(0, countImage);
        addImage(imageGroup);
    }
}

QVector<MatrixPtr> MainWindow::images(int groupIndex) {
    QVector<MatrixPtr> result;
    auto model = this->model(groupIndex);
    int countImage = model->rowCount();
    for(int i = 0; i != countImage; ++i) {
        if(!matrix(groupIndex, i).isNull()) {
            result.push_back(matrix(groupIndex, i));
        }
    }
    return result;
}

void MainWindow::closeEvent(QCloseEvent* /*ignored*/) {
   emit closed(this);
}
