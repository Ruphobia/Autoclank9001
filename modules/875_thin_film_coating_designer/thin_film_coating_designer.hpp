#pragma once

#include <string>

// Stub module for the "Thin Film Coating Designer" tool entry.
// Optics and photonics.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace thin_film_coating_designer {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
