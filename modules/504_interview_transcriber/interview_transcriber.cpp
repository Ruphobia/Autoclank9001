#include "interview_transcriber.hpp"

namespace interview_transcriber {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Interview transcriber (Writing, publishing, journalism). Awaits wire-up.";
    return s;
}

}
