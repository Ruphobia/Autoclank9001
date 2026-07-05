#include "podcast_and_dialogue_production.hpp"

namespace podcast_and_dialogue_production {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Podcast and dialogue production (Signal processing and audio). Awaits wire-up.";
    return s;
}

}
