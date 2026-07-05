#include "snippet_library.hpp"

#include "sqlite3.h"

#include <algorithm>
#include <filesystem>
#include <mutex>
#include <regex>
#include <set>
#include <sstream>
#include <stdexcept>

namespace snippet_library {
namespace {

namespace fs = std::filesystem;

std::mutex g_mu;
sqlite3 *  g_db = nullptr;

void exec(const char * sql) {
    char * err = nullptr;
    int rc = sqlite3_exec(g_db, sql, nullptr, nullptr, &err);
    if (rc != SQLITE_OK) {
        std::string m = err ? err : "(unknown)";
        if (err) sqlite3_free(err);
        throw std::runtime_error(std::string("snippet_library exec: ") + m);
    }
}
void must_ok(int rc, const char * where) {
    if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) {
        std::string m = g_db ? sqlite3_errmsg(g_db) : "(no db)";
        throw std::runtime_error(std::string("snippet_library ") + where + ": " + m);
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
        CREATE TABLE IF NOT EXISTS snippets (
            id          INTEGER PRIMARY KEY AUTOINCREMENT,
            name        TEXT    NOT NULL DEFAULT '',
            language    TEXT    NOT NULL DEFAULT '',
            body        TEXT    NOT NULL,
            description TEXT    NOT NULL DEFAULT '',
            tags        TEXT    NOT NULL DEFAULT ''
        );
    )sql");
    exec("CREATE INDEX IF NOT EXISTS idx_snip_lang ON snippets(language);");
    exec(R"sql(
        CREATE VIRTUAL TABLE IF NOT EXISTS snippets_fts USING fts5(
            name, body, description, tags,
            content='snippets',
            content_rowid='id',
            tokenize='porter unicode61'
        );
    )sql");
    exec(R"sql(
        CREATE TRIGGER IF NOT EXISTS snip_ai AFTER INSERT ON snippets BEGIN
            INSERT INTO snippets_fts(rowid, name, body, description, tags)
                VALUES (new.id, new.name, new.body, new.description, new.tags);
        END;
    )sql");
    exec(R"sql(
        CREATE TRIGGER IF NOT EXISTS snip_ad AFTER DELETE ON snippets BEGIN
            INSERT INTO snippets_fts(snippets_fts, rowid, name, body, description, tags)
                VALUES('delete', old.id, old.name, old.body, old.description, old.tags);
        END;
    )sql");
}

Snippet row_to_snippet(sqlite3_stmt * st) {
    Snippet s;
    auto col = [&](int i) -> std::string {
        const unsigned char * p = sqlite3_column_text(st, i);
        return p ? std::string(reinterpret_cast<const char *>(p)) : std::string{};
    };
    s.id          = sqlite3_column_int64(st, 0);
    s.name        = col(1);
    s.language    = col(2);
    s.body        = col(3);
    s.description = col(4);
    s.tags        = split_tags(col(5));
    s.params      = extract_params(s.body);
    return s;
}

}

void open(std::string_view project_root) {
    std::lock_guard<std::mutex> lk(g_mu);
    fs::path root = project_root.empty() ? fs::current_path()
                                         : fs::path(std::string(project_root));
    fs::path dir = root / ".tool" / "snippets";
    fs::create_directories(dir);
    fs::path path = dir / "snippets.sqlite";
    if (g_db) { sqlite3_close(g_db); g_db = nullptr; }
    int rc = sqlite3_open_v2(path.string().c_str(), &g_db,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, nullptr);
    if (rc != SQLITE_OK) {
        std::string m = g_db ? sqlite3_errmsg(g_db) : "(open failed)";
        if (g_db) { sqlite3_close(g_db); g_db = nullptr; }
        throw std::runtime_error("snippet_library::open: " + m);
    }
    install_schema();
}
void close() {
    std::lock_guard<std::mutex> lk(g_mu);
    if (g_db) { sqlite3_close(g_db); g_db = nullptr; }
}

int64_t add(const Snippet & s) {
    std::lock_guard<std::mutex> lk(g_mu);
    if (!g_db) throw std::runtime_error("snippet_library::add: not opened");
    sqlite3_stmt * st = nullptr;
    must_ok(sqlite3_prepare_v2(g_db,
        "INSERT INTO snippets(name, language, body, description, tags)"
        " VALUES(?,?,?,?,?)", -1, &st, nullptr), "prepare add");
    std::string tags = join_tags(s.tags);
    sqlite3_bind_text(st, 1, s.name.c_str(),        -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 2, s.language.c_str(),    -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 3, s.body.c_str(),        -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 4, s.description.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 5, tags.c_str(),          -1, SQLITE_TRANSIENT);
    must_ok(sqlite3_step(st), "step add");
    int64_t id = sqlite3_last_insert_rowid(g_db);
    sqlite3_finalize(st);
    return id;
}

void update(int64_t id, const Snippet & s) {
    std::lock_guard<std::mutex> lk(g_mu);
    if (!g_db) throw std::runtime_error("snippet_library::update: not opened");
    sqlite3_stmt * st = nullptr;
    must_ok(sqlite3_prepare_v2(g_db,
        "UPDATE snippets SET name=?, language=?, body=?, description=?, tags=? WHERE id=?",
        -1, &st, nullptr), "prepare update");
    std::string tags = join_tags(s.tags);
    sqlite3_bind_text (st, 1, s.name.c_str(),        -1, SQLITE_TRANSIENT);
    sqlite3_bind_text (st, 2, s.language.c_str(),    -1, SQLITE_TRANSIENT);
    sqlite3_bind_text (st, 3, s.body.c_str(),        -1, SQLITE_TRANSIENT);
    sqlite3_bind_text (st, 4, s.description.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text (st, 5, tags.c_str(),          -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(st, 6, id);
    must_ok(sqlite3_step(st), "step update");
    sqlite3_finalize(st);
}

void forget(int64_t id) {
    std::lock_guard<std::mutex> lk(g_mu);
    if (!g_db) throw std::runtime_error("snippet_library::forget: not opened");
    sqlite3_stmt * st = nullptr;
    must_ok(sqlite3_prepare_v2(g_db, "DELETE FROM snippets WHERE id=?",
        -1, &st, nullptr), "prepare forget");
    sqlite3_bind_int64(st, 1, id);
    must_ok(sqlite3_step(st), "step forget");
    sqlite3_finalize(st);
}

std::optional<Snippet> get(int64_t id) {
    std::lock_guard<std::mutex> lk(g_mu);
    if (!g_db) return std::nullopt;
    sqlite3_stmt * st = nullptr;
    must_ok(sqlite3_prepare_v2(g_db,
        "SELECT id, name, language, body, description, tags FROM snippets WHERE id=?",
        -1, &st, nullptr), "prepare get");
    sqlite3_bind_int64(st, 1, id);
    std::optional<Snippet> out;
    if (sqlite3_step(st) == SQLITE_ROW) out = row_to_snippet(st);
    sqlite3_finalize(st);
    return out;
}

std::vector<Snippet> list_by_language(std::string_view lang, size_t limit) {
    std::lock_guard<std::mutex> lk(g_mu);
    std::vector<Snippet> out;
    if (!g_db) return out;
    if (limit == 0) limit = 100;
    sqlite3_stmt * st = nullptr;
    must_ok(sqlite3_prepare_v2(g_db,
        "SELECT id, name, language, body, description, tags FROM snippets"
        " WHERE language=? ORDER BY id DESC LIMIT ?",
        -1, &st, nullptr), "prepare list_by_language");
    std::string l{lang};
    sqlite3_bind_text (st, 1, l.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(st, 2, (sqlite3_int64)limit);
    while (sqlite3_step(st) == SQLITE_ROW) out.push_back(row_to_snippet(st));
    sqlite3_finalize(st);
    return out;
}

std::vector<Snippet> list_by_tag(std::string_view tag, size_t limit) {
    std::lock_guard<std::mutex> lk(g_mu);
    std::vector<Snippet> out;
    if (!g_db) return out;
    if (limit == 0) limit = 100;
    sqlite3_stmt * st = nullptr;
    must_ok(sqlite3_prepare_v2(g_db,
        "SELECT id, name, language, body, description, tags FROM snippets"
        " WHERE (' ' || tags || ' ') LIKE ? ORDER BY id DESC LIMIT ?",
        -1, &st, nullptr), "prepare list_by_tag");
    std::string like = std::string("% ") + std::string(tag) + " %";
    sqlite3_bind_text (st, 1, like.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(st, 2, (sqlite3_int64)limit);
    while (sqlite3_step(st) == SQLITE_ROW) out.push_back(row_to_snippet(st));
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
        SELECT snippets.id, snippets.name, snippets.language, snippets.body,
               snippets.description, snippets.tags,
               bm25(snippets_fts) AS score,
               snippet(snippets_fts, 1, '[[', ']]', '...', 16) AS snip
          FROM snippets_fts
          JOIN snippets ON snippets.id = snippets_fts.rowid
         WHERE snippets_fts MATCH ?
         ORDER BY score
         LIMIT ?
    )sql", -1, &st, nullptr), "prepare search");
    std::string q{query};
    sqlite3_bind_text (st, 1, q.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(st, 2, (sqlite3_int64)top_k);
    while (sqlite3_step(st) == SQLITE_ROW) {
        Hit h;
        h.snippet      = row_to_snippet(st);
        h.score        = sqlite3_column_double(st, 6);
        const unsigned char * sn = sqlite3_column_text(st, 7);
        h.snippet_text = sn ? reinterpret_cast<const char *>(sn) : "";
        out.push_back(std::move(h));
    }
    sqlite3_finalize(st);
    return out;
}

std::vector<std::string> extract_params(std::string_view body) {
    std::vector<std::string> out;
    std::set<std::string> seen;
    std::string b{body};
    static const std::regex re(R"(\{\{\s*([A-Za-z_][A-Za-z0-9_]*)\s*\}\})");
    for (auto it = std::sregex_iterator(b.begin(), b.end(), re);
         it != std::sregex_iterator(); ++it) {
        std::string name = (*it)[1].str();
        if (seen.insert(name).second) out.push_back(name);
    }
    return out;
}

std::string render(std::string_view body,
                   const std::map<std::string, std::string> & params) {
    std::string b{body};
    static const std::regex re(R"(\{\{\s*([A-Za-z_][A-Za-z0-9_]*)\s*\}\})");
    std::string out;
    size_t pos = 0;
    for (auto it = std::sregex_iterator(b.begin(), b.end(), re);
         it != std::sregex_iterator(); ++it) {
        size_t mpos = (size_t)it->position(0);
        size_t mlen = (size_t)it->length(0);
        out.append(b, pos, mpos - pos);
        std::string name = (*it)[1].str();
        auto kv = params.find(name);
        if (kv != params.end()) out.append(kv->second);
        else                    out.append(b, mpos, mlen);     // leave marker
        pos = mpos + mlen;
    }
    out.append(b, pos, std::string::npos);
    return out;
}

}
