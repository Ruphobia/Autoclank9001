#include "daily_journal.hpp"

namespace daily_journal {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Daily journal (Office productivity). Awaits wire-up.";
    return s;
}

}
