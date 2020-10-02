#ifndef EXTENDEDCLUSTEROFNEURONS_H
#define EXTENDEDCLUSTEROFNEURONS_H

#include "AbstractClusterOfNeurons.hpp"

/// Класс запоминает суммарное значение входов
/// и весовые коэффициенты перед последней корректировкой
template<class Base>
class DebugClusterOfNeurons : public AbstractClusterOfNeurons<Base>
{
    bool debugEnabled = true;
    MatrixOnRow<Base> _reserveOutputSignal;
    MatrixOnRow<Base> _reserveWeightingShift;
    MatrixOnRow<Base> _reserveWeightingFactors;

protected:
    virtual void _correctionFunction();
    virtual void _processingSumm();

public:
    DebugClusterOfNeurons();

    inline void correctionFunction() final;
    inline void processingSumm() final;

    inline bool enabled() const;
    inline void setEnabled(bool value) const;

    inline const MatrixOnRow<Base>& reserveOutputSignal() const;
    inline const MatrixOnRow<Base>& reserveWeightingShift() const;
    inline const MatrixOnRow<Base>& reserveWeightingFactors() const;
};

template<class Base>
void DebugClusterOfNeurons<Base>::correctionFunction() {
    if(debugEnabled) {
        _reserveOutputSignal = outputSignal;

    }
}

template<class Base>
void DebugClusterOfNeurons<Base>::processingSumm() {

}

#endif // EXTENDEDCLUSTEROFNEURONS_H
