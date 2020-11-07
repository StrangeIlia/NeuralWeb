#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

#include <QSet>
#include "AbstractClusterOfNeurons.h"

typedef QSet<Cluster>   ClusterSet;
typedef QList<Cluster>  ClusterQueue;

class SimpleNeuralNetwork {
protected:
    ClusterSet _inputsClusters;
    ClusterSet _outputClusters;
    ClusterSet _hiddenClusters;
    ClusterQueue clusterSequence;

public:
    SimpleNeuralNetwork();
    virtual ~SimpleNeuralNetwork() {}

    /// Добавляет кластер в сеть
    /// Его размещение наличием входных и выходных кластеров
    virtual bool addCluster(Cluster cluster);

    /// Удаление кластера из сети
    virtual bool removeCluster(Cluster cluster);

    /// Возвращает истина, если данный кластер есть в сети
    bool contains(Cluster cluster);

    /// Обновления списка обхода кластеров
    /// !!! Внимание !!! Данная реализация не поддерживает обход нейрона более чем один раз
    virtual void updateClusterSequence();

    /// Обновление ролей кластерова
    virtual void updateClusterRoles();

    /// Производит обновление сети
    /// При в обновлении участвуют только нейроны в скрытом и выходном слое
    virtual void updateNetwork();

    /// Производит обучение сети
    /// В обучении участвуют только нейроны в скрытом и выходном слое
    virtual void training(BaseValueType learningFactor = 1.0);

    /// Очищает нейронную сеть
    void clear();

    /// Получить множество входных кластеров
    const ClusterSet& inputClusters() const;

    /// Получить множество выходных кластеров
    const ClusterSet& outputClusters() const;

    /// Получить множество скрытых кластеров
    const ClusterSet& hiddenClusters() const;
};

#endif // NEURALNETWORK_H
