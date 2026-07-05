#include "doc_reader.hpp"

#include "../011_scripting/scripting.hpp"
#include "../014_format_translators/translators.hpp"

#include "sqlite3.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <sstream>
#include <stdexcept>

namespace doc_reader {
namespace {

namespace fs = std::filesystem;

constexpr size_t kChunkSize    = 1200;   // characters per chunk
constexpr size_t kChunkOverlap = 200;    // characters of overlap between chunks

std::mutex   g_mu;
sqlite3 *    g_db = nullptr;
fs::path     g_index_path;

int64_t now_unix() {
    using namespace std::chrono;
    return duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
}

void must_ok(int rc, const char * where) {
    if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) {
        std::string m = g_db ? sqlite3_errmsg(g_db) : "(no db)";
        throw std::runtime_error(std::string("doc_reader ") + where + ": " + m);
    }
}

void exec(const char * sql) {
    char * err = nullptr;
    int rc = sqlite3_exec(g_db, sql, nullptr, nullptr, &err);
    if (rc != SQLITE_OK) {
        std::string m = err ? err : "(unknown sqlite error)";
        if (err) sqlite3_free(err);
        throw std::runtime_error(std::string("doc_reader exec: ") + m + " sql=" + sql);
    }
}

void install_schema() {
    exec("PRAGMA journal_mode = WAL;");
    exec("PRAGMA synchronous = NORMAL;");
    exec("PRAGMA foreign_keys = ON;");

    exec(R"sql(
        CREATE TABLE IF NOT EXISTS documents (
            id           INTEGER PRIMARY KEY AUTOINCREMENT,
            path         TEXT    NOT NULL UNIQUE,
            format       TEXT    NOT NULL,
            ingested_at  INTEGER NOT NULL,
            chunk_count  INTEGER NOT NULL DEFAULT 0
        );
    )sql");

    exec(R"sql(
        CREATE TABLE IF NOT EXISTS chunks (
            id        INTEGER PRIMARY KEY AUTOINCREMENT,
            doc_id    INTEGER NOT NULL REFERENCES documents(id) ON DELETE CASCADE,
            chunk_ix  INTEGER NOT NULL,
            text      TEXT    NOT NULL
        );
    )sql");
    exec("CREATE INDEX IF NOT EXISTS idx_chunks_doc ON chunks(doc_id);");

    exec(R"sql(
        CREATE VIRTUAL TABLE IF NOT EXISTS chunks_fts USING fts5(
            text,
            content='chunks',
            content_rowid='id',
            tokenize='porter unicode61'
        );
    )sql");
    exec(R"sql(
        CREATE TRIGGER IF NOT EXISTS chunks_ai AFTER INSERT ON chunks BEGIN
            INSERT INTO chunks_fts(rowid, text) VALUES (new.id, new.text);
        END;
    )sql");
    exec(R"sql(
        CREATE TRIGGER IF NOT EXISTS chunks_ad AFTER DELETE ON chunks BEGIN
            INSERT INTO chunks_fts(chunks_fts, rowid, text)
                VALUES('delete', old.id, old.text);
        END;
    )sql");
}

std::string read_file(const fs::path & p) {
    std::ifstream in(p, std::ios::binary);
    if (!in) throw std::runtime_error("doc_reader: cannot read " + p.string());
    std::ostringstream b; b << in.rdbuf();
    return b.str();
}

std::string strip_html(std::string_view in) {
    std::string out;
    out.reserve(in.size());
    bool in_tag = false;
    for (size_t i = 0; i < in.size(); ++i) {
        char c = in[i];
        if (c == '<') { in_tag = true; continue; }
        if (c == '>') { in_tag = false; out.push_back(' '); continue; }
        if (!in_tag) out.push_back(c);
    }
    // Collapse whitespace.
    std::string collapsed;
    collapsed.reserve(out.size());
    bool ws = false;
    for (char c : out) {
        if (std::isspace(static_cast<unsigned char>(c))) {
            if (!ws) { collapsed.push_back(' '); ws = true; }
        } else { collapsed.push_back(c); ws = false; }
    }
    return collapsed;
}

std::string lower_ext(const fs::path & p) {
    auto e = p.extension().string();
    if (!e.empty() && e.front() == '.') e.erase(0, 1);
    std::transform(e.begin(), e.end(), e.begin(),
                   [](unsigned char c){ return (char)std::tolower(c); });
    return e;
}

std::vector<std::string> chunk_text(const std::string & body) {
    std::vector<std::string> out;
    if (body.empty()) return out;
    const size_t step = (kChunkSize > kChunkOverlap) ? (kChunkSize - kChunkOverlap) : kChunkSize;
    for (size_t off = 0; off < body.size(); off += step) {
        size_t take = std::min(kChunkSize, body.size() - off);
        out.emplace_back(body.substr(off, take));
        if (off + take >= body.size()) break;
    }
    return out;
}

// Convert non-text formats to plain text via pandoc, returning the
// extracted body on success or std::nullopt if pandoc is missing or
// failed. The temporary output lives in /tmp and is removed.
std::optional<std::string> to_text_via_pandoc(const fs::path & p) {
    if (!translators::tool_available("pandoc")) return std::nullopt;
    fs::path tmp = fs::temp_directory_path() / ("doc_reader_" + std::to_string(now_unix()) + ".txt");
    std::string cmd = "pandoc -t plain -o '" + tmp.string() + "' -- '" + p.string() + "'";
    auto r = scripting::execute(cmd, {}, 60);
    if (r.exit_code != 0 || !fs::exists(tmp)) return std::nullopt;
    std::string body;
    try { body = read_file(tmp); } catch (...) { body.clear(); }
    std::error_code ec; fs::remove(tmp, ec);
    return body;
}

void insert_document(const fs::path & p, const std::string & format,
                     const std::vector<std::string> & chunks,
                     Document & out) {
    exec("BEGIN IMMEDIATE;");
    try {
        sqlite3_stmt * st = nullptr;
        must_ok(sqlite3_prepare_v2(g_db,
            "INSERT INTO documents(path, format, ingested_at, chunk_count) "
            "VALUES(?,?,?,?) "
            "ON CONFLICT(path) DO UPDATE SET "
            "  format=excluded.format, ingested_at=excluded.ingested_at, "
            "  chunk_count=excluded.chunk_count",
            -1, &st, nullptr), "prepare doc upsert");
        sqlite3_bind_text (st, 1, p.string().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text (st, 2, format.c_str(),     -1, SQLITE_TRANSIENT);
        sqlite3_bind_int64(st, 3, now_unix());
        sqlite3_bind_int64(st, 4, (sqlite3_int64)chunks.size());
        must_ok(sqlite3_step(st), "step doc upsert");
        sqlite3_finalize(st);

        // Look up the row id (handles both insert and on-conflict update).
        sqlite3_stmt * idst = nullptr;
        must_ok(sqlite3_prepare_v2(g_db,
            "SELECT id FROM documents WHERE path = ?", -1, &idst, nullptr),
            "prepare doc id");
        sqlite3_bind_text(idst, 1, p.string().c_str(), -1, SQLITE_TRANSIENT);
        int rc = sqlite3_step(idst);
        if (rc != SQLITE_ROW) {
            sqlite3_finalize(idst);
            throw std::runtime_error("doc_reader: doc id lookup empty");
        }
        int64_t doc_id = sqlite3_column_int64(idst, 0);
        sqlite3_finalize(idst);

        // Drop any old chunks, insert the new ones.
        sqlite3_stmt * delst = nullptr;
        must_ok(sqlite3_prepare_v2(g_db,
            "DELETE FROM chunks WHERE doc_id = ?", -1, &delst, nullptr),
            "prepare chunk wipe");
        sqlite3_bind_int64(delst, 1, doc_id);
        must_ok(sqlite3_step(delst), "step chunk wipe");
        sqlite3_finalize(delst);

        sqlite3_stmt * inst = nullptr;
        must_ok(sqlite3_prepare_v2(g_db,
            "INSERT INTO chunks(doc_id, chunk_ix, text) VALUES(?,?,?)",
            -1, &inst, nullptr), "prepare chunk insert");
        for (size_t i = 0; i < chunks.size(); ++i) {
            sqlite3_reset(inst);
            sqlite3_bind_int64(inst, 1, doc_id);
            sqlite3_bind_int64(inst, 2, (sqlite3_int64)i);
            sqlite3_bind_text (inst, 3, chunks[i].c_str(), -1, SQLITE_TRANSIENT);
            must_ok(sqlite3_step(inst), "step chunk insert");
        }
        sqlite3_finalize(inst);

        out.id          = doc_id;
        out.path        = p.string();
        out.format      = format;
        out.ingested_at = now_unix();
        out.chunk_count = chunks.size();

        exec("COMMIT;");
    } catch (...) {
        exec("ROLLBACK;");
        throw;
    }
}

}

void open(std::string_view project_root) {
    std::lock_guard<std::mutex> lk(g_mu);
    fs::path root = project_root.empty()
                        ? fs::current_path()
                        : fs::path(std::string(project_root));
    fs::path dir = root / ".tool" / "rag";
    fs::create_directories(dir);
    fs::path path = dir / "index.sqlite";

    if (g_db) { sqlite3_close(g_db); g_db = nullptr; }
    int rc = sqlite3_open_v2(
        path.string().c_str(), &g_db,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX,
        nullptr);
    if (rc != SQLITE_OK) {
        std::string m = g_db ? sqlite3_errmsg(g_db) : "(open failed)";
        if (g_db) { sqlite3_close(g_db); g_db = nullptr; }
        throw std::runtime_error("doc_reader::open: " + m);
    }
    g_index_path = path;
    install_schema();
}

void close() {
    std::lock_guard<std::mutex> lk(g_mu);
    if (g_db) { sqlite3_close(g_db); g_db = nullptr; }
    g_index_path.clear();
}

bool ingest(std::string_view file_path) {
    std::lock_guard<std::mutex> lk(g_mu);
    if (!g_db) throw std::runtime_error("doc_reader::ingest: index not opened");

    fs::path p{std::string(file_path)};
    if (!fs::exists(p)) throw std::runtime_error("doc_reader::ingest: no such file: " + p.string());

    std::string ext = lower_ext(p);
    std::string format;
    std::string body;

    if (ext == "txt" || ext == "log" || ext == "csv" || ext == "tsv" || ext.empty()) {
        format = "text"; body = read_file(p);
    } else if (ext == "md" || ext == "markdown") {
        format = "markdown"; body = read_file(p);
    } else if (ext == "html" || ext == "htm" || ext == "xml") {
        format = "html"; body = strip_html(read_file(p));
    } else {
        auto via = to_text_via_pandoc(p);
        if (!via) return false;
        format = "pandoc"; body = *via;
    }

    auto chunks = chunk_text(body);
    Document out;
    insert_document(p, format, chunks, out);
    return true;
}

void forget(int64_t doc_id) {
    std::lock_guard<std::mutex> lk(g_mu);
    if (!g_db) throw std::runtime_error("doc_reader::forget: index not opened");
    sqlite3_stmt * st = nullptr;
    must_ok(sqlite3_prepare_v2(g_db, "DELETE FROM documents WHERE id = ?", -1, &st, nullptr),
            "prepare forget");
    sqlite3_bind_int64(st, 1, doc_id);
    must_ok(sqlite3_step(st), "step forget");
    sqlite3_finalize(st);
}

void reset() {
    std::lock_guard<std::mutex> lk(g_mu);
    if (!g_db) throw std::runtime_error("doc_reader::reset: index not opened");
    exec("DELETE FROM documents;");
    exec("DELETE FROM chunks;");
    exec("INSERT INTO chunks_fts(chunks_fts) VALUES('rebuild');");
}

std::vector<Document> list() {
    std::lock_guard<std::mutex> lk(g_mu);
    std::vector<Document> out;
    if (!g_db) return out;
    sqlite3_stmt * st = nullptr;
    must_ok(sqlite3_prepare_v2(g_db,
        "SELECT id, path, format, ingested_at, chunk_count FROM documents ORDER BY id",
        -1, &st, nullptr), "prepare list");
    while (sqlite3_step(st) == SQLITE_ROW) {
        Document d;
        d.id          = sqlite3_column_int64(st, 0);
        d.path        = reinterpret_cast<const char *>(sqlite3_column_text(st, 1));
        d.format      = reinterpret_cast<const char *>(sqlite3_column_text(st, 2));
        d.ingested_at = sqlite3_column_int64(st, 3);
        d.chunk_count = (size_t)sqlite3_column_int64(st, 4);
        out.push_back(std::move(d));
    }
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
        SELECT documents.id, documents.path, documents.format,
               documents.ingested_at, documents.chunk_count,
               chunks.id, chunks.chunk_ix, chunks.text,
               bm25(chunks_fts) AS score,
               snippet(chunks_fts, 0, '[[', ']]', '...', 16) AS snip
          FROM chunks_fts
          JOIN chunks    ON chunks.id    = chunks_fts.rowid
          JOIN documents ON documents.id = chunks.doc_id
         WHERE chunks_fts MATCH ?
         ORDER BY score
         LIMIT ?
    )sql", -1, &st, nullptr), "prepare search");
    sqlite3_bind_text (st, 1, std::string(query).c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(st, 2, (sqlite3_int64)top_k);

    while (sqlite3_step(st) == SQLITE_ROW) {
        Hit h;
        h.document.id          = sqlite3_column_int64(st, 0);
        h.document.path        = reinterpret_cast<const char *>(sqlite3_column_text(st, 1));
        h.document.format      = reinterpret_cast<const char *>(sqlite3_column_text(st, 2));
        h.document.ingested_at = sqlite3_column_int64(st, 3);
        h.document.chunk_count = (size_t)sqlite3_column_int64(st, 4);
        h.chunk.chunk_id       = sqlite3_column_int64(st, 5);
        h.chunk.chunk_ix       = (size_t)sqlite3_column_int64(st, 6);
        h.chunk.doc_id         = h.document.id;
        const unsigned char * tx = sqlite3_column_text(st, 7);
        h.chunk.text           = tx ? reinterpret_cast<const char *>(tx) : "";
        h.score                = sqlite3_column_double(st, 8);
        const unsigned char * sn = sqlite3_column_text(st, 9);
        h.snippet              = sn ? reinterpret_cast<const char *>(sn) : "";
        out.push_back(std::move(h));
    }
    sqlite3_finalize(st);
    return out;
}

}
