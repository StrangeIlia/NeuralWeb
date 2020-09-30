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
    void changeImageSet(int index);
    void addImage(bool);
    void removeImage(bool);

private:
    Ui::MainWindow *ui;

    void showMessage();
    void addGroup(bool isValid);
    void removeGroup(bool isValid);


    void initImageSet();
    void initImageGroups();
    void initTableSize();

    QAbstractItemModel *modelForNewGroup() const;

    static QString Images;
};
#endif // MAINWINDOW_H
