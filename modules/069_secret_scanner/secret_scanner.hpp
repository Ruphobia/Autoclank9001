#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

// Secret scanner: regex + entropy heuristics for committed credentials
// in a working tree. Per-finding kind (AWS access key, GitHub PAT,
// PEM private key, JWT-shaped token, generic high-entropy string) +
// the file/line + a redacted excerpt + a 0..1 confidence.
namespace secret_scanner {

struct Finding {
    std::string  kind;            // "aws-access-key", "github-pat", ...
    std::string  path;
    size_t       line_number = 0;
    std::string  excerpt;         // partially redacted, safe for logging
    double       confidence  = 0.0;
};

struct Options {
    std::vector<std::string> ignore_dirs;
    size_t                   max_findings   = 200;
    size_t                   max_file_bytes = 2 * 1024 * 1024;
    double                   entropy_floor  = 4.0;     // bits/char
    bool                     include_history = false;  // hook for future git-log scan
};

void init();
void shutdown();

// Walk the working tree at `root` recursively (skipping the same
// builtin ignore set as 063_project_search) and report findings.
std::vector<Finding> scan(std::string_view root, const Options & opts = {});

// Scan a single body of text directly.
std::vector<Finding> scan_text(std::string_view body, std::string_view label,
                               const Options & opts = {});

// Shannon entropy bits/char over `s` (0 for empty / single-char).
double shannon_entropy(std::string_view s);

}
