#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

// Legal folder maintainer: one markdown file per regulatory pathway
// flagged in the project, stored under
// <project>/.tool/legal/pathways/<id>.md. The advisor in 016_safety
// can call has_been_explained(id) before re-emitting the same citation
// block, and the human can hand-edit the files (application numbers,
// fee receipts, agency contacts, dates) without breaking the
// maintainer's append-only event log.
namespace legal_folder {

enum class EventKind {
    Status   = 0,   // "submitted", "approved", "denied", "withdrawn", ...
    Followup = 1,
    Contact  = 2,
    Fee      = 3,
    Note     = 4,
};

const char * to_string(EventKind k);

struct Entry {
    std::string id;
    std::string jurisdiction;
    std::string cite;
    std::string body;
    size_t      status_count   = 0;
    size_t      followup_count = 0;
    size_t      contact_count  = 0;
    size_t      fee_count      = 0;
};

void init();
void shutdown();

bool has_been_explained(std::string_view id, std::string_view project_root);

// Idempotent. Creates the file with header + Status / Followups /
// Contacts / Fees / Notes sections if missing; updates the Last
// touched stamp if present.
void record_pathway(std::string_view id,
                    std::string_view jurisdiction,
                    std::string_view cite,
                    std::string_view summary,
                    std::string_view project_root);

// Append a dated bullet to the appropriate section. Auto-creates the
// file (with a stub header) if no prior record exists.
void log_event(std::string_view id,
               EventKind         kind,
               std::string_view note,
               std::string_view project_root);

std::vector<std::string>     list_known(std::string_view project_root);
std::optional<Entry>         read(std::string_view id, std::string_view project_root);

}
