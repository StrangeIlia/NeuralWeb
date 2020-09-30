#ifndef COMBOBOXITEMSEDITOR_H
#define COMBOBOXITEMSEDITOR_H

#include <QComboBox>

#include <QStyledItemDelegate>

class ComboBoxItemsEditor : public QObject
{
    Q_OBJECT
public:
    ComboBoxItemsEditor(QComboBox *, QObject *parent);

    enum Policy {
        TextChange,
        CreateNewItem
    };

    static QString ItemTextChange;

private slots:
    void currentIndexChanged(int index);

private:
    QComboBox *tracked;
};

#endif // COMBOBOXITEMSEDITOR_H
