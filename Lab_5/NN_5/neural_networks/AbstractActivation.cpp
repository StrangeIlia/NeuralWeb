#include "AbstractActivation.h"

AbstractActivation::AbstractActivation()
{

}

AbstractActivation::~AbstractActivation() {

}

void AbstractActivation::activate(Signal &/*input*/) {

}

Signal AbstractActivation::correct(const Signal &/*input*/,
                                   const Signal &/*output*/,
                                   RelatationWeights &/*weights*/,
                                   const Signal &/*error*/) {
    return Signal();
}
