#ifndef NEURALMETHODS_H
#define NEURALMETHODS_H

#include <QVector>

#include "math/matrix/MatrixOnRow.hpp"

class AbstractNeuralMethods
{
public:
    AbstractNeuralMethods();

    virtual MatrixOnRow<double> convertData(const QVector<bool>&) = 0;
    virtual MatrixOnRow<double> calculate(const MatrixOnRow<double> &in) = 0;
};

#endif // NEURALMETHODS_H
