#ifndef INFO_H
#define INFO_H

#include <QDialog>
#include <QTextDocument>

namespace Ui {
class Console;
}

class Console : public QDialog
{
    Q_OBJECT

public:
    explicit Console(QWidget *parent = nullptr);
    ~Console();

    QTextDocument *doc() const;

signals:
    void closed(QObject*);

public slots:
    void printHtml(QString html);
    void printText(QString text);
    void clear(bool ignored = false);

private:
    Ui::Console *ui;

    void closeEvent(QCloseEvent *event) override;
};

#endif // INFO_H
