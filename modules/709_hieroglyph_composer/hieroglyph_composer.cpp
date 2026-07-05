#include "hieroglyph_composer.hpp"

namespace hieroglyph_composer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Hieroglyph Composer (Ancient languages and paleography). Awaits wire-up.";
    return s;
}

}
