#include "ttt_cct_plotter.hpp"

namespace ttt_cct_plotter {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: TTT/CCT Plotter (Materials science and metallurgy). Awaits wire-up.";
    return s;
}

}
