#include "LinearBipolar.h"

BaseValueType LinearBipolar::activate(BaseValueType value) {
    if(value < _q1) return -1;
    if(value > _q2) return  1;
    return (value - _q1) / (_q2 - _q1) * 2 - 1;
}

LinearBipolar::LinearBipolar(int neuronCount, BaseValueType q1, BaseValueType q2) :
    AbstractClusterOfNeurons(neuronCount),
    _q1(q1),
    _q2(q2) {

}

LinearBipolar::~LinearBipolar() {}

BaseValueType LinearBipolar::q1() const { return _q1; }
void LinearBipolar::q1(BaseValueType value) { _q1 = value; }

BaseValueType LinearBipolar::q2() const { return _q2; }
void LinearBipolar::q2(BaseValueType value) { _q2 = value; }

BaseValueType LinearBipolar::correct(BaseValueType /*input*/, BaseValueType /*ouput*/)  {
    return 0;
}
