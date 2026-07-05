#include "gratitude_and_habit_tracker.hpp"

namespace gratitude_and_habit_tracker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Gratitude and habit tracker (Sleep science, wellness, mental health). Awaits wire-up.";
    return s;
}

}
