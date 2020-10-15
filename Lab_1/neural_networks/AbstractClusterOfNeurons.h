#ifndef CLUSTEROFNEURONS_H
#define CLUSTEROFNEURONS_H

#include <QList>
#include <stdexcept>

#include "math/matrix/MatrixOnRow.hpp"

class AbstractClusterOfNeurons;

typedef double                              BaseValueType;
typedef MatrixOnRow<BaseValueType>          Matrix;
typedef AbstractClusterOfNeurons           *Cluster;
typedef QList<Cluster>                      ClusterList;

class AbstractClusterOfNeurons
{
protected:
    Matrix _outputSignal;
    Matrix _weightingShift;
    Matrix _weightingFactors;
    ClusterList _inputs;
    ClusterList _outputs;

public:
    AbstractClusterOfNeurons(int neuronsCount);

    virtual ~AbstractClusterOfNeurons();

protected:
    /// Функия активации
    virtual BaseValueType activate(BaseValueType) = 0;
    /// Функция корректирования весых коэффициентов
    virtual BaseValueType correct(BaseValueType input, BaseValueType ouput) = 0;

public:
    inline const Matrix& outputSignal() const;
    inline void setOutputSignal(const Matrix &matrix);

    inline const Matrix& weightingShift() const;
    inline void setWeightingShift(const Matrix &matrix);

    inline const Matrix& weightingFactors() const;
    inline void setWeightingFactors(const Matrix &matrix);

    /// Возвращает список входных кластеров
    inline ClusterList inputClusters() const;

    /// Возвращает список выходных кластеров
    inline ClusterList outputClusters() const;

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
    void setThreadsCount(int count);

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

    /// Заполняет выходные сигналы значением по умолчанию типа BaseValueType
    inline void clearOutputSignal(BaseValueType value = BaseValueType());

    /// Заполняет выходные сигналы сигналом сдвига
    inline void initOutputSignal();

    /// Расчитывает входной сигнал
    virtual void summation();

    /// Преобразует суммарный входной сигнал в выходной
    virtual bool activation();

    /// Функция корректировки весовых коэффициентов
    virtual void correction();

protected:
    virtual BaseValueType getInitValue() const;

    void neuronsAppended(AbstractClusterOfNeurons *updated, int index, int count);

    void neuronsDeleted(AbstractClusterOfNeurons *updated, int first, int last);
};

#endif // CLUSTEROFNEURONS_H
