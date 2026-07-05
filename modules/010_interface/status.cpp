// SPDX-License-Identifier: GPL-3.0-or-later
#include "status.hpp"

#include <atomic>
#include <map>
#include <mutex>
#include <sstream>
#include <string>

namespace status {
namespace {

struct Component {
    std::string state;
    std::string detail;
};

std::mutex                       g_mtx;
std::map<std::string, Component> g_components;     // ordered for stable JSON
std::string                      g_headline = "starting";
bool                             g_ready    = false;

std::string json_escape(const std::string & s) {
    std::string out;
    out.reserve(s.size() + 8);
    for (char c : s) {
        switch (c) {
            case '"':  out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n";  break;
            case '\r': out += "\\r";  break;
            case '\t': out += "\\t";  break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) {
                    char buf[8];
                    std::snprintf(buf, sizeof(buf), "\\u%04x",
                                  static_cast<unsigned char>(c));
                    out += buf;
                } else {
                    out += c;
                }
        }
    }
    return out;
}

}

void note(std::string_view component,
          std::string_view state,
          std::string_view detail) {
    std::lock_guard<std::mutex> lk(g_mtx);
    auto & c = g_components[std::string(component)];
    c.state  = std::string(state);
    c.detail = std::string(detail);
}

std::atomic<std::uint64_t> g_pulses{0};

void pulse() { g_pulses.fetch_add(1, std::memory_order_relaxed); }

std::uint64_t pulse_count() { return g_pulses.load(std::memory_order_relaxed); }

std::atomic<std::uint64_t> g_turn_epoch{0};
std::atomic<std::uint64_t> g_cancel_epoch{0};

std::uint64_t begin_turn() {
    return g_turn_epoch.fetch_add(1, std::memory_order_relaxed) + 1;
}

void request_cancel() {
    g_cancel_epoch.store(g_turn_epoch.load(std::memory_order_relaxed),
                         std::memory_order_relaxed);
}

bool cancelled(std::uint64_t epoch) {
    return g_cancel_epoch.load(std::memory_order_relaxed) >= epoch;
}

bool generation_cancelled() {
    const auto t = g_turn_epoch.load(std::memory_order_relaxed);
    return t != 0 &&
           g_cancel_epoch.load(std::memory_order_relaxed) >= t;
}

void set_overall(std::string_view headline, bool ready) {
    std::lock_guard<std::mutex> lk(g_mtx);
    g_headline = std::string(headline);
    g_ready    = ready;
}

std::string snapshot_json() {
    std::lock_guard<std::mutex> lk(g_mtx);
    std::ostringstream out;
    out << "{\"headline\":\"" << json_escape(g_headline)
        << "\",\"ready\":" << (g_ready ? "true" : "false")
        << ",\"components\":[";
    bool first = true;
    for (const auto & [name, c] : g_components) {
        if (!first) out << ",";
        first = false;
        out << "{\"name\":\""   << json_escape(name)
            << "\",\"state\":\"" << json_escape(c.state)
            << "\",\"detail\":\"" << json_escape(c.detail)
            << "\"}";
    }
    out << "]}";
    return out.str();
}

}
