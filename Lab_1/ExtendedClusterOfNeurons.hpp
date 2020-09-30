#ifndef EXTENDEDCLUSTEROFNEURONS_H
#define EXTENDEDCLUSTEROFNEURONS_H

#include <QVector>
#include "AbstractClusterOfNeurons.hpp"

template<class Base>
class ExtendedClusterOfNeurons : public AbstractClusterOfNeurons<Base> {
public:
    virtual void readOutputSignals(const QVector<bool> &vec) = 0;
    virtual void writeOutputSignals(const QVector<bool> &vec) = 0;
};

#endif // EXTENDEDCLUSTEROFNEURONS_H
