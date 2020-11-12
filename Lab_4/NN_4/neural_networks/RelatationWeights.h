#ifndef CLUSTERLAYER_H
#define CLUSTERLAYER_H

#include "Definitions.h"

class RelatationWeights
{
public:
    RelatationWeights();

    inline int inputsCount() const;
    inline void setInputsCount(int size);
    inline void insertInputs(int pos, int count);
    /// [first, last]
    inline void removeInputs(int first, int last);

    inline int neuronsCount() const;
    inline void setNeuronsCount(int count);
    inline void insertNeurons(int pos, int count);
    /// [first, last]
    inline void removeNeurons(int first, int last);

    inline const Matrix& weightingShift() const;
    inline const Matrix& weightingFactors() const;

    BaseValueType shift(int neuronNumber) const;
    void setShift(BaseValueType value, int neuronNumber);

    BaseValueType weight(int neuronNumber, int inputNumber) const;
    void setWeight(BaseValueType value, int neuronNumber, int inputNumber);

    Matrix __weightingShift__;
    Matrix __weightingFactors__;
};

#endif // CLUSTERLAYER_H
