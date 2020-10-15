#ifndef BIPOLAR_H
#define BIPOLAR_H

#include "neural_networks/AbstractClusterOfNeurons.h"

class Bipolar : public AbstractClusterOfNeurons {
    BaseValueType _center;
protected:
    BaseValueType activate(BaseValueType value) override;
    BaseValueType correct(BaseValueType input, BaseValueType ouput) override;

public:
    Bipolar(int neuronCount, BaseValueType center = 0);
    ~Bipolar();

    inline BaseValueType center() const;
    inline void setCenter(BaseValueType center);
};

#endif // BIPOLAR_H
