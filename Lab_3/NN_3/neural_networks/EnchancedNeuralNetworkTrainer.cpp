#include "EnchancedNeuralNetworkTrainer.h"

EnchancedNeuralNetworkTrainer::EnchancedNeuralNetworkTrainer(SimpleNeuralNetwork *network, bool memoryControl)
    : NeuralNetworkTrainer(network, memoryControl)
{

}

int EnchancedNeuralNetworkTrainer::training(BaseValueType learningFactor, BaseValueType eps, int maxIteration) {
    int iterationNumber = 0;
    while(iterationNumber != maxIteration) {
        QList<TrainingSet*> mistakes;
        /// Проверка наличия ошибок
        for(auto trainingSet : _learningSet) {
            const auto &input = trainingSet->input;
            for(auto iter = input.begin(); iter != input.end(); ++iter) {
                iter.key()->setOutputSignal(iter.value());
            }
            _networkLearn->updateNetwork();
            const auto &output = trainingSet->output;

            for(auto iter = output.begin(); iter != output.end(); ++iter) {
                bool hasMistake = false;
                const auto &calculated = iter.key()->outputSignal();
                const auto &requiredMatrix = iter.value();
                for(int i = 0; i != calculated.rows(); ++i) {
                    for(int j = 0; j != calculated.columns(); ++j){
                        if(std::abs(requiredMatrix(i, j) - calculated(i, j)) > eps) {
                            hasMistake = true;
                            mistakes.append(trainingSet);
                            break;
                        }
                    }
                    if(hasMistake) break;
                }
            }
        }

        if(mistakes.empty()) break;

        /// Обучение
        for(auto trainingSet : mistakes) {
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
            _networkLearn->training(learningFactor);
        }

        ++iterationNumber;
    }

    return iterationNumber;
}
