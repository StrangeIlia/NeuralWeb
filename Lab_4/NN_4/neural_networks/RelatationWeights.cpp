#include "RelatationWeights.h"

RelatationWeights::RelatationWeights() {
    __weightingShift__.setColumns(1);
}

int RelatationWeights::inputsCount() const {
    return __weightingFactors__.columns();
}

void RelatationWeights::setInputsCount(int size) {
    __weightingFactors__.setRows(size);
}

void RelatationWeights::insertInputs(int pos, int count) {
    __weightingFactors__.insertColumns(pos, count);
}

void RelatationWeights::removeInputs(int first, int last) {
    __weightingFactors__.removeColumns(first, last);
}

int RelatationWeights::neuronsCount() const {
    return __weightingFactors__.rows();
}

void RelatationWeights::setNeuronsCount(int count) {
    __weightingShift__.setRows(count);
    __weightingFactors__.setRows(count);
}

void RelatationWeights::insertNeurons(int pos, int count) {
    __weightingShift__.insertRows(pos, count);
    __weightingFactors__.insertRows(pos, count);
}

void RelatationWeights::removeNeurons(int first, int last) {
    __weightingShift__.removeRows(first, last);
    __weightingFactors__.removeRows(first, last);
}

const Matrix& RelatationWeights::weightingShift() const {
    return __weightingShift__;
}

const Matrix& RelatationWeights::weightingFactors() const {
    return __weightingFactors__;
}

BaseValueType RelatationWeights::shift(int neuronNumber) const {
    return __weightingShift__.cell(neuronNumber, 0);
}

void RelatationWeights::setShift(BaseValueType value, int neuronNumber) {
    __weightingShift__.setCell(neuronNumber, 0, value);
}

BaseValueType RelatationWeights::weight(int neuronNumber, int inputNumber) const {
    return __weightingFactors__.cell(neuronNumber, inputNumber);
}

void RelatationWeights::setWeight(BaseValueType value, int neuronNumber, int inputNumber) {
    __weightingFactors__.setCell(neuronNumber, inputNumber, value);
}
