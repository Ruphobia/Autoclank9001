#pragma once

#include <functional>
#include <string>
#include <string_view>
#include <vector>

namespace shell_tool {

struct Result {
    std::string command;        // the shell command the coder produced
    std::string stdout_text;    // captured stdout+stderr from execution
    int         exit_code = -1;
    // Absolute paths of every file WRITEFILE'd (or refused-for-existing)
    // during this call. execute_plan uses this to carry forward the "we
    // already saw this file" set into subsequent steps so a step 2 rewrite
    // of a step 1 file is treated as an intentional edit (the current
    // content is inlined into the coder prompt) instead of a hallucinated
    // blind overwrite.
    std::vector<std::string> written_files;
};

// Idempotent. Loads the coder model on GPU 1 (Qwen2.5-Coder-14B). REQUIRES
// prompt_cleanup::init() to have been called first (for the ggml backend).
void init();
void shutdown();

// Translate `request` into a shell command via the coder model, execute it
// with bash -c in `cwd` (when non-empty; otherwise the server's cwd), and
// return both the command and its combined stdout/stderr plus exit code.
// `history` (optional) is a bullet list of the session's EARLIER user
// requests, oldest first; the coder is stateless, and without it every
// turn re-decides the stack from scratch and drops constraints the user
// stated once (language, ports, folder layout).
// Throws std::runtime_error on coder failure; shell execution failures are
// reported via exit_code, not exceptions.
Result execute(std::string_view request, std::string_view cwd = {},
               std::string_view history = {},
               const std::vector<std::string> * carry_files = nullptr);

// Testing hook: parse coder output into its executable segments WITHOUT
// executing anything, one rendered line per segment:
//   "SHELL <first line of the chunk>"  /  "FILE <path> (<n> bytes)"
// Exists so tool_test can pin the parser's edge cases (inline
// "&& WRITEFILE" splitting, fences); the live path uses the same parser.
std::vector<std::string> debug_segments(const std::string & coder_output);

// True when `request` reads like "fix these build/compile errors".
bool is_build_fix(std::string_view request);

// Compile-fix loop: build the project in `cwd`, and while the build fails
// (up to a few rounds) hand the real errors back to the coder, apply its
// fix, and rebuild. `note(name, content)` streams one trail entry per
// build attempt / fix so the user watches the loop work. The returned
// Result carries the FINAL build outcome: exit 0 only when the build
// genuinely succeeded.
Result fix_build(std::string_view request, std::string_view cwd,
                 const std::function<void(const std::string &,
                                          const std::string &)> & note,
                 std::string_view history = {},
                 const std::vector<std::string> * carry_files = nullptr);

// Multi-step requests: plan first, then execute each step separately.
// One giant coder call on a compound request ("remove X; create Y;
// build Z inside it; verify it compiles") has produced whole parallel
// build systems in subfolders and silently dropped the trailing verify
// clause. The planner breaks the request into an ordered checklist
// (emitted via `note` as a "tasks" trail entry), runs each step through
// execute(), routes verify/compile steps through fix_build(), and stops
// at the first failing step. Falls back to plain execute() when the
// planner returns fewer than two steps.
Result execute_plan(std::string_view request, std::string_view cwd,
                    std::string_view history,
                    const std::function<void(const std::string &,
                                             const std::string &)> & note);

}
