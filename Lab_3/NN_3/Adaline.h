#ifndef ADALINE_H
#define ADALINE_H

#include <QRandomGenerator>
#include "neural_networks/AbstractClusterOfNeurons.h"

class Adaline : public AbstractClusterOfNeurons {
public:
    BaseValueType activate(BaseValueType value) override;
    BaseValueType correct(BaseValueType input, BaseValueType ouput) override;

public:
    Adaline(int neuronCount);
    ~Adaline();

    void correction(BaseValueType learningFactor = 1.0) override;

protected:
    BaseValueType getInitValue() const override;
};
#endif // ADALINE_H
