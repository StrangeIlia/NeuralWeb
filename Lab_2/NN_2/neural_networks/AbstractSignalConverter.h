#ifndef SIGNALINTERPRETER_H
#define SIGNALINTERPRETER_H

#include <QHash>

#include "AbstractClusterOfNeurons.h"

class AbstractSignalConverter
{
public:
    AbstractSignalConverter();
    virtual ~AbstractSignalConverter();

    /// Преобразует исходный данные в сигнал
    virtual void convertToSignal(Matrix& matrix) = 0;
    /// Обратное преобразование
    virtual void convertToData(Matrix& matrix) = 0;
};

#endif // SIGNALINTERPRETER_H
