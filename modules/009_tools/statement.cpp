#include "statement.hpp"

#include "../005_context/context.hpp"

#include <string>

namespace statement {

std::string ingest(std::string_view cleaned,
                   std::string_view rewrite,
                   bool persistent) {
    const std::string meta = persistent ? "persistent" : "";
    context::append("statement", "asserted", cleaned, meta);
    if (!rewrite.empty()) {
        context::append("statement", "resolved", rewrite, meta);
    }
    return persistent
        ? "noted (persistent)"
        : "noted";
}

}
