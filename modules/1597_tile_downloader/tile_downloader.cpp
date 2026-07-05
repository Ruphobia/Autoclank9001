#include "tile_downloader.hpp"

namespace tile_downloader {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Tile Downloader (Geology, GIS, and earth sciences). Awaits wire-up.";
    return s;
}

}
