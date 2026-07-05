#include "log_viewer.hpp"

#include <algorithm>
#include <cctype>
#include <ctime>
#include <sstream>

#include <nlohmann/json.hpp>

namespace log_viewer {
namespace {

std::string lower(std::string_view s) {
    std::string out(s);
    std::transform(out.begin(), out.end(), out.begin(),
                   [](unsigned char c){ return (char)std::tolower(c); });
    return out;
}

int64_t parse_ts(std::string_view in) {
    // Accept unix seconds (integer), ISO 8601 (2026-06-29T17:30:00Z), or empty.
    if (in.empty()) return 0;
    std::string s{in};
    bool digits_only = true;
    for (char c : s) if (!std::isdigit(static_cast<unsigned char>(c))) { digits_only = false; break; }
    if (digits_only) {
        try { return std::stoll(s); } catch (...) { return 0; }
    }
    std::tm tm{};
    int y, mo, d, h, mi, sec;
    if (std::sscanf(s.c_str(), "%d-%d-%dT%d:%d:%d",
                    &y, &mo, &d, &h, &mi, &sec) == 6) {
        tm.tm_year = y - 1900; tm.tm_mon = mo - 1; tm.tm_mday = d;
        tm.tm_hour = h;        tm.tm_min = mi;    tm.tm_sec  = sec;
        return (int64_t)::timegm(&tm);
    }
    return 0;
}

}

const char * to_string(Level l) {
    switch (l) {
        case Level::Trace:   return "trace";
        case Level::Debug:   return "debug";
        case Level::Info:    return "info";
        case Level::Warn:    return "warn";
        case Level::Error:   return "error";
        case Level::Fatal:   return "fatal";
        case Level::Unknown: return "unknown";
    }
    return "unknown";
}
Level level_from_string(std::string_view s) {
    std::string l = lower(s);
    if (l == "trace") return Level::Trace;
    if (l == "debug") return Level::Debug;
    if (l == "info")  return Level::Info;
    if (l == "warn" || l == "warning")  return Level::Warn;
    if (l == "error" || l == "err")     return Level::Error;
    if (l == "fatal" || l == "critical" || l == "crit") return Level::Fatal;
    return Level::Unknown;
}

void init()     {}
void shutdown() {}

Entry parse_json_line(std::string_view line) {
    Entry e;
    e.raw = std::string(line);
    try {
        auto j = nlohmann::json::parse(line);
        if (!j.is_object()) return e;
        for (auto it = j.begin(); it != j.end(); ++it) {
            const auto & k = it.key();
            std::string  v;
            if      (it.value().is_string())          v = it.value().get<std::string>();
            else if (it.value().is_number_integer())  v = std::to_string(it.value().get<int64_t>());
            else if (it.value().is_number_float())    v = std::to_string(it.value().get<double>());
            else if (it.value().is_boolean())         v = it.value().get<bool>() ? "true" : "false";
            else                                       v = it.value().dump();
            std::string lk = lower(k);
            if (lk == "ts" || lk == "time" || lk == "timestamp" || lk == "@timestamp")
                e.ts_unix = parse_ts(v);
            else if (lk == "level" || lk == "lvl" || lk == "severity")
                e.level = level_from_string(v);
            else if (lk == "msg" || lk == "message")
                e.message = v;
            else
                e.fields[k] = v;
        }
    } catch (...) {
        e.message = std::string(line);
    }
    return e;
}

Entry parse_logfmt_line(std::string_view line) {
    Entry e;
    e.raw = std::string(line);
    size_t i = 0;
    std::string body{line};
    while (i < body.size()) {
        while (i < body.size() && std::isspace(static_cast<unsigned char>(body[i]))) ++i;
        if (i >= body.size()) break;
        size_t key_start = i;
        while (i < body.size() && body[i] != '=' && !std::isspace(static_cast<unsigned char>(body[i]))) ++i;
        std::string key = body.substr(key_start, i - key_start);
        std::string val;
        if (i < body.size() && body[i] == '=') {
            ++i;
            if (i < body.size() && body[i] == '"') {
                ++i;
                size_t s = i;
                while (i < body.size() && body[i] != '"') ++i;
                val = body.substr(s, i - s);
                if (i < body.size()) ++i;
            } else {
                size_t s = i;
                while (i < body.size() && !std::isspace(static_cast<unsigned char>(body[i]))) ++i;
                val = body.substr(s, i - s);
            }
        }
        std::string lk = lower(key);
        if (lk == "ts" || lk == "time" || lk == "timestamp")
            e.ts_unix = parse_ts(val);
        else if (lk == "level" || lk == "lvl" || lk == "severity")
            e.level = level_from_string(val);
        else if (lk == "msg" || lk == "message")
            e.message = val;
        else if (!key.empty())
            e.fields[key] = val;
    }
    return e;
}

std::vector<Entry> parse(std::string_view text) {
    std::vector<Entry> out;
    std::string buf{text};
    std::string line;
    std::istringstream is(buf);
    while (std::getline(is, line)) {
        if (line.empty()) continue;
        size_t i = 0;
        while (i < line.size() && std::isspace(static_cast<unsigned char>(line[i]))) ++i;
        if (i < line.size() && line[i] == '{') {
            out.push_back(parse_json_line(line));
        } else if (line.find('=') != std::string::npos) {
            out.push_back(parse_logfmt_line(line));
        } else {
            Entry e;
            e.raw     = line;
            e.message = line;
            out.push_back(std::move(e));
        }
    }
    return out;
}

std::vector<Entry> apply_filter(const std::vector<Entry> & entries, const Filter & f) {
    std::vector<Entry> out;
    std::string needle = lower(f.contains);
    for (const auto & e : entries) {
        // Unknown entries pass only when the floor is Trace (the
        // most permissive). Otherwise they are filtered out so the
        // "show me errors" path does not drag in unstructured noise.
        if (e.level == Level::Unknown) {
            if (f.level_min != Level::Trace) continue;
        } else if ((int)e.level < (int)f.level_min) {
            continue;
        }
        if (f.since_unix && e.ts_unix && e.ts_unix < f.since_unix) continue;
        if (f.until_unix && e.ts_unix && e.ts_unix > f.until_unix) continue;
        if (!needle.empty() && lower(e.message).find(needle) == std::string::npos) continue;
        bool ok = true;
        for (const auto & [k, v] : f.field_equals) {
            auto it = e.fields.find(k);
            if (it == e.fields.end() || it->second != v) { ok = false; break; }
        }
        if (!ok) continue;
        out.push_back(e);
    }
    return out;
}

std::map<std::string, size_t> facet_counts(const std::vector<Entry> & entries,
                                           std::string_view field_key) {
    std::map<std::string, size_t> out;
    std::string key{field_key};
    for (const auto & e : entries) {
        if (key == "_level") {
            ++out[to_string(e.level)];
            continue;
        }
        auto it = e.fields.find(key);
        if (it != e.fields.end()) ++out[it->second];
    }
    return out;
}

std::vector<Burst> error_bursts(const std::vector<Entry> & entries,
                                int64_t window_seconds, size_t min_per_window) {
    std::vector<Entry> errs;
    for (const auto & e : entries) {
        if ((int)e.level >= (int)Level::Error && e.ts_unix > 0) errs.push_back(e);
    }
    std::sort(errs.begin(), errs.end(),
        [](const Entry & a, const Entry & b){ return a.ts_unix < b.ts_unix; });
    std::vector<Burst> out;
    size_t i = 0;
    while (i < errs.size()) {
        size_t j = i;
        while (j < errs.size() &&
               errs[j].ts_unix - errs[i].ts_unix <= window_seconds) ++j;
        if (j - i >= min_per_window) {
            Burst b;
            b.start_unix = errs[i].ts_unix;
            b.end_unix   = errs[j - 1].ts_unix;
            b.count      = j - i;
            out.push_back(b);
            i = j;
        } else {
            ++i;
        }
    }
    return out;
}

}
