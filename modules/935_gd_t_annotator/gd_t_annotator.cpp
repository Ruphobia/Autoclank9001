#include "gd_t_annotator.hpp"

namespace gd_t_annotator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: GD&T annotator (Mechanical and manufacturing). Awaits wire-up.";
    return s;
}

}
