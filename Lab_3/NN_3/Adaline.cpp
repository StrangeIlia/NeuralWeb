#include "Adaline.h"

BaseValueType Adaline::activate(BaseValueType value) {
    return value;
}

BaseValueType Adaline::correct(BaseValueType /*input*/, BaseValueType /*ouput*/) {
    throw std::exception();
}

Adaline::Adaline(int neuronCount) : AbstractClusterOfNeurons(neuronCount) {

}

Adaline::~Adaline() {}

void Adaline::correction(BaseValueType learningFactor) {
    int shift = 0;
    if(learningFactor <= 0) learningFactor = std::numeric_limits<BaseValueType>::epsilon();
    if(learningFactor > 1)  learningFactor = 1;
    auto realSignal = _outputSignal;
    summation();

    for(auto cluster : this->_inputs) {
        for(int th = 0; th != this->threadsCount(); ++th) {
            for(int i = 0; i != this->neuronsCount(); ++i) {
                auto y = -this->_outputSignal(i, th);
                y += realSignal(i, th);
                for(int j = 0; j != cluster->neuronsCount(); ++j) {
                    auto x = cluster->outputSignal().cell(j, th);
                    this->_weightingFactors(i, j + shift) += learningFactor * x * y;
                }

                this->_weightingShift(i, th) += learningFactor * y;
            }
        }
        shift += cluster->neuronsCount();
    }

    _outputSignal = std::move(realSignal);
}

BaseValueType Adaline::getInitValue() const {
    QRandomGenerator *generator = QRandomGenerator::system();
    return generator->generateDouble() - 0.5;
}
