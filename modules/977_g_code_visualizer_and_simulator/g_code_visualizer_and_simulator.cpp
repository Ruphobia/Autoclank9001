#include "g_code_visualizer_and_simulator.hpp"

namespace g_code_visualizer_and_simulator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: G-code Visualizer and Simulator (Maker fab and personal manufacturing). Awaits wire-up.";
    return s;
}

}
