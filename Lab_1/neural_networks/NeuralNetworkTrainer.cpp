#include "NeuralNetworkTrainer.h"

NeuralNetworkTrainer::NeuralNetworkTrainer(SimpleNeuralNetwork *network, bool memoryControl)
    : _networkLearn(network)
    , _memoryControl(memoryControl) {

}

NeuralNetworkTrainer::~NeuralNetworkTrainer() {
    if(_memoryControl) {
        for(auto set : _learningSet) {
            delete set;
        }
    }
    _learningSet.clear();
}

void NeuralNetworkTrainer::addTrainingSet(TrainingSet *set) {
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

void NeuralNetworkTrainer::removeTrainingSet(TrainingSet *set) {
   _learningSet.removeOne(set);
}

bool NeuralNetworkTrainer::memoryControl() const {
    return _memoryControl;
}

void NeuralNetworkTrainer::setMemoryControl(bool value) {
    _memoryControl = value;
}

QList<NeuralNetworkTrainer::TrainingSet*> NeuralNetworkTrainer::learningSet() const {
    return _learningSet;
}

SimpleNeuralNetwork *NeuralNetworkTrainer::networkLearn() const {
    return _networkLearn;
}

void NeuralNetworkTrainer::setNetworkLearn(SimpleNeuralNetwork *network) {
    _networkLearn = network;
    _learningSet.clear();
}

void NeuralNetworkTrainer::removeSet() {
    _learningSet.clear();
}

void NeuralNetworkTrainer::removeInvalidSet() {
    auto set = _learningSet;
    _learningSet.clear();
    for(auto value : set) addTrainingSet(value);
}

void NeuralNetworkTrainer::training(BaseValueType eps, int maxIteration) {
    bool hasChanged = true;
    int iterationNumber = 0;
    while(iterationNumber != maxIteration) {
        hasChanged = false;

        for(auto trainingSet : _learningSet) {
            const auto &input = trainingSet->input;
            for(auto iter = input.begin(); iter != input.end(); ++iter) {
                iter.key()->setOutputSignal(iter.value());
            }
            _networkLearn->updateNetwork();
            const auto &output = trainingSet->output;
            for(auto iter = output.begin(); iter != output.end(); ++iter) {
                const auto &calculated = iter.key()->outputSignal();
                const auto &requiredMatrix = iter.value();
                for(int i = 0; i != calculated.rows(); ++i) {
                    for(int j = 0; j != calculated.columns(); ++j){
                        if(std::abs(requiredMatrix(i, j) - calculated(i, j)) > eps) {
                            hasChanged = true;
                            break;
                        }
                    }
                    if(hasChanged) break;
                }
                if(hasChanged) break;
            }
            if(hasChanged) break;
        }

        if(!hasChanged) break;

        for(auto trainingSet : _learningSet) {
            const auto &input = trainingSet->input;
            for(auto iter = input.begin(); iter != input.end(); ++iter) {
                iter.key()->setOutputSignal(iter.value());
            }
            _networkLearn->updateNetwork();
            const auto &output = trainingSet->output;
            for(auto iter = output.begin(); iter != output.end(); ++iter) {
                const auto &requiredMatrix = iter.value();
                iter.key()->setOutputSignal(requiredMatrix);
            }
            _networkLearn->training();
        }

        ++iterationNumber;
    }
}
