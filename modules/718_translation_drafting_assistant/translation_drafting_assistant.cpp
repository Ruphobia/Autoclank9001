#include "translation_drafting_assistant.hpp"

namespace translation_drafting_assistant {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Translation Drafting Assistant (Ancient languages and paleography). Awaits wire-up.";
    return s;
}

}
