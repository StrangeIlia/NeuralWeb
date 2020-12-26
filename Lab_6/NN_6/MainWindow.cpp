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
}


MainWindow::~MainWindow() {
    delete ui;
    delete outputCluster;
    delete inputCluster;
    delete hemming;
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

void MainWindow::recognize(bool /*ignored*/) {
    MatrixPtr matrixPtr = table();

    inputCluster->outputSignal() = convertToSignal(matrixPtr);

    QList<int> imageInGroup;
    QList<Signal> allSignals;
    for(int i = 0; i != ui->imageGroup->count(); ++i) {
        auto images = this->images(i);
        imageInGroup.append(images.size());
        for(auto matrixptr : images) {
            allSignals.append(convertToSignal(matrixptr));
        }
    }

    outputCluster->setNueronsCount(allSignals.size());
    auto &weight = outputCluster->weights();
    for(int i = 0; i != weight.neuronsCount(); ++i) {
        auto &input = allSignals[i];
        for(int j = 0; j != weight.inputsCount(); ++j) {
            weight.setWeight(input.signal(j) / 2, i, j);
        }
        weight.setShift(0.5, i);
    }

    hemming->__t__ = inputCluster->neuronsCount() / 2.0;

    neuralNetwork->updateNetwork();
    auto outputSignal = outputCluster->outputSignal();

    auto lastOutputSignal = outputSignal;
    auto maxnet = new SimpleClusterOfNeurons(outputSignal.size());
    maxnet->connect(maxnet, maxnet);
    maxnet->outputSignal() = outputSignal;
    maxnet->setActivationFunction(hemming);
    hemming->__t__ = 1;
    auto &maxnetWeight = maxnet->weights();
    for(int i = 0; i != maxnetWeight.neuronsCount(); ++i) {
        for(int j = 0; j != maxnetWeight.inputsCount(); ++j) {
            maxnetWeight.setWeight(i == j ? 1 : -__e__, i, j);
        }
    }

    int iterationCount = 0;
    double eps = 1;
    double reqEps = 0.001;
    while(eps > reqEps) {
        maxnet->summation();
        maxnet->activation();

        outputSignal = maxnet->outputSignal();
        eps = 0;
        for(int i = 0; i != outputSignal.size(); ++i)
            eps += std::pow(outputSignal.signal(i) - lastOutputSignal.signal(i), 2);

        eps = std::sqrt(eps);
        lastOutputSignal = outputSignal;
        ++iterationCount;
    }
    delete maxnet;

    QList<int> activeImages;
    for(int i = 0; i != outputSignal.size(); ++i){
        int s = outputSignal.signal(i) > std::numeric_limits<double>::epsilon();
        if (s != 0) activeImages.append(i);
    }

    QHash<int, QList<int>> activeGroups;
    int imageShift = 0;
    for(int i = 0; i != imageInGroup.size(); ++i){
        for(auto imageNumber : activeImages) {
            if(imageNumber >= imageShift) {
                if(imageNumber < imageShift + imageInGroup[i]) {
                    if(!activeGroups.contains(i)) {
                        activeGroups[i] = QList<int>();
                    }
                    activeGroups[i].append(imageNumber - imageShift);
                }
            }
        }
        imageShift += imageInGroup[i];
    }

    QString message;
    if(activeGroups.size() != 1) {
        if(activeGroups.size() == 0) {
            message = tr("Нейронная сеть не смогла определить группу данного изображения");
            message += "(" + QString::number(iterationCount) + ")<br>";
        } else {
            message = tr("Нейронная сеть считает, что это изображение может относится к группам:");
            message += "(" + QString::number(iterationCount) + ")<br>";
            int i = 0;
            for(auto iter = activeGroups.begin(); iter != activeGroups.end(); ++iter, ++i) {
                message += QString::number(i + 1) + ") \"";
                message += ui->imageGroup->itemData(iter.key(), Qt::DisplayRole).toString();
                message += "\"<br>";

                int j = 0;
                auto model = this->model(iter.key());
                for(auto imageNumber : iter.value()) {
                    message += "&nbsp;&nbsp;&nbsp;&nbsp;" + QString::number(j + 1) + ") \"";
                    message += model->data(model->index(imageNumber, 0)).toString();
                    message += "\"<br>";
                }
            }
        }
    } else {
        message = tr("Нейронная сеть считает, что это изображение относится к группе ");
        message += "(" + QString::number(iterationCount) + "): ";
        message += "\"" + ui->imageGroup->itemText(activeGroups.begin().key()) + "\"<br>";
    }
    message += "Вероятнее всего данное изображение относится к группе \"";

    int bestGroup = -1;
    imageShift = 0;
    int bestImage = closestImage(matrixPtr);
    for(int i = 0; i != imageInGroup.size(); ++i) {
        if(bestImage >= imageShift) {
            if(bestImage < imageShift + imageInGroup[i]) {
                bestGroup = i;
                break;
            }
        }
        imageShift += imageInGroup[i];
    }

    message += ui->imageGroup->itemData(bestGroup, Qt::DisplayRole).toString();
    message += "\"<br>";
    sendMessage(message);
}

void MainWindow::printfInfo(bool /*ignored*/) {
    QString buffer;
    QTextStream out(&buffer);

    out.setRealNumberPrecision(5);
    out.setRealNumberNotation(QTextStream::RealNumberNotation::FixedNotation);
    for(int i = 0; i != ui->imageGroup->count(); ++i) {
        out << tr("Информация по ") << i + 1 << tr(" группе<br>");

        auto images = this->images(i);
        for(int k = 0; k != images.size(); ++k) {
            auto image = images[k];
            out << tr("&nbsp;&nbsp;&nbsp;&nbsp;Отклонения для ") << k + 1 << tr(" изображения [");
            inputCluster->outputSignal() = convertToSignal(image);
            neuralNetwork->updateNetwork();
            auto &output = outputCluster->outputSignal();
            for(int j = 0; j != output.size(); ++j) {
                out << (i == j ? 1 - output.signal(j) : output.signal(j) - 0);
                out << "; ";
            }
            out << "]<br>";
        }
    }
    sendMessage(buffer);
}

void MainWindow::changeLearningFactor(double value) {
    __e__ = value;
}

Signal MainWindow::convertToSignal(MatrixPtr matrix) {
    Signal signal(matrix->rows() * matrix->columns());

    for(int i = 0; i != matrix->rows(); ++i) {
        for(int j = 0; j != matrix->columns(); ++j) {
            auto value = matrix->cell(i, j);
            value = value > 0.5 ? 1 : -1;
            signal.setSignal(value, i * matrix->columns() + j);
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

int MainWindow::closestImage(MatrixPtr matrix) {
    inputCluster->outputSignal() = convertToSignal(matrix);

    //hemming->__t__ = inputCluster->neuronsCount() / 2.0;
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

    summImageCount += 1;
    ui->smallTeta->setMaximum(1.0 / summImageCount);
}

void MainWindow::removeImage(int group, int image) {
    auto model = this->model(group);
    model->removeRow(image);

    summImageCount -= 1;
    ui->smallTeta->setMaximum(1.0 / summImageCount);
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
}

double MainWindow::maxError() {
    double maxError = -1;
    for(int i = 0; i != ui->imageGroup->count(); ++i) {
        for(auto image : this->images(i)) {
            inputCluster->outputSignal() = convertToSignal(image);
            neuralNetwork->updateNetwork();
            auto &output = outputCluster->outputSignal();
            for(int j = 0; j != output.size(); ++j) {
                double error = i == j ? 1 - output.signal(j) : output.signal(j) - 0;
                if (error > maxError) maxError = error;
            }
        }
    }
    return maxError;
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
    auto validator = new QIntValidator(this);
    validator->setBottom(1);

    connect(ui->recognizeImage, &QPushButton::clicked, this, &MainWindow::recognize);

    connect(ui->smallTeta, SIGNAL(valueChanged(double)), this, SLOT(changeLearningFactor(double)));

    inputCluster = new SimpleClusterOfNeurons(ui->table->rowCount() * ui->table->columnCount());
    outputCluster = new SimpleClusterOfNeurons();

    hemming = new Hemming();
    outputCluster->setActivationFunction(hemming);

    inputCluster->addOutput(outputCluster);

    neuralNetwork = new SimpleNeuralNetwork();

    neuralNetwork->addCluster(outputCluster);
    neuralNetwork->addCluster(inputCluster);

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
                summImageCount -= 1;
            } else ++i;
        }
    } else {
        imageModel->removeRows(0, countImage);
        summImageCount -= countImage;
        addImage(imageGroup);
    }
    ui->smallTeta->setMaximum(1.0 / summImageCount);
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
