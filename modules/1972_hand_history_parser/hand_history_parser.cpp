#include "hand_history_parser.hpp"

namespace hand_history_parser {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Hand history parser (Game playing assistants). Awaits wire-up.";
    return s;
}

}
