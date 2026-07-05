#include "symptom_and_medication_tracker.hpp"

namespace symptom_and_medication_tracker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Symptom and medication tracker (Sleep science, wellness, mental health). Awaits wire-up.";
    return s;
}

}
