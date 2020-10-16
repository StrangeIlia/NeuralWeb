#ifndef NEURALNETWORKTRAINER_H
#define NEURALNETWORKTRAINER_H

#include <limits>

#include <QHash>

#include "SimpleNeuralNetwork.h"

class NeuralNetworkTrainer
{
public:
    struct DataPair {
        Cluster      cluster;
        Matrix       signal;
    };

    typedef QHash<Cluster, Matrix>  Signal;

    struct TrainingSet {
        Signal input;
        Signal output;
    };
private:
    QList<TrainingSet*>     _learningSet;
    SimpleNeuralNetwork    *_networkLearn;
    bool                    _memoryControl;
public:
    NeuralNetworkTrainer(SimpleNeuralNetwork *network, bool memoryControl = false);
    ~NeuralNetworkTrainer();

    void addTrainingSet(TrainingSet *set);
    void removeTrainingSet(TrainingSet *set);

    bool memoryControl() const;
    void setMemoryControl(bool value);

    QList<TrainingSet*> learningSet() const;

    SimpleNeuralNetwork *networkLearn() const;
    void setNetworkLearn(SimpleNeuralNetwork *network);

    void removeSet();
    void removeInvalidSet();
    void training(
            BaseValueType learningFactor = 1,
            BaseValueType eps = std::numeric_limits<BaseValueType>::epsilon(),
            int maxIteration = -1);
};

#endif // NEURALNETWORKTRAINER_H
