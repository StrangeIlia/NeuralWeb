#include "Perceptron.h"

Perceptron::Perceptron()
{

}

BaseValueType Perceptron::ny() const {
    return __ny__;
}

void Perceptron::setNy(BaseValueType value) {
    __ny__ = value;
}

Signal Perceptron::correct(const Signal &input, RelatationWeights &weights, const Signal &error) {
    BaseValueType eps = 1e-7;

    for(int th = 0; th != input.threadCount(); ++th) {
        QList<int> activeNeurons;
        QList<int> passiveNeurons;

        for(int i = 0; i != input.size(); ++i) {
            if(input.signal(i, th) > 0.5)
                activeNeurons.append(i);
            else
                passiveNeurons.append(i);
        }

        for(int i = 0; i != weights.neuronsCount(); ++i) {
            QList<int> localActive, localPassive;
            if(error.signal(i, th) > -0.5 && error.signal(i, th) < 0.5)
                continue;

            if(error.signal(i, th) > 0.5) {
                localActive = activeNeurons;
                localPassive = passiveNeurons;
            } else if(error.signal(i, th) < -0.5) {
                localActive = passiveNeurons;
                localPassive = activeNeurons;
            }

            BaseValueType localNy = __ny__;

            /// Удаляем элементы, достигшие границ
            for(int j = 0; j != localActive.size(); ) {
                if(weights.weight(i, j) >= 1 - eps){
                    localActive.removeAt(j);
                    continue;
                }
                ++j;
            }
            for(int j = 0; j != localPassive.size(); ) {
                if(weights.weight(i, j) <= -1 + eps){
                    localPassive.removeAt(j);
                    continue;
                }
                ++j;
            }
            /// -------------------------

            if(localActive.size() == 0) continue;
            if(localPassive.size() == 0) continue;

            BaseValueType inc = localNy / localActive.size();
            for(int j = 0; j != localActive.size(); ++j) {
                if(weights.weight(i, j) + inc > 1) {
                    inc = 1 - weights.weight(i, j);
                }
            }

            BaseValueType dec = inc * localActive.size() / localPassive.size();
            for(int j = 0; j != localPassive.size(); ++j) {
                if(weights.weight(i, j) - dec < -1) {
                    dec = weights.weight(i, j) + 1;
                }
            }

            inc = dec * localPassive.size() / localActive.size();

            for(auto index : localActive)
                weights.setWeight(weights.weight(i, index) + inc, i, index);

            for(auto index : localPassive)
                weights.setWeight(weights.weight(i, index) - dec, i, index);
        }
    }


    return Signal();
}
