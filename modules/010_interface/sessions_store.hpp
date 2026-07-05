// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

// Manages per-session UI state and the picker list. Pairs with context.hpp
// (which owns the per-session SQLite chat history). One browser session =
// `<id>.json` (UI state blob) + `<id>.sqlite` (chat) under the directory
// returned by context::sessions_dir().
namespace sessions_store {

struct SessionMeta {
    std::string id;
    std::string name;
    std::string root_dir;
    int64_t     created_at  = 0;     // unix ms
    int64_t     last_active = 0;     // unix ms
    int64_t     message_count = 0;   // user/answer rows in <id>.sqlite
};

struct ChatMessage {
    std::string role;     // "user" or "ai"
    std::string text;
    int64_t     ts = 0;
    int64_t     turn = 0;
};

// List every session known on disk, sorted by last_active descending.
std::vector<SessionMeta> list();

// Create a fresh session (UUID, empty JSON blob, empty SQLite). Returns the
// new session's meta. Does NOT switch the context module to it; that's the
// caller's job if it wants the new session to become active.
SessionMeta create(std::string_view name = {}, std::string_view root_dir = {});

// Read `<id>.json` and return its UI-state blob as a JSON string (or "{}"
// if absent / unreadable). The blob is whatever the client PUT last; we
// don't interpret it.
std::string read_ui(std::string_view id);

// Overwrite `<id>.json` with `ui_json_blob`. Atomic (tmp+rename). Touches
// last_active.
void write_ui(std::string_view id, std::string_view ui_json_blob);

// Rename / set root_dir without touching the UI blob.
void patch(std::string_view id,
           std::string_view name,
           std::string_view root_dir);

// Update last_active = now_ms. Cheap.
void touch(std::string_view id);

// Delete this session's files (json + sqlite + WAL/SHM). Idempotent.
// Returns true if anything was actually deleted.
bool forget(std::string_view id);

// Read the chat history (user/answer rows) from <id>.sqlite, oldest first.
// If id == current active session, we read directly from the live handle
// via context; otherwise we open the file read-only just for this call.
std::vector<ChatMessage> chat_history(std::string_view id);

struct TurnRow {
    int64_t     ts   = 0;
    int64_t     turn = 0;
    std::string layer;
    std::string kind;
    std::string meta;
    std::string content;
};

// Every context row for this session, oldest first. Backs the session
// restore path, which rebuilds the full chat rendering (headline plus
// thinking chain) instead of replaying flat text.
std::vector<TurnRow> turn_rows(std::string_view id);

// True iff `<id>.sqlite` exists under the sessions dir AND id looks like a
// uuid (so we don't get tricked by `session_1234.sqlite` legacy archives).
bool exists(std::string_view id);

}
