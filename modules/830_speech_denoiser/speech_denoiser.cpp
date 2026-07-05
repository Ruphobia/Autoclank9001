#include "speech_denoiser.hpp"

namespace speech_denoiser {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Speech denoiser (Speech, language, and translation). Awaits wire-up.";
    return s;
}

}
