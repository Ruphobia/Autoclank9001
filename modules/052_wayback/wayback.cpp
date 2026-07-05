#include "wayback.hpp"

#include <sstream>
#include <stdexcept>

#include <nlohmann/json.hpp>

namespace wayback {

void init()     {}
void shutdown() {}

std::string cdx_query_url(std::string_view url_or_host,
                          std::string_view match_type, int limit) {
    std::ostringstream os;
    os << "https://web.archive.org/cdx/search/cdx"
       << "?url=" << url_or_host
       << "&matchType=" << match_type
       << "&output=json"
       << "&limit=" << limit;
    return os.str();
}

std::vector<Snapshot> snapshots(std::string_view url_or_host,
                                const api_harvester::HttpClient & client,
                                std::string_view match_type, int limit) {
    api_harvester::HttpRequest r;
    r.url = cdx_query_url(url_or_host, match_type, limit);
    auto resp = client(r);
    std::vector<Snapshot> out;
    if (resp.status < 200 || resp.status >= 300) return out;
    nlohmann::json doc;
    try { doc = nlohmann::json::parse(resp.body); }
    catch (...) { return out; }
    if (!doc.is_array() || doc.size() < 2) return out;

    // First row is the header; map column name -> index.
    const auto & header = doc[0];
    int ts_ix = -1, orig_ix = -1, mime_ix = -1, sc_ix = -1, len_ix = -1, dig_ix = -1;
    for (size_t i = 0; i < header.size(); ++i) {
        if (!header[i].is_string()) continue;
        std::string h = header[i].get<std::string>();
        if (h == "timestamp")   ts_ix   = (int)i;
        if (h == "original")    orig_ix = (int)i;
        if (h == "mimetype")    mime_ix = (int)i;
        if (h == "statuscode")  sc_ix   = (int)i;
        if (h == "length")      len_ix  = (int)i;
        if (h == "digest")      dig_ix  = (int)i;
    }

    auto col = [](const nlohmann::json & row, int ix) -> std::string {
        if (ix < 0 || (size_t)ix >= row.size()) return {};
        const auto & v = row[ix];
        if (v.is_string()) return v.get<std::string>();
        return v.dump();
    };

    for (size_t i = 1; i < doc.size(); ++i) {
        const auto & row = doc[i];
        if (!row.is_array()) continue;
        Snapshot s;
        s.timestamp    = col(row, ts_ix);
        s.original_url = col(row, orig_ix);
        s.mime         = col(row, mime_ix);
        s.status_code  = col(row, sc_ix);
        s.length       = col(row, len_ix);
        s.digest       = col(row, dig_ix);
        if (!s.timestamp.empty() && !s.original_url.empty()) out.push_back(s);
    }
    return out;
}

std::string snapshot_url(const Snapshot & s) {
    return "https://web.archive.org/web/" + s.timestamp + "/" + s.original_url;
}

std::optional<std::string> fetch_snapshot(const Snapshot & s,
                                          const api_harvester::HttpClient & client) {
    api_harvester::HttpRequest r;
    r.url = snapshot_url(s);
    auto resp = client(r);
    if (resp.status < 200 || resp.status >= 300) return std::nullopt;
    return resp.body;
}

}
