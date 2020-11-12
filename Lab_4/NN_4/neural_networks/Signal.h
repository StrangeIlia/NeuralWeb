#ifndef SIGNAL_H
#define SIGNAL_H

#include <QList>
#include "Definitions.h"

class Signal : protected Matrix
{
public:
    Signal();
    Signal(Signal &&othen);
    Signal(const Signal& othen);
    Signal(int size, int threadCount = 1);

    inline Signal& operator = (Signal &&othen);
    inline Signal& operator = (const Signal &&othen);

    inline int size() const;
    inline int threadCount() const;

    inline BaseValueType signal(int number, int thread = 0) const;
    inline void setSignal(BaseValueType value, int number, int thread = 0);

    inline void setSize(int size, BaseValueType filler = BaseValueType());
    inline void setThreadCount(int count, BaseValueType filler = BaseValueType());

    inline void removeSignals(int first, int last);
    inline void insertSignals(int pos, int size, BaseValueType filler = BaseValueType());

    inline void removeThreads(int first, int last);
    inline void insertThreads(int pos, int size, BaseValueType filler = BaseValueType());

    static Signal merge(QList<Signal> listSignals);
    static Signal merge(const Signal &first, const Signal &second);

    // summ(counts) == this->size()
    QList<Signal> split(QList<int> counts);

    inline std::vector<BaseValueType> &__data__();
    inline const std::vector<BaseValueType> &__data__() const;
    inline std::vector<BaseValueType>::iterator __begin__();
    inline std::vector<BaseValueType>::const_iterator __begin__() const;
    inline std::vector<BaseValueType>::iterator __end__();
    inline std::vector<BaseValueType>::const_iterator __end__() const;
};

#endif // SIGNAL_H
