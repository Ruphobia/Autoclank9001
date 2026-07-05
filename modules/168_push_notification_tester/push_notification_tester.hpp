#pragma once

#include <string>

// Stub module for the "Push notification tester" tool entry.
// Mobile development.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace push_notification_tester {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
