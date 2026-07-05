#include "dream_and_imagery_journal.hpp"

namespace dream_and_imagery_journal {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Dream and imagery journal (Sleep science, wellness, mental health). Awaits wire-up.";
    return s;
}

}
