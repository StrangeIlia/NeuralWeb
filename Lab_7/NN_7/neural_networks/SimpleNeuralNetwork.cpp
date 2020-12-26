#include "SimpleNeuralNetwork.h"


SimpleNeuralNetwork::SimpleNeuralNetwork() {

}


bool SimpleNeuralNetwork::addCluster(Cluster cluster) {
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
        QSet<Cluster> dependedClusters;
        for(auto outputCluster : cluster->outputClusters()) {
            if(_inputsClusters.contains(outputCluster)) {
                _inputsClusters.remove(outputCluster);
                dependedClusters.insert(outputCluster);
            }
        }
        for(auto depCluster : dependedClusters)
            addCluster(depCluster);

        return true;
    }

    bool isOutput = true;
    QSet<Cluster> dependedClusters;
    for(auto outputCluster : cluster->outputClusters()) {
        if(contains(outputCluster)) {
            isOutput = false;
            if(_inputsClusters.contains(outputCluster)) {
                _inputsClusters.remove(outputCluster);
                dependedClusters.insert(outputCluster);
            }
        }
    }
    if(isOutput) {
        _outputClusters.insert(cluster);
        for(auto depCluster : dependedClusters)
            addCluster(depCluster);
        return true;
    }

    _hiddenClusters.insert(cluster);

    return true;
}


bool SimpleNeuralNetwork::removeCluster(Cluster cluster) {
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


bool SimpleNeuralNetwork::contains(Cluster cluster) {
    bool cont = _hiddenClusters.contains(cluster);
    if(cont) return true;
    cont = _inputsClusters.contains(cluster);
    if(cont) return true;
    cont = _outputClusters.contains(cluster);
    return cont;
}


void SimpleNeuralNetwork::updateClusterSequence() {
    QSet<Cluster> traversedNeurons = _inputsClusters;
    QSet<Cluster> nextLayer, predLayer = _inputsClusters;
    while(predLayer.size()) {
        for(auto inputCluster : predLayer) {
            for(auto outputCluster : inputCluster->outputClusters()) {
                if(_hiddenClusters.contains(outputCluster)) {
                    if(!traversedNeurons.contains(outputCluster)) {
                        nextLayer.insert(outputCluster);
                        traversedNeurons.insert(outputCluster);
                    }
                }
            }
        }

        for(auto cluster : nextLayer)
            clusterSequence.push_back(cluster);
        predLayer = std::move(nextLayer);
    }
    for(auto cluster : _outputClusters)
        clusterSequence.push_back(cluster);
}


void SimpleNeuralNetwork::updateClusterRoles() {
    QSet<Cluster> tmp = _hiddenClusters;
    tmp.unite(_inputsClusters);
    tmp.unite(_outputClusters);
    clear();
    for(auto cluster : tmp)
        addCluster(cluster);
}


void SimpleNeuralNetwork::updateNetwork() {
    if(clusterSequence.empty()) updateClusterSequence();
    for(auto cluster : clusterSequence) {
        cluster->summation();
        cluster->activation();
    }
}


void SimpleNeuralNetwork::training(QHash<Cluster, Signal> errors) {
    if(clusterSequence.empty()) updateClusterSequence();
    {
        auto reqOutputs = QSet<Cluster>(_outputClusters.begin(), _outputClusters.end());
        auto listOutpus = errors.keys();
        auto outputs = QSet<Cluster>(listOutpus.begin(), listOutpus.end());
        if(reqOutputs != outputs)
            throw std::exception();
    }

    auto reverseIter = clusterSequence.rbegin();
    auto reverseEnd = clusterSequence.rend();
    while (reverseIter != reverseEnd) {
        auto cluster = *reverseIter;
        auto signal = errors.take(cluster);
        auto hash = cluster->correction(signal);
        for(auto iter = hash.begin(); iter != hash.end(); ++iter) {
            if(errors.contains(iter.key())) {
                auto &signal = errors[iter.key()];
                signal += iter.value();
            } else {
                errors.insert(iter.key(), std::move(iter.value()));
            }
        }

        ++reverseIter;
    }
}

void SimpleNeuralNetwork::clear() {
    _inputsClusters.clear();
    _outputClusters.clear();
    _hiddenClusters.clear();
    clusterSequence.clear();
}


const ClusterSet& SimpleNeuralNetwork::inputClusters() const {
    return _inputsClusters;
}


const ClusterSet& SimpleNeuralNetwork::outputClusters() const {
    return _outputClusters;
}


const ClusterSet& SimpleNeuralNetwork::hiddenClusters() const {
    return _hiddenClusters;
}
