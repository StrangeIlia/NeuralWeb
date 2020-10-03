#ifndef BIPOLAR_H
#define BIPOLAR_H

#include "neural_networks/AbstractClusterOfNeurons.hpp"

template<class Base>
class Bipolar : public AbstractClusterOfNeurons<Base> {
    Base _center;
protected:
    void normalizeReadedSignal(MatrixOnRow<Base>& matrix, Base minValue, Base maxValue) override {
        Base average = (minValue + maxValue) / 2;
        auto iter = matrix.getBaseRow().begin();
        auto end = matrix.getBaseRow().end();
        while (iter != end) {
            *iter += 1;
            *iter *= (maxValue - minValue) / 2;
            *iter -= minValue;
            ++iter;
        }
    }

    void normalizeWritedSignal(MatrixOnRow<Base>& matrix, Base minValue, Base maxValue) override {
        Base average = (minValue + maxValue) / 2;
        auto iter = matrix.getBaseRow().begin();
        auto end = matrix.getBaseRow().end();
        while (iter != end) {
            *iter = *iter < average ? -1 : 1;
            ++iter;
        }
    }

    inline Base activationFunction(const Base& value) override {
        return value < _center ? -1 : 1;
    }

public:
    Bipolar(int neuronCount, Base center = 0) : AbstractClusterOfNeurons<Base>(neuronCount),  _center(center) {

    }

    ~Bipolar() {}

    void correctionFunction() override {
        int shift = 0;
        for(auto cluster : this->inputs) {
            for(int th = 0; th != this->threadsCount(); ++th) {
                for(int i = 0; i != this->neuronsCount(); ++i) {
                    auto y = this->outputSignal(i, th);
                    for(int j = 0; j != cluster->neuronsCount(); ++j) {
                        auto x = cluster->getOutputSignal()(j, th);
                        this->weightingFactors(i, j + shift) += x * y;
                    }
                    this->weightingShift(i, th) += y;
                }
            }
            shift += cluster->neuronsCount();
        }
    }

    inline Base center() const { return _center; }
    inline void setCenter(const Base& center) { _center = center; }
};

#endif // BIPOLAR_H
