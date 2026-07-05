#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

// Linter and formatter hub: orchestrator over the external linters /
// formatters most projects already have on PATH (ruff, eslint,
// clippy, shellcheck, gofmt, prettier, clang-format, jq, yamllint).
// Each tool gets a Rule = (extension, mode, tool, command_template);
// plan() fills in the {src} placeholder, lint() / format() actually
// runs the command via 011_scripting::execute. Mirrors the shape of
// 014_format_translators so the workflow is familiar.
namespace linter_hub {

enum class Mode { Lint = 0, Format = 1 };
const char * to_string(Mode);

struct ToolPlan {
    std::string  tool;             // basename, e.g. "ruff"
    Mode         mode;
    std::string  command;          // exact shell command after substitution
    std::string  source_path;
};

struct Pairing {
    std::string  source_ext;
    Mode         mode;
    std::string  tool;
};

struct Result {
    int          exit_code = -1;
    bool         timed_out = false;
    std::string  stdout_text;
    std::string  stderr_text;
    ToolPlan     plan;
};

void init();
void shutdown();

bool tool_available(std::string_view tool);

// All supported (extension, mode) pairs the hub knows about.
const std::vector<Pairing> & supported_pairings();

// Build a plan; nullopt when no rule matches the extension/mode or
// when the required tool is not on PATH.
std::optional<ToolPlan> plan(std::string_view src_path, Mode mode);

// Plan + run. Throws std::runtime_error when no plan can be built.
Result run(std::string_view src_path, Mode mode, int timeout_seconds = 60);

}
