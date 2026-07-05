#include "injection_molding_analyzer.hpp"

namespace injection_molding_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Injection molding analyzer (Mechanical and manufacturing). Awaits wire-up.";
    return s;
}

}
