#ifndef PERCEPTRON_H
#define PERCEPTRON_H

#include "Binary.h"

class Perceptron : public Binary
{
public:
    Perceptron();

    BaseValueType ny() const;
    void setNy(BaseValueType value);


    Signal correct(const Signal &input, RelatationWeights &weights, const Signal &error) override;

    BaseValueType __ny__ = 1.2;
};

#endif // PERCEPTRON_H
