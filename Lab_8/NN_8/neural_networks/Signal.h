#ifndef SIGNAL_H
#define SIGNAL_H

#include <QList>
#include "Definitions.h"

class Signal : protected Matrix
{
public:
    Signal(Signal &&othen);
    Signal(const Signal& othen);
    Signal(int size = 0, int threadCount = 1);

    Signal& operator = (Signal &&othen);
    Signal& operator = (const Signal &othen);

    Signal& operator +=(const Signal &othen);
    Signal& operator -=(const Signal &othen);

    int size() const;
    int threadCount() const;

    BaseValueType signal(int number, int thread = 0) const;
    void setSignal(BaseValueType value, int number, int thread = 0);

    void setSize(int size, BaseValueType filler = BaseValueType());
    void setThreadCount(int count, BaseValueType filler = BaseValueType());

    void removeSignals(int first, int last);
    void insertSignals(int pos, int size);

    void removeThreads(int first, int last);
    void insertThreads(int pos, int size);

    static Signal merge(QList<Signal> listSignals);
    static Signal merge(const Signal &first, const Signal &second);

    // summ(counts) == this->size()
    QList<Signal> split(QList<int> counts);

    std::vector<BaseValueType> &__data__();
    const std::vector<BaseValueType> &__data__() const;
    std::vector<BaseValueType>::iterator __begin__();
    std::vector<BaseValueType>::const_iterator __begin__() const;
    std::vector<BaseValueType>::iterator __end__();
    std::vector<BaseValueType>::const_iterator __end__() const;
};

#endif // SIGNAL_H
