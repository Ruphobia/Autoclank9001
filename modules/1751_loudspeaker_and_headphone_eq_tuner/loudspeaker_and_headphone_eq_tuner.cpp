#include "loudspeaker_and_headphone_eq_tuner.hpp"

namespace loudspeaker_and_headphone_eq_tuner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Loudspeaker and headphone EQ tuner (Signal processing and audio). Awaits wire-up.";
    return s;
}

}
