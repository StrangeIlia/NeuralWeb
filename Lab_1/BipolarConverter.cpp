#include "BipolarConverter.h"

BipolarConverter::BipolarConverter(BaseValueType center) : _center(center) {

}

void BipolarConverter::convertToSignal(Matrix& matrix)  {
    auto &vector = matrix.getBaseRow();
    for(auto &value : vector) value = value > _center ? 1 : -1;
}

void BipolarConverter::convertToData(Matrix& matrix) {
    auto &vector = matrix.getBaseRow();
    for(auto &value : vector) value = value > _center ? 1 :  0;
}

BaseValueType BipolarConverter::center() const {
    return _center;
}

void BipolarConverter::setCenter(BaseValueType value) {
    _center = value;
}
