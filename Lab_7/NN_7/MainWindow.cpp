#include "MainWindow.h"
#include "ui_MainWindow.h"

QString MainWindow::Models      = "Images";
QString MainWindow::MatrixData  = "Matrix";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->widget_group->setVisible(false);

    initNeuralWebs();

    initImageSet();
    initTable();
    initImageGroups();
}


MainWindow::~MainWindow() {
    delete ui;
    delete outputCluster;
    delete inputCluster;
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
    auto allSignals = getAllSignals();
    outputCluster->setNueronsCount(allSignals.size());
    initRandonWeight(outputCluster);

    double eps = 1e-5;
    double deviation = 1;
    double decayRate = ui->decayRate->value();
    double learningFactor = ui->learningFactor->value();

    int iteration = 0;
    int maxIteration = 100000;
    while(deviation > eps && iteration != maxIteration) {
        deviation = 0;
        auto &weights = outputCluster->weights();

        for(const auto &signal : allSignals) {
            int bestIndex = -1;
            double bestValue = std::numeric_limits<double>::max();

            for(int i = 0; i != weights.neuronsCount(); ++i) {
                double value = 0;
                for(int j = 0; j != weights.inputsCount(); ++j) {
                    double diff = weights.weight(i, j) - signal.signal(j);
                    value += diff * diff;
                }
                if(bestValue > value) {
                    bestValue = value;
                    bestIndex = i;
                }
            }

            for(int j = 0; j != weights.inputsCount(); ++j) {
                double w = weights.weight(bestIndex, j);
                double diff = signal.signal(j) - w;
                double dw = learningFactor * diff;
                deviation = std::max(deviation, dw);
                weights.setWeight(w + dw, bestIndex, j);
            }
        }

        learningFactor *= decayRate;
        ++iteration;
    }

    removeDeadNeurons();
    auto message = tr("Нейронная сеть обучена");
    message += iteration == maxIteration ? "(false, " : "(true, ";
    message += QString::number(iteration) + ")";
    sendMessage(message);
}

void MainWindow::recognize(bool /*ignored*/) {
    if(outputCluster->neuronsCount() == 0) {
        sendMessage(tr("Нейронная сеть не обучена"));
        return;
    }

    QString message = tr("Нейронная сеть считает, что это изображение относится к группе ");
    message += ui->imageGroup->itemData(bestGroup(convertToSignal(table())), Qt::DisplayRole).toString() + "\"<br>";
    sendMessage(message);
}

void MainWindow::printfInfo(bool /*ignored*/) {
    QString buffer;
    QTextStream out(&buffer);

    QMultiHash<int, int> imageInGroups;
    auto allSignals = getAllSignals();
    for(int i = 0; i != allSignals.size(); ++i) {
        int group = bestGroup(allSignals[i]);
        imageInGroups.insert(group, i);
    }

    out.setRealNumberPrecision(5);
    out.setRealNumberNotation(QTextStream::RealNumberNotation::FixedNotation);

    auto model = this->model(0);
    for(int i = 0; i != outputCluster->neuronsCount(); ++i) {
        out << tr("К группе под номером ") << i + 1 << tr(" относятся следующие изображения<br>");
        auto pair = imageInGroups.equal_range(i);
        while(pair.first != pair.second) {
            out << tr("&nbsp;&nbsp;&nbsp;&nbsp;");
            out << ui->image->itemData(*pair.first, Qt::DisplayRole).toString();
            out << tr("<br>");
            ++pair.first;
        }
    }
    sendMessage(buffer);
}

void MainWindow::printHemming(bool /*ignored*/) {
    QString buffer;
    QTextStream out(&buffer);

    out << "<table border=\"1\" cellpadding=\"4\" cellspacing=\"0\">";

    out << "<tr><th />";
    for(int i = 0; i != ui->image->count(); ++i) {
        out << "<th aling=\"center\">" << ui->image->itemText(i) << "</th>";
    }
    out << "</tr>";
    auto allSignals = getAllSignals();
    for(int i = 0; i != ui->image->count(); ++i) {
        out << "<tr><th aling=\"center\">" << ui->image->itemText(i) << "</th>";

        for(int j = 0; j != ui->image->count(); ++j) {
            int hemming = 0;
            auto &firstSignal = allSignals[i];
            auto &secondSignal = allSignals[j];
            for(int k = 0; k != firstSignal.size(); ++k) {
                if(std::abs(firstSignal.signal(k) - secondSignal.signal(k)) < 1e-7) {
                    ++hemming;
                }
            }
            hemming = firstSignal.size() - hemming;
            out << "<td aling=\"right\">" << hemming << "</td>";
        }
        out << "</tr>";
    }
    out << "</table>";

    sendMessage(buffer);
}

void MainWindow::changeRange(double /*value*/) {
    double lowerBound = ui->lowerBound->value();
    double uppedBound = ui->upperBound->value();
    ui->lowerBound->setMaximum(uppedBound - 0.01);
    ui->upperBound->setMinimum(lowerBound + 0.01);
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

QList<Signal> MainWindow::getAllSignals() {
    QList<Signal> allSignals;
    for(int i = 0; i != ui->imageGroup->count(); ++i) {
        auto images = this->images(i);
        for(auto matrixptr : images) {
            allSignals.append(convertToSignal(matrixptr));
        }
    }
    return allSignals;
}

void MainWindow::initRandonWeight(Cluster cluster) {
    double lowerBound = ui->lowerBound->value();
    double range = ui->upperBound->value() - lowerBound;
    auto generator = QRandomGenerator::system();
    auto &weight = cluster->weights();
    for (int i = 0; i != weight.neuronsCount(); ++i) {
        for(int j = 0; j != weight.inputsCount(); ++j) {
            weight.setWeight(generator->generateDouble() * range + lowerBound, i, j);
        }
    }
}

int MainWindow::bestGroup(const Signal &signal) {
    inputCluster->outputSignal() = signal;
    neuralNetwork->updateNetwork();

    int bestGroup = -1;
    double bestValue = -std::numeric_limits<double>::max();
    for(int i = 0; i != outputCluster->neuronsCount(); ++i) {
        if(bestValue < outputCluster->outputSignal().signal(i)) {
            bestValue = outputCluster->outputSignal().signal(i);
            bestGroup = i;
        }
    }
    return bestGroup;
}

void MainWindow::removeDeadNeurons() {
    if(outputCluster->neuronsCount() == 0) {
        return;
    }

    QSet<int> activeNeurons;
    auto allSignals = getAllSignals();
    for(const auto &signal : allSignals) {
        activeNeurons.insert(bestGroup(signal));
    }

    for(int index = outputCluster->neuronsCount(); index != 0; --index) {
        if(!activeNeurons.contains(index - 1)) {
            outputCluster->removeNeurons(index - 1, index - 1);
        }
    }
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

    connect(ui->trainingNetwork, &QPushButton::clicked, this, &MainWindow::training);
    connect(ui->recognizeImage, &QPushButton::clicked, this, &MainWindow::recognize);
    connect(ui->hemming, &QPushButton::clicked, this, &MainWindow::printHemming);
    connect(ui->refInfo, &QPushButton::clicked, this, &MainWindow::printfInfo);

    inputCluster = new SimpleClusterOfNeurons(ui->table->rowCount() * ui->table->columnCount());
    outputCluster = new SimpleClusterOfNeurons();

    inputCluster->addOutput(outputCluster);

    neuralNetwork = new SimpleNeuralNetwork();

    neuralNetwork->addCluster(outputCluster);
    neuralNetwork->addCluster(inputCluster);

    neuralNetwork->updateClusterSequence();
}

void MainWindow::initImageGroups() {
    addGroup();
    connect(ui->lowerBound, SIGNAL(valueChanged(double)), this, SLOT(changeRange(double)));
    connect(ui->upperBound, SIGNAL(valueChanged(double)), this, SLOT(changeRange(double)));
    changeImageSet(0);
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
