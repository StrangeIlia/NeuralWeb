#ifndef DEBUGNEURALNETWORK_H
#define DEBUGNEURALNETWORK_H

#include "SimpleNeuralNetwork.h"

class DebugNeuralNetwork : public SimpleNeuralNetwork {
public:
    struct Weights {
        Matrix shift;
        Matrix factors;
    };

protected:
    bool _saveSummSignal = true;
    bool _saveWeigth = true;
    QHash<Cluster, Weights> _lastWeights;
    QHash<Cluster, Matrix> _summSignal;

public:
    DebugNeuralNetwork(bool saveSummSignal = true, bool saveWeigth = true);

    void updateNetwork() override;
    void training(BaseValueType learningFactor = 1) override;

    QHash<Cluster, Weights> lastWeights() const;
    QHash<Cluster, Matrix> summSignal() const;
    bool saveSummSignal() const;
    bool saveWeigth() const;

    void setSaveSummSignal(bool value);
    void setSaveWeigth(bool value);
};

#endif // DEBUGNEURALNETWORK_H
