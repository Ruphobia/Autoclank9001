#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

// Project search: ripgrep-style content search over a directory tree.
// In-process std::regex over UTF-8 bytes; honors a small built-in
// ignore set (.git, _deps, CMake artifact dirs) plus a caller-supplied
// extension filter. Structural / AST queries and AI rephrasing are
// future layers on top of this surface.
namespace project_search {

struct Match {
    std::string  path;
    size_t       line_number  = 0;     // 1-based
    std::string  line_text;            // the matched line, no trailing newline
    size_t       match_start  = 0;     // byte offset within line_text
    size_t       match_length = 0;
};

struct Options {
    std::vector<std::string> include_extensions;   // empty = all text-shaped files
    std::vector<std::string> ignore_dirs;          // appended to the built-ins
    size_t                   max_matches    = 1000;
    bool                     case_insensitive = false;
    size_t                   max_file_bytes = 4 * 1024 * 1024;
};

void init();
void shutdown();

// Search `root` recursively for `pattern` (ECMAScript regex). Returns
// matches in stable, sorted (path, line) order.
std::vector<Match> search(std::string_view root,
                          std::string_view pattern,
                          const Options & opts = {});

// Convenience that wraps the user's literal `phrase` with regex escape
// so callers can do "fuzzy contains" without pre-escaping themselves.
std::vector<Match> search_literal(std::string_view root,
                                  std::string_view phrase,
                                  const Options & opts = {});

}
