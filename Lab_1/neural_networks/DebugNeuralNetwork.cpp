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

void DebugNeuralNetwork::training(BaseValueType learningFactor) {
    if(_saveWeigth) {
        _lastWeights.clear();

        if(this->clusterSequence.empty()) this->updateClusterSequence();
        auto reverseIter = this->clusterSequence.rbegin();
        auto reverseEnd = this->clusterSequence.rend();
        while (reverseIter != reverseEnd) {
            auto cluster = *reverseIter;
            Weights reserve;
            reserve.shift = cluster->weightingShift();
            reserve.factors = cluster->weightingFactors();
            _lastWeights.insert(cluster, reserve);
            cluster->correction(learningFactor);
            ++reverseIter;
        }
    }
    this->SimpleNeuralNetwork::training(learningFactor);
}

QHash<Cluster, DebugNeuralNetwork::Weights> DebugNeuralNetwork::lastWeights() const {
    return _lastWeights;
}

QHash<Cluster, Matrix> DebugNeuralNetwork::summSignal() const {
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
