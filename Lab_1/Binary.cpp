#include "Binary.h"

BaseValueType Binary::activate(BaseValueType value) {
    return value < _center ? 0 : 1;
}

BaseValueType Binary::correct(BaseValueType input, BaseValueType ouput) {
    return input * ouput;
}

Binary::Binary(int neuronCount, BaseValueType center) : AbstractClusterOfNeurons(neuronCount),  _center(center) {

}

Binary::~Binary() {}

BaseValueType Binary::center() const { return _center; }
void Binary::setCenter(BaseValueType center) { _center = center; }
