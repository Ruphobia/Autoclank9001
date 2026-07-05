#pragma once

#include <string>

// Stub module for the "Surname Etymology and Patronymic Resolver" tool entry.
// Detailed genealogy and family history.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace surname_etymology_and_patronymic_resolver {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
