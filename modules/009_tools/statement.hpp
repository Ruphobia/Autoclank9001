#pragma once

#include <string>
#include <string_view>

namespace statement {

// Ingest a user assertion. For now: writes a "statement/asserted" row to
// session memory (and, when tagged persistent, also flags it). Future:
// extract structured facts and write to a persistent KB.
//
// Returns a one-line acknowledgement string for the user.
std::string ingest(std::string_view cleaned,
                   std::string_view rewrite,
                   bool persistent);

}
