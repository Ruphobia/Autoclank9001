// SPDX-License-Identifier: GPL-3.0-or-later
#include "lyric_timing_and_karaoke.hpp"

namespace lyric_timing_and_karaoke {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Lyric timing and karaoke (Speech, language, and translation). Awaits wire-up.";
    return s;
}

}
