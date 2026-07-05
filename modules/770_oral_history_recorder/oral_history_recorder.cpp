#include "oral_history_recorder.hpp"

namespace oral_history_recorder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Oral History Recorder (Mythology, folklore, and comparative narrative). Awaits wire-up.";
    return s;
}

}
