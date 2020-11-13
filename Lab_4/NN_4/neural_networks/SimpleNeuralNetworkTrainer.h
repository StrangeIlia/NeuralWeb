#ifndef NEURALNETWORKTRAINER_H
#define NEURALNETWORKTRAINER_H

#include <limits>

#include <QHash>
#include <QString>

#include "SimpleNeuralNetwork.h"

class SimpleNeuralNetworkTrainer
{
public:
    struct DataPair {
        Cluster      cluster;
        Matrix       signal;
    };

    typedef QHash<Cluster, Signal>  Values;

    struct TrainingSet {
        Values input;
        Values output;
    };
protected:
    QList<TrainingSet*>     _learningSet;
    SimpleNeuralNetwork    *_networkLearn;
    bool                    _memoryControl;
public:
    SimpleNeuralNetworkTrainer(SimpleNeuralNetwork *network, bool memoryControl = false);
    ~SimpleNeuralNetworkTrainer();

    void addTrainingSet(TrainingSet *set);
    void removeTrainingSet(TrainingSet *set);

    bool memoryControl() const;
    void setMemoryControl(bool value);

    QList<TrainingSet*> learningSet() const;

    SimpleNeuralNetwork *networkLearn() const;
    void setNetworkLearn(SimpleNeuralNetwork *network);

    void removeSet();
    void removeInvalidSet();
    virtual int training(
            BaseValueType eps = std::numeric_limits<BaseValueType>::epsilon(),
            int maxIteration = -1);
};

#endif // NEURALNETWORKTRAINER_H
