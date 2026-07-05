#include "cassette_audio_codec.hpp"

namespace cassette_audio_codec {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Cassette Audio Codec (Vintage hardware preservation and retrocomputing). Awaits wire-up.";
    return s;
}

}
