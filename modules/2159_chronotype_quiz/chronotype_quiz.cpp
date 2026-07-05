#include "chronotype_quiz.hpp"

namespace chronotype_quiz {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Chronotype Quiz (Sleep science, wellness, mental health). Awaits wire-up.";
    return s;
}

}
