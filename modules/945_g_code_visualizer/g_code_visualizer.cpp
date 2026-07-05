#include "g_code_visualizer.hpp"

namespace g_code_visualizer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: G-code visualizer (Mechanical and manufacturing). Awaits wire-up.";
    return s;
}

}
