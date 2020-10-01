#ifndef CLUSTEROFNEURONS_H
#define CLUSTEROFNEURONS_H

#include <QList>
#include <stdexcept>

#include "math/matrix/MatrixOnRow.hpp"
#include <QRandomGenerator>

/// Это шаблон, поэтому реализация прямо в файле с заголовками
/// TODO добавить число входных сигналов
template<class Base>
class AbstractClusterOfNeurons
{
protected:
    MatrixOnRow<Base> outputSignal;
    MatrixOnRow<Base> weightingShift;
    MatrixOnRow<Base> weightingFactors;
    QList<AbstractClusterOfNeurons*> inputs;
    QList<AbstractClusterOfNeurons*> outputs;

public:
    AbstractClusterOfNeurons(int neuronsCount);

    ~AbstractClusterOfNeurons();

    /// Функия активации
    virtual Base activationFunction(const Base&) = 0;

    /// Число нейронов в кластере
    inline int neuronsCount() const;

    /// Число одновременно обрабатываемых входных сигналов
    inline int threadsCount() const;

    /// Возвращает число входных сигналов
    inline int inputSignalCount() const;

    /// Устанавливает число одновременно
    /// обрабатываемых входных сигналов
    /// !!! ВНИМАНИЕ !!! Число входов будет изменено для все сети
    /// !!! ВНИМАНИЕ !!! Даннный метод пока не реализован (будет выкинуто исключение std::exception)
    inline void setThreadsCount(int count);

    /// Вставляет count нейронов на index место
    void insertNeurons(int index, int count);

    /// Удаляет нейроны [first; last]
    void removeNeurons(int first, int last);

    /// Устанавливает число нейронов в кластере
    void setNueronsCount(int count);

    /// Добавить входной кластер
    inline bool addInput(AbstractClusterOfNeurons *cluster);

    /// Добавить выходной кластер
    inline bool addOutput(AbstractClusterOfNeurons *cluster);

    /// Удалить входной кластер
    inline bool removeInput(AbstractClusterOfNeurons *cluster);

    /// Удалить выходной кластер
    inline bool removeOutput(AbstractClusterOfNeurons *cluster);

    /// Создать связь между кластерами
    static bool connect(AbstractClusterOfNeurons *in, AbstractClusterOfNeurons *out);

    /// Удалить связь между кластерами
    static bool disconnect(AbstractClusterOfNeurons *in, AbstractClusterOfNeurons *out);

    /// Удаляет связи со всеми кластерами
    void clearLinks();

    /// Заполняет выходные сигналы сигналом сдвига
    inline void clearOutputSignal();

    /// Расчитывает входной сигнал
    virtual void calculateSumm();

    /// Преобразует суммарный входной сигнал в выходной
    virtual bool processingSumm();

    /// Чтение выходных сигналов
    void readOutputSignals(MatrixOnRow<Base> &output);

    /// Запись выходных сигналов
    void writeOutputSignals(MatrixOnRow<Base> &input);

    /// Чтение весов для сдвига
    void readShiftWeight(MatrixOnRow<Base> &output);

    /// Запись весов для сдвига
    void writeShiftWeight(MatrixOnRow<Base> &input);

    /// Чтение весовых коэффициентов
    void readWeightingFactors(MatrixOnRow<Base> &output);

    /// Запись весовых коэффициентов
    void writeWeightingFactors(MatrixOnRow<Base> &input);

protected:
    virtual Base getInitValue() const;

    void neuronsAppended(AbstractClusterOfNeurons *updated, int index, int count);

    void neuronsDeleted(AbstractClusterOfNeurons *updated, int first, int last);
};


template<class Base>
AbstractClusterOfNeurons<Base>::AbstractClusterOfNeurons(int neuronsCount) {
    if(neuronsCount < 1)
        throw std::invalid_argument("AbstractClusterOfNeurons::AbstractClusterOfNeurons(int): The minimum number of neurons in a cluster 1");
    outputSignal.setColumns(1);
    weightingShift.setColumns(1);
    insertNeurons(0, neuronsCount);
}

template<class Base>
AbstractClusterOfNeurons<Base>::~AbstractClusterOfNeurons() {
    clearLinks();
}

template<class Base>
int AbstractClusterOfNeurons<Base>::neuronsCount() const {
    return outputSignal.rows();
}

template<class Base>
int AbstractClusterOfNeurons<Base>::threadsCount() const {
    return outputSignal.columns();
}

template<class Base>
void AbstractClusterOfNeurons<Base>::setThreadsCount(int /*count*/) {
    /// Пока нет реализации
    throw std::exception();
}

template<class Base>
void AbstractClusterOfNeurons<Base>::insertNeurons(int index, int count) {
    if(index < 0)
        throw std::invalid_argument("AbstractClusterOfNeurons::insertNeurons(int, int): Index can't be less than 0");
    if(count < 0)
        throw std::invalid_argument("AbstractClusterOfNeurons::insertNeurons(int, int): Count can't be less than 0");
    if(index > neuronsCount())
        throw std::invalid_argument("AbstractClusterOfNeurons::insertNeurons(int, int): Index can't be greater than neurons count");

    outputSignal.insertRows(index, count);
    weightingShift.insertRows(index, count);
    weightingFactors.insertRows(index, count);

    for(int i = index; i != weightingShift.rows(); ++i)
        for(int j = 0; j != weightingShift.columns(); ++j)
            weightingShift(i, j) = getInitValue();

    for(int i = index; i != weightingFactors.rows(); ++i)
        for(int j = 0; j != weightingFactors.columns(); ++j)
            weightingFactors(i, j) = getInitValue();


    for(auto cluster : outputs)
        cluster.neuronsAppended(this, index, count);
}

template<class Base>
void AbstractClusterOfNeurons<Base>::removeNeurons(int first, int last) {
    if(first < 0)
        throw std::invalid_argument("AbstractClusterOfNeurons::removeNeurons(int, int): First can't be less than 0");
    if(last < 0)
        throw std::invalid_argument("AbstractClusterOfNeurons::removeNeurons(int, int): Last can't be less than 0");
    if(first > neuronsCount())
        throw std::invalid_argument("AbstractClusterOfNeurons::removeNeurons(int, int): First can't be greater than neurons count");
    if(last > neuronsCount())
        throw std::invalid_argument("AbstractClusterOfNeurons::removeNeurons(int, int): Last can't be greater than neurons count");

    outputSignal.removeRows(first, last);
    weightingShift.removeRows(first, last);
    weightingFactors.removeRows(first, last);
    for(auto cluster : outputs)
        cluster.neuronsDeleted(this, first, last);
}


template<class Base>
void AbstractClusterOfNeurons<Base>::setNueronsCount(int count) {
    if(count < 0)
        throw std::invalid_argument("AbstractClusterOfNeurons::setNueronsCount(int): Count can't be less than 0");

     if(count > neuronsCount())
         insertNeurons(neuronsCount(), count - neuronsCount());
     else
         removeNeurons(count, neuronsCount() - 1);
}

template<class Base>
bool AbstractClusterOfNeurons<Base>::addInput(AbstractClusterOfNeurons *cluster) {
    return connect(cluster, this);
}

template<class Base>
bool AbstractClusterOfNeurons<Base>::addOutput(AbstractClusterOfNeurons *cluster) {
    return connect(this, cluster);
}

template<class Base>
bool AbstractClusterOfNeurons<Base>::removeInput(AbstractClusterOfNeurons *cluster) {
    return disconnect(cluster, this);
}

template<class Base>
bool AbstractClusterOfNeurons<Base>::removeOutput(AbstractClusterOfNeurons *cluster) {
    return disconnect(this, cluster);
}

template<class Base>
bool AbstractClusterOfNeurons<Base>::connect(AbstractClusterOfNeurons *in, AbstractClusterOfNeurons *out) {
    if(in->outputs.contains(out)) return false;
    if(in->threadsCount() != out->threadsCount()) return false;

    in->outputs.push_back(out);
    out->inputs.push_back(in);
    out->neuronsAppended(in, 0, in->neuronsCount());
    return true;
}

template<class Base>
bool AbstractClusterOfNeurons<Base>::disconnect(AbstractClusterOfNeurons *in, AbstractClusterOfNeurons *out) {
    if(!in->outputs.contains(out)) return false;

    in->outputs.removeOne(out);
    out->neuronsDeleted(in, 0, in->neuronsCount() - 1);
    out->inputs.removeOne(out);
    return true;
}

template<class Base>
void AbstractClusterOfNeurons<Base>::clearLinks() {
    auto list = inputs;
    for(auto cluster : list)
        disconnect(cluster, this);
    list = outputs;
    for(auto cluster : list)
        disconnect(this, cluster);
}

template<class Base>
void AbstractClusterOfNeurons<Base>::clearOutputSignal() {
    auto startSignal = outputSignal.getBaseRow().begin();
    auto endSignal = outputSignal.getBaseRow().end();
    auto shiftIter = weightingShift.getBaseRow().begin();
    while(startSignal != endSignal) {
        *startSignal += *shiftIter;
        ++startSignal;
        ++shiftIter;
    }
}

template<class Base>
void AbstractClusterOfNeurons<Base>::calculateSumm() {
    clearOutputSignal();
    if(outputSignal.columns() == 1) {
        /// Из - за особенности хранения данных в матрице
        /// Можно использовать итераторы

        /// Итератор по выходному сигналу
        auto startSignal = outputSignal.getBaseRow().begin();
        for(auto cluster : inputs) {
            auto signalIter = startSignal;

            /// Итераторы по входному сигналу
            auto startInput = cluster->outputSignal.begin();
            auto endInput = cluster->outputSignal.end();
            /// Итераторы по весовым коэффициентам
            auto startWeight = weightingFactors.getBaseRow().begin();
            auto endOfWeights = weightingFactors.getBaseRow().end();
            /// Сдвиг для i - ого кластера
            startWeight += signalIter - cluster->outputSignal.begin();
            while(startWeight != endOfWeights) {
                auto weightIter = startWeight;
                for(auto inputIter = startInput; inputIter != endInput; ++inputIter, ++weightIter) {
                    *signalIter += (*inputIter) * (weightIter);
                }
                startWeight += weightingFactors.columns();
                ++signalIter;
            }
        }

    } else {
        /// Здесь относительно нормальная индексация
        int shift = 0;
        for(auto cluster : inputs) {
            for(int i = 0; i != neuronsCount(); ++i) {
                Base *c = outputSignal.data() + i * cluster->neuronsCount() + shift;
                for(int k = 0; k != cluster->neuronsCount(); ++k) {
                    Base *b = cluster->outputSignal.data() + k * threadsCount();
                    Base a = weightingFactors(i, k);
                    for(int j = 0; j != threadsCount(); ++j)
                        c[j] = a * b[j];
                }
            }
            shift += cluster->neuronsCount();
        }
    }
}

template<class Base>
bool AbstractClusterOfNeurons<Base>::processingSumm() {
    for(auto& value : outputSignal)
        value = activationFunction(value);
}

template<class Base>
void AbstractClusterOfNeurons<Base>::readOutputSignals(MatrixOnRow<Base> &output) {
    output.setSize(outputSignal.rows(), outputSignal.columns());
    auto& out = output.getBaseRow();
    const auto& in = outputSignal.getBaseRow();
    std::copy(in.begin(), in.end(), out.begin());
}

template<class Base>
void AbstractClusterOfNeurons<Base>::writeOutputSignals(MatrixOnRow<Base> &input) {
    if(input.rows() != outputSignal.rows())
        throw std::invalid_argument("AbstractClusterOfNeurons::writeOutputSignals(MatrixOnRow&) input.rows != neuronsCount");
    if(input.columns() != outputSignal.columns())
        throw std::invalid_argument("AbstractClusterOfNeurons::writeOutputSignals(MatrixOnRow&) input.columns != threadsCount");
    auto& out = outputSignal.getBaseRow();
    const auto& in = input.getBaseRow();
    std::copy(in.begin(), in.end(), out.begin());
}

template<class Base>
void AbstractClusterOfNeurons<Base>::readShiftWeight(MatrixOnRow<Base> &output) {
    output.setSize(weightingShift.rows(), weightingShift.columns());
    auto& out = output.getBaseRow();
    const auto& in = weightingShift.getBaseRow();
    std::copy(in.begin(), in.end(), out.begin());
}

template<class Base>
void AbstractClusterOfNeurons<Base>::writeShiftWeight(MatrixOnRow<Base> &input) {
    if(input.rows() != weightingShift.rows())
        throw std::invalid_argument("AbstractClusterOfNeurons::writeShiftWeight(MatrixOnRow&) input.rows != neuronsCount");
    if(input.columns() != weightingShift.columns())
        throw std::invalid_argument("AbstractClusterOfNeurons::writeShiftWeight(MatrixOnRow&) input.columns != threadsCount");
    auto& out = weightingShift.getBaseRow();
    const auto& in = input.getBaseRow();
    std::copy(in.begin(), in.end(), out.begin());
}

template<class Base>
void AbstractClusterOfNeurons<Base>::readWeightingFactors(MatrixOnRow<Base> &output) {
    output.setSize(weightingFactors.rows(), weightingFactors.columns());
    auto& out = output.getBaseRow();
    const auto& in = weightingFactors.getBaseRow();
    std::copy(in.begin(), in.end(), out.begin());
}

template<class Base>
void AbstractClusterOfNeurons<Base>::writeWeightingFactors(MatrixOnRow<Base> &input) {
    if(input.rows() != weightingFactors.rows())
        throw std::invalid_argument("AbstractClusterOfNeurons::writeShiftWeight(MatrixOnRow&) input.rows != neuronsCount");
    if(input.columns() != weightingFactors.columns())
        throw std::invalid_argument("AbstractClusterOfNeurons::writeShiftWeight(MatrixOnRow&) input.columns != threadsCount");
    auto& out = weightingFactors.getBaseRow();
    const auto& in = input.getBaseRow();
    std::copy(in.begin(), in.end(), out.begin());
}

template<class Base>
Base AbstractClusterOfNeurons<Base>::getInitValue() const {
    QRandomGenerator *generator = QRandomGenerator::system();
    return generator->generateDouble();
}

template<class Base>
void AbstractClusterOfNeurons<Base>::neuronsAppended(AbstractClusterOfNeurons *updated, int index, int count) {
    if(!inputs.contains(updated)) return;
    for(auto cluster : inputs) {
        if(cluster == updated) break;
        index += cluster->neuronsCount();
    }
    weightingFactors.insertColumns(index, count);
    int end = index + count;
    for(int i = 0; i != weightingFactors.rows(); ++i)
        for(int j = index; j != end; ++j)
            weightingFactors(i, j) = getInitValue();
}

template<class Base>
void AbstractClusterOfNeurons<Base>::neuronsDeleted(AbstractClusterOfNeurons *updated, int first, int last) {
    if(!inputs.contains(updated)) return;
    for(auto cluster : inputs) {
        if(cluster == updated) break;
        first += cluster->neuronsCount();
        last += cluster->neuronsCount();
    }
    weightingFactors.removeColumns(first, last);
}

#endif // CLUSTEROFNEURONS_H
