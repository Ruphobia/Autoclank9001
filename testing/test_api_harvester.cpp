// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for 051_api_harvester. The HttpClient is an in-memory
// router that returns canned JSON pages so no real network is touched.

#include "test_runner.hpp"
#include "../modules/051_api_harvester/api_harvester.hpp"

#include <sstream>

namespace {

testing::TestOutcome run() {
    api_harvester::init();

    // Helpers ---------------------------------------------------------------
    auto build_offset_page = [&](int offset, int limit) {
        std::ostringstream os;
        os << R"({"items":[)";
        int produced = 0;
        for (int i = offset; i < 25 && produced < limit; ++i, ++produced) {
            if (produced) os << ",";
            os << R"({"id":)" << i << R"(,"name":"item-)" << i << R"("})";
        }
        os << "]}";
        return os.str();
    };

    // Offset pagination -----------------------------------------------------
    api_harvester::HttpClient offset_client = [&](const api_harvester::HttpRequest & r) {
        api_harvester::HttpResponse resp;
        resp.status = 200;
        int offset = 0, limit = 10;
        auto eq = r.url.find("offset=");
        if (eq != std::string::npos) offset = std::stoi(r.url.substr(eq + 7));
        auto lp = r.url.find("limit=");
        if (lp != std::string::npos) limit = std::stoi(r.url.substr(lp + 6));
        resp.body = build_offset_page(offset, limit);
        return resp;
    };
    api_harvester::EndpointSpec off;
    off.base.url = "https://example.com/items";
    off.pagination.kind = api_harvester::PaginationKind::Offset;
    off.pagination.page_size = 10;
    off.pagination.records_json_path = "items";
    auto recs_off = api_harvester::harvest(off, offset_client);
    if (recs_off.size() != 25)
        return testing::fail("offset paginate got " + std::to_string(recs_off.size()) + " items");
    if (recs_off.back().value("id", -1) != 24)
        return testing::fail("offset paginate missed last id");

    // Cursor pagination -----------------------------------------------------
    api_harvester::HttpClient cursor_client = [&](const api_harvester::HttpRequest & r) {
        api_harvester::HttpResponse resp;
        resp.status = 200;
        std::string cursor;
        auto eq = r.url.find("cursor=");
        if (eq != std::string::npos) cursor = r.url.substr(eq + 7);
        if (cursor.empty()) {
            resp.body = R"({"items":[{"id":1},{"id":2}],"next":"c1"})";
        } else if (cursor == "c1") {
            resp.body = R"({"items":[{"id":3},{"id":4}],"next":"c2"})";
        } else if (cursor == "c2") {
            resp.body = R"({"items":[{"id":5}],"next":null})";
        }
        return resp;
    };
    api_harvester::EndpointSpec cur;
    cur.base.url = "https://example.com/cursor";
    cur.pagination.kind = api_harvester::PaginationKind::Cursor;
    cur.pagination.cursor_json_path = "next";
    cur.pagination.records_json_path = "items";
    auto recs_cur = api_harvester::harvest(cur, cursor_client);
    if (recs_cur.size() != 5) return testing::fail("cursor paginate count != 5");

    // Link header pagination -----------------------------------------------
    api_harvester::HttpClient link_client = [&](const api_harvester::HttpRequest & r) {
        api_harvester::HttpResponse resp;
        resp.status = 200;
        if (r.url == "https://example.com/link" || r.url.find("page=") == std::string::npos) {
            resp.body = R"([{"id":"a"},{"id":"b"}])";
            resp.headers["Link"] = R"(<https://example.com/link?page=2>; rel="next")";
        } else if (r.url.find("page=2") != std::string::npos) {
            resp.body = R"([{"id":"c"}])";
            // no next link
        }
        return resp;
    };
    api_harvester::EndpointSpec lh;
    lh.base.url = "https://example.com/link";
    lh.pagination.kind = api_harvester::PaginationKind::LinkHeader;
    auto recs_lh = api_harvester::harvest(lh, link_client);
    if (recs_lh.size() != 3) return testing::fail("link-header paginate count != 3");

    // RFC 5988 helper.
    auto u = api_harvester::link_header_url(
        R"(<https://example.com/p2>; rel="next", <https://example.com/p1>; rel="prev")",
        "next");
    if (u != "https://example.com/p2") return testing::fail("link header parse wrong");

    api_harvester::shutdown();
    return testing::ok();
}

const int _r = testing::register_test(
    "api_harvester",
    "051_api_harvester: offset + cursor + link-header pagination with injected client",
    &run);

}
