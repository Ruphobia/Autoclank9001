#include "ssr_ssg_isr_strategist.hpp"

namespace ssr_ssg_isr_strategist {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: SSR/SSG/ISR strategist (Web development). Awaits wire-up.";
    return s;
}

}
