#pragma once

#include <string>
#include <string_view>
#include <vector>

// Scripting tool: persistent named bash scripts that automate work across
// the other tools. The intended UI flow is "AI drafts, user approves, the
// scripting tool runs"; the runner here is the deterministic piece, free
// of any model dependency so it stays usable from the test harness and
// from manual invocation. The optional `draft` entry point hooks in the
// coder model when callers want AI-assisted authoring.
namespace scripting {

struct Script {
    std::string name;          // basename only; no slashes
    std::string description;   // human-facing one-liner
    std::string body;          // bash text, no shebang required
};

struct ExecResult {
    std::string stdout_text;
    std::string stderr_text;
    int  exit_code = -1;
    bool timed_out = false;
};

// Idempotent. No model loads; safe to call from a smoke test.
void init();
void shutdown();

// Run `body` as a bash -c script in `cwd` (current directory if empty)
// with a wall-clock timeout in seconds. Captures stdout and stderr
// separately. If the timeout fires the script is killed with SIGTERM,
// then SIGKILL one second later, and `timed_out` is set true.
ExecResult execute(std::string_view body,
                   std::string_view cwd = {},
                   int              timeout_seconds = 30);

// Quick static check: rejects empty bodies, flags obvious unterminated
// quotes, flags well-known footguns (rm -rf /, dd of=/dev/sd*, fork
// bombs). Returns one human-readable string per concern. An empty
// return value means no concerns were found, not "definitely safe".
std::vector<std::string> validate(std::string_view body);

// Persistence under <project_root>/.tool/scripts/. The script body is
// written to <name>.sh and a JSON sidecar holds the description. Names
// are sanitized to [A-Za-z0-9_-]; save() throws if `s.name` is empty
// after sanitization.
void                     save(const Script & s, std::string_view project_root);
Script                   load(std::string_view name, std::string_view project_root);
std::vector<std::string> list(std::string_view project_root);

// Optional AI authoring. Asks the coder model for a bash script that
// satisfies `intent`. REQUIRES coder::init() to have been called first.
// Returns the raw script text; callers should run it past validate()
// and present it to the user before execute().
std::string draft(std::string_view intent);

}
