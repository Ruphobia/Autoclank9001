#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

// Privacy and Scope Guard: per-entry visibility labels plus a preflight
// check that warns before any entry leaves the local store via sync,
// share, or model context. The labels are advisory in the sense that
// nothing here actually performs the sync / share; callers must call
// preflight() before crossing a destination boundary and respect the
// blocked list. The default label is AssistantVisible, which permits
// model_context but blocks export-style destinations.
namespace scope_guard {

enum class Scope { Project = 0, Personal = 1 };

enum class Label {
    LocalOnly        = 0,    // never leaves the local store
    ProjectShared    = 1,    // OK to sync within the project tree
    AssistantVisible = 2,    // OK to send to a model in the local pipeline (DEFAULT)
    Redacted         = 3,    // never leaves; UI may not even display
};

enum class Destination {
    Sync         = 0,    // remote sync / git push of memory contents
    Share        = 1,    // export / link to a third party
    ModelContext = 2,    // send to a (local) answering model
    Display      = 3,    // render in the UI
};

const char * to_string(Label);
const char * to_string(Destination);
Label        label_from_string(std::string_view);

struct Decision {
    int64_t      id;
    Label        label;
    bool         allowed;
    std::string  reason;
};

struct Verdict {
    std::vector<Decision> allowed;
    std::vector<Decision> blocked;
};

void init();
void shutdown();

void  set_label(Scope, int64_t id, Label, std::string_view project_root);
Label get_label(Scope, int64_t id, std::string_view project_root);

// Run the preflight check. allowed contains the entries that may
// leave via `dest`; blocked carries every other entry plus a
// human-readable reason. Default rules (Label cross Destination):
//   LocalOnly         : blocks Sync / Share; allows ModelContext / Display
//   ProjectShared     : allows Sync; blocks Share; allows ModelContext / Display
//   AssistantVisible  : blocks Sync / Share; allows ModelContext / Display
//   Redacted          : blocks everything
Verdict preflight(Scope scope, const std::vector<int64_t> & ids,
                  Destination dest, std::string_view project_root);

}
