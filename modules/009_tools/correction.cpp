#include "correction.hpp"

#include "../005_context/context.hpp"

#include <string>

namespace correction {

std::string apply(std::string_view cleaned, std::string_view rewrite) {
    // Find the latest non-correction, non-acknowledgment turn for the
    // "applies to" pointer. Cheapest: scan recent records.
    auto recent = context::recent(40);
    int64_t target_turn = 0;
    for (const auto & r : recent) {
        if (r.layer == "classify" && r.kind == "act") {
            if (r.content == "command" || r.content == "question" ||
                r.content == "statement") {
                target_turn = r.turn;
                break;
            }
        }
    }
    const std::string meta = "applies_to_turn=" + std::to_string(target_turn);
    context::append("correction", "raw",      cleaned, meta);
    if (!rewrite.empty()) {
        context::append("correction", "resolved", rewrite, meta);
    }

    if (target_turn > 0) {
        return "correction noted (applies to turn " + std::to_string(target_turn) + ")";
    }
    return "correction noted";
}

}
