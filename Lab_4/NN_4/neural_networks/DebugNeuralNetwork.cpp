#include "DebugNeuralNetwork.h"

DebugNeuralNetwork::DebugNeuralNetwork(bool saveSummSignal, bool saveWeigth) {
    _saveSummSignal = saveSummSignal;
    _saveWeigth = saveWeigth;
}

void DebugNeuralNetwork::updateNetwork() {
    if(_saveSummSignal) {
        _summSignal.clear();

        if(this->clusterSequence.empty()) this->updateClusterSequence();
        for(auto cluster : this->clusterSequence) {
            cluster->summation();
            _summSignal.insert(cluster, cluster->outputSignal());
            cluster->activation();
        }
    } else this->SimpleNeuralNetwork::updateNetwork();
}

void DebugNeuralNetwork::training(QHash<Cluster, Signal> errors) {
    if(_saveWeigth) {
        _lastWeights.clear();

        if(clusterSequence.empty()) updateClusterSequence();
        {
            auto reqOutputs = QSet<Cluster>(clusterSequence.begin(), clusterSequence.end());
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
            _lastWeights.insert(cluster, cluster->weights());
            auto hash = cluster->correction(signal);
            for(auto iter = hash.begin(); iter != hash.end(); ++iter)
                errors.insert(iter.key(), std::move(iter.value()));
            ++reverseIter;
        }
    }
    else this->SimpleNeuralNetwork::training(errors);
}

QHash<Cluster, RelatationWeights> DebugNeuralNetwork::lastWeights() const {
    return _lastWeights;
}

QHash<Cluster, Signal> DebugNeuralNetwork::summSignal() const {
    return _summSignal;
}

bool DebugNeuralNetwork::saveSummSignal() const {
    return  _saveSummSignal;
}

bool DebugNeuralNetwork::saveWeigth() const {
    return  _saveWeigth;
}

void DebugNeuralNetwork::setSaveSummSignal(bool value) {
    _saveSummSignal = value;
}

void DebugNeuralNetwork:: setSaveWeigth(bool value) {
    _saveWeigth = value;
}
