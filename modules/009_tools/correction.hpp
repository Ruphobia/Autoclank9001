#pragma once

#include <string>
#include <string_view>

namespace correction {

// Record a user correction. Finds the most recent non-correction turn,
// writes a "correction/applies-to" row referencing it, and stores the
// correction body. Future: actually retract the corrected statement
// and optionally re-run downstream handlers.
//
// Returns a one-line acknowledgement.
std::string apply(std::string_view cleaned,
                  std::string_view rewrite);

}
