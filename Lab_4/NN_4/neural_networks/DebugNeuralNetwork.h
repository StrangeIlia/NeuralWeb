#ifndef DEBUGNEURALNETWORK_H
#define DEBUGNEURALNETWORK_H

#include "SimpleNeuralNetwork.h"

class DebugNeuralNetwork : public SimpleNeuralNetwork {
protected:
    bool _saveSummSignal = true;
    bool _saveWeigth = true;
    QHash<Cluster, RelatationWeights> _lastWeights;
    QHash<Cluster, Signal> _summSignal;

public:
    DebugNeuralNetwork(bool saveSummSignal = true, bool saveWeigth = true);

    void updateNetwork() override;
    void training(QHash<Cluster, Signal> errors) override;

    QHash<Cluster, RelatationWeights> lastWeights() const;
    QHash<Cluster, Signal> summSignal() const;
    bool saveSummSignal() const;
    bool saveWeigth() const;

    void setSaveSummSignal(bool value);
    void setSaveWeigth(bool value);
};

#endif // DEBUGNEURALNETWORK_H
