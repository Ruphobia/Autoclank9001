// SPDX-License-Identifier: GPL-3.0-or-later
#include "sessions_store.hpp"

#include "../005_context/context.hpp"
#include "../005_context/sqlite3.h"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <random>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

namespace sessions_store {
namespace {

namespace fs = std::filesystem;
using json   = nlohmann::json;

std::mutex g_mtx;

int64_t now_ms() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()).count();
}

// IDs are UUIDv4 hex w/ dashes - strict shape so legacy filenames
// (`session.sqlite`, `session_1782...sqlite`) don't get listed.
bool looks_like_uuid(const std::string & s) {
    static const std::regex re(
        R"([0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12})",
        std::regex::optimize);
    return std::regex_match(s, re);
}

std::string make_uuid_v4() {
    static std::random_device rd;
    static std::mt19937_64    rng{
        ((uint64_t)rd() << 32) ^ (uint64_t)rd() ^
        (uint64_t)std::chrono::steady_clock::now().time_since_epoch().count()
    };
    char buf[37];
    uint64_t a = rng(), b = rng();
    std::snprintf(buf, sizeof(buf),
        "%08x-%04x-4%03x-%04x-%012lx",
        (unsigned)((a >> 32) & 0xffffffff),
        (unsigned)((a >> 16) & 0xffff),
        (unsigned)(a & 0xfff),
        (unsigned)(((b >> 48) & 0x3fff) | 0x8000),
        (unsigned long)(b & 0xffffffffffffull));
    return std::string(buf);
}

fs::path dir() { return fs::path(context::sessions_dir()); }
fs::path json_path(const std::string & id)   { return dir() / (id + ".json"); }
fs::path sqlite_path(const std::string & id) { return dir() / (id + ".sqlite"); }

// Count user-input + AI-answer rows in <id>.sqlite without disturbing the
// active context handle. Read-only open. Returns 0 on any error.
int64_t count_messages(const std::string & id) {
    fs::path p = sqlite_path(id);
    if (!fs::exists(p)) return 0;
    sqlite3 * db = nullptr;
    if (sqlite3_open_v2(p.string().c_str(), &db,
                        SQLITE_OPEN_READONLY | SQLITE_OPEN_FULLMUTEX,
                        nullptr) != SQLITE_OK) {
        if (db) sqlite3_close(db);
        return 0;
    }
    int64_t n = 0;
    sqlite3_stmt * st = nullptr;
    const char * sql =
        "SELECT COUNT(*) FROM turns "
        "WHERE (layer='user' AND kind='input') "
        "   OR (layer LIKE '%answer' AND kind='response') "
        "   OR (layer='shell' AND kind='command')";
    if (sqlite3_prepare_v2(db, sql, -1, &st, nullptr) == SQLITE_OK) {
        if (sqlite3_step(st) == SQLITE_ROW) n = sqlite3_column_int64(st, 0);
        sqlite3_finalize(st);
    }
    sqlite3_close(db);
    return n;
}

SessionMeta read_meta_locked(const std::string & id) {
    SessionMeta m;
    m.id = id;
    fs::path p = json_path(id);
    if (fs::exists(p)) {
        std::ifstream f(p);
        json j = json::parse(f, nullptr, false);
        if (j.is_object()) {
            m.name        = j.value("name", std::string{});
            m.root_dir    = j.value("root_dir", std::string{});
            m.created_at  = j.value("created_at", (int64_t)0);
            m.last_active = j.value("last_active", (int64_t)0);
        }
    }
    if (m.created_at == 0) {
        std::error_code ec;
        auto t = fs::last_write_time(sqlite_path(id), ec);
        if (!ec) {
            using namespace std::chrono;
            auto sysT = time_point_cast<system_clock::duration>(
                t - decltype(t)::clock::now() + system_clock::now());
            m.created_at = duration_cast<milliseconds>(
                sysT.time_since_epoch()).count();
        }
    }
    if (m.last_active == 0) m.last_active = m.created_at;
    m.message_count = count_messages(id);
    return m;
}

void write_meta_locked(const SessionMeta & m,
                       const std::string & ui_blob_or_empty) {
    json j;
    fs::path p = json_path(m.id);
    if (fs::exists(p)) {
        std::ifstream f(p);
        j = json::parse(f, nullptr, false);
        if (!j.is_object()) j = json::object();
    } else {
        j = json::object();
    }
    j["id"]          = m.id;
    j["name"]        = m.name;
    j["root_dir"]    = m.root_dir;
    j["created_at"]  = m.created_at;
    j["last_active"] = m.last_active;
    if (!ui_blob_or_empty.empty()) {
        json u = json::parse(ui_blob_or_empty, nullptr, false);
        if (!u.is_discarded()) j["ui"] = u;
    } else if (!j.contains("ui")) {
        j["ui"] = json::object();
    }
    // tmp + rename for atomicity.
    fs::path tmp = p;
    tmp += ".tmp";
    {
        std::ofstream f(tmp, std::ios::trunc);
        f << j.dump(2);
    }
    fs::rename(tmp, p);
}

}  // namespace

std::vector<SessionMeta> list() {
    std::lock_guard<std::mutex> lk(g_mtx);
    std::vector<SessionMeta> out;
    fs::path d = dir();
    std::error_code ec;
    if (!fs::exists(d, ec)) return out;
    for (auto & ent : fs::directory_iterator(d, ec)) {
        if (!ent.is_regular_file()) continue;
        if (ent.path().extension() != ".sqlite") continue;
        std::string stem = ent.path().stem().string();
        if (!looks_like_uuid(stem)) continue;
        out.push_back(read_meta_locked(stem));
    }
    std::sort(out.begin(), out.end(),
              [](const SessionMeta & a, const SessionMeta & b) {
                  return a.last_active > b.last_active;
              });
    return out;
}

SessionMeta create(std::string_view name_sv, std::string_view root_sv) {
    std::lock_guard<std::mutex> lk(g_mtx);
    SessionMeta m;
    m.id          = make_uuid_v4();
    m.name        = std::string(name_sv);
    m.root_dir    = std::string(root_sv);
    m.created_at  = now_ms();
    m.last_active = m.created_at;
    // Touch the sqlite file so list() picks this session up. The schema
    // is installed lazily by context::switch_to when the session actually
    // becomes active; an empty file is enough for now.
    {
        std::ofstream f(sqlite_path(m.id).string(), std::ios::binary);
    }
    write_meta_locked(m, {});
    m.message_count = 0;
    return m;
}

std::string read_ui(std::string_view id_sv) {
    std::lock_guard<std::mutex> lk(g_mtx);
    std::string id(id_sv);
    fs::path p = json_path(id);
    if (!fs::exists(p)) return "{}";
    std::ifstream f(p);
    json j = json::parse(f, nullptr, false);
    if (!j.is_object()) return "{}";
    if (!j.contains("ui")) return "{}";
    return j["ui"].dump();
}

void write_ui(std::string_view id_sv, std::string_view ui_blob) {
    std::lock_guard<std::mutex> lk(g_mtx);
    std::string id(id_sv);
    if (!looks_like_uuid(id)) return;
    SessionMeta m = read_meta_locked(id);
    if (m.created_at == 0) m.created_at = now_ms();
    m.last_active = now_ms();
    // Promote any folder hint from the UI blob into root_dir for the picker.
    json u = json::parse(std::string(ui_blob), nullptr, false);
    if (u.is_object()) {
        if (u.contains("rootDir") && u["rootDir"].is_string())
            m.root_dir = u["rootDir"].get<std::string>();
    }
    write_meta_locked(m, std::string(ui_blob));
}

void patch(std::string_view id_sv,
           std::string_view name_sv,
           std::string_view root_sv) {
    std::lock_guard<std::mutex> lk(g_mtx);
    std::string id(id_sv);
    SessionMeta m = read_meta_locked(id);
    if (!name_sv.empty()) m.name     = std::string(name_sv);
    if (!root_sv.empty()) m.root_dir = std::string(root_sv);
    m.last_active = now_ms();
    write_meta_locked(m, {});
}

void touch(std::string_view id_sv) {
    std::lock_guard<std::mutex> lk(g_mtx);
    std::string id(id_sv);
    SessionMeta m = read_meta_locked(id);
    if (m.created_at == 0) m.created_at = now_ms();
    m.last_active = now_ms();
    write_meta_locked(m, {});
}

bool forget(std::string_view id_sv) {
    std::lock_guard<std::mutex> lk(g_mtx);
    std::string id(id_sv);
    if (!looks_like_uuid(id)) return false;
    bool any = false;
    std::error_code ec;
    for (const std::string suf : { ".sqlite", ".sqlite-wal", ".sqlite-shm",
                                   ".json" }) {
        fs::path p = dir() / (id + suf);
        if (fs::remove(p, ec)) any = true;
    }
    return any;
}

bool exists(std::string_view id_sv) {
    std::string id(id_sv);
    if (!looks_like_uuid(id)) return false;
    std::error_code ec;
    return fs::exists(sqlite_path(id), ec);
}

std::vector<ChatMessage> chat_history(std::string_view id_sv) {
    std::vector<ChatMessage> out;
    std::string id(id_sv);
    if (!looks_like_uuid(id)) return out;

    fs::path p = sqlite_path(id);
    if (!fs::exists(p)) return out;

    sqlite3 * db = nullptr;
    if (sqlite3_open_v2(p.string().c_str(), &db,
                        SQLITE_OPEN_READONLY | SQLITE_OPEN_FULLMUTEX,
                        nullptr) != SQLITE_OK) {
        if (db) sqlite3_close(db);
        return out;
    }

    sqlite3_stmt * st = nullptr;
    const char * sql =
        "SELECT ts, turn, layer, kind, content FROM turns "
        "WHERE (layer='user' AND kind='input') "
        "   OR (layer LIKE '%answer' AND kind='response') "
        "   OR (layer='shell' AND kind='command') "
        "   OR (layer='statement' AND kind='echo') "
        "ORDER BY ts ASC, id ASC";
    if (sqlite3_prepare_v2(db, sql, -1, &st, nullptr) != SQLITE_OK) {
        sqlite3_close(db);
        return out;
    }
    while (sqlite3_step(st) == SQLITE_ROW) {
        ChatMessage m;
        m.ts   = sqlite3_column_int64(st, 0);
        m.turn = sqlite3_column_int64(st, 1);
        std::string layer = reinterpret_cast<const char *>(
            sqlite3_column_text(st, 2));
        m.text = reinterpret_cast<const char *>(
            sqlite3_column_text(st, 4));
        m.role = (layer == "user") ? "user" : "ai";
        out.push_back(std::move(m));
    }
    sqlite3_finalize(st);
    sqlite3_close(db);
    return out;
}

std::vector<TurnRow> turn_rows(std::string_view id_sv) {
    std::vector<TurnRow> out;
    std::string id(id_sv);
    if (!looks_like_uuid(id)) return out;

    fs::path p = sqlite_path(id);
    if (!fs::exists(p)) return out;

    sqlite3 * db = nullptr;
    if (sqlite3_open_v2(p.string().c_str(), &db,
                        SQLITE_OPEN_READONLY | SQLITE_OPEN_FULLMUTEX,
                        nullptr) != SQLITE_OK) {
        if (db) sqlite3_close(db);
        return out;
    }

    sqlite3_stmt * st = nullptr;
    const char * sql =
        "SELECT ts, turn, layer, kind, COALESCE(meta,''), content FROM turns "
        "ORDER BY ts ASC, id ASC";
    if (sqlite3_prepare_v2(db, sql, -1, &st, nullptr) != SQLITE_OK) {
        sqlite3_close(db);
        return out;
    }
    while (sqlite3_step(st) == SQLITE_ROW) {
        TurnRow r;
        r.ts    = sqlite3_column_int64(st, 0);
        r.turn  = sqlite3_column_int64(st, 1);
        r.layer   = reinterpret_cast<const char *>(sqlite3_column_text(st, 2));
        r.kind    = reinterpret_cast<const char *>(sqlite3_column_text(st, 3));
        r.meta    = reinterpret_cast<const char *>(sqlite3_column_text(st, 4));
        r.content = reinterpret_cast<const char *>(sqlite3_column_text(st, 5));
        out.push_back(std::move(r));
    }
    sqlite3_finalize(st);
    sqlite3_close(db);
    return out;
}

int64_t message_count(std::string_view id_sv) {
    std::string id(id_sv);
    if (!looks_like_uuid(id)) return 0;
    return count_messages(id);
}

int sweep_empty_ghosts(int64_t older_than_ms) {
    int n = 0;
    fs::path d = dir();
    std::error_code ec;
    if (!fs::exists(d, ec)) return 0;
    const std::string keep = context::current_id();
    const auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    for (auto & ent : fs::directory_iterator(d, ec)) {
        if (!ent.is_regular_file()) continue;
        if (ent.path().extension() != ".sqlite") continue;
        const std::string id = ent.path().stem().string();
        if (!looks_like_uuid(id))               continue;
        if (id == keep)                         continue;
        // Ghost = no companion .json (context::new_session doesn't
        // write one; sessions_store::create does).
        if (fs::exists(json_path(id), ec))      continue;
        if (count_messages(id) != 0)            continue;
        auto ft = fs::last_write_time(ent.path(), ec);
        if (ec) continue;
        auto sysT = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            ft - decltype(ft)::clock::now() + std::chrono::system_clock::now());
        int64_t age_ms = now_ms -
            std::chrono::duration_cast<std::chrono::milliseconds>(
                sysT.time_since_epoch()).count();
        if (age_ms < older_than_ms) continue;
        // Delete .sqlite and any WAL/SHM siblings; .json is already
        // known absent (that's the ghost signature).
        fs::remove(ent.path(), ec);
        fs::remove(dir() / (id + ".sqlite-wal"), ec);
        fs::remove(dir() / (id + ".sqlite-shm"), ec);
        ++n;
    }
    if (n > 0) {
        std::fprintf(stderr,
            "sessions_store: swept %d empty ghost session(s) "
            "(older than %lld ms)\n", n, (long long) older_than_ms);
    }
    return n;
}

}  // namespace sessions_store
