#include "AbstractActivation.h"

AbstractActivation::AbstractActivation()
{

}

void AbstractActivation::activate(Signal &/*input*/) {

}

Signal AbstractActivation::correct(const Signal &input, const RelatationWeights &/*weights*/, const Signal &/*error*/) {
    return Signal(input.size(), input.threadCount());
}
