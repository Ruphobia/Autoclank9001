#pragma once

#include <string>

// Stub module for the "CW trainer and decoder" tool entry.
// Aviation, amateur radio, and RF.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace cw_trainer_and_decoder {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
