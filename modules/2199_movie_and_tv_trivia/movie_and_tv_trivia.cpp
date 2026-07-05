// SPDX-License-Identifier: GPL-3.0-or-later
#include "movie_and_tv_trivia.hpp"

namespace movie_and_tv_trivia {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Movie and TV trivia (Entertainment, trivia, pop culture). Awaits wire-up.";
    return s;
}

}
