#ifndef BIPOLARCONVERTER_H
#define BIPOLARCONVERTER_H

#include "neural_networks/AbstractSignalConverter.h"

class BipolarConverter : public AbstractSignalConverter
{
    BaseValueType _center;
public:
    BipolarConverter(BaseValueType center = 0);

    BaseValueType center() const;
    void setCenter(BaseValueType value);

    void convertToSignal(Matrix& matrix) override;
    void convertToData(Matrix& matrix) override;
};

#endif // BIPOLARCONVERTER_H
