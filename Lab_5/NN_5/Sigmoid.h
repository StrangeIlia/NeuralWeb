#ifndef SIGMOID_H
#define SIGMOID_H

#include "neural_networks/AbstractActivation.h"

class Sigmoid : public AbstractActivation
{
public:
    Sigmoid();

    BaseValueType learningRate() const;
    void setLearningRate(BaseValueType value);

    void activate(Signal &input) override;

    Signal correct(const Signal &input,
                   const Signal &output,
                   RelatationWeights &weights,
                   const Signal &error) override;

    BaseValueType __learningRate__ = 0.1;
};

#endif // SIGMOID_H
