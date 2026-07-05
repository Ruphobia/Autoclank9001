#include "encrypted_therapy_session_notes.hpp"

namespace encrypted_therapy_session_notes {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Encrypted therapy session notes (Sleep science, wellness, mental health). Awaits wire-up.";
    return s;
}

}
