#ifndef SWITCHBUTTON_H
#define SWITCHBUTTON_H

#include <QColor>
#include <QStyle>
#include <QPushButton>

class SwitchButton : public QPushButton
{
    Q_OBJECT
public:
    SwitchButton(QColor setUnselect, QColor setSelected, bool buttonClicked = false, QWidget *parent = nullptr);

    QColor selected() const;
    QColor unselect() const;
    bool isSelected() const;

    void setSelected(QColor color);
    void setSelected(bool value);
    void setUnselect(QColor color);

private slots:
    void buttonClicked(bool);

private:
    QColor _select;
    QColor _unselect;
    bool _isSelected = false;
};

#endif // SWITCHBUTTON_H
