#include "attribution_assistant.hpp"

namespace attribution_assistant {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Attribution Assistant (Numismatics, philately, collecting, antiques). Awaits wire-up.";
    return s;
}

}
