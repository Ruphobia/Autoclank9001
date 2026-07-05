#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

// People and Relationships: a lightweight CRM-style store for
// collaborators and contacts. Records carry primary name, aliases,
// role, affiliations, free-form notes, and a back-reference set of
// memory entry ids that mention this person. Two scopes: Project (the
// people relevant to one working tree) and Personal (recurring
// contacts the user knows across projects).
//
// Stored as JSON: <project>/.tool/people/people.json for Project, and
// $TOOL_PERSONAL_DIR / $HOME/.tool/personal/people.json for Personal.
// The format is trivial to read, edit, and back up.
namespace people {

enum class Scope { Project = 0, Personal = 1 };

struct Person {
    int64_t                  id     = 0;
    std::string              name;             // primary display name
    std::vector<std::string> aliases;          // other names / handles
    std::string              role;             // e.g. "EE", "dealer", "PI"
    std::vector<std::string> affiliations;     // org / company / project names
    std::string              notes;            // free-form
    std::vector<int64_t>     linked_entries;   // entry ids in 022/023 stores
};

void init();
void shutdown();

int64_t add   (Scope, const Person & p, std::string_view project_root);
void    update(Scope, int64_t id, const Person & p, std::string_view project_root);
void    forget(Scope, int64_t id, std::string_view project_root);

std::optional<Person>  get (Scope, int64_t id, std::string_view project_root);
std::vector<Person>    list(Scope, std::string_view project_root);

// Case-insensitive lookup by name or any alias. Returns the first hit.
std::optional<Person>  find(Scope, std::string_view query, std::string_view project_root);

// Scan `text` for every person whose name or alias appears as a
// case-insensitive substring. Returns ids in order of first occurrence.
std::vector<int64_t>   find_mentions(Scope, std::string_view text,
                                     std::string_view project_root);

// Append entry_id to a person's linked_entries (idempotent).
void link_entry(Scope, int64_t person_id, int64_t entry_id,
                std::string_view project_root);

// Resolved store path for callers / tests.
std::string store_path(Scope, std::string_view project_root);

}
