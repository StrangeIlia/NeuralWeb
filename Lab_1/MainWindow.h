#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QStandardItemModel>

#include "SwitchButton.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void changeImage(int index);
    void changeImageSet(int index);
    void addImage(bool);
    void removeImage(bool);
    void saveImage(bool);

private:
    Ui::MainWindow *ui;

    void showMessage();
    void addGroup(bool isValid);
    void removeGroup(bool isValid);

    void initImageSet();
    void initImageGroups();
    void initTableSize();

    void setImageData(int index, const QVector<bool> &vec);
    void setTableData(const QVector<bool> &vec);
    void readTableData(QVector<bool> &vec);
    void clearTableData();

    QAbstractItemModel *modelForNewGroup() const;

    static QString Images;
    static QString Matrix;
};
#endif // MAINWINDOW_H
