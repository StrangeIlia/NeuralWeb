#include "SimpleNeuralNetworkTrainer.h"

SimpleNeuralNetworkTrainer::SimpleNeuralNetworkTrainer(SimpleNeuralNetwork *network, bool memoryControl)
    : _networkLearn(network)
    , _memoryControl(memoryControl) {

}

SimpleNeuralNetworkTrainer::~SimpleNeuralNetworkTrainer() {
    if(_memoryControl) {
        for(auto set : _learningSet) {
            delete set;
        }
    }
    _learningSet.clear();
}

void SimpleNeuralNetworkTrainer::addTrainingSet(TrainingSet *set) {
    for(auto iter = set->input.begin(); iter != set->input.end(); ++iter) {
        if(!_networkLearn->inputClusters().contains(iter.key()))
            return;
    }
    for(auto iter = set->output.begin(); iter != set->output.end(); ++iter) {
        if(!_networkLearn->outputClusters().contains(iter.key()))
            return;
    }
    if(!_learningSet.contains(set))
        _learningSet.append(set);
}

void SimpleNeuralNetworkTrainer::removeTrainingSet(TrainingSet *set) {
   _learningSet.removeOne(set);
}

bool SimpleNeuralNetworkTrainer::memoryControl() const {
    return _memoryControl;
}

void SimpleNeuralNetworkTrainer::setMemoryControl(bool value) {
    _memoryControl = value;
}

QList<SimpleNeuralNetworkTrainer::TrainingSet*> SimpleNeuralNetworkTrainer::learningSet() const {
    return _learningSet;
}

SimpleNeuralNetwork *SimpleNeuralNetworkTrainer::networkLearn() const {
    return _networkLearn;
}

void SimpleNeuralNetworkTrainer::setNetworkLearn(SimpleNeuralNetwork *network) {
    _networkLearn = network;
    _learningSet.clear();
}

void SimpleNeuralNetworkTrainer::removeSet() {
    _learningSet.clear();
}

void SimpleNeuralNetworkTrainer::removeInvalidSet() {
    auto set = _learningSet;
    _learningSet.clear();
    for(auto value : set) addTrainingSet(value);
}

int SimpleNeuralNetworkTrainer::training(BaseValueType eps, int maxIteration) {
    int iterationNumber = 0;
    while(iterationNumber != maxIteration) {
        QList<TrainingSet*> mistakes;

        for(auto trainingSet : _learningSet) {
            const auto &input = trainingSet->input;
            for(auto iter = input.begin(); iter != input.end(); ++iter) {
                iter.key()->outputSignal() = iter.value();
            }
            _networkLearn->updateNetwork();
            const auto &output = trainingSet->output;
            bool hasChanged = false;
            for(auto iter = output.begin(); iter != output.end(); ++iter) {
                const auto &calculated = iter.key()->outputSignal();
                const auto &requiredSignal = iter.value();
                for(int i = 0; i != calculated.size(); ++i){
                    for(int th = 0; th != calculated.threadCount(); ++th) {
                        if(std::abs(requiredSignal.signal(i, th) - calculated.signal(i, th)) > eps) {
                            mistakes.append(trainingSet);
                            hasChanged = true;
                            break;
                        }
                        if(hasChanged) break;
                    }
                }
            }
        }

        if(mistakes.empty()) break;

        for(auto trainingSet : mistakes) {
            const auto &input = trainingSet->input;
            for(auto iter = input.begin(); iter != input.end(); ++iter) {
                iter.key()->outputSignal() = iter.value();
            }
            _networkLearn->updateNetwork();

            QHash<Cluster, Signal> errors;
            const auto &output = trainingSet->output;
            for(auto iter = output.begin(); iter != output.end(); ++iter) {
                auto error = iter.value();
                error -= iter.key()->outputSignal();
                errors.insert(iter.key(), std::move(error));
            }
            _networkLearn->training(errors);
        }

        ++iterationNumber;
    }

    return iterationNumber;
}
