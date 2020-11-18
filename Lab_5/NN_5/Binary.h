#ifndef BINARY_H
#define BINARY_H

#include "neural_networks/AbstractActivation.h"

class Binary : public AbstractActivation
{
public:
    Binary();

    void activate(Signal &input) override;
};

#endif // BINARY_H
