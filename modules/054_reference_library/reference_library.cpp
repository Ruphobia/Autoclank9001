#include "reference_library.hpp"

#include "sqlite3.h"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <mutex>
#include <sstream>
#include <stdexcept>

namespace reference_library {
namespace {

namespace fs = std::filesystem;

std::mutex g_mu;
sqlite3 *  g_db = nullptr;
fs::path   g_root;

void exec(const char * sql) {
    char * err = nullptr;
    int rc = sqlite3_exec(g_db, sql, nullptr, nullptr, &err);
    if (rc != SQLITE_OK) {
        std::string m = err ? err : "(unknown)";
        if (err) sqlite3_free(err);
        throw std::runtime_error(std::string("reference_library exec: ") + m);
    }
}
void must_ok(int rc, const char * where) {
    if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) {
        std::string m = g_db ? sqlite3_errmsg(g_db) : "(no db)";
        throw std::runtime_error(std::string("reference_library ") + where + ": " + m);
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
        CREATE TABLE IF NOT EXISTS items (
            id              INTEGER PRIMARY KEY AUTOINCREMENT,
            citation_key    TEXT    NOT NULL UNIQUE,
            type            TEXT    NOT NULL DEFAULT '',
            title           TEXT    NOT NULL DEFAULT '',
            year            TEXT    NOT NULL DEFAULT '',
            authors_joined  TEXT    NOT NULL DEFAULT '',
            container_title TEXT    NOT NULL DEFAULT '',
            tags            TEXT    NOT NULL DEFAULT '',
            abstract        TEXT    NOT NULL DEFAULT '',
            csl             TEXT    NOT NULL DEFAULT '{}'
        );
    )sql");
    exec("CREATE INDEX IF NOT EXISTS idx_refs_type ON items(type);");
    exec("CREATE INDEX IF NOT EXISTS idx_refs_year ON items(year);");
    exec(R"sql(
        CREATE VIRTUAL TABLE IF NOT EXISTS refs_fts USING fts5(
            title, abstract, authors_joined,
            content='items',
            content_rowid='id',
            tokenize='porter unicode61'
        );
    )sql");
    exec(R"sql(
        CREATE TRIGGER IF NOT EXISTS refs_ai AFTER INSERT ON items BEGIN
            INSERT INTO refs_fts(rowid, title, abstract, authors_joined)
                VALUES (new.id, new.title, new.abstract, new.authors_joined);
        END;
    )sql");
    exec(R"sql(
        CREATE TRIGGER IF NOT EXISTS refs_ad AFTER DELETE ON items BEGIN
            INSERT INTO refs_fts(refs_fts, rowid, title, abstract, authors_joined)
                VALUES('delete', old.id, old.title, old.abstract, old.authors_joined);
        END;
    )sql");
    exec(R"sql(
        CREATE TRIGGER IF NOT EXISTS refs_au AFTER UPDATE ON items BEGIN
            INSERT INTO refs_fts(refs_fts, rowid, title, abstract, authors_joined)
                VALUES('delete', old.id, old.title, old.abstract, old.authors_joined);
            INSERT INTO refs_fts(rowid, title, abstract, authors_joined)
                VALUES (new.id, new.title, new.abstract, new.authors_joined);
        END;
    )sql");
}

std::string author_string(const nlohmann::json & csl) {
    std::ostringstream os;
    if (!csl.contains("author") || !csl["author"].is_array()) return {};
    bool first = true;
    for (const auto & a : csl["author"]) {
        if (!a.is_object()) continue;
        if (!first) os << " | ";
        first = false;
        std::string fam = a.value("family", "");
        std::string giv = a.value("given",  "");
        if (!fam.empty()) os << fam;
        if (!giv.empty()) os << ", " << giv;
    }
    return os.str();
}

std::string csl_year(const nlohmann::json & csl) {
    if (!csl.contains("issued")) return {};
    const auto & iss = csl["issued"];
    if (iss.contains("date-parts") && iss["date-parts"].is_array() &&
        !iss["date-parts"].empty() && iss["date-parts"][0].is_array() &&
        !iss["date-parts"][0].empty()) {
        const auto & y = iss["date-parts"][0][0];
        if (y.is_number_integer()) return std::to_string(y.get<int>());
        if (y.is_string())         return y.get<std::string>();
    }
    if (iss.contains("literal") && iss["literal"].is_string())
        return iss["literal"].get<std::string>();
    return {};
}

Item row_to_item(sqlite3_stmt * st) {
    Item it;
    auto col = [&](int i) -> std::string {
        const unsigned char * p = sqlite3_column_text(st, i);
        return p ? std::string(reinterpret_cast<const char *>(p)) : std::string{};
    };
    it.id              = sqlite3_column_int64(st, 0);
    it.citation_key    = col(1);
    it.type            = col(2);
    it.title           = col(3);
    it.year            = col(4);
    std::string aj     = col(5);
    if (!aj.empty()) {
        std::istringstream is(aj);
        std::string a;
        while (std::getline(is, a, '|')) {
            // trim spaces
            size_t b = 0, e = a.size();
            while (b < e && std::isspace(static_cast<unsigned char>(a[b]))) ++b;
            while (e > b && std::isspace(static_cast<unsigned char>(a[e - 1]))) --e;
            if (b < e) it.authors.push_back(a.substr(b, e - b));
        }
    }
    it.container_title = col(6);
    it.tags            = split_tags(col(7));
    try { it.csl = nlohmann::json::parse(col(9)); } catch (...) { it.csl = nlohmann::json{}; }
    return it;
}

}

void open(std::string_view project_root) {
    std::lock_guard<std::mutex> lk(g_mu);
    fs::path root = project_root.empty() ? fs::current_path()
                                         : fs::path(std::string(project_root));
    g_root = root / ".tool" / "refs";
    fs::create_directories(g_root / "attachments");
    fs::path path = g_root / "refs.sqlite";
    if (g_db) { sqlite3_close(g_db); g_db = nullptr; }
    int rc = sqlite3_open_v2(
        path.string().c_str(), &g_db,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX,
        nullptr);
    if (rc != SQLITE_OK) {
        std::string m = g_db ? sqlite3_errmsg(g_db) : "(open failed)";
        if (g_db) { sqlite3_close(g_db); g_db = nullptr; }
        throw std::runtime_error("reference_library::open: " + m);
    }
    install_schema();
}
void close() {
    std::lock_guard<std::mutex> lk(g_mu);
    if (g_db) { sqlite3_close(g_db); g_db = nullptr; }
    g_root.clear();
}

int64_t add(std::string_view citation_key, const nlohmann::json & csl,
            const std::vector<std::string> & tags) {
    std::lock_guard<std::mutex> lk(g_mu);
    if (!g_db) throw std::runtime_error("reference_library::add: not opened");
    sqlite3_stmt * st = nullptr;
    must_ok(sqlite3_prepare_v2(g_db,
        "INSERT INTO items(citation_key, type, title, year, authors_joined, "
        " container_title, tags, abstract, csl) VALUES(?,?,?,?,?,?,?,?,?)",
        -1, &st, nullptr), "prepare add");
    std::string key{citation_key};
    std::string type    = csl.value("type",    "");
    std::string title   = csl.value("title",   "");
    std::string year    = csl_year(csl);
    std::string aj      = author_string(csl);
    std::string contain = csl.value("container-title", "");
    std::string tagstr  = join_tags(tags);
    std::string abs     = csl.value("abstract", "");
    std::string cslstr  = csl.dump();
    sqlite3_bind_text(st, 1, key.c_str(),     -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 2, type.c_str(),    -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 3, title.c_str(),   -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 4, year.c_str(),    -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 5, aj.c_str(),      -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 6, contain.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 7, tagstr.c_str(),  -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 8, abs.c_str(),     -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 9, cslstr.c_str(),  -1, SQLITE_TRANSIENT);
    must_ok(sqlite3_step(st), "step add");
    int64_t id = sqlite3_last_insert_rowid(g_db);
    sqlite3_finalize(st);
    return id;
}

void update(int64_t id, const nlohmann::json & csl,
            const std::vector<std::string> & tags) {
    std::lock_guard<std::mutex> lk(g_mu);
    if (!g_db) throw std::runtime_error("reference_library::update: not opened");
    sqlite3_stmt * st = nullptr;
    must_ok(sqlite3_prepare_v2(g_db,
        "UPDATE items SET type=?, title=?, year=?, authors_joined=?, "
        " container_title=?, tags=?, abstract=?, csl=? WHERE id=?",
        -1, &st, nullptr), "prepare update");
    std::string type    = csl.value("type",    "");
    std::string title   = csl.value("title",   "");
    std::string year    = csl_year(csl);
    std::string aj      = author_string(csl);
    std::string contain = csl.value("container-title", "");
    std::string tagstr  = join_tags(tags);
    std::string abs     = csl.value("abstract", "");
    std::string cslstr  = csl.dump();
    sqlite3_bind_text (st, 1, type.c_str(),    -1, SQLITE_TRANSIENT);
    sqlite3_bind_text (st, 2, title.c_str(),   -1, SQLITE_TRANSIENT);
    sqlite3_bind_text (st, 3, year.c_str(),    -1, SQLITE_TRANSIENT);
    sqlite3_bind_text (st, 4, aj.c_str(),      -1, SQLITE_TRANSIENT);
    sqlite3_bind_text (st, 5, contain.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text (st, 6, tagstr.c_str(),  -1, SQLITE_TRANSIENT);
    sqlite3_bind_text (st, 7, abs.c_str(),     -1, SQLITE_TRANSIENT);
    sqlite3_bind_text (st, 8, cslstr.c_str(),  -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(st, 9, id);
    must_ok(sqlite3_step(st), "step update");
    sqlite3_finalize(st);
}

void forget(int64_t id) {
    std::lock_guard<std::mutex> lk(g_mu);
    if (!g_db) throw std::runtime_error("reference_library::forget: not opened");
    sqlite3_stmt * st = nullptr;
    must_ok(sqlite3_prepare_v2(g_db, "DELETE FROM items WHERE id=?", -1, &st, nullptr),
            "prepare forget");
    sqlite3_bind_int64(st, 1, id);
    must_ok(sqlite3_step(st), "step forget");
    sqlite3_finalize(st);
}

std::optional<Item> get(int64_t id) {
    std::lock_guard<std::mutex> lk(g_mu);
    if (!g_db) return std::nullopt;
    sqlite3_stmt * st = nullptr;
    must_ok(sqlite3_prepare_v2(g_db,
        "SELECT id, citation_key, type, title, year, authors_joined, "
        " container_title, tags, abstract, csl FROM items WHERE id=?",
        -1, &st, nullptr), "prepare get");
    sqlite3_bind_int64(st, 1, id);
    std::optional<Item> out;
    if (sqlite3_step(st) == SQLITE_ROW) {
        Item it = row_to_item(st);
        // tags lives in column 7; row_to_item picked it up.
        out = it;
    }
    sqlite3_finalize(st);
    return out;
}

std::optional<Item> by_citation_key(std::string_view key) {
    std::lock_guard<std::mutex> lk(g_mu);
    if (!g_db) return std::nullopt;
    sqlite3_stmt * st = nullptr;
    must_ok(sqlite3_prepare_v2(g_db,
        "SELECT id, citation_key, type, title, year, authors_joined, "
        " container_title, tags, abstract, csl FROM items WHERE citation_key=?",
        -1, &st, nullptr), "prepare by_key");
    std::string k{key};
    sqlite3_bind_text(st, 1, k.c_str(), -1, SQLITE_TRANSIENT);
    std::optional<Item> out;
    if (sqlite3_step(st) == SQLITE_ROW) out = row_to_item(st);
    sqlite3_finalize(st);
    return out;
}

std::vector<Item> list(std::string_view type, std::string_view tag, size_t limit) {
    std::lock_guard<std::mutex> lk(g_mu);
    std::vector<Item> out;
    if (!g_db) return out;
    if (limit == 0) limit = 100;
    std::string sql =
        "SELECT id, citation_key, type, title, year, authors_joined, "
        " container_title, tags, abstract, csl FROM items WHERE 1=1";
    if (!type.empty()) sql += " AND type = ?";
    if (!tag.empty())  sql += " AND (' ' || tags || ' ') LIKE ?";
    sql += " ORDER BY id DESC LIMIT ?";
    sqlite3_stmt * st = nullptr;
    must_ok(sqlite3_prepare_v2(g_db, sql.c_str(), -1, &st, nullptr), "prepare list");
    int ix = 1;
    std::string t{type};
    if (!type.empty()) { sqlite3_bind_text(st, ix++, t.c_str(), -1, SQLITE_TRANSIENT); }
    std::string like = "% " + std::string(tag) + " %";
    if (!tag.empty()) sqlite3_bind_text(st, ix++, like.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(st, ix++, (sqlite3_int64)limit);
    while (sqlite3_step(st) == SQLITE_ROW) out.push_back(row_to_item(st));
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
        SELECT items.id, items.citation_key, items.type, items.title, items.year,
               items.authors_joined, items.container_title, items.tags,
               items.abstract, items.csl,
               bm25(refs_fts) AS score,
               snippet(refs_fts, 0, '[[', ']]', '...', 16) AS snip
          FROM refs_fts
          JOIN items ON items.id = refs_fts.rowid
         WHERE refs_fts MATCH ?
         ORDER BY score
         LIMIT ?
    )sql", -1, &st, nullptr), "prepare search");
    std::string q{query};
    sqlite3_bind_text (st, 1, q.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(st, 2, (sqlite3_int64)top_k);
    while (sqlite3_step(st) == SQLITE_ROW) {
        Hit h;
        h.item = row_to_item(st);
        h.score = sqlite3_column_double(st, 10);
        const unsigned char * sn = sqlite3_column_text(st, 11);
        h.snippet = sn ? reinterpret_cast<const char *>(sn) : "";
        out.push_back(std::move(h));
    }
    sqlite3_finalize(st);
    return out;
}

}
