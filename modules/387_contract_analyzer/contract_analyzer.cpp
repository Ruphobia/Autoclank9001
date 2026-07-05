#include "contract_analyzer.hpp"

namespace contract_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Contract Analyzer (Cryptography and blockchain). Awaits wire-up.";
    return s;
}

}
