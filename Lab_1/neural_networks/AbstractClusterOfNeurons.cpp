#include "AbstractClusterOfNeurons.h"

AbstractClusterOfNeurons::AbstractClusterOfNeurons(int neuronsCount) {
    if(neuronsCount < 1)
        throw std::invalid_argument("AbstractClusterOfNeurons::AbstractClusterOfNeurons(int): The minimum number of neurons in a cluster 1");
    _outputSignal.setColumns(1);
    _weightingShift.setColumns(1);
    insertNeurons(0, neuronsCount);
}

AbstractClusterOfNeurons::~AbstractClusterOfNeurons() {
    clearLinks();
}

const Matrix &AbstractClusterOfNeurons::outputSignal() const {
    return _outputSignal;
}

void AbstractClusterOfNeurons::setOutputSignal(const Matrix &input) {
    if(input.rows() != _outputSignal.rows())
        throw std::invalid_argument("AbstractClusterOfNeurons::writeOutputSignals(MatrixOnRow&) input.rows != neuronsCount");
    if(input.columns() != _outputSignal.columns())
        throw std::invalid_argument("AbstractClusterOfNeurons::writeOutputSignals(MatrixOnRow&) input.columns != threadsCount");
    auto& out = _outputSignal.getBaseRow();
    const auto& in = input.getBaseRow();
    std::copy(in.begin(), in.end(), out.begin());
}

const Matrix &AbstractClusterOfNeurons::weightingShift() const {
    return _weightingShift;
}

void AbstractClusterOfNeurons::setWeightingShift(const Matrix &input) {
    if(input.rows() != _weightingShift.rows())
        throw std::invalid_argument("AbstractClusterOfNeurons::writeShiftWeight(MatrixOnRow&) input.rows != neuronsCount");
    if(input.columns() != _weightingShift.columns())
        throw std::invalid_argument("AbstractClusterOfNeurons::writeShiftWeight(MatrixOnRow&) input.columns != 1");
    auto& out = _weightingShift.getBaseRow();
    const auto& in = input.getBaseRow();
    std::copy(in.begin(), in.end(), out.begin());
}

const Matrix &AbstractClusterOfNeurons::weightingFactors() const {
    return _weightingFactors;
}

void AbstractClusterOfNeurons::setWeightingFactors(const Matrix &input) {
    if(input.rows() != _weightingFactors.rows())
        throw std::invalid_argument("AbstractClusterOfNeurons::writeShiftWeight(MatrixOnRow&) input.rows != neuronsCount");
    if(input.columns() != _weightingFactors.columns())
        throw std::invalid_argument("AbstractClusterOfNeurons::writeShiftWeight(MatrixOnRow&) input.columns != inputsCount");
    auto& out = _weightingFactors.getBaseRow();
    const auto& in = input.getBaseRow();
    std::copy(in.begin(), in.end(), out.begin());
}

ClusterList AbstractClusterOfNeurons::inputClusters() const {
    return _inputs;
}


ClusterList AbstractClusterOfNeurons::outputClusters() const {
    return _outputs;
}

int AbstractClusterOfNeurons::neuronsCount() const {
    return _outputSignal.rows();
}


int AbstractClusterOfNeurons::threadsCount() const {
    return _outputSignal.columns();
}


int AbstractClusterOfNeurons::inputSignalCount() const {
    return _weightingFactors.columns();
}

void AbstractClusterOfNeurons::setThreadsCount(int /*count*/) {
    /// Пока нет реализации
    throw std::exception();
}

void AbstractClusterOfNeurons::insertNeurons(int index, int count) {
    if(index < 0)
        throw std::invalid_argument("AbstractClusterOfNeurons::insertNeurons(int, int): Index can't be less than 0");
    if(count < 0)
        throw std::invalid_argument("AbstractClusterOfNeurons::insertNeurons(int, int): Count can't be less than 0");
    if(index > neuronsCount())
        throw std::invalid_argument("AbstractClusterOfNeurons::insertNeurons(int, int): Index can't be greater than neurons count");

    _outputSignal.insertRows(index, count);
    _weightingShift.insertRows(index, count);
    _weightingFactors.insertRows(index, count);

    for(int i = index; i != _weightingShift.rows(); ++i)
        for(int j = 0; j != _weightingShift.columns(); ++j)
            _weightingShift(i, j) = getInitValue();

    for(int i = index; i != _weightingFactors.rows(); ++i)
        for(int j = 0; j != _weightingFactors.columns(); ++j)
            _weightingFactors(i, j) = getInitValue();


    for(auto cluster : _outputs)
        cluster->neuronsAppended(this, index, count);
}


void AbstractClusterOfNeurons::removeNeurons(int first, int last) {
    if(first < 0)
        throw std::invalid_argument("AbstractClusterOfNeurons::removeNeurons(int, int): First can't be less than 0");
    if(last < 0)
        throw std::invalid_argument("AbstractClusterOfNeurons::removeNeurons(int, int): Last can't be less than 0");
    if(first > neuronsCount())
        throw std::invalid_argument("AbstractClusterOfNeurons::removeNeurons(int, int): First can't be greater than neurons count");
    if(last > neuronsCount())
        throw std::invalid_argument("AbstractClusterOfNeurons::removeNeurons(int, int): Last can't be greater than neurons count");

    _outputSignal.removeRows(first, last);
    _weightingShift.removeRows(first, last);
    _weightingFactors.removeRows(first, last);
    for(auto cluster : _outputs)
        cluster->neuronsDeleted(this, first, last);
}



void AbstractClusterOfNeurons::setNueronsCount(int count) {
    if(count < 0)
        throw std::invalid_argument("AbstractClusterOfNeurons::setNueronsCount(int): Count can't be less than 0");

     if(count > neuronsCount())
         insertNeurons(neuronsCount(), count - neuronsCount());
     else
         removeNeurons(count, neuronsCount() - 1);
}


bool AbstractClusterOfNeurons::addInput(AbstractClusterOfNeurons *cluster) {
    return connect(cluster, this);
}


bool AbstractClusterOfNeurons::addOutput(AbstractClusterOfNeurons *cluster) {
    return connect(this, cluster);
}


bool AbstractClusterOfNeurons::removeInput(AbstractClusterOfNeurons *cluster) {
    return disconnect(cluster, this);
}


bool AbstractClusterOfNeurons::removeOutput(AbstractClusterOfNeurons *cluster) {
    return disconnect(this, cluster);
}


bool AbstractClusterOfNeurons::connect(AbstractClusterOfNeurons *in, AbstractClusterOfNeurons *out) {
    if(in->_outputs.contains(out)) return false;
    if(in->threadsCount() != out->threadsCount()) return false;

    in->_outputs.push_back(out);
    out->_inputs.push_back(in);
    out->neuronsAppended(in, 0, in->neuronsCount());
    return true;
}


bool AbstractClusterOfNeurons::disconnect(AbstractClusterOfNeurons *in, AbstractClusterOfNeurons *out) {
    if(!in->_outputs.contains(out)) return false;

    in->_outputs.removeOne(out);
    out->neuronsDeleted(in, 0, in->neuronsCount() - 1);
    out->_inputs.removeOne(out);
    return true;
}


void AbstractClusterOfNeurons::clearLinks() {
    auto list = _inputs;
    for(auto cluster : list)
        disconnect(cluster, this);
    list = _outputs;
    for(auto cluster : list)
        disconnect(this, cluster);
}


void AbstractClusterOfNeurons::clearOutputSignal(BaseValueType value) {
    auto startSignal = _outputSignal.getBaseRow().begin();
    auto endSignal = _outputSignal.getBaseRow().end();
    while(startSignal != endSignal) {
        *startSignal += value;
        ++startSignal;
    }
}


void AbstractClusterOfNeurons::initOutputSignal() {
    if(threadsCount() == 1) {
        std::copy(_weightingShift.getBaseRow().begin(),
                  _weightingShift.getBaseRow().end(),
                  _outputSignal.getBaseRow().begin());
    } else {
        auto startSignal = _outputSignal.getBaseRow().begin();
        auto endSignal = _outputSignal.getBaseRow().end();
        auto shiftIter = _weightingShift.getBaseRow().begin();
        while(startSignal != endSignal) {
            for(int i = 0; i != threadsCount(); ++i) {
                *startSignal = *shiftIter;
                ++startSignal;
            }
            ++shiftIter;
        }
    }
}


void AbstractClusterOfNeurons::summation() {
    initOutputSignal();
    if(_outputSignal.columns() == 1) {
        /// Из - за особенности хранения данных в матрице
        /// Можно использовать итераторы

        /// Итератор по выходному сигналу
        int shift = 0;
        auto startSignal = _outputSignal.getBaseRow().begin();
        for(auto cluster : _inputs) {
            auto signalIter = startSignal;

            /// Итераторы по входному сигналу
            auto startInput = cluster->_outputSignal.getBaseRow().begin();
            auto endInput = cluster->_outputSignal.getBaseRow().end();
            /// Итераторы по весовым коэффициентам
            auto startWeight = _weightingFactors.getBaseRow().begin();
            auto endOfWeights = _weightingFactors.getBaseRow().end();
            /// Сдвиг для i - ого кластера
            startWeight += shift;
            while(startWeight != endOfWeights) {
                auto weightIter = startWeight;
                for(auto inputIter = startInput; inputIter != endInput; ++inputIter, ++weightIter) {
                    *signalIter += (*inputIter) * (*weightIter);
                }
                startWeight += _weightingFactors.columns();
                ++signalIter;
            }
            shift += _outputSignal.rows();
        }

    } else {
        /// Здесь относительно нормальная индексация
        int shift = 0;
        for(auto cluster : _inputs) {
            for(int i = 0; i != neuronsCount(); ++i) {
                BaseValueType *c = _outputSignal.getBaseRow().data() + i * cluster->neuronsCount() + shift;
                for(int k = 0; k != cluster->neuronsCount(); ++k) {
                    BaseValueType *b = cluster->_outputSignal.getBaseRow().data() + k * threadsCount();
                    BaseValueType a = _weightingFactors(i, k);
                    for(int j = 0; j != threadsCount(); ++j)
                        c[j] = a * b[j];
                }
            }
            shift += cluster->neuronsCount();
        }
    }

}


bool AbstractClusterOfNeurons::activation() {
    for(auto& value : _outputSignal.getBaseRow())
        value = activate(value);
    return true;
}


void AbstractClusterOfNeurons::correction(BaseValueType learningFactor) {
    int shift = 0;
    if(learningFactor <= 0) learningFactor = std::numeric_limits<BaseValueType>::epsilon();
    if(learningFactor > 1)  learningFactor = 1;
    for(auto cluster : this->_inputs) {
        for(int th = 0; th != this->threadsCount(); ++th) {
            for(int i = 0; i != this->neuronsCount(); ++i) {
                auto y = this->_outputSignal(i, th);
                for(int j = 0; j != cluster->neuronsCount(); ++j) {
                    auto x = cluster->_outputSignal(j, th);
                    this->_weightingFactors(i, j + shift) += learningFactor * correct(x, y);
                }

                this->_weightingShift(i, th) += learningFactor * correct(1, y);
            }
        }
        shift += cluster->neuronsCount();
    }
}

BaseValueType AbstractClusterOfNeurons::getInitValue() const {
//    QRandomGenerator *generator = QRandomGenerator::system();
//    return generator->generateDouble();
    return 0;
}


void AbstractClusterOfNeurons::neuronsAppended(AbstractClusterOfNeurons *updated, int index, int count) {
    if(!_inputs.contains(updated)) return;
    for(auto cluster : _inputs) {
        if(cluster == updated) break;
        index += cluster->neuronsCount();
    }
    _weightingFactors.insertColumns(index, count);
    int end = index + count;
    for(int i = 0; i != _weightingFactors.rows(); ++i)
        for(int j = index; j != end; ++j)
            _weightingFactors(i, j) = getInitValue();
}


void AbstractClusterOfNeurons::neuronsDeleted(AbstractClusterOfNeurons *updated, int first, int last) {
    if(!_inputs.contains(updated)) return;
    for(auto cluster : _inputs) {
        if(cluster == updated) break;
        first += cluster->neuronsCount();
        last += cluster->neuronsCount();
    }
    _weightingFactors.removeColumns(first, last);
}
