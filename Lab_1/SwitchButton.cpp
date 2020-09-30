#include "SwitchButton.h"

SwitchButton::SwitchButton(QColor unselect, QColor select, bool isSelected, QWidget *parent) : QPushButton(parent) {
    _unselect = unselect;
    _select = select;
    _isSelected = isSelected;
    this->select(_isSelected);
    setAutoFillBackground(true);
    connect(this, SIGNAL(clicked(bool)), this, SLOT(buttonClicked(bool)));
}

QColor SwitchButton::select() const {
    return _select;
}

QColor SwitchButton::unselect() const {
    return _unselect;
}

bool SwitchButton::buttonClicked() const {
    return _isSelected;
}

void SwitchButton::select(QColor color) {
    _select = color;
}

void SwitchButton::select(bool value) {
    QColor activeColor = value ? _select : _unselect;
    QString str = "background-color: rgb(";
    str += QString::number(activeColor.red()) + ", ";
    str += QString::number(activeColor.green()) + ", ";
    str += QString::number(activeColor.blue()) + ");";
    setStyleSheet(str);
    _isSelected = value;
}

void SwitchButton::unselect(QColor color) {
    _unselect = color;
}

void SwitchButton::buttonClicked(bool) {
    select(!_isSelected);
}
