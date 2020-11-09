#include "BinaryConverter.h"

BinaryConverter::BinaryConverter(BaseValueType center) : _center(center) {

}

void BinaryConverter::convertToSignal(Matrix& matrix)  {
    auto &vector = matrix.getBaseRow();
    for(auto &value : vector) value = value > _center ? 1 : 0;
    matrix.setSize(matrix.rows() * matrix.columns(), 1);
}

void BinaryConverter::convertToData(Matrix& matrix) {
    auto &vector = matrix.getBaseRow();
    for(auto &value : vector) value = value > _center ? 1 : 0;
}

BaseValueType BinaryConverter::center() const {
    return _center;
}

void BinaryConverter::setCenter(BaseValueType value) {
    _center = value;
}
