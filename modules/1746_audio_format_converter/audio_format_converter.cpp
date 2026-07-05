#include "audio_format_converter.hpp"

namespace audio_format_converter {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Audio format converter (Signal processing and audio). Awaits wire-up.";
    return s;
}

}
