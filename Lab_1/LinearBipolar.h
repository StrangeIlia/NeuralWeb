#ifndef MYACTIVATION_H
#define MYACTIVATION_H

#include "neural_networks/AbstractClusterOfNeurons.h"

class LinearBipolar : public AbstractClusterOfNeurons {
    BaseValueType _q1, _q2;
protected:
    inline BaseValueType activate(BaseValueType value) override;

public:
    LinearBipolar(int neuronCount, BaseValueType q1, BaseValueType q2);

    ~LinearBipolar();

    BaseValueType q1() const;
    void q1(BaseValueType value);

    BaseValueType q2() const;
    void q2(BaseValueType value);

protected:
    BaseValueType correct(BaseValueType input, BaseValueType ouput) override;
};


#endif // MYACTIVATION_H
