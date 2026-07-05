#include "advanced_raster_image_editor_photoshop_gimp_class.hpp"

namespace advanced_raster_image_editor_photoshop_gimp_class {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Advanced raster image editor (Photoshop / GIMP class) (Image and video). Awaits wire-up.";
    return s;
}

}
