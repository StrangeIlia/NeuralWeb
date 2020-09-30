#include "ComboBoxItemsEditor.h"

ComboBoxItemsEditor::ComboBoxItemsEditor(QComboBox *comboBox, QObject *parent) : QObject(parent) {
    tracked = comboBox;
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged(int)));
}

void ComboBoxItemsEditor::currentIndexChanged(int index) {

}
