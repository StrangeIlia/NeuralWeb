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

    qDebug() << "";
}

MainWindow::~MainWindow() {
    delete ui;
    delete binary;
    delete inputBinary;
    delete binaryNet;
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
    EnchancedNeuralNetworkTrainer binaryTrainer(binaryNet, true);
    SignalConverterPtr binaryConverter(new BinaryConverter);

    binary->initWeight();

    for(int i = 0; i != ui->imageGroup->count(); ++i) {
        NeuralNetworkTrainer::Signal binaryOutputSignal;

        Matrix out(ui->imageGroup->count(), 1);
        for(int k = 0; k != out.rows(); ++k)
            out(k, 0) = k == i ? 1 : 0;

        Matrix binaryMatrix = out;
        Matrix bipolarMatrix = out;
        binaryConverter->convertToSignal(binaryMatrix);

        binaryOutputSignal[binary] = binaryMatrix;

        NeuralNetworkTrainer::Signal binaryInputSignal;
        QVector<MatrixPtr> images = this->images(i);
        if(images.empty()) {
            QString message = tr("Ошибка: группа \"");
            message += ui->imageGroup->itemData(i, Qt::DisplayRole).toString() + "\"";
            message += tr(" не содержит изображений");
            sendMessage(message);
            return;
        }
        for(MatrixPtr matrix : images) {
            Matrix binaryMatrix = *matrix;
            binaryConverter->convertToSignal(binaryMatrix);
            binaryInputSignal[inputBinary] = binaryMatrix;

            binaryTrainer.addTrainingSet(new NeuralNetworkTrainer::TrainingSet{
                                             binaryInputSignal,
                                             binaryOutputSignal,
                                         });
        }
    }

    double learningFactor = ui->learningFactor->value();

    binaryTrainer.training(learningFactor);

    sendMessage(tr("Нейронные сети обучены"));
}

void MainWindow::recognize(bool /*ignored*/) {
    MatrixPtr matrix = table();
    SignalConverterPtr binaryConverter(new BinaryConverter);

    Matrix binaryMatrix = *matrix;
    binaryConverter->convertToSignal(binaryMatrix);
    inputBinary->setOutputSignal(binaryMatrix);

    binaryNet->updateNetwork();

    binaryMatrix = binary->outputSignal();
    binaryConverter->convertToData(binaryMatrix);

    QList<int> groups;
    for(int i = 0; i != binaryMatrix.rows(); ++i){
        int s = round(binaryMatrix(i, 0));
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
        message += "Вероятнее всего данное изображение относится к группе \"";
        int bestGroup = closestGroup(matrix);
        message += ui->imageGroup->itemData(bestGroup, Qt::DisplayRole).toString();
        message += "\"<br>";
    } else {
        message = tr("Нейронная сеть считает, что это изображение относится к группе:");
        message += "\"" + ui->imageGroup->itemText(groups.front()) + "\"<br>";
    }
    sendMessage(message);
}

void MainWindow::centring(bool /*ignored*/) {
    binaryNet->setSaveSummSignal(true);

    SignalConverterPtr binaryConverter(new BinaryConverter);

    int signalCount = 0;
    Matrix avgBinarySignal(ui->imageGroup->count(), 1);

    for(int i = 0; i != ui->imageGroup->count(); ++i) {
        auto images = this->images(i);
        for(auto matrixptr : images) {
            Matrix binaryMatrix = *matrixptr;
            binaryConverter->convertToSignal(binaryMatrix);
            inputBinary->setOutputSignal(binaryMatrix);

            binaryNet->updateNetwork();

            Matrix summBinarySignal = binaryNet->summSignal().value(binary);

            for(int i = 0; i != summBinarySignal.rows(); ++i)
                avgBinarySignal(i, 0) += summBinarySignal(i, 0);
        }
        signalCount += images.size();
    }

    for(int i = 0; i != avgBinarySignal.rows(); ++i) {
        avgBinarySignal(i, 0) /= signalCount;
        avgBinarySignal(i, 0) = binary->weightingShift().cell(i, 0) - avgBinarySignal(i, 0);
    }


    binary->setWeightingShift(avgBinarySignal);

    binaryNet->setSaveSummSignal(false);
    sendMessage(tr("Веса откорректированы"));
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
    binaryNet->setSaveSummSignal(true);
    SignalConverterPtr binaryConverter(new BinaryConverter);

    Matrix binaryMatrix = *matrix;
    binaryConverter->convertToSignal(binaryMatrix);
    inputBinary->setOutputSignal(binaryMatrix);
    binaryNet->updateNetwork();
    auto hash = binaryNet->summSignal();
    auto signal = hash[binary];

    int indexMaxValue = -1;
    double maxValue = -std::numeric_limits<double>::max();
    for(int i = 0; i != signal.rows(); ++i) {
        if(signal.cell(i, 0) > maxValue) {
            maxValue = signal.cell(i, 0);
            indexMaxValue = i;
        }
    }

    binaryNet->setSaveSummSignal(false);

    return indexMaxValue;
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
    binary->insertNeurons(binary->neuronsCount(), 1);
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
    binary->removeNeurons(group, group);
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
    binary = new Binary(1);
    binary->removeNeurons(0, 0);

    int inputCount = ui->table->rowCount() * ui->table->columnCount();
    inputBinary = new Binary(inputCount);

    inputBinary->addOutput(binary);

    binaryNet = new DebugNeuralNetwork();

    binaryNet->addCluster(binary);
    binaryNet->addCluster(inputBinary);

    binaryNet->updateClusterSequence();

    connect(ui->info, &QPushButton::clicked, this, &MainWindow::printfInfo);
    connect(ui->learning, &QPushButton::clicked, this, &MainWindow::training);
    connect(ui->centring, &QPushButton::clicked, this, &MainWindow::centring);
    connect(ui->recognizeImage, &QPushButton::clicked, this, &MainWindow::recognize);
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

void MainWindow::printfInfo(bool /*ignored*/) {
    QString str;
    QTextStream out(&str);

    int rowsCount = ui->table->rowCount();
    int columnsCount = ui->table->columnCount();

    auto functor = [&out, rowsCount, columnsCount] (AbstractClusterOfNeurons *cluster){
        int precision = out.realNumberPrecision();
        auto notation = out.realNumberNotation();
        out.setRealNumberPrecision(5);
        out.setRealNumberNotation(QTextStream::FixedNotation);
        for(int k = 0; k != cluster->neuronsCount(); ++k) {
            out << tr("Весовые коэффициенты для ");
            out << QString::number(k + 1) << tr(" нейрона");
            const auto &shift = cluster->weightingShift();
            const auto &weight = cluster->weightingFactors();
            out << "<table border=\"1\" cellpadding=\"3\" cellspacing=\"3\">";
            for(int i = 0; i != rowsCount; ++i) {
                out << "<tr>";
                for(int j = 0; j != columnsCount; ++j) {
                    out << "<td align=\"center\">" << weight(k, i * columnsCount + j) << "</td>";
                }
                out << "</tr>";
            }
            out << "<tr><td width=\"100%\" align=\"center\" colspan=\"" << columnsCount << "\">" << shift(k, 0) << "</td></tr></table><br><br>";
        }
        out.setRealNumberPrecision(precision);
        out.setRealNumberNotation(notation);
    };


    out << tr("Информация по бинарной функции:<br>");
    functor(binary);

    sendMessage(str);
}

void MainWindow::closeEvent(QCloseEvent* /*ignored*/) {
   emit closed(this);
}
