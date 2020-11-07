#ifndef ENCHANCEDNEURALNETWORKTRAINER_H
#define ENCHANCEDNEURALNETWORKTRAINER_H

#include "DebugNeuralNetwork.h"
#include "NeuralNetworkTrainer.h"

class EnchancedNeuralNetworkTrainer : public NeuralNetworkTrainer
{
public:
    EnchancedNeuralNetworkTrainer(SimpleNeuralNetwork *network, bool memoryControl = false);

    void training(
            BaseValueType learningFactor = 1,
            BaseValueType eps = std::numeric_limits<BaseValueType>::epsilon(),
            int maxIteration = -1) override;
};

#endif // ENCHANCEDNEURALNETWORKTRAINER_H
