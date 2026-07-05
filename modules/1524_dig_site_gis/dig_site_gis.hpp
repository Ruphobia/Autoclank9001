#pragma once

#include <string>

// Stub module for the "Dig site GIS" tool entry.
// Paleontology and archaeology.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace dig_site_gis {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
