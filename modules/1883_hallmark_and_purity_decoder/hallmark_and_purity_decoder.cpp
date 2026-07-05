#include "hallmark_and_purity_decoder.hpp"

namespace hallmark_and_purity_decoder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Hallmark and Purity Decoder (Numismatics, philately, collecting, antiques). Awaits wire-up.";
    return s;
}

}
