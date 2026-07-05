#include "kb_store.hpp"

#include "sqlite3.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <sstream>
#include <stdexcept>

namespace kb_store {
namespace {

namespace fs = std::filesystem;

std::mutex g_mu;
sqlite3 *  g_db = nullptr;
fs::path   g_root;

void must_ok(int rc, const char * where) {
    if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) {
        std::string m = g_db ? sqlite3_errmsg(g_db) : "(no db)";
        throw std::runtime_error(std::string("kb_store ") + where + ": " + m);
    }
}
void exec(const char * sql) {
    char * err = nullptr;
    int rc = sqlite3_exec(g_db, sql, nullptr, nullptr, &err);
    if (rc != SQLITE_OK) {
        std::string m = err ? err : "(unknown)";
        if (err) sqlite3_free(err);
        throw std::runtime_error(std::string("kb_store exec: ") + m);
    }
}

std::string fnv64_hex(std::string_view s) {
    uint64_t h = 0xcbf29ce484222325ULL;
    for (unsigned char c : s) { h ^= c; h *= 0x100000001b3ULL; }
    char buf[24];
    std::snprintf(buf, sizeof(buf), "fnv64:%016llx", (unsigned long long)h);
    return buf;
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
    std::istringstream is(s); std::string t;
    while (is >> t) out.push_back(t);
    return out;
}

void install_schema() {
    exec("PRAGMA journal_mode = WAL;");
    exec("PRAGMA synchronous = NORMAL;");
    exec(R"sql(
        CREATE TABLE IF NOT EXISTS items (
            id            INTEGER PRIMARY KEY AUTOINCREMENT,
            url           TEXT    NOT NULL,
            host          TEXT    NOT NULL,
            fetched_at    INTEGER NOT NULL,
            mime          TEXT    NOT NULL DEFAULT '',
            content_hash  TEXT    NOT NULL UNIQUE,
            tags          TEXT    NOT NULL DEFAULT '',
            raw_bytes     INTEGER NOT NULL DEFAULT 0,
            extracted     TEXT    NOT NULL DEFAULT ''
        );
    )sql");
    exec("CREATE INDEX IF NOT EXISTS idx_items_url  ON items(url);");
    exec("CREATE INDEX IF NOT EXISTS idx_items_host ON items(host);");
    exec("CREATE INDEX IF NOT EXISTS idx_items_ts   ON items(fetched_at);");

    exec(R"sql(
        CREATE VIRTUAL TABLE IF NOT EXISTS items_fts USING fts5(
            extracted,
            content='items',
            content_rowid='id',
            tokenize='porter unicode61'
        );
    )sql");
    exec(R"sql(
        CREATE TRIGGER IF NOT EXISTS items_ai AFTER INSERT ON items BEGIN
            INSERT INTO items_fts(rowid, extracted) VALUES (new.id, new.extracted);
        END;
    )sql");
    exec(R"sql(
        CREATE TRIGGER IF NOT EXISTS items_ad AFTER DELETE ON items BEGIN
            INSERT INTO items_fts(items_fts, rowid, extracted)
                VALUES('delete', old.id, old.extracted);
        END;
    )sql");
}

fs::path raw_path(const std::string & content_hash) {
    return g_root / "raw" / content_hash;
}

Item row_to_item(sqlite3_stmt * st) {
    Item it;
    it.id           = sqlite3_column_int64(st, 0);
    auto col = [&](int i) -> std::string {
        const unsigned char * p = sqlite3_column_text(st, i);
        return p ? std::string(reinterpret_cast<const char *>(p)) : std::string{};
    };
    it.url          = col(1);
    it.host         = col(2);
    it.fetched_at   = sqlite3_column_int64(st, 3);
    it.mime         = col(4);
    it.content_hash = col(5);
    it.tags         = split_tags(col(6));
    it.raw_bytes    = (size_t)sqlite3_column_int64(st, 7);
    return it;
}

}

void open(std::string_view project_root) {
    std::lock_guard<std::mutex> lk(g_mu);
    fs::path root = project_root.empty() ? fs::current_path()
                                         : fs::path(std::string(project_root));
    g_root = root / ".tool" / "kb";
    fs::create_directories(g_root / "raw");
    fs::path path = g_root / "kb.sqlite";
    if (g_db) { sqlite3_close(g_db); g_db = nullptr; }
    int rc = sqlite3_open_v2(
        path.string().c_str(), &g_db,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX,
        nullptr);
    if (rc != SQLITE_OK) {
        std::string m = g_db ? sqlite3_errmsg(g_db) : "(open failed)";
        if (g_db) { sqlite3_close(g_db); g_db = nullptr; }
        throw std::runtime_error("kb_store::open: " + m);
    }
    install_schema();
}
void close() {
    std::lock_guard<std::mutex> lk(g_mu);
    if (g_db) { sqlite3_close(g_db); g_db = nullptr; }
    g_root.clear();
}

int64_t ingest(std::string_view url, std::string_view host,
               int64_t fetched_at, std::string_view mime,
               std::string_view raw_body, std::string_view extracted_text,
               const std::vector<std::string> & tags) {
    std::lock_guard<std::mutex> lk(g_mu);
    if (!g_db) throw std::runtime_error("kb_store::ingest: not opened");

    std::string hash = fnv64_hex(raw_body);

    // Already have it?
    sqlite3_stmt * lk_st = nullptr;
    must_ok(sqlite3_prepare_v2(g_db, "SELECT id FROM items WHERE content_hash=?",
                               -1, &lk_st, nullptr), "prepare lookup");
    sqlite3_bind_text(lk_st, 1, hash.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(lk_st) == SQLITE_ROW) {
        int64_t id = sqlite3_column_int64(lk_st, 0);
        sqlite3_finalize(lk_st);
        return id;
    }
    sqlite3_finalize(lk_st);

    // Write the raw bytes once.
    fs::path rp = raw_path(hash);
    {
        std::ofstream o(rp, std::ios::binary | std::ios::trunc);
        if (!o) throw std::runtime_error("kb_store::ingest: cannot write " + rp.string());
        o.write(raw_body.data(), (std::streamsize)raw_body.size());
    }

    sqlite3_stmt * st = nullptr;
    must_ok(sqlite3_prepare_v2(g_db,
        "INSERT INTO items(url, host, fetched_at, mime, content_hash, tags, raw_bytes, extracted)"
        " VALUES(?,?,?,?,?,?,?,?)",
        -1, &st, nullptr), "prepare insert");
    sqlite3_bind_text (st, 1, std::string(url).c_str(),  -1, SQLITE_TRANSIENT);
    sqlite3_bind_text (st, 2, std::string(host).c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(st, 3, fetched_at);
    sqlite3_bind_text (st, 4, std::string(mime).c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text (st, 5, hash.c_str(),              -1, SQLITE_TRANSIENT);
    std::string tagstr = join_tags(tags);
    sqlite3_bind_text (st, 6, tagstr.c_str(),            -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(st, 7, (sqlite3_int64)raw_body.size());
    sqlite3_bind_text (st, 8, std::string(extracted_text).c_str(), -1, SQLITE_TRANSIENT);
    must_ok(sqlite3_step(st), "step insert");
    int64_t id = sqlite3_last_insert_rowid(g_db);
    sqlite3_finalize(st);
    return id;
}

std::optional<std::string> raw(std::string_view content_hash) {
    std::lock_guard<std::mutex> lk(g_mu);
    fs::path rp = g_root / "raw" / std::string{content_hash};
    if (!fs::exists(rp)) return std::nullopt;
    std::ifstream in(rp, std::ios::binary);
    std::ostringstream b; b << in.rdbuf();
    return b.str();
}

namespace {
std::vector<Item> list_with(const char * sql,
                             const std::vector<std::pair<int,std::string>> & str_binds,
                             const std::vector<std::pair<int,int64_t>>     & int_binds,
                             size_t limit) {
    std::vector<Item> out;
    if (!g_db) return out;
    sqlite3_stmt * st = nullptr;
    must_ok(sqlite3_prepare_v2(g_db, sql, -1, &st, nullptr), "prepare list");
    for (const auto & [ix, v] : str_binds) sqlite3_bind_text (st, ix, v.c_str(), -1, SQLITE_TRANSIENT);
    for (const auto & [ix, v] : int_binds) sqlite3_bind_int64(st, ix, v);
    int limit_ix = (int)(str_binds.size() + int_binds.size() + 1);
    sqlite3_bind_int64(st, limit_ix, (sqlite3_int64)limit);
    while (sqlite3_step(st) == SQLITE_ROW) out.push_back(row_to_item(st));
    sqlite3_finalize(st);
    return out;
}
}

std::vector<Item> list_by_url(std::string_view url, size_t limit) {
    std::lock_guard<std::mutex> lk(g_mu);
    return list_with(
        "SELECT id, url, host, fetched_at, mime, content_hash, tags, raw_bytes"
        "  FROM items WHERE url = ? ORDER BY fetched_at DESC LIMIT ?",
        {{1, std::string(url)}}, {}, limit);
}
std::vector<Item> list_by_host(std::string_view host, size_t limit) {
    std::lock_guard<std::mutex> lk(g_mu);
    return list_with(
        "SELECT id, url, host, fetched_at, mime, content_hash, tags, raw_bytes"
        "  FROM items WHERE host = ? ORDER BY fetched_at DESC LIMIT ?",
        {{1, std::string(host)}}, {}, limit);
}
std::vector<Item> list_by_time(int64_t since, int64_t until, size_t limit) {
    std::lock_guard<std::mutex> lk(g_mu);
    return list_with(
        "SELECT id, url, host, fetched_at, mime, content_hash, tags, raw_bytes"
        "  FROM items WHERE fetched_at BETWEEN ? AND ? ORDER BY fetched_at DESC LIMIT ?",
        {}, {{1, since}, {2, until}}, limit);
}

std::vector<Hit> search(std::string_view query, size_t top_k) {
    std::lock_guard<std::mutex> lk(g_mu);
    std::vector<Hit> out;
    if (!g_db) return out;
    if (top_k == 0) top_k = 8;
    sqlite3_stmt * st = nullptr;
    must_ok(sqlite3_prepare_v2(g_db, R"sql(
        SELECT items.id, items.url, items.host, items.fetched_at,
               items.mime, items.content_hash, items.tags, items.raw_bytes,
               items.extracted,
               bm25(items_fts) AS score,
               snippet(items_fts, 0, '[[', ']]', '...', 16) AS snip
          FROM items_fts
          JOIN items ON items.id = items_fts.rowid
         WHERE items_fts MATCH ?
         ORDER BY score
         LIMIT ?
    )sql", -1, &st, nullptr), "prepare search");
    sqlite3_bind_text (st, 1, std::string(query).c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(st, 2, (sqlite3_int64)top_k);
    while (sqlite3_step(st) == SQLITE_ROW) {
        Hit h;
        h.item.id           = sqlite3_column_int64(st, 0);
        auto col = [&](int i) -> std::string {
            const unsigned char * p = sqlite3_column_text(st, i);
            return p ? std::string(reinterpret_cast<const char *>(p)) : std::string{};
        };
        h.item.url          = col(1);
        h.item.host         = col(2);
        h.item.fetched_at   = sqlite3_column_int64(st, 3);
        h.item.mime         = col(4);
        h.item.content_hash = col(5);
        h.item.tags         = split_tags(col(6));
        h.item.raw_bytes    = (size_t)sqlite3_column_int64(st, 7);
        h.extracted_text    = col(8);
        h.score             = sqlite3_column_double(st, 9);
        h.snippet           = col(10);
        out.push_back(std::move(h));
    }
    sqlite3_finalize(st);
    return out;
}

}
