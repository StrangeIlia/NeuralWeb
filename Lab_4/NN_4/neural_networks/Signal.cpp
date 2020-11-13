#include "Signal.h"

Signal::Signal(Signal &&othen) : Matrix(std::move(othen)){

}

Signal::Signal(const Signal& othen) : Matrix(othen) {

}

Signal::Signal(int size, int threadCount) : Matrix(size, threadCount) {

}

Signal& Signal::operator = (Signal &&othen) {
    Matrix::operator=(std::move(othen));
    return *this;
}

Signal& Signal::operator = (const Signal &othen) {
    Matrix::operator=(othen);
    return *this;
}

Signal& Signal::operator +=(const Signal &othen) {
    if(threadCount() != othen.threadCount())
        throw std::invalid_argument("");
    if(size() != othen.size())
        throw std::invalid_argument("");
    for(int i = 0; i != size(); ++i)
        for(int j = 0; j != threadCount(); ++j)
            setSignal(signal(i, j) + othen.signal(i, j), i, j);
    return *this;
}

Signal& Signal::operator -=(const Signal &othen) {
    if(threadCount() != othen.threadCount())
        throw std::invalid_argument("");
    if(size() != othen.size())
        throw std::invalid_argument("");
    for(int i = 0; i != size(); ++i)
        for(int j = 0; j != threadCount(); ++j)
            setSignal(signal(i, j) - othen.signal(i, j), i, j);
    return *this;
}

int Signal::size() const {
    return rows();
}

int Signal::threadCount() const {
    return columns();
}

BaseValueType Signal::signal(int number, int thread) const {
    return Matrix::cell(number, thread);
}

void Signal::setSignal(BaseValueType value, int number, int thread) {
    Matrix::setCell(number, thread, value);
}

void Signal::setSize(int size, BaseValueType filler) {
    if(size != rows()) {
        Matrix::setRows(size);
        std::fill(getBaseRow().begin(), getBaseRow().end(), filler);
    }
}

void Signal::setThreadCount(int count, BaseValueType filler) {
    if(count != columns()) {
        Matrix::setColumns(count);
        std::fill(getBaseRow().begin(), getBaseRow().end(), filler);
    }
}

void Signal::removeSignals(int first, int last) {
    removeRows(first, last);
}

void Signal::insertSignals(int pos, int size) {
    insertRows(pos, size);
}

void Signal::removeThreads(int first, int last) {
    removeColumns(first, last);
}

void Signal::insertThreads(int pos, int size) {
    insertColumns(pos, size);
}

Signal Signal::merge(QList<Signal> listSignals) {
    if(listSignals.size() == 0)
        throw std::invalid_argument("List empty");
    if(listSignals.size() == 1)
        return listSignals.first();

    Signal result;
    result._rows = 0;
    result._columns = listSignals.first().threadCount();
    for(const auto &signal : listSignals) {
        if(signal.threadCount() != result._columns)
            throw std::invalid_argument("Not the same number of threads");
        result._rows += signal.size();
    }

    result._matrix.reserve(result._rows * result._columns);
    for(const auto &signal : listSignals) {
        for(auto value : signal.getBaseRow())
            result._matrix.push_back(value);
    }

    return result;
}

Signal Signal::merge(const Signal &first, const Signal &second) {
    if(first.threadCount() != second.threadCount())
        throw std::invalid_argument("Not the same number of threads");

    Signal result;
    result._rows = first.size();
    result._columns = first.threadCount();
    result._matrix.reserve(result._rows * result._columns);
    for(auto value : first._matrix)
        result._matrix.push_back(value);
    for(auto value : second._matrix)
        result._matrix.push_back(value);

    return result;
}

QList<Signal> Signal::split(QList<int> counts) {
    int summ = 0; for(int v : counts) {
        summ += v;
    }
    if(summ != size()) throw std::exception();

    int shift = 0;
    QList<Signal> result;
    for(int i = 0; i != counts.size(); ++i) {
        int reqSize = counts[i];
        Signal signal;
        signal._rows = reqSize;
        signal._columns = threadCount();
        signal._matrix.reserve(signal._rows * signal._columns);
        for(int i = 0; i != signal._rows; ++i)
            for(int j = 0; j != signal._columns; ++j)
                signal._matrix.push_back(this->cell(i + shift, j));
        shift += signal._rows;
        result.push_back(signal);
    }

    return result;
}

std::vector<BaseValueType> &Signal::__data__() {
    return _matrix;
}

const std::vector<BaseValueType> &Signal::__data__() const {
    return _matrix;
}

std::vector<BaseValueType>::iterator Signal::__begin__() {
    return _matrix.begin();
}

std::vector<BaseValueType>::const_iterator Signal::__begin__() const {
    return _matrix.begin();
}

std::vector<BaseValueType>::iterator Signal::__end__() {
    return _matrix.end();
}

std::vector<BaseValueType>::const_iterator Signal::__end__() const {
    return _matrix.end();
}
