#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

// Decisions folder maintainer: ADR-style entries under
// <project>/.tool/decisions/ so the project's architectural reasoning
// lives somewhere other than chat history. One markdown file per
// decision, sequentially numbered (0001-<slug>.md, 0002-<slug>.md ...)
// per the conventional ADR layout. Status is one of:
//     proposed, accepted, deprecated, superseded
// and supersession is recorded as a bidirectional link.
namespace decisions_folder {

enum class Status {
    Proposed   = 0,
    Accepted   = 1,
    Deprecated = 2,
    Superseded = 3,
};
const char * to_string(Status s);
Status        status_from_string(std::string_view s);

struct Decision {
    int          number = 0;        // ADR ordinal
    std::string  slug;               // url-safe slug
    std::string  title;
    Status       status = Status::Proposed;
    std::string  context;
    std::string  decision;
    std::string  consequences;
    std::vector<int> supersedes;     // numbers of ADRs this one supersedes
    std::vector<int> superseded_by;  // numbers of ADRs that superseded this one
    std::string  body;               // raw markdown
};

void init();
void shutdown();

// Append a new ADR. Auto-assigns the next ordinal. Returns the
// assigned number. `slug` is sanitized to [A-Za-z0-9_-]; required.
int record(std::string_view  slug,
           std::string_view  title,
           std::string_view  context,
           std::string_view  decision_text,
           std::string_view  consequences,
           std::string_view  project_root,
           Status            initial_status = Status::Accepted);

// Flip the Status line on a recorded ADR.
void set_status(int number, Status s, std::string_view project_root);

// Link a supersession: ADR `newer` supersedes ADR `older`. Updates
// both files' status / link sections. Throws if either is missing.
void supersede(int newer, int older, std::string_view project_root);

std::vector<int>             list(std::string_view project_root);
std::optional<Decision>      read(int number, std::string_view project_root);

}
