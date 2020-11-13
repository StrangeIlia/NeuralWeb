#ifndef CLUSTEROFNEURONS_H
#define CLUSTEROFNEURONS_H

#include <QHash>

#include "Signal.h"
#include "RelatationWeights.h"
#include "AbstractActivation.h"

class SimpleClusterOfNeurons;

typedef double                              BaseValueType;
typedef MatrixOnRow<BaseValueType>          Matrix;
typedef SimpleClusterOfNeurons             *Cluster;
typedef QList<Cluster>                      ClusterList;
typedef AbstractActivation                 *AbstractActivationPtr;

class SimpleClusterOfNeurons
{
public:
    SimpleClusterOfNeurons(int neuronsCount = 0);

    virtual ~SimpleClusterOfNeurons();

    /// Возвращает список входных кластеров
    ClusterList inputClusters() const;

    /// Возвращает список выходных кластеров
    ClusterList outputClusters() const;

    /// Возвращает веса связей
    RelatationWeights &weights();
    const RelatationWeights &weights() const;

    /// Возвращает выходной сигнал
    Signal &outputSignal();
    const Signal &outputSignal() const;

    /// Число нейронов в кластере
    int neuronsCount() const;

    /// Число одновременно обрабатываемых входных сигналов
    int threadsCount() const;

    /// Возвращает число входных сигналов
    int inputSignalCount() const;

    /// Вставляет count нейронов на index место
    void insertNeurons(int index, int count);

    /// Удаляет нейроны [first; last]
    void removeNeurons(int first, int last);

    /// Устанавливает число нейронов в кластере
    void setNueronsCount(int count);

    /// Добавить входной кластер
    bool addInput(SimpleClusterOfNeurons *cluster);

    /// Добавить выходной кластер
    bool addOutput(SimpleClusterOfNeurons *cluster);

    /// Удалить входной кластер
    bool removeInput(SimpleClusterOfNeurons *cluster);

    /// Удалить выходной кластер
    bool removeOutput(SimpleClusterOfNeurons *cluster);

    /// Создать связь между кластерами
    static bool connect(SimpleClusterOfNeurons *in, SimpleClusterOfNeurons *out);

    /// Удалить связь между кластерами
    static bool disconnect(SimpleClusterOfNeurons *in, SimpleClusterOfNeurons *out);

    /// Удаляет связи со всеми кластерами
    void clearLinks();

    /// Расчитывает входной сигнал
    virtual void summation();

    /// Преобразует суммарный входной сигнал в выходной
    void activation();

    /// Функция корректировки весовых коэффициентов
    QHash<Cluster, Signal> correction(Signal error);

    void _neuronsAppended_(SimpleClusterOfNeurons *updated, int index, int count);

    void _neuronsDeleted_(SimpleClusterOfNeurons *updated, int first, int last);

    ClusterList __inputs__;
    ClusterList __outputs__;
    Signal __outputSignal__;
    RelatationWeights __weights__;
    AbstractActivationPtr __activationFunction__ = nullptr;
};

#endif // CLUSTEROFNEURONS_H
