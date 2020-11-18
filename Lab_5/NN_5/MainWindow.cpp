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
    delete rCluster;
    delete aCluster;
    delete sCluster;
    delete binary;
    delete perceptron;
    delete nueralNetwork;
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
    deleteUnusedImages(ui->imageGroup->currentIndex());

    if(!initWeights()) {
        sendMessage(tr("Не удалось сгенерировать весовые коэффициенты"));
        return;
    }

    SimpleNeuralNetworkTrainer trainer(nueralNetwork, true);

    for(int i = 0; i != ui->imageGroup->count(); ++i) {
        SimpleNeuralNetworkTrainer::Values outputSignal;

        Signal out(ui->imageGroup->count());
        for(int k = 0; k != out.size(); ++k)
            out.setSignal(k == i ? 1 : 0, k);

        outputSignal[rCluster] = out;

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

            inputSignal[sCluster] = in;

            trainer.addTrainingSet(new SimpleNeuralNetworkTrainer::TrainingSet{
                                             inputSignal,
                                             outputSignal,
                                         });
        }
    }

    int iterCount = trainer.training(1e-7, 1000);
    sendMessage(tr("Нейронные сети обучены (") + QString::number(iterCount) + ")");
}

void MainWindow::balancing(bool /*ignored*/) {
    __balancing();
    sendMessage(tr("Сигналы сдвига для R слоя отбалансированы"));
}

void MainWindow::__balancing() {
    Matrix range(ui->imageGroup->count(), 2);
    for(int i = 0; i != range.rows(); ++i) {
        range.setCell(i, 0, -std::numeric_limits<double>::max());
        range.setCell(i, 1,  std::numeric_limits<double>::max());
    }
    for(int i = 0; i != ui->imageGroup->count(); ++i) {
        QVector<MatrixPtr> images = this->images(i);
        for(MatrixPtr matrix : images) {
            sCluster->outputSignal() = convertToSignal(matrix);
            nueralNetwork->updateNetwork();
            rCluster->summation();
            const auto &signal = rCluster->outputSignal();
            for(int j = 0; j != signal.size(); ++j) {
                if(i == j) {
                    if(range.cell(j, 0) > signal.signal(j)) {
                        range.setCell(j, 0, signal.signal(j));
                    }
                } else {
                    if(range.cell(j, 1) < signal.signal(j)) {
                        range.setCell(j, 0, signal.signal(j));
                    }
                }
            }
        }
    }

    auto &weight = rCluster->weights();
    for(int i = 0; i != rCluster->neuronsCount(); ++i) {
        weight.setShift((range.cell(i, 0) + range.cell(i, 1)) / 2, i);
    }
}

void MainWindow::recognize(bool /*ignored*/) {
    MatrixPtr matrixPtr = table();

    sCluster->outputSignal() = convertToSignal(matrixPtr);

    nueralNetwork->updateNetwork();

    auto outputSignal = rCluster->outputSignal();

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
//    QString str;
//    QTextStream out(&str);

//    int rowsCount = ui->table->rowCount();
//    int columnsCount = ui->table->columnCount();

//    auto functor = [&out, rowsCount, columnsCount] (SimpleClusterOfNeurons *cluster){
//        int precision = out.realNumberPrecision();
//        auto notation = out.realNumberNotation();
//        out.setRealNumberPrecision(5);
//        out.setRealNumberNotation(QTextStream::FixedNotation);
//        for(int k = 0; k != cluster->neuronsCount(); ++k) {
//            out << tr("Весовые коэффициенты для ");
//            out << QString::number(k + 1) << tr(" нейрона");
//            const auto &shift = cluster->weights().weightingShift();
//            const auto &weight = cluster->weights().weightingFactors();
//            out << "<table border=\"1\" cellpadding=\"3\" cellspacing=\"3\">";
//            for(int i = 0; i != rowsCount; ++i) {
//                out << "<tr>";
//                for(int j = 0; j != columnsCount; ++j) {
//                    out << "<td align=\"center\">" << weight(k, i * columnsCount + j) << "</td>";
//                }
//                out << "</tr>";
//            }
//            out << "<tr><td width=\"100%\" align=\"center\" colspan=\"" << columnsCount << "\">" << shift(k, 0) << "</td></tr></table><br><br>";
//        }
//        out.setRealNumberPrecision(precision);
//        out.setRealNumberNotation(notation);
//    };


//    out << tr("Информация по A-слою:<br>");
//    functor(aCluster);

//    out << tr("Информация по R-слою:<br>");
//    functor(rCluster);


//    sendMessage(str);
}

void MainWindow::changeAttributeCount(int count) {
    aCluster->setNueronsCount(count);
}

void MainWindow::changeRange(double /*value*/) {
    double lowerBound = ui->lowerBound->value();
    double uppedBound = ui->uppedBound->value();
    ui->lowerBound->setMaximum(uppedBound - 0.01);
    ui->uppedBound->setMinimum(lowerBound + 0.01);
}

void MainWindow::changeSmallTeta(double value) {
    perceptron->__ny__ = value;
}

void MainWindow::initShifts() {
    int imageCount = 0;
    Matrix shift = aCluster->weights().weightingShift();
    for(int i = 0; i != ui->imageGroup->count(); ++i) {
        for(auto matrixptr : this->images(i)) {
            sCluster->outputSignal() = convertToSignal(matrixptr);
            aCluster->summation();
            for(int j = 0; j != shift.rows(); ++j) {
                auto value = shift.cell(j, 0);
                value -= aCluster->outputSignal().signal(j);
                shift.setCell(j, 0, value);
            }
            ++imageCount;
        }
    }
    for(int j = 0; j != shift.rows(); ++j) {
        auto value = shift.cell(j, 0);
        value /= imageCount;
        shift.setCell(j, 0, value);
    }
    aCluster->__weights__.__weightingShift__ = shift;
}

QList<QList<Signal>> MainWindow::calcALayer() {
    QList<QList<Signal>> allSignals;
    for(int i = 0; i != ui->imageGroup->count(); ++i) {
        QList<Signal> groupSignals;
        for(auto matrixptr : this->images(i)) {
            sCluster->outputSignal() = convertToSignal(matrixptr);
            aCluster->summation();
            aCluster->activation();
            groupSignals.append(aCluster->outputSignal());
        }
        allSignals.append(groupSignals);
    }
    return allSignals;
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
    }
}

bool MainWindow::initWeights() {
    initRandomWeight(rCluster->weights());

    for(int iter = 0; iter != 100; ++iter) {
        initRandomWeight(aCluster->weights());
        initShifts();

        auto allSignals = calcALayer();

        bool okey = true;
        for(int selGrInd = 0; selGrInd != allSignals.size(); ++selGrInd) {
            auto selectedGroup = allSignals[selGrInd];
            for(int chGrInd = selGrInd + 1; chGrInd != allSignals.size(); ++chGrInd) {
                auto checkedGroup = allSignals[chGrInd];
                for(int selImInd = 0; selImInd != selectedGroup.size(); ++selImInd) {
                    auto selectedImage = selectedGroup[selImInd];
                    for(int chImInd = 0; chImInd != selectedGroup.size(); ++chImInd) {
                        auto checkedImage = checkedGroup[chImInd];
                        auto difference = diff(selectedImage, checkedImage);
                        if(difference / checkedImage.size() < 0.3) {
                            okey = false;
                            break;
                        }
                    }
                    if(!okey) break;
                }
                if(!okey) break;
            }
            if(!okey) break;
        }

        if(okey) return true;

    }
    return false;
}

Signal MainWindow::convertToSignal(MatrixPtr matrix) {
    Signal signal(matrix->rows() * matrix->columns());
    double summSignal = 0;
    for(int i = 0; i != matrix->rows(); ++i)
        for(int j = 0; j != matrix->columns(); ++j)
            summSignal += matrix->cell(i, j);

    for(int i = 0; i != matrix->rows(); ++i) {
        for(int j = 0; j != matrix->columns(); ++j)
            signal.setSignal(matrix->cell(i, j) / summSignal, i * matrix->columns() + j);
    }
    return signal;
}

double MainWindow::diff(const Signal &first, const Signal &second) {
    double d = 0;
    for(int i = 0; i != first.size(); ++i)
        d += std::abs(first.signal(i) - second.signal(i));
    return d;
}

double MainWindow::diffInProcent(const Signal &first, const Signal &second) {
    return diff(first, second) / first.size();
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
    sCluster->outputSignal() = convertToSignal(matrix);
    nueralNetwork->updateNetwork();
    rCluster->summation();

    int bestIndex = -1;
    double bestValue = -std::numeric_limits<double>::max();
    auto outputSignal = rCluster->outputSignal();
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
    rCluster->insertNeurons(rCluster->neuronsCount(), 1);
    ui->attributesCount->setMinimum(ui->imageGroup->count());
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
    rCluster->removeNeurons(group, group);
    ui->attributesCount->setMinimum(ui->imageGroup->count());
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
    connect(ui->balancing, &QPushButton::clicked, this, &MainWindow::balancing);
    connect(ui->recognizeImage, &QPushButton::clicked, this, &MainWindow::recognize);

    connect(ui->lowerBound, SIGNAL(valueChanged(double)), this, SLOT(changeRange(double)));
    connect(ui->uppedBound, SIGNAL(valueChanged(double)), this, SLOT(changeRange(double)));
    connect(ui->smallTeta, SIGNAL(valueChanged(double)), this, SLOT(changeSmallTeta(double)));

    ui->lowerBound->setMinimum(-1.0);
    ui->uppedBound->setMaximum( 1.0);

    ui->lowerBound->setValue(-0.1);
    ui->uppedBound->setValue( 0.1);

    sCluster = new SimpleClusterOfNeurons(ui->table->rowCount() * ui->table->columnCount());
    aCluster = new SimpleClusterOfNeurons(ui->attributesCount->value());
    rCluster = new SimpleClusterOfNeurons();

    binary = new Binary();
    perceptron = new Perceptron();
    aCluster->__activationFunction__ = binary;
    rCluster->__activationFunction__ = perceptron;

    sCluster->addOutput(aCluster);
    aCluster->addOutput(rCluster);

    nueralNetwork = new SimpleNeuralNetwork();

    nueralNetwork->addCluster(rCluster);
    nueralNetwork->addCluster(aCluster);
    nueralNetwork->addCluster(sCluster);

    nueralNetwork->updateClusterSequence();
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
