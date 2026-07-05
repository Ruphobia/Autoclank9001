#include "linter_hub.hpp"

#include "../011_scripting/scripting.hpp"

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <stdexcept>

namespace linter_hub {
namespace {

namespace fs = std::filesystem;

std::string lower_ext(std::string_view path) {
    fs::path p{std::string(path)};
    auto ext = p.extension().string();
    if (!ext.empty() && ext.front() == '.') ext.erase(0, 1);
    std::transform(ext.begin(), ext.end(), ext.begin(),
                   [](unsigned char c){ return (char)std::tolower(c); });
    return ext;
}

std::string shell_quote(std::string_view s) {
    std::string out;
    out.reserve(s.size() + 2);
    out.push_back('\'');
    for (char c : s) {
        if (c == '\'') out.append("'\\''");
        else           out.push_back(c);
    }
    out.push_back('\'');
    return out;
}

std::string fill(std::string_view tmpl, std::string_view src) {
    std::string out;
    for (size_t i = 0; i < tmpl.size();) {
        if (tmpl[i] == '{' && i + 4 < tmpl.size() &&
            tmpl.compare(i, 5, "{src}") == 0) {
            out += shell_quote(src);
            i += 5;
        } else {
            out.push_back(tmpl[i++]);
        }
    }
    return out;
}

struct Rule {
    const char * ext;
    Mode         mode;
    const char * tool;
    const char * cmd_template;
};

const Rule kRules[] = {
    // Python
    {"py",   Mode::Lint,   "ruff",        "ruff check -- {src}"},
    {"py",   Mode::Format, "ruff",        "ruff format -- {src}"},

    // JavaScript / TypeScript
    {"js",   Mode::Lint,   "eslint",      "eslint -- {src}"},
    {"jsx",  Mode::Lint,   "eslint",      "eslint -- {src}"},
    {"ts",   Mode::Lint,   "eslint",      "eslint -- {src}"},
    {"tsx",  Mode::Lint,   "eslint",      "eslint -- {src}"},
    {"js",   Mode::Format, "prettier",    "prettier --write -- {src}"},
    {"jsx",  Mode::Format, "prettier",    "prettier --write -- {src}"},
    {"ts",   Mode::Format, "prettier",    "prettier --write -- {src}"},
    {"tsx",  Mode::Format, "prettier",    "prettier --write -- {src}"},

    // Rust (cargo clippy / cargo fmt are workspace-wide; we run from the
    // file's parent so the workspace root is found).
    {"rs",   Mode::Lint,   "cargo",       "(cd \"$(dirname {src})\" && cargo clippy --quiet)"},
    {"rs",   Mode::Format, "rustfmt",     "rustfmt -- {src}"},

    // Shell
    {"sh",   Mode::Lint,   "shellcheck",  "shellcheck -- {src}"},
    {"sh",   Mode::Format, "shfmt",       "shfmt -w -- {src}"},
    {"bash", Mode::Lint,   "shellcheck",  "shellcheck -- {src}"},
    {"bash", Mode::Format, "shfmt",       "shfmt -w -- {src}"},

    // Go
    {"go",   Mode::Lint,   "golangci-lint", "golangci-lint run -- {src}"},
    {"go",   Mode::Format, "gofmt",       "gofmt -w -- {src}"},

    // C / C++
    {"c",    Mode::Format, "clang-format","clang-format -i -- {src}"},
    {"cc",   Mode::Format, "clang-format","clang-format -i -- {src}"},
    {"cpp",  Mode::Format, "clang-format","clang-format -i -- {src}"},
    {"cxx",  Mode::Format, "clang-format","clang-format -i -- {src}"},
    {"h",    Mode::Format, "clang-format","clang-format -i -- {src}"},
    {"hpp",  Mode::Format, "clang-format","clang-format -i -- {src}"},
    {"c",    Mode::Lint,   "cppcheck",    "cppcheck --enable=warning -- {src}"},
    {"cpp",  Mode::Lint,   "cppcheck",    "cppcheck --enable=warning -- {src}"},

    // JSON / YAML / Markdown / CSS / HTML formatting via prettier.
    {"json", Mode::Format, "prettier",    "prettier --write -- {src}"},
    {"yaml", Mode::Format, "prettier",    "prettier --write -- {src}"},
    {"yml",  Mode::Format, "prettier",    "prettier --write -- {src}"},
    {"md",   Mode::Format, "prettier",    "prettier --write -- {src}"},
    {"css",  Mode::Format, "prettier",    "prettier --write -- {src}"},
    {"html", Mode::Format, "prettier",    "prettier --write -- {src}"},
    {"yaml", Mode::Lint,   "yamllint",    "yamllint -- {src}"},
    {"yml",  Mode::Lint,   "yamllint",    "yamllint -- {src}"},
    {"json", Mode::Lint,   "jq",          "jq empty -- {src}"},
};

const Rule * find_rule(std::string_view ext, Mode mode) {
    for (const auto & r : kRules) {
        if (r.mode == mode && ext == r.ext) return &r;
    }
    return nullptr;
}

}

const char * to_string(Mode m) {
    return m == Mode::Lint ? "lint" : "format";
}

void init()     {}
void shutdown() {}

bool tool_available(std::string_view tool) {
    std::string cmd = "command -v ";
    cmd += shell_quote(tool);
    cmd += " >/dev/null 2>&1";
    return std::system(cmd.c_str()) == 0;
}

const std::vector<Pairing> & supported_pairings() {
    static const std::vector<Pairing> v = []{
        std::vector<Pairing> out;
        for (const auto & r : kRules) out.push_back({r.ext, r.mode, r.tool});
        return out;
    }();
    return v;
}

std::optional<ToolPlan> plan(std::string_view src_path, Mode mode) {
    const Rule * r = find_rule(lower_ext(src_path), mode);
    if (!r) return std::nullopt;
    if (!tool_available(r->tool)) return std::nullopt;
    ToolPlan p;
    p.tool        = r->tool;
    p.mode        = mode;
    p.source_path = std::string(src_path);
    p.command     = fill(r->cmd_template, src_path);
    return p;
}

Result run(std::string_view src_path, Mode mode, int timeout_seconds) {
    auto p = plan(src_path, mode);
    if (!p) {
        throw std::runtime_error("linter_hub::run: no rule (or tool missing) for " +
                                 lower_ext(src_path) + " mode=" + to_string(mode));
    }
    Result r;
    r.plan = *p;
    auto er = scripting::execute(p->command, {}, timeout_seconds);
    r.stdout_text = er.stdout_text;
    r.stderr_text = er.stderr_text;
    r.exit_code   = er.exit_code;
    r.timed_out   = er.timed_out;
    return r;
}

}
