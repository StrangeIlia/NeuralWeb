#ifndef DEBUGNEURALNETWORK_H
#define DEBUGNEURALNETWORK_H

#include "SimpleNeuralNetwork.hpp"

template<class Base>
class DebugNeuralNetwork : public SimpleNeuralNetwork<Base> {
public:
    typedef AbstractClusterOfNeurons<Base> *Cluster;
    struct Weights {
        MatrixOnRow<Base> shift;
        MatrixOnRow<Base> factors;
    };

protected:
    bool _saveSummSignal = true;
    bool _saveWeigth = true;
    QHash<Cluster, Weights> _lastWeights;
    QHash<Cluster, MatrixOnRow<Base>> _summSignal;

public:
    DebugNeuralNetwork(bool saveSummSignal = true, bool saveWeigth = true) {
        _saveSummSignal = saveSummSignal;
        _saveWeigth = saveWeigth;
    }

    void updateNetwork() override {
        if(_saveSummSignal) {
            _summSignal.clear();

            if(this->clusterSequence.empty()) this->updateClusterSequence();
            for(auto cluster : this->clusterSequence) {
                cluster->calculateSumm();

                MatrixOnRow<Base> reserve;
                cluster->readOutputSignals(reserve);
                _summSignal.insert(cluster, reserve);

                cluster->processingSumm();
            }
        }
    }

    void training() override {
        if(_saveWeigth) {
            _lastWeights.clear();

            if(this->clusterSequence.empty()) this->updateClusterSequence();
            auto reverseIter = this->clusterSequence.rbegin();
            auto reverseEnd = this->clusterSequence.rend();
            while (reverseIter != reverseEnd) {
                auto cluster = *reverseIter;
                Weights reserve;
                cluster->readShiftWeight(reserve.shift);
                cluster->readWeightingFactors(reserve.factors);
                _lastWeights.insert(cluster, reserve);
                cluster->correctionFunction();
                ++reverseIter;
            }
        }
        this->SimpleNeuralNetwork<Base>::training();
    }

    QHash<Cluster, Weights> lastWeights() const {
        return _lastWeights;
    }

    QHash<Cluster, MatrixOnRow<Base>> summSignal() const {
        return _summSignal;
    }

    bool saveSummSignal() const { return  _saveSummSignal; }
    bool saveWeigth() const { return  _saveWeigth; }

    void setSaveSummSignal(bool value) { _saveSummSignal = value; }
    void setSaveWeigth(bool value) { _saveWeigth = value; }
};

#endif // DEBUGNEURALNETWORK_H
