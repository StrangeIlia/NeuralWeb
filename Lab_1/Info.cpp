#include "Info.h"
#include "ui_Info.h"

Info::Info(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Info)
{
    ui->setupUi(this);
}

Info::~Info()
{
    delete ui;
}

QTextDocument *Info::doc() const {
    return ui->textBrowser->document();
}
