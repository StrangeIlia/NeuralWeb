#ifndef SWITCHBUTTON_H
#define SWITCHBUTTON_H

#include <QColor>
#include <QStyle>
#include <QPushButton>

class SwitchButton : public QPushButton
{
    Q_OBJECT
public:
    SwitchButton(QColor unselect, QColor select, bool buttonClicked = false, QWidget *parent = nullptr);

    QColor select() const;
    QColor unselect() const;
    bool buttonClicked() const;

    void select(QColor color);
    void select(bool value);
    void unselect(QColor color);

private slots:
    void buttonClicked(bool);

private:
    QColor _select;
    QColor _unselect;
    bool _isSelected = false;
};

#endif // SWITCHBUTTON_H
