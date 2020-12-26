#ifndef CLUSTERLAYER_H
#define CLUSTERLAYER_H

#include "Definitions.h"

class RelatationWeights
{
public:
    RelatationWeights();

    int inputsCount() const;
    void setInputsCount(int size);
    void insertInputs(int pos, int count);
    /// [first, last]
    void removeInputs(int first, int last);

    int neuronsCount() const;
    void setNeuronsCount(int count);
    void insertNeurons(int pos, int count);
    /// [first, last]
    void removeNeurons(int first, int last);

    const Matrix& weightingShift() const;
    const Matrix& weightingFactors() const;

    BaseValueType shift(int neuronNumber) const;
    void setShift(BaseValueType value, int neuronNumber);

    BaseValueType weight(int neuronNumber, int inputNumber) const;
    void setWeight(BaseValueType value, int neuronNumber, int inputNumber);

    Matrix __weightingShift__;
    Matrix __weightingFactors__;
};

#endif // CLUSTERLAYER_H
