#include "Binary.h"

Binary::Binary()
{

}

void Binary::activate(Signal &input) {
    for(int i = 0; i != input.threadCount(); ++i) {
        for(int j = 0; j != input.size(); ++j) {
            auto value = input.signal(j, i);
            value = value > 0 ? 1 : 0;
            input.setSignal(value, j, i);
        }
    }
}
