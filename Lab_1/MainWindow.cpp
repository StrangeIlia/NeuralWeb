#include "MainWindow.h"
#include "ui_MainWindow.h"

QString MainWindow::Models      = "Images";
QString MainWindow::MatrixData  = "Matrix";

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
    delete linearBipolar;
    delete inputBipolar;
    delete inputBinary;
    delete binaryNet;
    delete bipolarNet;
}

void MainWindow::changeImage(int imageIndex) {
    int groupIndex = ui->imageGroup->currentIndex();
    setTable(matrix(groupIndex, imageIndex));
}

void MainWindow::changeImageSet(int groupIndex) {
    ui->image->setModel(model(groupIndex).data());
}

void MainWindow::addImage(bool /*ignored*/) {
    addImage(ui->imageGroup->currentIndex());
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
    setMatrix(imageIndex, groupIndex, table());
}

void MainWindow::addGroup(bool /*ignored*/) {
    showMessage();
    return;

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
    setModel(ui->imageGroup->count(), modelForNewGroup());
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

    auto variant = ui->imageGroup->currentData();
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
    NeuralNetworkTrainer binaryTrainer(binaryNet, true);
    NeuralNetworkTrainer bipolarTrainer(bipolarNet, true);
    QSharedPointer<AbstractSignalConverter> binaryConverter(new BinaryConverter);
    QSharedPointer<AbstractSignalConverter> bipolarConverter(new BipolarConverter);
    for(int i = 0; i != ui->imageGroup->count(); ++i) {
        NeuralNetworkTrainer::Signal binaryInputSignal;
        NeuralNetworkTrainer::Signal bipolarInputSignal;
        QVector<MatrixPtr> images = this->images(i);
        if(images.empty()) {

            return;
        }
        for(MatrixPtr matrix : images) {
            Matrix binaryMatrix = *matrix;
            Matrix bipolarMatrix = *matrix;
            binaryConverter->convertToSignal(binaryMatrix);
            bipolarConverter->convertToSignal(bipolarMatrix);
            binaryInputSignal[inputBinary] = binaryMatrix;
            bipolarInputSignal[inputBipolar] = bipolarMatrix;
        }
    }
}

void MainWindow::recognize(bool /*ignored*/) {

}

template<class T>
T MainWindow::dataInHash(QVariantHash *hash, QString field) {
    if(!hash->contains(field))           return T();

    auto variantData = hash->value(field);
    if(!variantData.canConvert<T>())    return T();

    return variantData.value<T>();
}

QAbstractItemModelPtr MainWindow::model(int groupIndex) {
    if(groupIndex > ui->imageGroup->count())            throw std::error_code();

    auto variantUserData = ui->imageGroup->itemData(groupIndex);
    if(!variantUserData.canConvert<QVariantHash*>())    throw std::error_code();

    auto hash = variantUserData.value<QVariantHash*>();
    return dataInHash<QAbstractItemModelPtr>(hash, Models);
}

void MainWindow::setModel(int groupIndex, QAbstractItemModelPtr model) {
    if(groupIndex > ui->imageGroup->count())            throw std::error_code();
    auto variantUserData = ui->imageGroup->itemData(groupIndex);

    QVariantHash *hash;
    if(!variantUserData.isValid() || !variantUserData.canConvert<QVariantHash>()) {
        hash = new QVariantHash();
        ui->imageGroup->setItemData(groupIndex, QVariant::fromValue(hash));
    } else hash = variantUserData.value<QVariantHash*>();

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
    if(model == nullptr)                                return nullptr;

    auto index = model->index(imageIndex, 0);
    auto variantUserData = model->data(index, Qt::UserRole);
    if(!variantUserData.canConvert<QVariantHash*>())    return nullptr;

    auto hash = variantUserData.value<QVariantHash*>();
    return dataInHash<MatrixPtr>(hash, MatrixData);
}

void MainWindow::setMatrix(int groupIndex, int imageIndex, MatrixPtr matrix) {
    auto model = this->model(groupIndex);
    if(model == nullptr)                                throw std::error_code();

    auto index = model->index(imageIndex, 0);
    auto variantUserData = model->data(index, Qt::UserRole);

    QVariantHash *hash;
    if(!variantUserData.canConvert<QVariantHash*>())    {
        hash = new QVariantHash();
        model->setData(index, QVariant::fromValue(hash), Qt::UserRole);
    } else hash = variantUserData.value<QVariantHash*>();

    if(hash->contains(MatrixData)) {
        delete dataInHash<Matrix*>(hash, MatrixData);
    }

    hash->insert(MatrixData, QVariant::fromValue(matrix));
}

void MainWindow::addImage(int group) {
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
    auto model = this->model(group);
    int imageIndex = model->rowCount();
    model->insertRow(imageIndex);
    setMatrix(group, imageIndex, nullptr);
}

void MainWindow::removeImage(int group, int image) {
    auto model = this->model(group);
    model->removeRow(image);
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


void MainWindow::initImageSet() {
    connect(ui->addImage, SIGNAL(clicked(bool)), this, SLOT(addImage(bool)));
    connect(ui->saveImage, SIGNAL(clicked(bool)), this, SLOT(saveImage(bool)));
    connect(ui->removeImage, SIGNAL(clicked(bool)), this, SLOT(removeImage(bool)));
    connect(ui->image, SIGNAL(currentIndexChanged(int)), this, SLOT(changeImage(int)));
}

void MainWindow::initImageGroups() {
    addGroup(true);
    addGroup(true);
//    ui->addGroup->setEnabled(false);        ui->addGroup->setVisible(false);
//    ui->removeGroup->setEnabled(false);     ui->removeGroup->setVisible(false);

    connect(ui->addGroup, &QAbstractButton::clicked, this, &MainWindow::addGroup);
    connect(ui->removeGroup, &QAbstractButton::clicked, this, &MainWindow::removeGroup);
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
    connect(ui->clear, &QAbstractButton::clicked, this, &MainWindow::clearTableData);
}

void MainWindow::initNeuralWebs() {
    bipolar = new Bipolar(1);
    binary = new Binary(1);
    linearBipolar = new LinearBipolar(1, -1, 1);

    int inputCount = ui->table->rowCount() * ui->table->columnCount();
    inputBipolar = new Bipolar(inputCount);
    inputBinary = new Binary(inputCount);

    inputBipolar->addOutput(bipolar);
    inputBipolar->addOutput(linearBipolar);

    inputBinary->addOutput(binary);

    binaryNet = new DebugNeuralNetwork(true, false);
    bipolarNet = new DebugNeuralNetwork(true, false);

    binaryNet->addCluster(binary);
    binaryNet->addCluster(inputBinary);

    bipolarNet->addCluster(linearBipolar);
    bipolarNet->addCluster(bipolar);
    bipolarNet->addCluster(inputBipolar);

    binaryNet->updateClusterSequence();
    bipolarNet->updateClusterSequence();

    connect(ui->info, &QPushButton::clicked, this, &MainWindow::printfInfo);
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
        for(int k = 0; k != cluster->neuronsCount(); ++k) {
            out << tr("Весовые коэффициенты для ");
            out << QString::number(k + 1) << tr(" нейрона");
            const auto &shift = cluster->weightingShift();
            const auto &weight = cluster->weightingFactors();
            for(int i = 0; i != rowsCount; ++i) {
                out << "\n";
                out.setFieldWidth(8);
                for(int j = 0; j != columnsCount; ++j) {
                    auto value = std::round(weight(0, i * columnsCount + j) * 1e+5) * 1e-5;
                    out << value << " ";
                }
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
    out << "Q1 = " << QString::number(linearBipolar->q1()) << "\n";
    out << "Q2 = " << QString::number(linearBipolar->q2()) << "\n";
    out << "k = " << QString::number(2 / (linearBipolar->q2() - linearBipolar->q1())) << "\n";
    functor(linearBipolar);

    Info info(this);
    info.doc()->setPlainText(str);
    info.exec();
}

QAbstractItemModelPtr MainWindow::modelForNewGroup() const {
    QStandardItemModel *model = new QStandardItemModel();
    model->insertRows(0, 1);
    model->insertColumns(0, 1);
    auto index = model->index(0, 0);
    model->setData(index, tr("Новое изображение"));
    QVariantHash hash;
    hash[MatrixData] = QVariant::fromValue(QVector<bool>());
    model->setData(index, hash, Qt::UserRole);
    return QAbstractItemModelPtr(model);
}
