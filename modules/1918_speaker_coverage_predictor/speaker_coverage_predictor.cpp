#include "speaker_coverage_predictor.hpp"

namespace speaker_coverage_predictor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Speaker Coverage Predictor (Performing arts: theater, sound, lighting, stage). Awaits wire-up.";
    return s;
}

}
