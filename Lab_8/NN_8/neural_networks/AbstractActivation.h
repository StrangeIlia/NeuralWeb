#ifndef FUNCTIONACTIVATION_H
#define FUNCTIONACTIVATION_H

#include "Signal.h"
#include "RelatationWeights.h"

class AbstractActivation
{
public:
    AbstractActivation();
    virtual ~AbstractActivation();

    virtual void activate(Signal &input);
    virtual Signal correct(const Signal &input,
                           const Signal &output,
                           RelatationWeights &weights,
                           const Signal &error);
};

#endif // FUNCTIONACTIVATION_H
