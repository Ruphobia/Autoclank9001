#include "api_harvester.hpp"

#include <algorithm>
#include <cctype>
#include <regex>
#include <sstream>

namespace api_harvester {
namespace {

std::vector<std::string> split_path(std::string_view dotted) {
    std::vector<std::string> out;
    std::string cur;
    for (char c : dotted) {
        if (c == '.') { if (!cur.empty()) { out.push_back(cur); cur.clear(); } }
        else cur.push_back(c);
    }
    if (!cur.empty()) out.push_back(cur);
    return out;
}

const nlohmann::json * walk(const nlohmann::json & root,
                            const std::vector<std::string> & parts) {
    const nlohmann::json * cur = &root;
    for (const auto & p : parts) {
        if (!cur->is_object() || !cur->contains(p)) return nullptr;
        cur = &(*cur)[p];
    }
    return cur;
}

std::string header_lookup_ci(const std::map<std::string, std::string> & h,
                             std::string_view key) {
    std::string lk;
    for (char c : key) lk.push_back((char)std::tolower((unsigned char)c));
    for (const auto & [k, v] : h) {
        std::string kk;
        for (char c : k) kk.push_back((char)std::tolower((unsigned char)c));
        if (kk == lk) return v;
    }
    return {};
}

}

const char * to_string(PaginationKind k) {
    switch (k) {
        case PaginationKind::Offset:     return "offset";
        case PaginationKind::Cursor:     return "cursor";
        case PaginationKind::LinkHeader: return "link-header";
        case PaginationKind::None:       return "none";
    }
    return "none";
}

void init()     {}
void shutdown() {}

std::string build_query(const std::map<std::string, std::string> & q) {
    if (q.empty()) return {};
    std::string out = "?";
    bool first = true;
    for (const auto & [k, v] : q) {
        if (!first) out.push_back('&');
        first = false;
        out += k;
        out.push_back('=');
        out += v;
    }
    return out;
}

std::string url_with_query(std::string_view url_v,
                           const std::map<std::string, std::string> & q) {
    std::string url{url_v};
    if (q.empty()) return url;
    auto qpos = url.find('?');
    if (qpos == std::string::npos) return url + build_query(q);
    std::string tail = build_query(q);
    if (tail.empty()) return url;
    return url + "&" + tail.substr(1);   // drop the leading '?'
}

std::string link_header_url(std::string_view link_header, std::string_view rel) {
    // Each comma-separated chunk looks like:  <https://...>; rel="next"
    std::string h{link_header};
    std::string want_rel{rel};
    static const std::regex re(R"(<([^>]+)>\s*;\s*rel=\"([^\"]+)\")");
    auto begin = std::sregex_iterator(h.begin(), h.end(), re);
    for (auto it = begin; it != std::sregex_iterator(); ++it) {
        if ((*it)[2].str() == want_rel) return (*it)[1].str();
    }
    return {};
}

std::vector<nlohmann::json> harvest(const EndpointSpec & spec,
                                    const HttpClient & client) {
    std::vector<nlohmann::json> out;

    HttpRequest req = spec.base;
    int     offset  = spec.pagination.start_offset;
    std::string cursor;
    std::string next_link = req.url;
    bool    have_next_link = false;
    size_t  pages = 0;

    while (pages < spec.pagination.max_pages) {
        ++pages;
        HttpRequest r = req;

        if (spec.pagination.kind == PaginationKind::Offset) {
            r.query[spec.pagination.offset_param] = std::to_string(offset);
            r.query["limit"] = std::to_string(spec.pagination.page_size);
            r.url = url_with_query(req.url, r.query);
            r.query.clear();
        } else if (spec.pagination.kind == PaginationKind::Cursor) {
            if (!cursor.empty()) r.query[spec.pagination.cursor_param] = cursor;
            r.url = url_with_query(req.url, r.query);
            r.query.clear();
        } else if (spec.pagination.kind == PaginationKind::LinkHeader) {
            if (have_next_link) r.url = next_link;
            else                r.url = url_with_query(req.url, r.query);
            r.query.clear();
        } else {
            r.url = url_with_query(req.url, r.query);
            r.query.clear();
        }

        HttpResponse resp = client(r);
        if (resp.status < 200 || resp.status >= 300) break;

        nlohmann::json doc;
        try { doc = nlohmann::json::parse(resp.body); }
        catch (...) { break; }

        auto path = split_path(spec.pagination.records_json_path);
        const nlohmann::json * arr = path.empty() ? &doc : walk(doc, path);
        if (!arr) break;
        if (arr->is_array()) {
            if (arr->empty()) break;
            for (const auto & rec : *arr) out.push_back(rec);
        } else {
            // Single object response: treat as one record and stop.
            out.push_back(*arr);
            break;
        }

        // Compute next position.
        switch (spec.pagination.kind) {
            case PaginationKind::Offset: {
                if (spec.pagination.offset_is_page) offset += 1;
                else                                 offset += spec.pagination.page_size;
                if ((int)arr->size() < spec.pagination.page_size) return out;
                break;
            }
            case PaginationKind::Cursor: {
                auto cp = split_path(spec.pagination.cursor_json_path);
                const nlohmann::json * c = walk(doc, cp);
                if (!c || c->is_null()) return out;
                if (c->is_string()) cursor = c->get<std::string>();
                else                cursor = c->dump();
                if (cursor.empty() || cursor == "\"\"") return out;
                break;
            }
            case PaginationKind::LinkHeader: {
                std::string link = header_lookup_ci(resp.headers, "Link");
                std::string u    = link_header_url(link, "next");
                if (u.empty()) return out;
                next_link      = u;
                have_next_link = true;
                break;
            }
            case PaginationKind::None:
                return out;
        }
    }
    return out;
}

}
