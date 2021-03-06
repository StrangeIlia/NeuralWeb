#ifndef BINARY_HPP
#define BINARY_HPP

#include "neural_networks/AbstractClusterOfNeurons.h"

class Binary : public AbstractClusterOfNeurons {
    BaseValueType _center = 0;

public:
    BaseValueType activate(BaseValueType value) override;
    BaseValueType correct(BaseValueType input, BaseValueType ouput) override;

public:
    Binary(int neuronCount, BaseValueType center = 0);
    ~Binary();

    BaseValueType center() const;
    void setCenter(BaseValueType center);
};
#endif // BINARY_HPP
