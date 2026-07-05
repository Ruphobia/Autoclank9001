#pragma once

#include <string>

// Stub module for the "Language Identifier" tool entry.
// Linguistics and conlanging.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace language_identifier {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
