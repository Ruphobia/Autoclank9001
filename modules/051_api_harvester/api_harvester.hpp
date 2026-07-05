#pragma once

#include <cstdint>
#include <functional>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <nlohmann/json.hpp>

// API Harvester: declarative HTTP client for paginated JSON endpoints.
// The network is injected (HttpClient) so the harvester itself has no
// network dependency; tests use an in-memory page map keyed by URL.
//
// Three pagination strategies cover most real APIs:
//   - Offset: a numeric "page=" or "offset=" parameter that walks the
//     result set; stops when the response carries no items.
//   - Cursor: a token returned in the response (typically
//     "next_cursor", "next", or a nested path) that the next request
//     echoes back as a query parameter.
//   - LinkHeader: RFC 5988 Link: <url>; rel="next" header.
namespace api_harvester {

enum class PaginationKind { Offset = 0, Cursor = 1, LinkHeader = 2, None = 3 };
const char * to_string(PaginationKind);

struct HttpResponse {
    int                                       status = 0;
    std::string                               body;
    std::map<std::string, std::string>        headers;     // case as the server gives them
};

struct HttpRequest {
    std::string                                url;
    std::string                                method = "GET";
    std::map<std::string, std::string>         headers;
    std::map<std::string, std::string>         query;       // appended to url
    std::string                                body;        // for POST / GraphQL
};

using HttpClient = std::function<HttpResponse(const HttpRequest &)>;

struct PaginationSpec {
    PaginationKind  kind                = PaginationKind::None;
    // Offset
    std::string     offset_param        = "offset";   // or "page"
    int             page_size           = 100;
    int             start_offset        = 0;
    bool            offset_is_page      = false;      // true => +1 per page
    // Cursor
    std::string     cursor_param        = "cursor";
    std::string     cursor_json_path    = "next";      // dotted; "" => no cursor
    // LinkHeader (no extra knobs)
    // Common
    std::string     records_json_path   = "";          // dotted; "" => root array
    size_t          max_pages           = 50;
};

struct EndpointSpec {
    HttpRequest      base;
    PaginationSpec   pagination;
};

void init();
void shutdown();

// Drive the spec. Returns the concatenated records from every page;
// pages with no usable records terminate the walk.
std::vector<nlohmann::json> harvest(const EndpointSpec & spec,
                                    const HttpClient & client);

// RFC 5988 Link header parser: returns the URL for rel="<name>" or
// empty when absent. Useful directly to callers building their own
// pagination loops on top of the HttpClient.
std::string link_header_url(std::string_view link_header, std::string_view rel);

// Build a `?a=1&b=2`-style query string, properly delimited.
std::string build_query(const std::map<std::string, std::string> & q);

// Apply a `?k=v&...` query to a URL, merging with any existing query.
std::string url_with_query(std::string_view url,
                           const std::map<std::string, std::string> & q);

}
