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

    inline BaseValueType q1() const;
    inline void q1(BaseValueType value);

    inline BaseValueType q2() const;
    inline void q2(BaseValueType value);

protected:
    BaseValueType correct(BaseValueType input, BaseValueType ouput) override;
};


#endif // MYACTIVATION_H
