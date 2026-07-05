#pragma once

#include <string>

// Stub module for the "Advanced raster image editor (Photoshop / GIMP class)" tool entry.
// Image and video.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace advanced_raster_image_editor_photoshop_gimp_class {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
