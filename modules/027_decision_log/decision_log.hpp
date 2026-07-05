#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

// Decision log: append-only journal of project decisions in an
// ADR-like shape (context, options, chosen, consequences) plus
// bidirectional links to memory entries and code locations. Distinct
// from 021_artifact_decisions (which lives as one-file-per-ADR
// markdown and supports mutation / supersession): this module is the
// strict append-only ledger that proves what was decided and when,
// without rewriting history.
//
// Backed by JSONL at <project>/.tool/decision-log/decisions.jsonl.
// Amendments (links added later, status changes) are themselves
// appended as new records so the file is the audit trail.
namespace decision_log {

enum class RecordKind {
    Decision  = 0,    // initial record
    Amendment = 1,    // links / notes added later
    Reverse   = 2,    // explicit reversal of a prior decision
};
const char * to_string(RecordKind k);

struct Record {
    int64_t      id           = 0;     // sequential, monotonically increasing
    int64_t      ts           = 0;     // unix seconds
    RecordKind   kind         = RecordKind::Decision;
    int64_t      parent_id    = 0;     // 0 for kind=Decision, original id otherwise

    std::string  title;
    std::string  context;
    std::vector<std::string> options;        // strings, "name: description"
    std::string  chosen;                     // one of options' names
    std::string  consequences;

    std::vector<int64_t>     linked_entries; // ids in 022/023 memory stores
    std::vector<std::string> linked_code;    // "commit:<sha>", "file:<path>:<line>"
    std::string              note;           // amendment / reverse rationale
};

void init();
void shutdown();

// Append a new decision. Returns its assigned id.
int64_t record(std::string_view              title,
               std::string_view              context,
               const std::vector<std::string> & options,
               std::string_view              chosen,
               std::string_view              consequences,
               const std::vector<int64_t>    & linked_entries,
               const std::vector<std::string> & linked_code,
               std::string_view              project_root);

// Append an Amendment to an existing decision (new links / new note).
// Does not mutate the original record. Returns the amendment's id.
int64_t amend(int64_t decision_id,
              const std::vector<int64_t>    & added_entries,
              const std::vector<std::string> & added_code,
              std::string_view              note,
              std::string_view              project_root);

// Explicitly reverse a prior decision. Appended as kind=Reverse.
int64_t reverse(int64_t decision_id,
                std::string_view note,
                std::string_view project_root);

// Read all records in append order.
std::vector<Record> all(std::string_view project_root);

// Read one record by id.
std::optional<Record> get(int64_t id, std::string_view project_root);

// Returns the original Decision plus a list of every Amendment /
// Reverse whose parent_id matches `decision_id`. Convenience for the
// UI "show the full history of this decision" view.
struct History {
    Record              decision;
    std::vector<Record> amendments;
    std::vector<Record> reversals;
};
std::optional<History> history(int64_t decision_id, std::string_view project_root);

}
