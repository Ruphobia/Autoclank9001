#include "reading_queue.hpp"

#include "sqlite3.h"

#include <chrono>
#include <filesystem>
#include <mutex>
#include <stdexcept>

namespace reading_queue {
namespace {

namespace fs = std::filesystem;

std::mutex g_mu;
sqlite3 *  g_db = nullptr;

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
        throw std::runtime_error(std::string("reading_queue exec: ") + m);
    }
}
void must_ok(int rc, const char * where) {
    if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) {
        std::string m = g_db ? sqlite3_errmsg(g_db) : "(no db)";
        throw std::runtime_error(std::string("reading_queue ") + where + ": " + m);
    }
}

void install_schema() {
    exec("PRAGMA journal_mode = WAL;");
    exec("PRAGMA synchronous = NORMAL;");
    exec(R"sql(
        CREATE TABLE IF NOT EXISTS items (
            id                     INTEGER PRIMARY KEY AUTOINCREMENT,
            ref_id                 INTEGER NOT NULL DEFAULT 0,
            title                  TEXT    NOT NULL,
            priority               INTEGER NOT NULL DEFAULT 3,
            status                 TEXT    NOT NULL DEFAULT 'unread',
            eta_minutes            INTEGER NOT NULL DEFAULT 0,
            total_session_minutes  INTEGER NOT NULL DEFAULT 0,
            added_unix             INTEGER NOT NULL,
            last_session_unix      INTEGER NOT NULL DEFAULT 0
        );
    )sql");
    exec("CREATE INDEX IF NOT EXISTS idx_q_status ON items(status);");
    exec("CREATE INDEX IF NOT EXISTS idx_q_prio   ON items(priority);");
}

Item row_to_item(sqlite3_stmt * st) {
    Item it;
    auto col = [&](int i) -> std::string {
        const unsigned char * p = sqlite3_column_text(st, i);
        return p ? std::string(reinterpret_cast<const char *>(p)) : std::string{};
    };
    it.id                    = sqlite3_column_int64(st, 0);
    it.ref_id                = sqlite3_column_int64(st, 1);
    it.title                 = col(2);
    it.priority              = (int)sqlite3_column_int(st, 3);
    it.status                = status_from_string(col(4));
    it.eta_minutes           = (int)sqlite3_column_int(st, 5);
    it.total_session_minutes = (int)sqlite3_column_int(st, 6);
    it.added_unix            = sqlite3_column_int64(st, 7);
    it.last_session_unix     = sqlite3_column_int64(st, 8);
    return it;
}

}

const char * to_string(Status s) {
    switch (s) {
        case Status::Unread:  return "unread";
        case Status::Reading: return "reading";
        case Status::Read:    return "read";
        case Status::Dropped: return "dropped";
    }
    return "unread";
}
Status status_from_string(std::string_view s) {
    if (s == "unread")  return Status::Unread;
    if (s == "reading") return Status::Reading;
    if (s == "read")    return Status::Read;
    if (s == "dropped") return Status::Dropped;
    return Status::Unread;
}

void open(std::string_view project_root) {
    std::lock_guard<std::mutex> lk(g_mu);
    fs::path root = project_root.empty() ? fs::current_path()
                                         : fs::path(std::string(project_root));
    fs::path dir = root / ".tool" / "reading";
    fs::create_directories(dir);
    fs::path path = dir / "queue.sqlite";
    if (g_db) { sqlite3_close(g_db); g_db = nullptr; }
    int rc = sqlite3_open_v2(path.string().c_str(), &g_db,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, nullptr);
    if (rc != SQLITE_OK) {
        std::string m = g_db ? sqlite3_errmsg(g_db) : "(open failed)";
        if (g_db) { sqlite3_close(g_db); g_db = nullptr; }
        throw std::runtime_error("reading_queue::open: " + m);
    }
    install_schema();
}
void close() {
    std::lock_guard<std::mutex> lk(g_mu);
    if (g_db) { sqlite3_close(g_db); g_db = nullptr; }
}

int64_t add(int64_t ref_id, std::string_view title, int priority, int eta_minutes) {
    std::lock_guard<std::mutex> lk(g_mu);
    if (!g_db) throw std::runtime_error("reading_queue::add: not opened");
    if (priority < 1) priority = 1;
    if (priority > 5) priority = 5;
    sqlite3_stmt * st = nullptr;
    must_ok(sqlite3_prepare_v2(g_db,
        "INSERT INTO items(ref_id, title, priority, eta_minutes, added_unix)"
        " VALUES(?,?,?,?,?)", -1, &st, nullptr), "prepare add");
    std::string t{title};
    sqlite3_bind_int64(st, 1, ref_id);
    sqlite3_bind_text (st, 2, t.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int  (st, 3, priority);
    sqlite3_bind_int  (st, 4, eta_minutes);
    sqlite3_bind_int64(st, 5, now_unix());
    must_ok(sqlite3_step(st), "step add");
    int64_t id = sqlite3_last_insert_rowid(g_db);
    sqlite3_finalize(st);
    return id;
}

namespace {
void set_int_col(int64_t id, const char * col, int v) {
    sqlite3_stmt * st = nullptr;
    std::string sql = std::string("UPDATE items SET ") + col + "=? WHERE id=?";
    must_ok(sqlite3_prepare_v2(g_db, sql.c_str(), -1, &st, nullptr), "prepare set");
    sqlite3_bind_int  (st, 1, v);
    sqlite3_bind_int64(st, 2, id);
    must_ok(sqlite3_step(st), "step set");
    sqlite3_finalize(st);
}
}

void set_status(int64_t id, Status s) {
    std::lock_guard<std::mutex> lk(g_mu);
    if (!g_db) throw std::runtime_error("reading_queue::set_status: not opened");
    sqlite3_stmt * st = nullptr;
    must_ok(sqlite3_prepare_v2(g_db, "UPDATE items SET status=? WHERE id=?",
                               -1, &st, nullptr), "prepare set_status");
    sqlite3_bind_text (st, 1, to_string(s), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(st, 2, id);
    must_ok(sqlite3_step(st), "step set_status");
    sqlite3_finalize(st);
}
void set_priority(int64_t id, int priority) {
    std::lock_guard<std::mutex> lk(g_mu);
    if (priority < 1) priority = 1;
    if (priority > 5) priority = 5;
    if (!g_db) throw std::runtime_error("reading_queue::set_priority: not opened");
    set_int_col(id, "priority", priority);
}
void set_eta(int64_t id, int eta_minutes) {
    std::lock_guard<std::mutex> lk(g_mu);
    if (!g_db) throw std::runtime_error("reading_queue::set_eta: not opened");
    set_int_col(id, "eta_minutes", eta_minutes);
}

void log_session(int64_t id, int minutes, int64_t when_unix) {
    std::lock_guard<std::mutex> lk(g_mu);
    if (!g_db) throw std::runtime_error("reading_queue::log_session: not opened");
    sqlite3_stmt * st = nullptr;
    must_ok(sqlite3_prepare_v2(g_db,
        "UPDATE items SET total_session_minutes = total_session_minutes + ?, "
        " last_session_unix = ? WHERE id=?", -1, &st, nullptr), "prepare log_session");
    sqlite3_bind_int  (st, 1, minutes);
    sqlite3_bind_int64(st, 2, when_unix ? when_unix : now_unix());
    sqlite3_bind_int64(st, 3, id);
    must_ok(sqlite3_step(st), "step log_session");
    sqlite3_finalize(st);
}

void forget(int64_t id) {
    std::lock_guard<std::mutex> lk(g_mu);
    if (!g_db) throw std::runtime_error("reading_queue::forget: not opened");
    sqlite3_stmt * st = nullptr;
    must_ok(sqlite3_prepare_v2(g_db, "DELETE FROM items WHERE id=?",
                               -1, &st, nullptr), "prepare forget");
    sqlite3_bind_int64(st, 1, id);
    must_ok(sqlite3_step(st), "step forget");
    sqlite3_finalize(st);
}

std::optional<Item> get(int64_t id) {
    std::lock_guard<std::mutex> lk(g_mu);
    if (!g_db) return std::nullopt;
    sqlite3_stmt * st = nullptr;
    must_ok(sqlite3_prepare_v2(g_db,
        "SELECT id, ref_id, title, priority, status, eta_minutes,"
        " total_session_minutes, added_unix, last_session_unix"
        " FROM items WHERE id=?", -1, &st, nullptr), "prepare get");
    sqlite3_bind_int64(st, 1, id);
    std::optional<Item> out;
    if (sqlite3_step(st) == SQLITE_ROW) out = row_to_item(st);
    sqlite3_finalize(st);
    return out;
}

std::vector<Item> list_by_status(Status s, size_t limit) {
    std::lock_guard<std::mutex> lk(g_mu);
    std::vector<Item> out;
    if (!g_db) return out;
    if (limit == 0) limit = 100;
    sqlite3_stmt * st = nullptr;
    must_ok(sqlite3_prepare_v2(g_db,
        "SELECT id, ref_id, title, priority, status, eta_minutes,"
        " total_session_minutes, added_unix, last_session_unix"
        " FROM items WHERE status=? ORDER BY priority ASC, id ASC LIMIT ?",
        -1, &st, nullptr), "prepare list_by_status");
    sqlite3_bind_text (st, 1, to_string(s), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(st, 2, (sqlite3_int64)limit);
    while (sqlite3_step(st) == SQLITE_ROW) out.push_back(row_to_item(st));
    sqlite3_finalize(st);
    return out;
}

std::vector<Item> list_all(size_t limit) {
    std::lock_guard<std::mutex> lk(g_mu);
    std::vector<Item> out;
    if (!g_db) return out;
    if (limit == 0) limit = 200;
    sqlite3_stmt * st = nullptr;
    must_ok(sqlite3_prepare_v2(g_db,
        "SELECT id, ref_id, title, priority, status, eta_minutes,"
        " total_session_minutes, added_unix, last_session_unix"
        " FROM items ORDER BY priority ASC, id ASC LIMIT ?",
        -1, &st, nullptr), "prepare list_all");
    sqlite3_bind_int64(st, 1, (sqlite3_int64)limit);
    while (sqlite3_step(st) == SQLITE_ROW) out.push_back(row_to_item(st));
    sqlite3_finalize(st);
    return out;
}

std::vector<Item> recommend(size_t k) {
    std::lock_guard<std::mutex> lk(g_mu);
    std::vector<Item> out;
    if (!g_db) return out;
    if (k == 0) k = 5;
    sqlite3_stmt * st = nullptr;
    must_ok(sqlite3_prepare_v2(g_db,
        "SELECT id, ref_id, title, priority, status, eta_minutes,"
        " total_session_minutes, added_unix, last_session_unix"
        " FROM items WHERE status='unread'"
        " ORDER BY priority ASC, CASE WHEN eta_minutes = 0 THEN 999999 ELSE eta_minutes END ASC,"
        " id ASC LIMIT ?",
        -1, &st, nullptr), "prepare recommend");
    sqlite3_bind_int64(st, 1, (sqlite3_int64)k);
    while (sqlite3_step(st) == SQLITE_ROW) out.push_back(row_to_item(st));
    sqlite3_finalize(st);
    return out;
}

}
