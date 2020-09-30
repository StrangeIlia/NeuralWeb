#ifndef CLUSTEROFNEURONS_H
#define CLUSTEROFNEURONS_H

#include <QList>
#include <stdexcept>

#include "math/matrix/MatrixOnRow.hpp"
#include <QRandomGenerator>

/// Это шаблон, поэтому реализация прямо в файле с заголовками
template<class Base>
class AbstractClusterOfNeurons
{
protected:
    MatrixOnRow<Base> outputSignal;
    MatrixOnRow<Base> weightingShift;
    MatrixOnRow<Base> weightingFactors;
    QList<AbstractClusterOfNeurons*> inputs;
    QList<AbstractClusterOfNeurons*> outputs;

public:
    AbstractClusterOfNeurons(int neuronsCount) {
        if(neuronsCount < 1)
            throw std::invalid_argument("AbstractClusterOfNeurons::AbstractClusterOfNeurons(int): The minimum number of neurons in a cluster 1");
        insertNeurons(0, neuronsCount);
    }

    int neuronsCount() const {
        return outputSignal.rows();
    }

    void insertNeurons(int index, int count) {
        if(index < 0)
            throw std::invalid_argument("AbstractClusterOfNeurons::insertNeurons(int, int): Index can't be less than 0");
        if(count < 0)
            throw std::invalid_argument("AbstractClusterOfNeurons::insertNeurons(int, int): Count can't be less than 0");
        if(index > neuronsCount())
            throw std::invalid_argument("AbstractClusterOfNeurons::insertNeurons(int, int): Index can't be greater than neurons count");

        outputSignal.insertRows(index, count);
        weightingShift.insertRows(index, count);
        weightingFactors.insertRows(index, count);

        for(int i = index; i != weightingShift.rows(); ++i)
            for(int j = 0; j != weightingShift.columns(); ++j)
                weightingShift(i, j) = getInitValue();

        for(int i = index; i != weightingFactors.rows(); ++i)
            for(int j = 0; j != weightingFactors.columns(); ++j)
                weightingFactors(i, j) = getInitValue();


        for(auto cluster : outputs)
            cluster.neuronsAppended(this, index, count);
    }

    void removeNeurons(int first, int last) {
        if(first < 0)
            throw std::invalid_argument("AbstractClusterOfNeurons::removeNeurons(int, int): First can't be less than 0");
        if(last < 0)
            throw std::invalid_argument("AbstractClusterOfNeurons::removeNeurons(int, int): Last can't be less than 0");
        if(first > neuronsCount())
            throw std::invalid_argument("AbstractClusterOfNeurons::removeNeurons(int, int): First can't be greater than neurons count");
        if(last > neuronsCount())
            throw std::invalid_argument("AbstractClusterOfNeurons::removeNeurons(int, int): Last can't be greater than neurons count");

        outputSignal.removeRows(first, last);
        weightingShift.removeRows(first, last);
        weightingFactors.removeRows(first, last);
        for(auto cluster : outputs)
            cluster.neuronsDeleted(this, first, last);
    }

    void setNueronsCount(int count) {
        if(count < 0)
            throw std::invalid_argument("AbstractClusterOfNeurons::setNueronsCount(int): Count can't be less than 0");

         if(count > neuronsCount())
             insertNeurons(neuronsCount(), count - neuronsCount());
         else
             removeNeurons(count, neuronsCount() - 1);
    }

    inline void addInput(AbstractClusterOfNeurons *cluster) {
        connect(cluster, this);
    }

    inline void addOutput(AbstractClusterOfNeurons *cluster) {
        connect(this, cluster);
    }

    inline void removeInput(AbstractClusterOfNeurons *cluster) {
        disconnect(cluster, this);
    }

    inline void removeOutput(AbstractClusterOfNeurons *cluster) {
        disconnect(this, cluster);
    }

    virtual bool calculateOut() = 0;
    virtual bool weightCorrection() = 0;

    void readOutputSignals(MatrixOnRow<Base> &output) {
        output.setSize(outputSignal.rows(), outputSignal.columns());
        auto& out = output.getBaseRow();
        const auto& in = outputSignal.getBaseRow();
        std::copy(in.begin(), in.end(), out.begin());
    }

    void writeOutputSignals(MatrixOnRow<Base> &input) {
        if(input.rows() != outputSignal.rows())
            throw std::invalid_argument("");
        if(input.columns() != outputSignal.columns())
            throw std::invalid_argument("");
        auto& out = outputSignal.getBaseRow();
        const auto& in = input.getBaseRow();
        std::copy(in.begin(), in.end(), out.begin());
    }

protected:
    virtual Base getInitValue() const {
        QRandomGenerator *generator = QRandomGenerator::system();
        return generator->generateDouble();
    }

    void neuronsAppended(AbstractClusterOfNeurons *updated, int index, int count) {
        if(!inputs.contains(updated)) return;
        for(auto cluster : inputs) {
            if(cluster == updated) break;
            index += cluster->neuronsCount();
        }
        weightingFactors.insertColumns(index, count);
        int end = index + count;
        for(int i = 0; i != weightingFactors.rows(); ++i)
            for(int j = index; j != end; ++j)
                weightingFactors(i, j) = getInitValue();
    }

    void neuronsDeleted(AbstractClusterOfNeurons *updated, int first, int last) {
        if(!inputs.contains(updated)) return;
        for(auto cluster : inputs) {
            if(cluster == updated) break;
            first += cluster->neuronsCount();
            last += cluster->neuronsCount();
        }
        weightingFactors.removeColumns(first, last);
    }

    static void connect(AbstractClusterOfNeurons *in, AbstractClusterOfNeurons *out) {
        if(in->outputs.contains(out)) return;

        in->outputs.push_back(out);
        out->inputs.push_back(in);
        out->neuronsAppended(in, 0, in->neuronsCount());
    }

    static void disconnect(AbstractClusterOfNeurons *in, AbstractClusterOfNeurons *out) {
        if(!in->outputs.contains(out)) return;

        in->outputs.removeOne(out);
        out->neuronsDeleted(in, 0, in->neuronsCount() - 1);
        out->inputs.removeOne(out);
    }
};

#endif // CLUSTEROFNEURONS_H
