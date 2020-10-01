#ifndef DEBUGCLUSTEROFNEURONS_H
#define DEBUGCLUSTEROFNEURONS_H

#include "AbstractClusterOfNeurons.hpp"

template<class Base>
class DebugClusterOfNeurons : public AbstractClusterOfNeurons<Base>
{
public:
    DebugClusterOfNeurons();

    void processingSumm() override;
};

#endif // DEBUGCLUSTEROFNEURONS_H
