#include "project_memory.hpp"

#include "sqlite3.h"

#include <algorithm>
#include <chrono>
#include <cstring>
#include <filesystem>
#include <mutex>
#include <sstream>
#include <stdexcept>

namespace project_memory {
namespace {

namespace fs = std::filesystem;

std::mutex   g_mu;
sqlite3 *    g_db   = nullptr;
fs::path     g_path;

int64_t now_unix() {
    using namespace std::chrono;
    return duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
}

void exec(const char * sql) {
    char * err = nullptr;
    int rc = sqlite3_exec(g_db, sql, nullptr, nullptr, &err);
    if (rc != SQLITE_OK) {
        std::string m = err ? err : "(unknown sqlite error)";
        if (err) sqlite3_free(err);
        throw std::runtime_error(std::string("project_memory exec: ") + m + " sql=" + sql);
    }
}

void must_ok(int rc, const char * where) {
    if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) {
        std::string m = g_db ? sqlite3_errmsg(g_db) : "(no db)";
        throw std::runtime_error(std::string("project_memory ") + where + ": " + m);
    }
}

std::string join_tags(const std::vector<std::string> & v) {
    std::string out;
    for (size_t i = 0; i < v.size(); ++i) {
        if (i) out.push_back(' ');
        out += v[i];
    }
    return out;
}

std::vector<std::string> split_tags(const std::string & s) {
    std::vector<std::string> out;
    std::istringstream is(s);
    std::string t;
    while (is >> t) out.push_back(t);
    return out;
}

void install_schema() {
    exec("PRAGMA journal_mode = WAL;");
    exec("PRAGMA synchronous = NORMAL;");
    exec("PRAGMA foreign_keys = ON;");
    exec(R"sql(
        CREATE TABLE IF NOT EXISTS entries (
            id        INTEGER PRIMARY KEY AUTOINCREMENT,
            ts        INTEGER NOT NULL,
            category  TEXT    NOT NULL,
            content   TEXT    NOT NULL,
            tags      TEXT    NOT NULL DEFAULT '',
            source    TEXT    NOT NULL DEFAULT ''
        );
    )sql");
    exec("CREATE INDEX IF NOT EXISTS idx_entries_cat ON entries(category);");
    exec("CREATE INDEX IF NOT EXISTS idx_entries_ts  ON entries(ts);");

    exec(R"sql(
        CREATE VIRTUAL TABLE IF NOT EXISTS entries_fts USING fts5(
            content, tags,
            content='entries',
            content_rowid='id',
            tokenize='porter unicode61'
        );
    )sql");
    exec(R"sql(
        CREATE TRIGGER IF NOT EXISTS entries_ai AFTER INSERT ON entries BEGIN
            INSERT INTO entries_fts(rowid, content, tags) VALUES (new.id, new.content, new.tags);
        END;
    )sql");
    exec(R"sql(
        CREATE TRIGGER IF NOT EXISTS entries_ad AFTER DELETE ON entries BEGIN
            INSERT INTO entries_fts(entries_fts, rowid, content, tags)
                VALUES('delete', old.id, old.content, old.tags);
        END;
    )sql");
    exec(R"sql(
        CREATE TRIGGER IF NOT EXISTS entries_au AFTER UPDATE ON entries BEGIN
            INSERT INTO entries_fts(entries_fts, rowid, content, tags)
                VALUES('delete', old.id, old.content, old.tags);
            INSERT INTO entries_fts(rowid, content, tags) VALUES (new.id, new.content, new.tags);
        END;
    )sql");
}

Entry row_to_entry(sqlite3_stmt * st, int base = 0) {
    Entry e;
    e.id       = sqlite3_column_int64(st, base + 0);
    e.ts       = sqlite3_column_int64(st, base + 1);
    auto col = [&](int i) -> std::string {
        const unsigned char * p = sqlite3_column_text(st, i);
        return p ? std::string(reinterpret_cast<const char *>(p)) : std::string{};
    };
    e.category = col(base + 2);
    e.content  = col(base + 3);
    e.tags     = split_tags(col(base + 4));
    e.source   = col(base + 5);
    return e;
}

}

void open(std::string_view project_root) {
    std::lock_guard<std::mutex> lk(g_mu);
    fs::path root = project_root.empty()
                        ? fs::current_path()
                        : fs::path(std::string(project_root));
    fs::path dir = root / ".tool" / "memory";
    fs::create_directories(dir);
    fs::path path = dir / "project.sqlite";

    if (g_db) { sqlite3_close(g_db); g_db = nullptr; }
    int rc = sqlite3_open_v2(
        path.string().c_str(), &g_db,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX,
        nullptr);
    if (rc != SQLITE_OK) {
        std::string m = g_db ? sqlite3_errmsg(g_db) : "(open failed)";
        if (g_db) { sqlite3_close(g_db); g_db = nullptr; }
        throw std::runtime_error("project_memory::open: " + m);
    }
    g_path = path;
    install_schema();
}

void close() {
    std::lock_guard<std::mutex> lk(g_mu);
    if (g_db) { sqlite3_close(g_db); g_db = nullptr; }
    g_path.clear();
}

int64_t add(std::string_view              category,
            std::string_view              content,
            const std::vector<std::string> & tags,
            std::string_view              source) {
    std::lock_guard<std::mutex> lk(g_mu);
    if (!g_db) throw std::runtime_error("project_memory::add: not opened");
    sqlite3_stmt * st = nullptr;
    must_ok(sqlite3_prepare_v2(g_db,
        "INSERT INTO entries(ts, category, content, tags, source) VALUES(?,?,?,?,?)",
        -1, &st, nullptr), "prepare add");
    sqlite3_bind_int64(st, 1, now_unix());
    sqlite3_bind_text (st, 2, std::string(category).c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text (st, 3, std::string(content).c_str(),  -1, SQLITE_TRANSIENT);
    std::string tagstr = join_tags(tags);
    sqlite3_bind_text (st, 4, tagstr.c_str(),                -1, SQLITE_TRANSIENT);
    sqlite3_bind_text (st, 5, std::string(source).c_str(),   -1, SQLITE_TRANSIENT);
    must_ok(sqlite3_step(st), "step add");
    int64_t id = sqlite3_last_insert_rowid(g_db);
    sqlite3_finalize(st);
    return id;
}

void update(int64_t id, std::string_view content,
            const std::vector<std::string> & tags,
            std::string_view source) {
    std::lock_guard<std::mutex> lk(g_mu);
    if (!g_db) throw std::runtime_error("project_memory::update: not opened");
    sqlite3_stmt * st = nullptr;
    must_ok(sqlite3_prepare_v2(g_db,
        "UPDATE entries SET content=?, tags=?, source=?, ts=? WHERE id=?",
        -1, &st, nullptr), "prepare update");
    sqlite3_bind_text (st, 1, std::string(content).c_str(),  -1, SQLITE_TRANSIENT);
    std::string tagstr = join_tags(tags);
    sqlite3_bind_text (st, 2, tagstr.c_str(),                -1, SQLITE_TRANSIENT);
    sqlite3_bind_text (st, 3, std::string(source).c_str(),   -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(st, 4, now_unix());
    sqlite3_bind_int64(st, 5, id);
    must_ok(sqlite3_step(st), "step update");
    sqlite3_finalize(st);
}

void forget(int64_t id) {
    std::lock_guard<std::mutex> lk(g_mu);
    if (!g_db) throw std::runtime_error("project_memory::forget: not opened");
    sqlite3_stmt * st = nullptr;
    must_ok(sqlite3_prepare_v2(g_db, "DELETE FROM entries WHERE id=?", -1, &st, nullptr),
            "prepare forget");
    sqlite3_bind_int64(st, 1, id);
    must_ok(sqlite3_step(st), "step forget");
    sqlite3_finalize(st);
}

std::vector<Entry> list(std::string_view category, std::string_view tag, size_t limit) {
    std::lock_guard<std::mutex> lk(g_mu);
    std::vector<Entry> out;
    if (!g_db) return out;
    if (limit == 0) limit = 100;

    std::string sql = "SELECT id, ts, category, content, tags, source FROM entries WHERE 1=1";
    if (!category.empty()) sql += " AND category = ?";
    if (!tag.empty())      sql += " AND (' ' || tags || ' ') LIKE ?";
    sql += " ORDER BY id DESC LIMIT ?";

    sqlite3_stmt * st = nullptr;
    must_ok(sqlite3_prepare_v2(g_db, sql.c_str(), -1, &st, nullptr), "prepare list");
    int ix = 1;
    if (!category.empty()) sqlite3_bind_text(st, ix++, std::string(category).c_str(), -1, SQLITE_TRANSIENT);
    if (!tag.empty()) {
        std::string like = std::string("% ") + std::string(tag) + " %";
        sqlite3_bind_text(st, ix++, like.c_str(), -1, SQLITE_TRANSIENT);
    }
    sqlite3_bind_int64(st, ix++, (sqlite3_int64)limit);
    while (sqlite3_step(st) == SQLITE_ROW) out.push_back(row_to_entry(st));
    sqlite3_finalize(st);
    return out;
}

std::vector<Hit> search(std::string_view query, size_t top_k) {
    std::lock_guard<std::mutex> lk(g_mu);
    std::vector<Hit> out;
    if (!g_db) return out;
    if (top_k == 0) top_k = 8;

    sqlite3_stmt * st = nullptr;
    must_ok(sqlite3_prepare_v2(g_db, R"sql(
        SELECT entries.id, entries.ts, entries.category, entries.content,
               entries.tags, entries.source,
               bm25(entries_fts) AS score,
               snippet(entries_fts, 0, '[[', ']]', '...', 16) AS snip
          FROM entries_fts
          JOIN entries ON entries.id = entries_fts.rowid
         WHERE entries_fts MATCH ?
         ORDER BY score
         LIMIT ?
    )sql", -1, &st, nullptr), "prepare search");
    sqlite3_bind_text (st, 1, std::string(query).c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(st, 2, (sqlite3_int64)top_k);
    while (sqlite3_step(st) == SQLITE_ROW) {
        Hit h;
        h.entry  = row_to_entry(st);
        h.score  = sqlite3_column_double(st, 6);
        const unsigned char * sn = sqlite3_column_text(st, 7);
        h.snippet = sn ? reinterpret_cast<const char *>(sn) : "";
        out.push_back(std::move(h));
    }
    sqlite3_finalize(st);
    return out;
}

std::optional<Entry> get(int64_t id) {
    std::lock_guard<std::mutex> lk(g_mu);
    if (!g_db) return std::nullopt;
    sqlite3_stmt * st = nullptr;
    must_ok(sqlite3_prepare_v2(g_db,
        "SELECT id, ts, category, content, tags, source FROM entries WHERE id=?",
        -1, &st, nullptr), "prepare get");
    sqlite3_bind_int64(st, 1, id);
    std::optional<Entry> out;
    if (sqlite3_step(st) == SQLITE_ROW) out = row_to_entry(st);
    sqlite3_finalize(st);
    return out;
}

}
