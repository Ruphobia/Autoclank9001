// SPDX-License-Identifier: GPL-3.0-or-later
#include "bench.hpp"

#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace bench {
namespace {

// State for the currently-active ticket. Guarded by g_mtx. Only one
// ticket runs at a time in ac9's ticket_run_worker, so a single global
// slot is sufficient; if that ever changes to multiple concurrent
// runs, this would become a map keyed by ticket_id.
struct Event {
    std::string  role;
    std::string  event;             // "load" | "generate"
    int          gpu{-1};
    std::uint64_t load_ms{0};
    std::string  displaced_role;
    std::uint64_t prompt_tokens{0};
    std::uint64_t output_tokens{0};
    std::uint64_t prefill_ms{0};
    std::uint64_t decode_ms{0};
    std::uint64_t total_ms{0};
    std::string  ts;
};

std::mutex                g_mtx;
std::string               g_ticket_id;
std::string               g_cwd;
std::vector<Event>        g_events;
std::chrono::steady_clock::time_point g_started{};

std::string now_iso_utc() {
    using namespace std::chrono;
    const auto now = system_clock::now();
    const auto t   = system_clock::to_time_t(now);
    std::tm tm{};
    ::gmtime_r(&t, &tm);
    char buf[32];
    std::snprintf(buf, sizeof(buf),
        "%04d-%02d-%02dT%02d:%02d:%02dZ",
        tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
        tm.tm_hour, tm.tm_min, tm.tm_sec);
    return buf;
}

// Minimal JSON string-escaper. Keeps the bench log free of a
// dependency on nlohmann/json inside the leaf-most module (which
// keeps compile time small; only server.cpp needs the full parser).
std::string je(std::string_view s) {
    std::string out;
    out.reserve(s.size() + 2);
    for (char c : s) {
        switch (c) {
            case '"':  out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n";  break;
            case '\r': out += "\\r";  break;
            case '\t': out += "\\t";  break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) {
                    char b[8];
                    std::snprintf(b, sizeof(b), "\\u%04x", c);
                    out += b;
                } else {
                    out += c;
                }
        }
    }
    return out;
}

std::string event_to_jsonl(const Event & e, std::size_t seq,
                           std::string_view ticket_id) {
    std::ostringstream os;
    os << "{\"ticket_id\":\"" << je(ticket_id) << "\","
       << "\"seq\":" << seq << ","
       << "\"ts\":\""     << e.ts << "\","
       << "\"event\":\""  << e.event << "\","
       << "\"role\":\""   << je(e.role) << "\","
       << "\"gpu\":"      << e.gpu;
    if (e.event == "load") {
        os << ",\"load_ms\":" << e.load_ms
           << ",\"displaced_role\":\""
           << je(e.displaced_role) << "\"";
    } else {
        os << ",\"prompt_tokens\":"  << e.prompt_tokens
           << ",\"output_tokens\":"  << e.output_tokens
           << ",\"prefill_ms\":"     << e.prefill_ms
           << ",\"decode_ms\":"      << e.decode_ms
           << ",\"total_ms\":"       << e.total_ms;
        // Derived: tokens/sec for the token-emit loop only, not counting
        // prefill. Cast to double for the divide; guard div-by-zero.
        double tok_s = 0.0;
        if (e.decode_ms > 0) {
            tok_s = static_cast<double>(e.output_tokens) * 1000.0 /
                    static_cast<double>(e.decode_ms);
        }
        char tbuf[32];
        std::snprintf(tbuf, sizeof(tbuf), "%.2f", tok_s);
        os << ",\"decode_tok_s\":" << tbuf;
    }
    os << "}";
    return os.str();
}

void write_jsonl_locked(std::string_view ticket_id,
                        std::string_view cwd,
                        const std::vector<Event> & events) {
    if (cwd.empty() || ticket_id.empty()) return;
    const fs::path dir = fs::path(std::string(cwd)) / ".ac9_runs";
    std::error_code ec;
    fs::create_directories(dir, ec);
    const fs::path p = dir / (std::string(ticket_id) + ".bench.jsonl");
    std::ofstream f(p, std::ios::binary | std::ios::trunc);
    if (!f) {
        std::fprintf(stderr,
            "bench: cannot open %s for write\n", p.c_str());
        return;
    }
    std::size_t seq = 0;
    for (const auto & e : events) {
        f << event_to_jsonl(e, ++seq, ticket_id) << '\n';
    }
}

void print_summary_locked(std::string_view ticket_id,
                          const std::vector<Event> & events) {
    if (events.empty()) return;
    std::uint64_t total_load_ms   = 0;
    std::uint64_t total_prefill_ms = 0;
    std::uint64_t total_decode_ms = 0;
    std::uint64_t total_out_tok    = 0;
    std::uint64_t total_in_tok     = 0;
    int n_loads = 0, n_gen = 0;
    for (const auto & e : events) {
        if (e.event == "load")    { total_load_ms   += e.load_ms; ++n_loads; }
        if (e.event == "generate") {
            total_prefill_ms += e.prefill_ms;
            total_decode_ms  += e.decode_ms;
            total_out_tok    += e.output_tokens;
            total_in_tok     += e.prompt_tokens;
            ++n_gen;
        }
    }
    double overall_tok_s = 0.0;
    if (total_decode_ms > 0) {
        overall_tok_s = static_cast<double>(total_out_tok) * 1000.0 /
                        static_cast<double>(total_decode_ms);
    }
    std::fprintf(stderr,
        "bench: %.*s  n_loads=%d  load_ms=%llu  n_gen=%d  "
        "in_tok=%llu  out_tok=%llu  prefill_ms=%llu  decode_ms=%llu  "
        "overall_tok_s=%.2f\n",
        static_cast<int>(ticket_id.size()), ticket_id.data(),
        n_loads,
        static_cast<unsigned long long>(total_load_ms),
        n_gen,
        static_cast<unsigned long long>(total_in_tok),
        static_cast<unsigned long long>(total_out_tok),
        static_cast<unsigned long long>(total_prefill_ms),
        static_cast<unsigned long long>(total_decode_ms),
        overall_tok_s);
}

}  // namespace

void begin_ticket(std::string_view ticket_id, std::string_view cwd) {
    std::lock_guard<std::mutex> lk(g_mtx);
    g_ticket_id.assign(ticket_id);
    g_cwd.assign(cwd);
    g_events.clear();
    g_started = std::chrono::steady_clock::now();
}

void end_ticket() {
    std::lock_guard<std::mutex> lk(g_mtx);
    if (g_ticket_id.empty()) return;
    write_jsonl_locked(g_ticket_id, g_cwd, g_events);
    print_summary_locked(g_ticket_id, g_events);
    g_ticket_id.clear();
    g_cwd.clear();
    g_events.clear();
}

void record_load(std::string_view role, int gpu,
                 std::uint64_t load_ms,
                 std::string_view displaced_role) {
    std::lock_guard<std::mutex> lk(g_mtx);
    if (g_ticket_id.empty()) return;
    Event e;
    e.role           = std::string(role);
    e.event          = "load";
    e.gpu            = gpu;
    e.load_ms        = load_ms;
    e.displaced_role = std::string(displaced_role);
    e.ts             = now_iso_utc();
    g_events.push_back(std::move(e));
}

void record_generate(std::string_view role, int gpu,
                     std::uint64_t prompt_tokens,
                     std::uint64_t output_tokens,
                     std::uint64_t prefill_ms,
                     std::uint64_t decode_ms,
                     std::uint64_t total_ms) {
    std::lock_guard<std::mutex> lk(g_mtx);
    if (g_ticket_id.empty()) return;
    Event e;
    e.role          = std::string(role);
    e.event         = "generate";
    e.gpu           = gpu;
    e.prompt_tokens = prompt_tokens;
    e.output_tokens = output_tokens;
    e.prefill_ms    = prefill_ms;
    e.decode_ms     = decode_ms;
    e.total_ms      = total_ms;
    e.ts            = now_iso_utc();
    g_events.push_back(std::move(e));
}

// ---- RAII helpers ---------------------------------------------------

struct LoadScope::Impl {
    std::string role;
    int         gpu{-1};
    std::string displaced_role;
    std::chrono::steady_clock::time_point start;
    std::atomic<bool> cancelled{false};
};

LoadScope::LoadScope(std::string_view role, int gpu,
                     std::string_view displaced_role) {
    impl_                 = new Impl();
    impl_->role           = std::string(role);
    impl_->gpu            = gpu;
    impl_->displaced_role = std::string(displaced_role);
    impl_->start          = std::chrono::steady_clock::now();
}

LoadScope::~LoadScope() {
    if (!impl_) return;
    if (!impl_->cancelled.load(std::memory_order_relaxed)) {
        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - impl_->start).count();
        record_load(impl_->role, impl_->gpu,
                    static_cast<std::uint64_t>(ms),
                    impl_->displaced_role);
    }
    delete impl_;
    impl_ = nullptr;
}

void LoadScope::cancel() {
    if (impl_) impl_->cancelled.store(true, std::memory_order_relaxed);
}

struct GenScope::Impl {
    std::string role;
    int         gpu{-1};
    std::uint64_t prompt_tokens{0};
    std::uint64_t output_tokens{0};
    std::uint64_t prefill_ms{0};
    std::chrono::steady_clock::time_point start;
    std::atomic<bool> cancelled{false};
};

GenScope::GenScope(std::string_view role, int gpu,
                   std::uint64_t prompt_tokens) {
    impl_                = new Impl();
    impl_->role          = std::string(role);
    impl_->gpu           = gpu;
    impl_->prompt_tokens = prompt_tokens;
    impl_->start         = std::chrono::steady_clock::now();
}

GenScope::~GenScope() {
    if (!impl_) return;
    if (!impl_->cancelled.load(std::memory_order_relaxed)) {
        const auto total_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - impl_->start).count();
        std::uint64_t decode_ms = 0;
        if (static_cast<std::uint64_t>(total_ms) > impl_->prefill_ms) {
            decode_ms = static_cast<std::uint64_t>(total_ms) - impl_->prefill_ms;
        }
        record_generate(impl_->role, impl_->gpu,
                        impl_->prompt_tokens, impl_->output_tokens,
                        impl_->prefill_ms, decode_ms,
                        static_cast<std::uint64_t>(total_ms));
    }
    delete impl_;
    impl_ = nullptr;
}

void GenScope::set_output_tokens(std::uint64_t n) {
    if (impl_) impl_->output_tokens = n;
}

void GenScope::set_prefill_ms(std::uint64_t ms) {
    if (impl_) impl_->prefill_ms = ms;
}

void GenScope::cancel() {
    if (impl_) impl_->cancelled.store(true, std::memory_order_relaxed);
}

}  // namespace bench
