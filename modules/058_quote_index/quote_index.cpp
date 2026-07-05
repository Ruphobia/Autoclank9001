#include "quote_index.hpp"

#include "sqlite3.h"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <mutex>
#include <sstream>
#include <stdexcept>

namespace quote_index {
namespace {

namespace fs = std::filesystem;

std::mutex g_mu;
sqlite3 *  g_db = nullptr;
fs::path   g_path;

int64_t now_unix() {
    using namespace std::chrono;
    return duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
}

void exec(const char * sql) {
    char * err = nullptr;
    int rc = sqlite3_exec(g_db, sql, nullptr, nullptr, &err);
    if (rc != SQLITE_OK) {
        std::string m = err ? err : "(unknown)";
        if (err) sqlite3_free(err);
        throw std::runtime_error(std::string("quote_index exec: ") + m);
    }
}
void must_ok(int rc, const char * where) {
    if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) {
        std::string m = g_db ? sqlite3_errmsg(g_db) : "(no db)";
        throw std::runtime_error(std::string("quote_index ") + where + ": " + m);
    }
}

std::string join_tags(const std::vector<std::string> & v) {
    std::string out;
    for (size_t i = 0; i < v.size(); ++i) { if (i) out.push_back(' '); out += v[i]; }
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
        CREATE TABLE IF NOT EXISTS quotes (
            id          INTEGER PRIMARY KEY AUTOINCREMENT,
            source_ref  TEXT    NOT NULL DEFAULT '',
            locator     TEXT    NOT NULL DEFAULT '',
            text        TEXT    NOT NULL,
            before_ctx  TEXT    NOT NULL DEFAULT '',
            after_ctx   TEXT    NOT NULL DEFAULT '',
            tags        TEXT    NOT NULL DEFAULT '',
            added_unix  INTEGER NOT NULL
        );
    )sql");
    exec("CREATE INDEX IF NOT EXISTS idx_quotes_src ON quotes(source_ref);");
    exec(R"sql(
        CREATE VIRTUAL TABLE IF NOT EXISTS quotes_fts USING fts5(
            text, before_ctx, after_ctx,
            content='quotes',
            content_rowid='id',
            tokenize='porter unicode61'
        );
    )sql");
    exec(R"sql(
        CREATE TRIGGER IF NOT EXISTS quotes_ai AFTER INSERT ON quotes BEGIN
            INSERT INTO quotes_fts(rowid, text, before_ctx, after_ctx)
                VALUES (new.id, new.text, new.before_ctx, new.after_ctx);
        END;
    )sql");
    exec(R"sql(
        CREATE TRIGGER IF NOT EXISTS quotes_ad AFTER DELETE ON quotes BEGIN
            INSERT INTO quotes_fts(quotes_fts, rowid, text, before_ctx, after_ctx)
                VALUES('delete', old.id, old.text, old.before_ctx, old.after_ctx);
        END;
    )sql");
}

Quote row_to_quote(sqlite3_stmt * st) {
    Quote q;
    auto col = [&](int i) -> std::string {
        const unsigned char * p = sqlite3_column_text(st, i);
        return p ? std::string(reinterpret_cast<const char *>(p)) : std::string{};
    };
    q.id         = sqlite3_column_int64(st, 0);
    q.source_ref = col(1);
    q.locator    = col(2);
    q.text       = col(3);
    q.before     = col(4);
    q.after      = col(5);
    q.tags       = split_tags(col(6));
    q.added_unix = sqlite3_column_int64(st, 7);
    return q;
}

}

void open(std::string_view project_root) {
    std::lock_guard<std::mutex> lk(g_mu);
    fs::path root = project_root.empty() ? fs::current_path()
                                         : fs::path(std::string(project_root));
    fs::path dir = root / ".tool" / "quotes";
    fs::create_directories(dir);
    g_path = dir / "quotes.sqlite";
    if (g_db) { sqlite3_close(g_db); g_db = nullptr; }
    int rc = sqlite3_open_v2(g_path.string().c_str(), &g_db,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, nullptr);
    if (rc != SQLITE_OK) {
        std::string m = g_db ? sqlite3_errmsg(g_db) : "(open failed)";
        if (g_db) { sqlite3_close(g_db); g_db = nullptr; }
        throw std::runtime_error("quote_index::open: " + m);
    }
    install_schema();
}
void close() {
    std::lock_guard<std::mutex> lk(g_mu);
    if (g_db) { sqlite3_close(g_db); g_db = nullptr; }
    g_path.clear();
}

int64_t add(const Quote & q) {
    std::lock_guard<std::mutex> lk(g_mu);
    if (!g_db) throw std::runtime_error("quote_index::add: not opened");
    sqlite3_stmt * st = nullptr;
    must_ok(sqlite3_prepare_v2(g_db,
        "INSERT INTO quotes(source_ref, locator, text, before_ctx, after_ctx, tags, added_unix)"
        " VALUES(?,?,?,?,?,?,?)", -1, &st, nullptr), "prepare add");
    std::string tags = join_tags(q.tags);
    sqlite3_bind_text (st, 1, q.source_ref.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text (st, 2, q.locator.c_str(),    -1, SQLITE_TRANSIENT);
    sqlite3_bind_text (st, 3, q.text.c_str(),       -1, SQLITE_TRANSIENT);
    sqlite3_bind_text (st, 4, q.before.c_str(),     -1, SQLITE_TRANSIENT);
    sqlite3_bind_text (st, 5, q.after.c_str(),      -1, SQLITE_TRANSIENT);
    sqlite3_bind_text (st, 6, tags.c_str(),         -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(st, 7, q.added_unix ? q.added_unix : now_unix());
    must_ok(sqlite3_step(st), "step add");
    int64_t id = sqlite3_last_insert_rowid(g_db);
    sqlite3_finalize(st);
    return id;
}

void update(int64_t id, const Quote & q) {
    std::lock_guard<std::mutex> lk(g_mu);
    if (!g_db) throw std::runtime_error("quote_index::update: not opened");
    sqlite3_stmt * st = nullptr;
    must_ok(sqlite3_prepare_v2(g_db,
        "UPDATE quotes SET source_ref=?, locator=?, text=?, before_ctx=?, after_ctx=?, tags=? WHERE id=?",
        -1, &st, nullptr), "prepare update");
    std::string tags = join_tags(q.tags);
    sqlite3_bind_text (st, 1, q.source_ref.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text (st, 2, q.locator.c_str(),    -1, SQLITE_TRANSIENT);
    sqlite3_bind_text (st, 3, q.text.c_str(),       -1, SQLITE_TRANSIENT);
    sqlite3_bind_text (st, 4, q.before.c_str(),     -1, SQLITE_TRANSIENT);
    sqlite3_bind_text (st, 5, q.after.c_str(),      -1, SQLITE_TRANSIENT);
    sqlite3_bind_text (st, 6, tags.c_str(),         -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(st, 7, id);
    must_ok(sqlite3_step(st), "step update");
    sqlite3_finalize(st);
}

void forget(int64_t id) {
    std::lock_guard<std::mutex> lk(g_mu);
    if (!g_db) throw std::runtime_error("quote_index::forget: not opened");
    sqlite3_stmt * st = nullptr;
    must_ok(sqlite3_prepare_v2(g_db, "DELETE FROM quotes WHERE id=?", -1, &st, nullptr),
            "prepare forget");
    sqlite3_bind_int64(st, 1, id);
    must_ok(sqlite3_step(st), "step forget");
    sqlite3_finalize(st);
}

std::optional<Quote> get(int64_t id) {
    std::lock_guard<std::mutex> lk(g_mu);
    if (!g_db) return std::nullopt;
    sqlite3_stmt * st = nullptr;
    must_ok(sqlite3_prepare_v2(g_db,
        "SELECT id, source_ref, locator, text, before_ctx, after_ctx, tags, added_unix"
        " FROM quotes WHERE id=?", -1, &st, nullptr), "prepare get");
    sqlite3_bind_int64(st, 1, id);
    std::optional<Quote> out;
    if (sqlite3_step(st) == SQLITE_ROW) out = row_to_quote(st);
    sqlite3_finalize(st);
    return out;
}

std::vector<Quote> list_by_source(std::string_view source_ref, size_t limit) {
    std::lock_guard<std::mutex> lk(g_mu);
    std::vector<Quote> out;
    if (!g_db) return out;
    if (limit == 0) limit = 100;
    sqlite3_stmt * st = nullptr;
    must_ok(sqlite3_prepare_v2(g_db,
        "SELECT id, source_ref, locator, text, before_ctx, after_ctx, tags, added_unix"
        " FROM quotes WHERE source_ref=? ORDER BY id DESC LIMIT ?",
        -1, &st, nullptr), "prepare list_by_source");
    std::string s{source_ref};
    sqlite3_bind_text (st, 1, s.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(st, 2, (sqlite3_int64)limit);
    while (sqlite3_step(st) == SQLITE_ROW) out.push_back(row_to_quote(st));
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
        SELECT quotes.id, quotes.source_ref, quotes.locator, quotes.text,
               quotes.before_ctx, quotes.after_ctx, quotes.tags, quotes.added_unix,
               bm25(quotes_fts) AS score,
               snippet(quotes_fts, 0, '[[', ']]', '...', 16) AS snip
          FROM quotes_fts
          JOIN quotes ON quotes.id = quotes_fts.rowid
         WHERE quotes_fts MATCH ?
         ORDER BY score
         LIMIT ?
    )sql", -1, &st, nullptr), "prepare search");
    std::string q{query};
    sqlite3_bind_text (st, 1, q.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(st, 2, (sqlite3_int64)top_k);
    while (sqlite3_step(st) == SQLITE_ROW) {
        Hit h;
        h.quote = row_to_quote(st);
        h.score = sqlite3_column_double(st, 8);
        const unsigned char * sn = sqlite3_column_text(st, 9);
        h.snippet = sn ? reinterpret_cast<const char *>(sn) : "";
        out.push_back(std::move(h));
    }
    sqlite3_finalize(st);
    return out;
}

std::string format_markdown(const Quote & q) {
    std::ostringstream os;
    os << "> " << q.text << "\n\n";
    os << "_";
    if (!q.source_ref.empty()) os << q.source_ref;
    if (!q.locator.empty())    os << " (" << q.locator << ")";
    os << "_\n";
    return os.str();
}

}
