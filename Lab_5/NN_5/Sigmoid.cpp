#include "Sigmoid.h"

Sigmoid::Sigmoid() {

}

BaseValueType Sigmoid::learningRate() const {
    return __learningRate__;
}

void Sigmoid::setLearningRate(BaseValueType value) {
    if(value < 0)
        __learningRate__ = 0.0001;
    else if(value > 1)
        __learningRate__ = 1;
    else
        __learningRate__ = value;
}

void Sigmoid::activate(Signal &input) {
    for(int i = 0; i != input.size(); ++i) {
        for (int j = 0; j != input.threadCount(); ++j) {
            auto signal = input.signal(i, j);
            signal = exp(-signal);
            signal = 1 / (1 + signal);
            input.setSignal(signal, i, j);
        }
    }
}

Signal Sigmoid::correct(const Signal &/*input*/, const Signal &output, RelatationWeights &weights, const Signal &errors) {
    Signal returnedError(weights.weightingFactors().rows(), errors.threadCount());

    Signal calcError(output.size(), output.threadCount());
    for(int i = 0; i != calcError.size(); ++i) {
        for(int j = 0; j != calcError.threadCount(); ++j) {
            auto e = output.signal(i, j);
            e *= (1 - output.signal(i, j));
            e *= errors.signal(i, j);
            calcError.setSignal(e, i, j);
        }
    }

    for(int i = 0; i != weights.neuronsCount(); ++i) {
        BaseValueType correction = 0;
        for(int th = 0; th != calcError.threadCount(); ++th) {
            correction += calcError.signal(i, th) * output.signal(i, th);
        }
        correction *= __learningRate__;
        for(int j = 0; j != weights.inputsCount(); ++j) {
            for(int th = 0; th != calcError.threadCount(); ++th) {
                auto error = returnedError.signal(j, th);
                error += calcError.signal(i, th) * weights.weight(i, j);
                returnedError.setSignal(error, j, th);
            }
            weights.setWeight(weights.weight(i, j) + correction, i, j);
        }
        weights.setShift(weights.shift(i) + correction, i);
    }


    return returnedError;
}
