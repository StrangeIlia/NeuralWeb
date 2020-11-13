#include "AbstractActivation.h"

AbstractActivation::AbstractActivation()
{

}

AbstractActivation::~AbstractActivation() {

}

void AbstractActivation::activate(Signal &/*input*/) {

}

Signal AbstractActivation::correct(const Signal &input, RelatationWeights &/*weights*/, const Signal &/*error*/) {
    return Signal(input.size(), input.threadCount());
}
