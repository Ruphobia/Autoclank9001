#pragma once

#include <string>

// Stub module for the "Feed and sitemap inspector" tool entry.
// Web development.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace feed_and_sitemap_inspector {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
