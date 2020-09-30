#include "ComboBoxItemsEditor.h"

QString ComboBoxItemsEditor::ItemInsertPolicy = "ItemInsertPolicy";

ComboBoxItemsEditor::ComboBoxItemsEditor(QComboBox *comboBox, QObject *parent) : QObject(parent) {
    tracked = comboBox;
    connect(comboBox, SIGNAL(editTextChanged(QString)), this, SLOT(editTextChanged(QString)));
}

void ComboBoxItemsEditor::editTextChanged(QString str) {

}

QComboBox::InsertPolicy ComboBoxItemsEditor::getPolicy() const {
    auto variantHash = tracked->currentData(Qt::UserRole);
    if(!variantHash.isValid() || !variantHash.canConvert<QVariantHash>())
        return QComboBox::NoInsert;
    auto hash = variantHash.value<QVariantHash>();
    if(hash.find(ItemInsertPolicy) == hash.end())
        return QComboBox::NoInsert;

    auto variantInsertPolicy = hash.find(ItemInsertPolicy).value();
    if(!variantInsertPolicy.isValid() || !variantInsertPolicy.canConvert<QComboBox::InsertPolicy>())
        return QComboBox::NoInsert;

    return variantInsertPolicy.value<QComboBox::InsertPolicy>();
}
