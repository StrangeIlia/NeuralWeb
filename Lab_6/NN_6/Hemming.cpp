#include "Hemming.h"

Hemming::Hemming()
{

}

void Hemming::activate(Signal &input) {
    for(int i = 0; i != input.size(); ++i) {
        for(int j = 0; j != input.threadCount(); ++j) {
            if(input.signal(i, j) < 0) input.setSignal(0, i, j);
            else if(input.signal(i, j) > __t__) input.setSignal(1, i, j);
            else input.setSignal(input.signal(i, j) / __t__, i, j);
        }
    }
}
