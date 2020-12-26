#ifndef HEMMING_H
#define HEMMING_H

#include "neural_networks/AbstractActivation.h"

class Hemming : public AbstractActivation
{
public:
    Hemming();

    void activate(Signal &input) override;

    BaseValueType __t__ = 1;
};

#endif // HEMMING_H
