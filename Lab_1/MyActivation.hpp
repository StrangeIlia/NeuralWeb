#ifndef MYACTIVATION_H
#define MYACTIVATION_H

#include "neural_networks/AbstractClusterOfNeurons.hpp"

template<class Base>
class MyActivation : public AbstractClusterOfNeurons<Base> {
    Base _q1, _q2;
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
        if(value < _q1) return -1;
        if(value > _q2) return  1;
        return (value - _q1) / (_q2 - _q1) * 2 - 1;
    }

public:
    MyActivation(int neuronCount, const Base &q1, const Base &q2) :  AbstractClusterOfNeurons<Base>(neuronCount), _q1(q1), _q2(q2) {

    }

    ~MyActivation() {}

    void correctionFunction() override {
        //throw std::exception();
    }

    inline Base q1() const { return _q1; }
    inline void q1(const Base& value) { _q1 = value; }

    inline Base q2() const { return _q2; }
    inline void q2(const Base& value) { _q2 = value; }
};


#endif // MYACTIVATION_H
