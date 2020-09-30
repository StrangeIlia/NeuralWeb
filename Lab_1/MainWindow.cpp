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
    map[0] = "A";
    model->setItemData(indexFirst, map);
    map[0] = "B";
    model->setItemData(indexSecond, map);
//    ui->comboBox_ImGr->setEditable(true);
//    ui->comboBox_ImGr->setInsertPolicy(QComboBox::InsertAtCurrent);

//    ui->comboBox_ImGr->addItem(tr("A"));
//    ui->comboBox_ImGr->addItem(tr("B"));

//    ComboBoxInfo info;
//    info.setRole(ImageGroup);
//    ui->comboBox_ImGr->addItem(tr("A"), info);
//    ui->comboBox_ImGr->addItem(tr("B"), info);

//    info.setRole(Finctitious);
//    ui->comboBox_ImGr->addItem(tr("Добавить группу изображений"), info);
}
