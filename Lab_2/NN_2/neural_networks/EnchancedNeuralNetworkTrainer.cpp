#include "EnchancedNeuralNetworkTrainer.h"

EnchancedNeuralNetworkTrainer::EnchancedNeuralNetworkTrainer(SimpleNeuralNetwork *network, bool memoryControl)
    : NeuralNetworkTrainer(network, memoryControl)
{

}

void EnchancedNeuralNetworkTrainer::training(BaseValueType learningFactor, BaseValueType eps, int maxIteration) {
    DebugNeuralNetwork *networkLearn = new DebugNeuralNetwork;
    for(auto cluster: _networkLearn->inputClusters())
        networkLearn->addCluster(cluster);
    for(auto cluster: _networkLearn->hiddenClusters())
        networkLearn->addCluster(cluster);
    for(auto cluster: _networkLearn->outputClusters())
        networkLearn->addCluster(cluster);

    networkLearn->updateClusterSequence();

    int iterationNumber = 0;
    while(iterationNumber != maxIteration) {
        QList<TrainingSet*> mistakes;

        for(auto trainingSet : _learningSet) {
            const auto &input = trainingSet->input;
            for(auto iter = input.begin(); iter != input.end(); ++iter) {
                iter.key()->setOutputSignal(iter.value());
            }
            networkLearn->updateNetwork();
            const auto &output = trainingSet->output;
            bool hasChanged = false;
            for(auto iter = output.begin(); iter != output.end(); ++iter) {
                const auto &calculated = iter.key()->outputSignal();
                const auto &requiredMatrix = iter.value();
                for(int i = 0; i != calculated.rows(); ++i) {
                    for(int j = 0; j != calculated.columns(); ++j){
                        if(std::abs(requiredMatrix(i, j) - calculated(i, j)) > eps) {
                            mistakes.append(trainingSet);
                            hasChanged = true;
                            break;
                        }
                    }
                    if(hasChanged) break;
                }
            }
        }

        if(mistakes.empty()) break;

        for(auto trainingSet : mistakes) {
            const auto &input = trainingSet->input;
            for(auto iter = input.begin(); iter != input.end(); ++iter) {
                iter.key()->setOutputSignal(iter.value());
            }
            networkLearn->updateNetwork();
            const auto &output = trainingSet->output;
            for(auto iter = output.begin(); iter != output.end(); ++iter) {
                const auto &requiredMatrix = iter.value();
                iter.key()->setOutputSignal(requiredMatrix);
            }
            networkLearn->training(learningFactor);
        }

        networkLearn->setSaveSummSignal(true);

        QHash<Cluster, Matrix> shifts;
        for(auto cluster: networkLearn->hiddenClusters())  {
            shifts[cluster] = cluster->weightingShift();
        }
        for(auto cluster: networkLearn->outputClusters())  {
            shifts[cluster] = cluster->weightingShift();
        }

        QHash<Cluster, MatrixOnRow<int>> positiveCount;
        QHash<Cluster, MatrixOnRow<int>> negativeCount;
        QHash<Cluster, Matrix> avgPositiveShift;
        QHash<Cluster, Matrix> avgNegativeShift;
        for(auto cluster: networkLearn->hiddenClusters())  {
            avgPositiveShift[cluster] = Matrix(cluster->neuronsCount(), cluster->threadsCount());
            avgNegativeShift[cluster] = Matrix(cluster->neuronsCount(), cluster->threadsCount());
            positiveCount[cluster] = MatrixOnRow<int>(cluster->neuronsCount(), cluster->threadsCount());
            negativeCount[cluster] = MatrixOnRow<int>(cluster->neuronsCount(), cluster->threadsCount());
        }
        for(auto cluster: networkLearn->outputClusters())  {
            avgPositiveShift[cluster] = Matrix(cluster->neuronsCount(), cluster->threadsCount());
            avgNegativeShift[cluster] = Matrix(cluster->neuronsCount(), cluster->threadsCount());
            positiveCount[cluster] = MatrixOnRow<int>(cluster->neuronsCount(), cluster->threadsCount());
            negativeCount[cluster] = MatrixOnRow<int>(cluster->neuronsCount(), cluster->threadsCount());
        }

        for(auto trainingSet : _learningSet) {
            const auto &input = trainingSet->input;
            for(auto iter = input.begin(); iter != input.end(); ++iter) {
                iter.key()->setOutputSignal(iter.value());
            }
            networkLearn->updateNetwork();
            auto hash = networkLearn->summSignal();

            for(auto cluster: networkLearn->hiddenClusters()) {
                auto signal = hash[cluster];
                auto &positiveMatrixCount = positiveCount[cluster];
                auto &negativeMatrixCount = positiveCount[cluster];
                auto &positiveShift = avgPositiveShift[cluster];
                auto &negativeShift = avgNegativeShift[cluster];
                for(int i = 0; i != positiveShift.rows(); ++i) {
                    for(int j = 0; j != positiveShift.columns(); ++j) {
                        auto v = signal(i, j);
                        if(v > 0) {
                            positiveShift(i, j) += v;
                            positiveMatrixCount(i, j) += 1;
                        }
                        else {
                            negativeShift(i, j) += v;
                            negativeMatrixCount(i, j) += 1;
                        }
                    }
                }
            }
            for(auto cluster: networkLearn->outputClusters()) {
                auto signal = hash[cluster];
                auto &positiveMatrixCount = positiveCount[cluster];
                auto &negativeMatrixCount = positiveCount[cluster];
                auto &positiveShift = avgPositiveShift[cluster];
                auto &negativeShift = avgNegativeShift[cluster];
                for(int i = 0; i != positiveShift.rows(); ++i) {
                    for(int j = 0; j != positiveShift.columns(); ++j) {
                        auto v = signal(i, j);
                        if(v > 0) {
                            positiveShift(i, j) += v;
                            positiveMatrixCount(i, j) += 1;
                        }
                        else {
                            negativeShift(i, j) += v;
                            negativeMatrixCount(i, j) += 1;
                        }
                    }
                }
            }
        }

        for(auto iter = shifts.begin(); iter != shifts.end(); ++iter) {
            auto cluster = iter.key();
            auto &positiveMatrixCount = positiveCount[cluster];
            auto &negativeMatrixCount = positiveCount[cluster];
            auto &positiveShift = avgPositiveShift[cluster];
            auto &negativeShift = avgNegativeShift[cluster];
            for(int i = 0; i != iter.value().rows(); ++i) {
                for(int j = 0; j != iter.value().columns(); ++j) {
                    auto negShift = negativeShift(i, j);
                    if(negativeMatrixCount(i, j) != 0) negShift /= negativeMatrixCount(i, j);
                    auto posShift = positiveShift(i, j);
                    if(positiveMatrixCount(i, j) != 0) posShift /= positiveMatrixCount(i, j);
                    iter.value()(i, j) -= (posShift + negShift) / 2;
                }
            }
            iter.key()->setWeightingShift(iter.value());
        }

        networkLearn->setSaveSummSignal(false);

        ++iterationNumber;
    }

    delete networkLearn;
}
