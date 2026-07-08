// SPDX-License-Identifier: GPL-3.0-or-later
#include "context.hpp"

#include "sqlite3.h"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

namespace context {
namespace {

namespace fs = std::filesystem;

std::mutex      g_mtx;
sqlite3 *       g_db   = nullptr;
fs::path        g_dir;
fs::path        g_path;
int64_t         g_turn = 0;
std::string     g_id;          // UUID of the active session (basename of g_path)

std::string make_uuid_v4() {
    // Crypto strength isn't required; uniqueness within a few seconds is.
    static std::random_device rd;
    static std::mt19937_64    rng{
        ((uint64_t)rd() << 32) ^ (uint64_t)rd() ^
        (uint64_t)std::chrono::steady_clock::now().time_since_epoch().count()
    };
    auto hex16 = [](uint16_t v) {
        char b[5]; std::snprintf(b, sizeof(b), "%04x", v);
        return std::string(b);
    };
    auto hex32 = [](uint32_t v) {
        char b[9]; std::snprintf(b, sizeof(b), "%08x", v);
        return std::string(b);
    };
    auto hex48 = [](uint64_t v) {
        char b[13]; std::snprintf(b, sizeof(b), "%012lx", (unsigned long)(v & 0xffffffffffffull));
        return std::string(b);
    };
    uint64_t a = rng(), b = rng();
    return hex32((uint32_t)(a >> 32))                + "-" +
           hex16((uint16_t)((a >> 16) & 0xffff))     + "-" +
           "4" + hex16((uint16_t)(a & 0xfff)).substr(1) + "-" +  // version 4
           hex16((uint16_t)(((b >> 48) & 0x3fff) | 0x8000)) + "-" +  // variant 1
           hex48(b);
}

void write_active_file_locked() {
    std::ofstream f(g_dir / "active.txt", std::ios::trunc);
    if (f) f << g_id;
}
std::string read_active_file_in(const fs::path & dir) {
    std::ifstream f(dir / "active.txt");
    if (!f) return {};
    std::string s; std::getline(f, s);
    // Strip any stray whitespace.
    while (!s.empty() && (s.back() == '\n' || s.back() == '\r' || s.back() == ' '))
        s.pop_back();
    return s;
}

int64_t now_ms() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()).count();
}

void check(int rc, const char * what) {
    if (rc == SQLITE_OK || rc == SQLITE_DONE || rc == SQLITE_ROW) return;
    const char * msg = g_db ? sqlite3_errmsg(g_db) : sqlite3_errstr(rc);
    throw std::runtime_error(
        std::string("context: ") + what + ": " + (msg ? msg : "(no message)"));
}

void exec(const char * sql) {
    char * err = nullptr;
    int rc = sqlite3_exec(g_db, sql, nullptr, nullptr, &err);
    if (rc != SQLITE_OK) {
        std::string m = err ? err : "(no message)";
        sqlite3_free(err);
        throw std::runtime_error(std::string("context: exec: ") + m + "; sql=" + sql);
    }
}

void install_schema() {
    exec("PRAGMA journal_mode = WAL;");
    exec("PRAGMA synchronous = NORMAL;");
    exec("PRAGMA temp_store = MEMORY;");
    exec("PRAGMA cache_size = -8000;");        // 8 MiB page cache
    exec("PRAGMA foreign_keys = ON;");

    exec(R"sql(
        CREATE TABLE IF NOT EXISTS turns (
            id      INTEGER PRIMARY KEY AUTOINCREMENT,
            ts      INTEGER NOT NULL,
            turn    INTEGER NOT NULL,
            layer   TEXT    NOT NULL,
            kind    TEXT    NOT NULL,
            content TEXT    NOT NULL,
            meta    TEXT
        );
    )sql");
    exec("CREATE INDEX IF NOT EXISTS idx_turns_turn  ON turns(turn);");
    exec("CREATE INDEX IF NOT EXISTS idx_turns_layer ON turns(layer);");
    exec("CREATE INDEX IF NOT EXISTS idx_turns_ts    ON turns(ts);");

    exec(R"sql(
        CREATE VIRTUAL TABLE IF NOT EXISTS turns_fts USING fts5(
            content,
            content='turns',
            content_rowid='id',
            tokenize='porter unicode61'
        );
    )sql");
    exec(R"sql(
        CREATE TRIGGER IF NOT EXISTS turns_ai AFTER INSERT ON turns BEGIN
            INSERT INTO turns_fts(rowid, content) VALUES (new.id, new.content);
        END;
    )sql");
    exec(R"sql(
        CREATE TRIGGER IF NOT EXISTS turns_ad AFTER DELETE ON turns BEGIN
            INSERT INTO turns_fts(turns_fts, rowid, content)
                VALUES('delete', old.id, old.content);
        END;
    )sql");
    exec(R"sql(
        CREATE TRIGGER IF NOT EXISTS turns_au AFTER UPDATE ON turns BEGIN
            INSERT INTO turns_fts(turns_fts, rowid, content)
                VALUES('delete', old.id, old.content);
            INSERT INTO turns_fts(rowid, content) VALUES (new.id, new.content);
        END;
    )sql");
}

void open_db_locked(const fs::path & p) {
    if (g_db) {
        sqlite3_close(g_db);
        g_db = nullptr;
    }
    int rc = sqlite3_open_v2(
        p.string().c_str(), &g_db,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX,
        nullptr);
    if (rc != SQLITE_OK) {
        std::string m = g_db ? sqlite3_errmsg(g_db) : "(open failed)";
        if (g_db) { sqlite3_close(g_db); g_db = nullptr; }
        throw std::runtime_error("context: open " + p.string() + ": " + m);
    }
    g_path = p;
    install_schema();
}

Record row_to_record(sqlite3_stmt * st) {
    Record r;
    r.id   = sqlite3_column_int64(st, 0);
    r.ts   = sqlite3_column_int64(st, 1);
    r.turn = sqlite3_column_int64(st, 2);
    auto col_text = [&](int i) -> std::string {
        const unsigned char * p = sqlite3_column_text(st, i);
        if (!p) return {};
        return std::string(reinterpret_cast<const char *>(p));
    };
    r.layer   = col_text(3);
    r.kind    = col_text(4);
    r.content = col_text(5);
    r.meta    = col_text(6);
    return r;
}

}

void init(std::string_view path_dir) {
    std::lock_guard<std::mutex> lk(g_mtx);
    g_dir = path_dir.empty()
        ? fs::path("005_context/sessions")
        : fs::path(std::string(path_dir));
    fs::create_directories(g_dir);

    // If active.txt names an existing session, attach to it. Otherwise
    // leave the handle null - the server will switch_to() once the
    // browser tells us which session it owns. This avoids auto-spawning
    // a phantom session that the picker would then have to display.
    std::string id = read_active_file_in(g_dir);
    if (!id.empty()) {
        fs::path p = g_dir / (id + ".sqlite");
        if (fs::exists(p)) {
            g_id   = id;
            g_path = p;
            open_db_locked(g_path);
            g_turn = 0;
        }
    }
}

void shutdown() {
    std::lock_guard<std::mutex> lk(g_mtx);
    if (!g_db) return;
    sqlite3_close(g_db);
    g_db = nullptr;
    g_id.clear();
}

void new_session() {
    std::lock_guard<std::mutex> lk(g_mtx);
    if (g_db) { sqlite3_close(g_db); g_db = nullptr; }
    g_id   = make_uuid_v4();
    g_path = g_dir / (g_id + ".sqlite");
    open_db_locked(g_path);
    write_active_file_locked();
    g_turn = 0;
}

void switch_to(std::string_view id) {
    std::lock_guard<std::mutex> lk(g_mtx);
    std::string sid(id);
    if (sid.empty()) throw std::runtime_error("context::switch_to: empty id");
    if (sid == g_id && g_db) return;          // already active
    if (g_db) { sqlite3_close(g_db); g_db = nullptr; }
    g_id   = sid;
    g_path = g_dir / (g_id + ".sqlite");
    open_db_locked(g_path);
    write_active_file_locked();
    g_turn = 0;
}

std::string current_id() {
    std::lock_guard<std::mutex> lk(g_mtx);
    return g_id;
}

std::string sessions_dir() {
    std::lock_guard<std::mutex> lk(g_mtx);
    return g_dir.string();
}

int64_t next_turn() {
    std::lock_guard<std::mutex> lk(g_mtx);
    return ++g_turn;
}

int64_t current_turn() {
    std::lock_guard<std::mutex> lk(g_mtx);
    return g_turn;
}

void append(std::string_view layer,
            std::string_view kind,
            std::string_view content,
            std::string_view meta) {
    std::lock_guard<std::mutex> lk(g_mtx);
    if (!g_db) throw std::runtime_error("context: append before init()");

    static thread_local sqlite3_stmt * st = nullptr;
    if (!st) {
        check(sqlite3_prepare_v2(g_db,
            "INSERT INTO turns(ts, turn, layer, kind, content, meta) "
            "VALUES (?, ?, ?, ?, ?, ?);",
            -1, &st, nullptr), "prepare append");
    }
    sqlite3_reset(st);
    sqlite3_clear_bindings(st);

    sqlite3_bind_int64(st, 1, now_ms());
    sqlite3_bind_int64(st, 2, g_turn);
    sqlite3_bind_text(st, 3, layer.data(),   static_cast<int>(layer.size()),   SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 4, kind.data(),    static_cast<int>(kind.size()),    SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 5, content.data(), static_cast<int>(content.size()), SQLITE_TRANSIENT);
    if (meta.empty()) {
        sqlite3_bind_null(st, 6);
    } else {
        sqlite3_bind_text(st, 6, meta.data(), static_cast<int>(meta.size()), SQLITE_TRANSIENT);
    }
    int rc = sqlite3_step(st);
    if (rc != SQLITE_DONE) check(rc, "step append");
}

std::vector<Record> recent(int n) {
    std::lock_guard<std::mutex> lk(g_mtx);
    std::vector<Record> out;
    if (!g_db) return out;
    sqlite3_stmt * st = nullptr;
    check(sqlite3_prepare_v2(g_db,
        "SELECT id, ts, turn, layer, kind, content, meta "
        "FROM turns ORDER BY id DESC LIMIT ?;",
        -1, &st, nullptr), "prepare recent");
    sqlite3_bind_int(st, 1, n);
    while (sqlite3_step(st) == SQLITE_ROW) out.push_back(row_to_record(st));
    sqlite3_finalize(st);
    return out;
}

std::vector<Record> by_turn(int64_t turn) {
    std::lock_guard<std::mutex> lk(g_mtx);
    std::vector<Record> out;
    if (!g_db) return out;
    sqlite3_stmt * st = nullptr;
    check(sqlite3_prepare_v2(g_db,
        "SELECT id, ts, turn, layer, kind, content, meta "
        "FROM turns WHERE turn = ? ORDER BY id ASC;",
        -1, &st, nullptr), "prepare by_turn");
    sqlite3_bind_int64(st, 1, turn);
    while (sqlite3_step(st) == SQLITE_ROW) out.push_back(row_to_record(st));
    sqlite3_finalize(st);
    return out;
}

void delete_turn(int64_t turn) {
    std::lock_guard<std::mutex> lk(g_mtx);
    if (!g_db) return;
    sqlite3_stmt * st = nullptr;
    check(sqlite3_prepare_v2(g_db,
        "DELETE FROM turns WHERE turn = ?;",
        -1, &st, nullptr), "prepare delete_turn");
    sqlite3_bind_int64(st, 1, turn);
    sqlite3_step(st);
    sqlite3_finalize(st);
}

std::vector<Record> by_layer(std::string_view layer, int limit) {
    std::lock_guard<std::mutex> lk(g_mtx);
    std::vector<Record> out;
    if (!g_db) return out;
    sqlite3_stmt * st = nullptr;
    check(sqlite3_prepare_v2(g_db,
        "SELECT id, ts, turn, layer, kind, content, meta "
        "FROM turns WHERE layer = ? ORDER BY id DESC LIMIT ?;",
        -1, &st, nullptr), "prepare by_layer");
    sqlite3_bind_text(st, 1, layer.data(), static_cast<int>(layer.size()), SQLITE_TRANSIENT);
    sqlite3_bind_int(st, 2, limit);
    while (sqlite3_step(st) == SQLITE_ROW) out.push_back(row_to_record(st));
    sqlite3_finalize(st);
    return out;
}

std::vector<Record> search(std::string_view query, int top_k) {
    std::lock_guard<std::mutex> lk(g_mtx);
    std::vector<Record> out;
    if (!g_db || query.empty()) return out;
    sqlite3_stmt * st = nullptr;
    check(sqlite3_prepare_v2(g_db,
        "SELECT t.id, t.ts, t.turn, t.layer, t.kind, t.content, t.meta "
        "FROM turns_fts f JOIN turns t ON t.id = f.rowid "
        "WHERE turns_fts MATCH ? "
        "ORDER BY bm25(turns_fts) ASC LIMIT ?;",
        -1, &st, nullptr), "prepare search");
    sqlite3_bind_text(st, 1, query.data(), static_cast<int>(query.size()), SQLITE_TRANSIENT);
    sqlite3_bind_int(st, 2, top_k);
    while (sqlite3_step(st) == SQLITE_ROW) out.push_back(row_to_record(st));
    sqlite3_finalize(st);
    return out;
}

int64_t count() {
    std::lock_guard<std::mutex> lk(g_mtx);
    if (!g_db) return 0;
    sqlite3_stmt * st = nullptr;
    check(sqlite3_prepare_v2(g_db, "SELECT COUNT(*) FROM turns;", -1, &st, nullptr),
          "prepare count");
    int64_t n = 0;
    if (sqlite3_step(st) == SQLITE_ROW) n = sqlite3_column_int64(st, 0);
    sqlite3_finalize(st);
    return n;
}

std::string current_session_path() {
    std::lock_guard<std::mutex> lk(g_mtx);
    return g_path.string();
}

std::string render_for_prompt(int recent_n) {
    static const std::unordered_set<std::string> kSkipLayers = {
        "dictionary",
        "stylize",
    };
    auto records = recent(recent_n);  // newest-first
    std::reverse(records.begin(), records.end());

    std::string out;
    for (const auto & r : records) {
        if (kSkipLayers.count(r.layer)) continue;
        out.append("[turn ");
        out.append(std::to_string(r.turn));
        out.append(" ");
        out.append(r.layer);
        out.append("/");
        out.append(r.kind);
        out.append("] ");
        out.append(r.content);
        out.push_back('\n');
    }
    if (out.empty()) out = "(memory is empty)\n";
    return out;
}

}
