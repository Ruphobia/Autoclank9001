#include "secret_scanner.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

namespace secret_scanner {
namespace {

namespace fs = std::filesystem;

const std::unordered_set<std::string> & builtin_ignore_dirs() {
    static const std::unordered_set<std::string> s = {
        ".git", ".svn", ".hg", "_deps", "CMakeFiles", "build",
        "node_modules", "venv", ".venv", ".env", "__pycache__",
        "target", ".cache", ".tool"
    };
    return s;
}

bool ignored_dir(const std::string & name,
                 const std::vector<std::string> & extra) {
    if (!name.empty() && name.front() == '.' && name != "." && name != "..") return true;
    if (builtin_ignore_dirs().count(name)) return true;
    for (const auto & e : extra) if (e == name) return true;
    return false;
}

bool looks_binary(const std::string & body) {
    size_t n = std::min<size_t>(body.size(), 2048);
    for (size_t i = 0; i < n; ++i) {
        if ((unsigned char)body[i] == 0) return true;
    }
    return false;
}

std::string redact(std::string_view raw, size_t keep_each_side = 4) {
    if (raw.size() <= keep_each_side * 2 + 3) return std::string(raw.size(), '*');
    std::string out;
    out += raw.substr(0, keep_each_side);
    out += "...";
    out += std::string(raw.substr(raw.size() - keep_each_side));
    return out;
}

struct PatternRule {
    const char * kind;
    const char * regex;
    double       confidence;
};

const std::array<PatternRule, 7> kRules = {{
    {"aws-access-key",      R"(\bAKIA[0-9A-Z]{16}\b)",                                    0.95},
    {"aws-secret-key",      R"(aws_secret_access_key\s*=\s*['"]?[A-Za-z0-9/+=]{40}['"]?)", 0.85},
    {"github-pat",          R"(\bghp_[A-Za-z0-9]{36}\b)",                                 0.95},
    {"github-fine-grained", R"(\bgithub_pat_[A-Za-z0-9_]{82}\b)",                         0.95},
    {"slack-token",         R"(\bxox[abposr]-[A-Za-z0-9-]{10,72}\b)",                     0.90},
    {"jwt",                 R"(\beyJ[A-Za-z0-9_-]{8,}\.[A-Za-z0-9_-]{8,}\.[A-Za-z0-9_-]{8,}\b)", 0.80},
    {"private-key-pem",     R"(-----BEGIN [A-Z ]+PRIVATE KEY-----)",                      0.99},
}};

}

double shannon_entropy(std::string_view s) {
    if (s.size() < 2) return 0.0;
    std::unordered_map<char, int> counts;
    for (char c : s) ++counts[c];
    const double inv = 1.0 / (double)s.size();
    double h = 0.0;
    for (auto & [_, c] : counts) {
        double p = c * inv;
        h -= p * (std::log(p) / std::log(2.0));
    }
    return h;
}

void init()     {}
void shutdown() {}

std::vector<Finding> scan_text(std::string_view body_v, std::string_view label,
                               const Options & opts) {
    std::vector<Finding> out;
    std::string body{body_v};
    if (looks_binary(body)) return out;

    auto try_rules = [&](const std::string & line, size_t line_no) {
        for (const auto & rule : kRules) {
            try {
                std::regex re(rule.regex);
                auto begin = std::sregex_iterator(line.begin(), line.end(), re);
                for (auto it = begin; it != std::sregex_iterator(); ++it) {
                    Finding f;
                    f.kind        = rule.kind;
                    f.path        = std::string(label);
                    f.line_number = line_no;
                    f.excerpt     = redact(it->str());
                    f.confidence  = rule.confidence;
                    out.push_back(std::move(f));
                    if (out.size() >= opts.max_findings) return;
                }
            } catch (...) {}
        }
    };

    auto try_entropy = [&](const std::string & line, size_t line_no) {
        // Look for long-ish quoted strings or assignment values.
        static const std::regex tok(R"(['"]([A-Za-z0-9+/=_\-]{24,})['"])");
        auto begin = std::sregex_iterator(line.begin(), line.end(), tok);
        for (auto it = begin; it != std::sregex_iterator(); ++it) {
            std::string token = (*it)[1].str();
            double e = shannon_entropy(token);
            if (e < opts.entropy_floor) continue;
            Finding f;
            f.kind        = "high-entropy-string";
            f.path        = std::string(label);
            f.line_number = line_no;
            f.excerpt     = redact(token);
            // Confidence scales with entropy headroom over the floor.
            f.confidence  = std::min(0.85, 0.4 + (e - opts.entropy_floor) * 0.1);
            out.push_back(std::move(f));
            if (out.size() >= opts.max_findings) return;
        }
    };

    size_t line_start = 0;
    size_t line_no    = 1;
    for (size_t i = 0; i <= body.size(); ++i) {
        if (i == body.size() || body[i] == '\n') {
            std::string line(body.data() + line_start, i - line_start);
            try_rules(line, line_no);
            if (out.size() < opts.max_findings) try_entropy(line, line_no);
            line_start = i + 1;
            ++line_no;
            if (out.size() >= opts.max_findings) break;
        }
    }
    return out;
}

std::vector<Finding> scan(std::string_view root_v, const Options & opts) {
    std::vector<Finding> out;
    fs::path root{std::string(root_v)};
    if (!fs::exists(root)) return out;

    for (auto it = fs::recursive_directory_iterator(root,
             fs::directory_options::skip_permission_denied);
         it != fs::recursive_directory_iterator(); ++it) {
        if (out.size() >= opts.max_findings) break;
        std::string name = it->path().filename().string();
        if (it->is_directory()) {
            if (ignored_dir(name, opts.ignore_dirs)) it.disable_recursion_pending();
            continue;
        }
        if (!it->is_regular_file()) continue;
        if ((size_t)fs::file_size(it->path()) > opts.max_file_bytes) continue;

        std::ifstream in(it->path(), std::ios::binary);
        std::ostringstream b; b << in.rdbuf();
        auto findings = scan_text(b.str(), it->path().string(), opts);
        for (auto & f : findings) {
            out.push_back(std::move(f));
            if (out.size() >= opts.max_findings) break;
        }
    }
    return out;
}

}
