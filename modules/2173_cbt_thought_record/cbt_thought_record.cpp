#include "cbt_thought_record.hpp"

namespace cbt_thought_record {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: CBT thought record (Sleep science, wellness, mental health). Awaits wire-up.";
    return s;
}

}
