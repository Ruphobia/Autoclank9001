// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace context {

struct Record {
    int64_t     id   = 0;     // sqlite rowid; 0 for uninitialised
    int64_t     ts   = 0;     // unix milliseconds at insertion
    int64_t     turn = 0;     // REPL turn number this record belongs to
    std::string layer;        // e.g. "user", "cleanup", "stylize", "expertise"
    std::string kind;         // e.g. "input", "output", "label", "interpretation"
    std::string content;      // searched by full-text index
    std::string meta;         // optional JSON for layer-specific extras
};

// Open the active browser session. Idempotent.
// If <path_dir>/active.txt exists, opens the session it points at.
// Otherwise generates a fresh UUID, writes active.txt, and opens
// <path_dir>/<uuid>.sqlite.
//
// path_dir: directory to hold session files (default: "005_context/sessions").
// Creates the directory if missing.
void init(std::string_view path_dir = {});

// Close any open handle. Safe to call multiple times.
void shutdown();

// Roll to a brand-new session in the same directory. Generates a new UUID,
// updates active.txt, closes the current handle, opens the new file.
// (Old session files are NOT archived - they stay on disk and remain visible
// to the multi-session picker.)
void new_session();

// Close the current handle (if any) and open <path_dir>/<id>.sqlite as the
// active session. Updates active.txt. Creates the file with schema if it
// doesn't exist yet. Resets the per-process turn counter.
void switch_to(std::string_view id);

// UUID of the currently-active session (the basename of the open .sqlite,
// without the extension). Empty string if no session is open.
std::string current_id();

// Absolute directory holding all session files.
std::string sessions_dir();

// Begin a new REPL turn; returns its turn number. All subsequent append()
// calls without an explicit turn arg use this turn until next_turn() is
// called again.
int64_t next_turn();

// Current turn id (0 until next_turn() is called once).
int64_t current_turn();

// Append a record under the current turn (set by next_turn()).
void append(std::string_view layer,
            std::string_view kind,
            std::string_view content,
            std::string_view meta = {});

// Retrieval ----------------------------------------------------------------

// Most-recent N records, newest first.
std::vector<Record> recent(int n);

// All records belonging to a specific turn (chronological order).
std::vector<Record> by_turn(int64_t turn);

// Delete every record of a turn (user input and all layer rows). Used by
// the chat stop button: a halted turn is handed back to the user for
// editing, so it must not replay from the session store.
void delete_turn(int64_t turn);

// All records emitted by a specific layer (most-recent first), capped.
std::vector<Record> by_layer(std::string_view layer, int limit = 100);

// FTS5 full-text search over `content`, ordered by BM25 relevance.
// `query` is an FTS5 MATCH expression; pass a plain phrase for simple search.
std::vector<Record> search(std::string_view query, int top_k);

// Total record count in the current session.
int64_t count();

// Absolute filesystem path of the active session file.
std::string current_session_path();

// Compact chronological dump of the last `recent_n` records, with noisy
// layers (dictionary, stylize) filtered out. Suitable for inlining into
// LLM prompts. Returns "(memory is empty)\n" when nothing matches.
std::string render_for_prompt(int recent_n = 40);

}
