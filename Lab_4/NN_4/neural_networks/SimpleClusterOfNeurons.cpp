#include "SimpleClusterOfNeurons.h"

SimpleClusterOfNeurons::SimpleClusterOfNeurons(int neuronsCount) {
    __outputSignal__.setSize(neuronsCount);
    __weights__.setNeuronsCount(neuronsCount);
}

SimpleClusterOfNeurons::~SimpleClusterOfNeurons() {
    clearLinks();
}

ClusterList SimpleClusterOfNeurons::inputClusters() const {
    return __inputs__;
}


ClusterList SimpleClusterOfNeurons::outputClusters() const {
    return __outputs__;
}

RelatationWeights &SimpleClusterOfNeurons::weights() {
    return __weights__;
}

const RelatationWeights &SimpleClusterOfNeurons::weights() const {
    return __weights__;
}

Signal &SimpleClusterOfNeurons::outputSignal() {
    return __outputSignal__;
}

const Signal &SimpleClusterOfNeurons::outputSignal() const {
    return __outputSignal__;
}

int SimpleClusterOfNeurons::neuronsCount() const {
    return __weights__.neuronsCount();
}


int SimpleClusterOfNeurons::threadsCount() const {
    return __outputSignal__.threadCount();
}


int SimpleClusterOfNeurons::inputSignalCount() const {
    return __weights__.inputsCount();
}

void SimpleClusterOfNeurons::insertNeurons(int index, int count) {
    if(index < 0)
        throw std::invalid_argument("AbstractClusterOfNeurons::insertNeurons(int, int): Index can't be less than 0");
    if(count < 0)
        throw std::invalid_argument("AbstractClusterOfNeurons::insertNeurons(int, int): Count can't be less than 0");
    if(index > neuronsCount())
        throw std::invalid_argument("AbstractClusterOfNeurons::insertNeurons(int, int): Index can't be greater than neurons count");

    __weights__.insertNeurons(index, count);
    __outputSignal__.insertSignals(index, count);

    for(auto cluster : __outputs__)
        cluster->_neuronsAppended_(this, index, count);
}


void SimpleClusterOfNeurons::removeNeurons(int first, int last) {
    if(first < 0)
        throw std::invalid_argument("AbstractClusterOfNeurons::removeNeurons(int, int): First can't be less than 0");
    if(last < 0)
        throw std::invalid_argument("AbstractClusterOfNeurons::removeNeurons(int, int): Last can't be less than 0");
    if(first > neuronsCount())
        throw std::invalid_argument("AbstractClusterOfNeurons::removeNeurons(int, int): First can't be greater than neurons count");
    if(last > neuronsCount())
        throw std::invalid_argument("AbstractClusterOfNeurons::removeNeurons(int, int): Last can't be greater than neurons count");

    __weights__.removeNeurons(first, last);
    __outputSignal__.removeSignals(first, last);
    for(auto cluster : __outputs__)
        cluster->_neuronsDeleted_(this, first, last);
}


void SimpleClusterOfNeurons::setNueronsCount(int count) {
    if(count < 0)
        throw std::invalid_argument("AbstractClusterOfNeurons::setNueronsCount(int): Count can't be less than 0");

     if(count > neuronsCount())
         insertNeurons(neuronsCount(), count - neuronsCount());
     else
         removeNeurons(count, neuronsCount() - 1);
}


bool SimpleClusterOfNeurons::addInput(SimpleClusterOfNeurons *cluster) {
    return connect(cluster, this);
}


bool SimpleClusterOfNeurons::addOutput(SimpleClusterOfNeurons *cluster) {
    return connect(this, cluster);
}


bool SimpleClusterOfNeurons::removeInput(SimpleClusterOfNeurons *cluster) {
    return disconnect(cluster, this);
}


bool SimpleClusterOfNeurons::removeOutput(SimpleClusterOfNeurons *cluster) {
    return disconnect(this, cluster);
}


bool SimpleClusterOfNeurons::connect(SimpleClusterOfNeurons *in, SimpleClusterOfNeurons *out) {
    if(in->__outputs__.contains(out)) return false;
    if(in->threadsCount() != out->threadsCount()) return false;

    in->__outputs__.push_back(out);
    out->__inputs__.push_back(in);
    out->_neuronsAppended_(in, 0, in->neuronsCount());
    return true;
}


bool SimpleClusterOfNeurons::disconnect(SimpleClusterOfNeurons *in, SimpleClusterOfNeurons *out) {
    if(!in->__outputs__.contains(out)) return false;

    in->__outputs__.removeOne(out);
    out->_neuronsDeleted_(in, 0, in->neuronsCount() - 1);
    out->__inputs__.removeOne(out);
    return true;
}


void SimpleClusterOfNeurons::clearLinks() {
    auto list = __inputs__;
    for(auto cluster : list)
        disconnect(cluster, this);
    list = __outputs__;
    for(auto cluster : list)
        disconnect(this, cluster);
}

void SimpleClusterOfNeurons::summation() {
    __outputSignal__.setSize(__weights__.neuronsCount());
    __outputSignal__.setThreadCount(__inputs__.first()->threadsCount());

    const auto &shiftVector = __weights__.__weightingShift__.getBaseRow();
    for(unsigned i = 0; i != shiftVector.size(); ++i) {
        for(int j = 0; j != __outputSignal__.threadCount(); ++j) {
            __outputSignal__.setSignal(shiftVector[i], i, j);
        }
    }

    int shift = 0;
    for(auto cluster : __inputs__) {
        if(__outputSignal__.threadCount() != cluster->threadsCount())
            throw std::exception();

        const auto &inputSignal = cluster->__outputSignal__;

        for(int i = 0; i != __outputSignal__.size(); ++i)  {
            for(int k = 0; k != inputSignal.size(); ++k) {
                auto weight = __weights__.weight(i, k + shift);
                for(int j = 0; j != __outputSignal__.threadCount(); ++j) {
                    auto value = __outputSignal__.signal(i, j);
                    value += weight * inputSignal.signal(k, j);
                    __outputSignal__.setSignal(value, i, j);
                }
            }
        }
        shift += inputSignal.size();
    }
}

void SimpleClusterOfNeurons::activation() {
    if(__activationFunction__ != nullptr)
        __activationFunction__->activate(__outputSignal__);
}

QHash<Cluster, Signal> SimpleClusterOfNeurons::correction(Signal error) {
    if(__activationFunction__ == nullptr)
        return QHash<Cluster, Signal>();

    QList<int> counts;
    QList<Signal> inputSignals;
    for(auto cluster : this->__inputs__) {
        counts.push_back(cluster->neuronsCount());
        inputSignals.push_back(cluster->__outputSignal__);
    }
    auto mergedSignals = __activationFunction__->correct(Signal::merge(inputSignals), __weights__, error);
    if(mergedSignals.size() == 0)
        return QHash<Cluster, Signal>();

    auto splitedSignals = mergedSignals.split(counts);

    QHash<Cluster, Signal> result;
    for(int i = 0; i != splitedSignals.size(); ++i)
        result.insert(this->__inputs__[i], std::move(splitedSignals[i]));

    return result;
}

void SimpleClusterOfNeurons::_neuronsAppended_(SimpleClusterOfNeurons *updated, int index, int count) {
    if(!__inputs__.contains(updated)) return;
    for(auto cluster : __inputs__) {
        if(cluster == updated) break;
        index += cluster->neuronsCount();
    }
    __weights__.insertInputs(index, count);
}

void SimpleClusterOfNeurons::_neuronsDeleted_(SimpleClusterOfNeurons *updated, int first, int last) {
    if(!__inputs__.contains(updated)) return;
    for(auto cluster : __inputs__) {
        if(cluster == updated) break;
        first += cluster->neuronsCount();
        last += cluster->neuronsCount();
    }
    __weights__.removeInputs(first, last);
}
