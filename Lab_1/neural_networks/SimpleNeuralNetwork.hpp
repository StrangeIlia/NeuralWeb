#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

#include <QSet>
#include "AbstractClusterOfNeurons.hpp"

template<class Base>
class SimpleNeuralNetwork {
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
    SimpleNeuralNetwork();

    /// Добавляет кластер в сеть
    /// Его размещение наличием входных и выходных кластеров
    virtual bool addCluster(Cluster cluster);

    /// Удаление кластера из сети
    virtual bool removeCluster(Cluster cluster);

    /// Возвращает истина, если данный кластер есть в сети
    bool contains(Cluster cluster);

    /// Обновления списка обхода кластеров
    /// !!! Внимание !!! Данная реализация не поддерживает обход нейрона более чем один раз
    virtual void updateСlusterSequence();

    /// Обновление ролей кластерова
    virtual void updateClusterRoles();

    /// Производит обновление сети
    /// При в обновлении участвуют только нейроны в скрытом и выходном слое
    virtual void updateNetwork();

    /// Очищает нейронную сеть
    void clear();

    /// Получить множество входных кластеров
    const ClusterSet& inputsClusters() const;

    /// Получить множество выходных кластеров
    const ClusterSet& outputClusters() const;

    /// Получить множество скрытых кластеров
    const ClusterSet& hiddenClusters() const;
};

template<class Base>
SimpleNeuralNetwork<Base>::SimpleNeuralNetwork() {

}

template<class Base>
bool SimpleNeuralNetwork<Base>::addCluster(Cluster cluster) {
    if(contains(cluster)) return false;
    clusterSequence.clear();

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
    if(isInput) {
        _inputsClusters.insert(cluster);
        return true;
    }

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
    if(isOutput) {
        _outputClusters.insert(cluster);
        return true;
    }

    _hiddenClusters.insert(cluster);

    return true;
}

template<class Base>
bool SimpleNeuralNetwork<Base>::removeCluster(Cluster cluster) {
    if(!contains(cluster)) return false;
    clusterSequence.clear();

    QSet<Cluster> dependedClusters;
    if(_inputsClusters.contains(cluster)) {
        for(auto outputCluster : cluster->inputClusters())
            if(contains(outputCluster))
                dependedClusters.insert(outputCluster);
        _inputsClusters.remove(cluster);

    }
    else if(_outputClusters.contains(cluster)) {
        for(auto inputCluster : cluster->outputClusters())
            if(contains(inputCluster))
                dependedClusters.insert(inputCluster);
        _outputClusters.remove(cluster);

    } else {
        _hiddenClusters.remove(cluster);

    }

    for(auto depCluster : dependedClusters) {
        _inputsClusters.remove(depCluster);
        _hiddenClusters.remove(depCluster);
        _outputClusters.remove(depCluster);
        addCluster(depCluster);
    }

    return true;
}

template<class Base>
bool SimpleNeuralNetwork<Base>::contains(Cluster cluster) {
    bool cont = _hiddenClusters.contains(cluster);
    if(cont) return true;
    cont = _inputsClusters.contains(cluster);
    if(cont) return true;
    cont = _outputClusters.contains(cluster);
    return cont;
}

template<class Base>
void SimpleNeuralNetwork<Base>::updateСlusterSequence() {
    QSet<Cluster> traversedNeurons = _inputsClusters;
    QSet<Cluster> nextLayer, predLayer = _inputsClusters;
    while(predLayer.size()) {
        for(auto inputCluster : predLayer) {
            for(auto outputCluster : inputCluster->inputClusters()) {
                if(_hiddenClusters.contains(outputCluster)) {
                    if(!traversedNeurons.contains(outputCluster)) {
                        nextLayer.insert(outputCluster);
                        traversedNeurons.insert(outputCluster);
                    }
                }
            }
        }
        for(auto cluster : nextLayer) clusterSequence.insert(cluster);
        predLayer = std::move(nextLayer);
    }
    for(auto cluster : _outputClusters) clusterSequence.insert(cluster);
}

template<class Base>
void SimpleNeuralNetwork<Base>::updateClusterRoles() {
    QSet<Cluster> tmp = _hiddenClusters;
    tmp.intersect(_inputsClusters);
    tmp.intersect(_outputClusters);
    clear();
    for(auto cluster : tmp)
        addCluster(cluster);
}

template<class Base>
void SimpleNeuralNetwork<Base>::updateNetwork() {
    if(clusterSequence.empty()) updateСlusterSequence();
    for(auto cluster : clusterSequence) {
        cluster->calculateSumm();
        cluster->processingSumm();
    }
}

template<class Base>
void SimpleNeuralNetwork<Base>::clear() {
    _inputsClusters.clear();
    _outputClusters.clear();
    _hiddenClusters.clear();
    clusterSequence.clear();
}

template<class Base>
const typename SimpleNeuralNetwork<Base>::ClusterSet& SimpleNeuralNetwork<Base>::inputsClusters() const {
    return _inputsClusters;
}

template<class Base>
const typename SimpleNeuralNetwork<Base>::ClusterSet& SimpleNeuralNetwork<Base>::outputClusters() const {
    return _outputClusters;
}

template<class Base>
const typename SimpleNeuralNetwork<Base>::ClusterSet& SimpleNeuralNetwork<Base>::hiddenClusters() const {
    return _hiddenClusters;
}

#endif // NEURALNETWORK_H
