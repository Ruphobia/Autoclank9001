#include "mtf_chart_analyzer.hpp"

namespace mtf_chart_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: MTF Chart Analyzer (Photography and cinematography). Awaits wire-up.";
    return s;
}

}
