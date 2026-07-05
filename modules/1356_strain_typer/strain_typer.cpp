// SPDX-License-Identifier: GPL-3.0-or-later
#include "strain_typer.hpp"

namespace strain_typer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Strain typer (Microbiology, virology, and immunology). Awaits wire-up.";
    return s;
}

}
