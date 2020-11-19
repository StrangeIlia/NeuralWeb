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
            signal = std::exp(-signal);
            signal = 1 / (1 + signal);
            input.setSignal(signal, i, j);
        }
    }
}

Signal Sigmoid::correct(const Signal &input, const Signal &output, RelatationWeights &weights, const Signal &errors) {
    Signal returnedError(input.size(), input.threadCount());

    Signal calcError(output.size(), output.threadCount());
    for(int i = 0; i < calcError.size(); ++i) {
        for(int th = 0; th != calcError.threadCount(); ++th) {
            auto e = output.signal(i, th);
            e *= (1 - output.signal(i, th));
            e *= errors.signal(i, th);
            calcError.setSignal(e, i, th);
        }
    }

    for(int i = 0; i < weights.neuronsCount(); ++i) {
        BaseValueType correction = 0;
        for(int j = 0; j < weights.inputsCount(); ++j) {
            correction = 0;
            auto weight = weights.weight(i, j);
            for(int th = 0; th != calcError.threadCount(); ++th) {
                correction += calcError.signal(i, th) * input.signal(j, th);
                auto error = returnedError.signal(j, th);
                error += calcError.signal(i, th) * weight;
                returnedError.setSignal(error, j, th);
            }
            weight += correction * __learningRate__;
            weights.setWeight(weight, i, j);
        }

        correction = 0;
        for(int th = 0; th != calcError.threadCount(); ++th) {
            correction += calcError.signal(i, th);
        }
        weights.setShift(weights.shift(i) + correction * __learningRate__, i);
    }

    return returnedError;
}
