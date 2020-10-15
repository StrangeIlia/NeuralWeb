#ifndef BINARYCONVERTER_H
#define BINARYCONVERTER_H

#include "neural_networks/AbstractSignalConverter.h"

class BinaryConverter : public AbstractSignalConverter
{
    BaseValueType _center;
public:
    BinaryConverter(BaseValueType center = 0);

    BaseValueType center() const;
    void setCenter(BaseValueType value);

    void convertToSignal(Matrix& matrix) override;
    void convertToData(Matrix& matrix) override;
};

#endif // BINARYCONVERTER_H
