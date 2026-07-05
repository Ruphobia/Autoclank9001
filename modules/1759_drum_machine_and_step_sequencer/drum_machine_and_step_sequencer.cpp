#include "drum_machine_and_step_sequencer.hpp"

namespace drum_machine_and_step_sequencer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Drum machine and step sequencer (Signal processing and audio). Awaits wire-up.";
    return s;
}

}
