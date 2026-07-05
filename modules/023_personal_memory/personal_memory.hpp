#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

// Personal / editable memory: the user-scoped store that follows the
// user across projects. Preferences, style notes, recurring
// jurisdictions, owned equipment, accepted risk profile, recurring
// contacts. Mirrors Claude Code's user-memory model: visible, editable,
// scoped to the user account rather than to one project.
//
// Stored at $TOOL_PERSONAL_DIR or, when that environment variable is
// unset, $HOME/.tool/personal/, so the store is one file regardless of
// which project the user is currently working in. The data shape
// mirrors 022_project_memory (Entry / Hit) deliberately; the two
// stores can be merged into a single view at the call site.
namespace personal_memory {

struct Entry {
    int64_t                  id          = 0;
    int64_t                  ts          = 0;
    std::string              category;     // "preference", "equipment", "contact", ...
    std::string              content;
    std::vector<std::string> tags;
    std::string              source;
};

struct Hit {
    Entry        entry;
    double       score = 0.0;
    std::string  snippet;
};

// Opens (or creates) the user-scoped store. Reads $TOOL_PERSONAL_DIR
// first, falls back to $HOME/.tool/personal/. Pass `override_root`
// non-empty to force a specific directory (useful in tests).
void open(std::string_view override_root = {});
void close();

// Where did open() actually land? Useful for the UI "where is this
// stored?" indicator. Empty if not open.
std::string store_path();

int64_t add(std::string_view              category,
            std::string_view              content,
            const std::vector<std::string> & tags    = {},
            std::string_view              source     = {});

void update(int64_t id, std::string_view content,
            const std::vector<std::string> & tags,
            std::string_view source);

void forget(int64_t id);

std::vector<Entry> list(std::string_view category = {},
                        std::string_view tag      = {},
                        size_t           limit    = 100);
std::vector<Hit>   search(std::string_view query, size_t top_k = 8);
std::optional<Entry> get(int64_t id);

// Plain-markdown export of the entire store. The format is intended
// to be re-importable by Obsidian / Logseq / a text editor.
std::string export_markdown();

}
