#include "snore_and_apnea_audio_analyzer.hpp"

namespace snore_and_apnea_audio_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Snore and Apnea Audio Analyzer (Sleep science, wellness, mental health). Awaits wire-up.";
    return s;
}

}
