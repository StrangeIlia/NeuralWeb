#include "MainWindow.h"
#include "ui_MainWindow.h"

QString MainWindow::ImageGroup = "ImageGroup";
QString MainWindow::Finctitious = "Finctitious";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initImageGroup();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initImageGroup() {
    QAbstractItemModel *model = ui->comboBox_ImGr->model();

    model->insertRows(0, 2);
    model->insertColumns(0, 1);

    auto indexFirst = model->index(0, 0);
    auto indexSecond = model->index(1, 0);
    QMap<int, QVariant> map;
    map[Qt::DisplayRole] = "A";
    map[Qt::UserRole] = QVariantHash();
    model->setItemData(indexFirst, map);
    map[Qt::DisplayRole] = "B";
    model->setItemData(indexSecond, map);
}
