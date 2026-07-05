#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

// File format translators: thin orchestrator over the system's converter
// CLIs (gzip, tar, zip, pandoc, ImageMagick convert, ffmpeg, 7z, etc.).
// The point is not to reimplement those tools; it is to give the chat
// pipeline one entry point that "picks the right converter for this
// extension pair, fills in the command template, and runs it", with
// a dry-run mode so the user can see the command before it runs.
//
// Extension is the primary signal; callers that want stronger format
// identification can prefilter using 013_hex_editor::detect_format.
namespace translators {

struct TranslationPlan {
    std::string tool;            // basename of the external tool, e.g. "gzip"
    std::string command;         // exact shell command that would be run
    std::string source_path;
    std::string target_path;
};

// No model load. Translation calls scripting::execute.
void init();
void shutdown();

// Returns true if `tool` is on PATH.
bool tool_available(std::string_view tool);

// All extension pairs the translator knows how to handle. Each entry
// has the source extension (lowercase, no dot), target extension, and
// the tool it would use. Useful for "what can we convert this to?".
struct Pairing {
    std::string source_ext;
    std::string target_ext;
    std::string tool;
};
const std::vector<Pairing> & supported_pairings();

// Plan a translation: looks up the (src_ext, dst_ext) pair, fills in
// the command template, and returns it. nullopt if no rule exists or
// if the required tool is not installed.
std::optional<TranslationPlan> plan(std::string_view src_path,
                                    std::string_view dst_path);

// Plan and execute. `timeout_seconds` is forwarded to scripting::execute.
// Returns 0 and writes the plan when successful; non-zero on tool exit
// failure; throws when no rule exists.
struct Result {
    int          exit_code = -1;
    bool         timed_out = false;
    std::string  stdout_text;
    std::string  stderr_text;
    TranslationPlan plan;
};
Result translate(std::string_view src_path,
                 std::string_view dst_path,
                 int              timeout_seconds = 60);

}
