#include "porkchop_plot_generator.hpp"

namespace porkchop_plot_generator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Porkchop Plot Generator (Aerospace and orbital mechanics). Awaits wire-up.";
    return s;
}

}
