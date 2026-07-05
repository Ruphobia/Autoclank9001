#pragma once

#include <string>

// Stub module for the "GLM rate modeler" tool entry.
// Real estate, insurance, actuarial.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace glm_rate_modeler {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
