#ifndef COMBOBOXITEMSEDITOR_H
#define COMBOBOXITEMSEDITOR_H

#include <QComboBox>
#include <QVariant>
#include <QStyledItemDelegate>

class ComboBoxItemsEditor : public QObject
{
    Q_OBJECT
public:
    static QString ItemInsertPolicy;
public:
    ComboBoxItemsEditor(QComboBox *, QObject *parent);

private slots:
    void editTextChanged(QString str);

private:
    QComboBox *tracked;

    QComboBox::InsertPolicy getPolicy() const;
};


#endif // COMBOBOXITEMSEDITOR_H
