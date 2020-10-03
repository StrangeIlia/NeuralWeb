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
    QHash<Cluster, Weights> _lastWeights;
    QHash<Cluster, MatrixOnRow<Base>> _summSignal;

public:
    void updateNetwork() override {
        _summSignal.clear();
        for(auto cluster : this->hiddenClusters()) {
           MatrixOnRow<Base> reserve;
           cluster->readOutputSignals(reserve);
           _summSignal.insert(cluster, reserve);
        }
        this->SimpleNeuralNetwork<Base>::updateNetwork();
    }

    void training() override {
        _lastWeights.clear();
        for(auto cluster : this->hiddenClusters()) {
           Weights reserve;
           cluster->readShiftWeight(reserve.shift);
           cluster->readWeightingFactors(reserve.factors);
           _lastWeights.insert(cluster, reserve);
        }
        this->SimpleNeuralNetwork<Base>::training();
    }

    QHash<Cluster, Weights> lastWeights() const {
        return _lastWeights;
    }

    QHash<Cluster, MatrixOnRow<Base>> summSignal() const {
        return _summSignal;
    }
};

#endif // DEBUGNEURALNETWORK_H
