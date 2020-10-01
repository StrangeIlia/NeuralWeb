#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

#include <QSet>
#include "AbstractClusterOfNeurons.hpp"

template<class Base>
class AbstractNeuralNetwork {
public:
    typedef AbstractClusterOfNeurons<Base> *Cluster;
    typedef QSet<Cluster> ClusterSet;
    typedef QList<Cluster> ClusterQueue;

protected:
    ClusterSet _inputsClusters;
    ClusterSet _outputClusters;
    ClusterSet _hiddenClusters;
    ClusterQueue clusterSequence;

public:
    AbstractNeuralNetwork();

    /// Добавляет кластер в сеть
    /// Его размещение наличием входных и выходных кластерова
    virtual bool addCluster(Cluster cluster);

    /// Удаление кластера из сети
    virtual bool removeCluster(Cluster cluster);

    /// Возвращает истина, если данный кластер есть в сети
    bool contains(Cluster cluster);

    /// Обновления списка обхода кластеров
    virtual void updateСlusterSequence();

    /// Производит обновление сети
    /// !!! Внимание !!! Данная реализация не поддерживает рекуррентные связи
    virtual void updateNetwork();

    /// Получить множество входных кластеров
    const ClusterSet& inputsClusters() const;

    /// Получить множество выходных кластеров
    const ClusterSet& outputClusters() const;

    /// Получить множество скрытых кластеров
    const ClusterSet& hiddenClusters() const;
};

template<class Base>
AbstractNeuralNetwork<Base>::AbstractNeuralNetwork() {

}

template<class Base>
bool AbstractNeuralNetwork<Base>::addCluster(Cluster cluster) {
    if(contains(cluster)) return false;
    bool isInput = true;
    for(auto inputCluster : cluster->inputClusters()) {
        if(contains(inputCluster)) {
            isInput = false;
            if(_outputClusters.contains(inputCluster)) {
                _hiddenClusters.insert(inputCluster);
                _outputClusters.remove(inputCluster);
            }
        }
    }
    if(isInput) _inputsClusters.insert(cluster);

    bool isOutput = false;
    for(auto outputCluster : cluster->outputClusters()) {
        if(contains(outputCluster)) {
            isOutput = false;
            if(_inputsClusters.contains(outputCluster)) {
                _hiddenClusters.insert(outputCluster);
                _inputsClusters.remove(outputCluster);
            }
        }
    }
    if(isOutput) _outputClusters.insert(cluster);

    if(!isInput && !isOutput) _hiddenClusters.insert(cluster);

    return true;
}

template<class Base>
bool AbstractNeuralNetwork<Base>::removeCluster(Cluster cluster) {
    if(!contains(cluster)) return false;

}

template<class Base>
bool AbstractNeuralNetwork<Base>::contains(Cluster cluster) {
    bool cont = _hiddenClusters.contains(cluster);
    if(cont) return true;
    cont = _inputsClusters.contains(cluster);
    if(cont) return true;
    cont = _outputClusters.contains(cluster);
    return cont;
}

template<class Base>
void AbstractNeuralNetwork<Base>::updateСlusterSequence() {

}

template<class Base>
void AbstractNeuralNetwork<Base>::updateNetwork() {
    if(clusterSequence.empty()) updateСlusterSequence();
    for(auto cluster : clusterSequence) {
        cluster->calculateSumm();
        cluster->processingSumm();
    }
}

template<class Base>
const typename AbstractNeuralNetwork<Base>::ClusterSet& AbstractNeuralNetwork<Base>::inputsClusters() const {
    return _inputsClusters;
}

template<class Base>
const typename AbstractNeuralNetwork<Base>::ClusterSet& AbstractNeuralNetwork<Base>::outputClusters() const {
    return _outputClusters;
}

template<class Base>
const typename AbstractNeuralNetwork<Base>::ClusterSet& AbstractNeuralNetwork<Base>::hiddenClusters() const {
    return _hiddenClusters;
}

#endif // NEURALNETWORK_H
