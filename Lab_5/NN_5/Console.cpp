#include "Console.h"
#include "ui_Console.h"

Console::Console(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Console)
{
    ui->setupUi(this);
    connect(ui->clear, &QPushButton::clicked, this, &Console::clear);
}

Console::~Console()
{
    delete ui;
}

QTextDocument *Console::doc() const {
    return ui->console->document();
}

void Console::printHtml(QString html) {
    if(doc()->toPlainText().size() == 0) {
        doc()->setHtml(html);
    } else {
        doc()->setHtml(doc()->toHtml() + "<br>" + html);
    }
}

void Console::printText(QString text) {
    QTextDocument doc;
    doc.setPlainText(text);
    printHtml(doc.toHtml());
}

void Console::clear(bool /*ignored*/) {
    doc()->clear();
}

void Console::closeEvent(QCloseEvent */*event*/) {
    emit closed(this);
}
