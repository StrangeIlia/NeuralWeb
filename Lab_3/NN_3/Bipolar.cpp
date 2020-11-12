#include "Bipolar.h"


BaseValueType Bipolar::activate(BaseValueType value) {
    return value < _center ? -1 : 1;
}

BaseValueType Bipolar::correct(BaseValueType input, BaseValueType ouput) {
    return input * ouput;
}

Bipolar::Bipolar(int neuronCount, BaseValueType center) : AbstractClusterOfNeurons(neuronCount),  _center(center) {

}

Bipolar::~Bipolar() {}


BaseValueType Bipolar::center() const { return _center; }
void Bipolar::setCenter(BaseValueType center) { _center = center; }

