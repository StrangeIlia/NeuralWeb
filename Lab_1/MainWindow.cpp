#include "MainWindow.h"
#include "ui_MainWindow.h"

QString MainWindow::Images = "Images";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initImageSet();
    initTableSize();
    initImageGroups();
}

MainWindow::~MainWindow()
{
    delete ui;
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

void MainWindow::initImageSet() {
    connect(ui->addImage, SIGNAL(clicked(bool)), this, SLOT(addImage(bool)));
    connect(ui->removeImage, SIGNAL(clicked(bool)), this, SLOT(removeImage(bool)));
}

void MainWindow::initImageGroups() {
    addGroup(true);
    addGroup(true);
    connect(ui->addGroup, &QAbstractButton::clicked, [this] (bool) { this->addGroup(true); });
    connect(ui->removeGroup, &QAbstractButton::clicked, [this] (bool) { this->removeGroup(true); });
    connect(ui->imageGroup, SIGNAL(currentIndexChanged(int)), this, SLOT(changeImageSet(int)));
    ui->imageGroup->setCurrentIndex(0);
}

void MainWindow::initTableSize() {
    ui->table->verticalHeader()->setVisible(false);
    ui->table->horizontalHeader()->setVisible(false);
    ui->table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
    ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
    for(int i = 0; i != ui->table->rowCount(); ++i) {
        for (int j = 0; j != ui->table->columnCount(); ++j) {
            ui->table->setCellWidget(i, j, new SwitchButton(Qt::white, Qt::black));
        }
    }
}

QAbstractItemModel *MainWindow::modelForNewGroup() const {
    QStandardItemModel *model = new QStandardItemModel();
    model->insertRows(0, 1);
    model->insertColumns(0, 1);
    auto index = model->index(0, 0);
    model->setData(index, tr("Новое изображение"));
    return model;
}
