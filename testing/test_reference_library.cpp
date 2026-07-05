// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for 054_reference_library.

#include "test_runner.hpp"
#include "../modules/054_reference_library/reference_library.hpp"

#include <cstdlib>
#include <filesystem>

namespace {

namespace fs = std::filesystem;

fs::path scratch_root() {
    if (const char * e = std::getenv("TMPDIR")) return fs::path(e) / "tool_test_refs";
    return fs::path("/tmp") / "tool_test_refs";
}

testing::TestOutcome run() {
    fs::path root = scratch_root();
    std::error_code ec;
    fs::remove_all(root, ec);
    fs::create_directories(root);

    reference_library::open(root.string());

    nlohmann::json csl1 = {
        {"type", "article-journal"},
        {"title", "Capybara thermoregulation in subtropical wetlands"},
        {"author", nlohmann::json::array({
            {{"family","Bauer"},{"given","Jane"}},
            {{"family","Lee"},  {"given","Sam"}}
        })},
        {"issued", {{"date-parts", nlohmann::json::array({{2026}})}}},
        {"container-title", "Journal of Mammalogy"},
        {"abstract", "We measure capybara surface temperature across seasons."},
    };
    int64_t id1 = reference_library::add("bauer2026capybara", csl1,
        {"animal","thermoreg"});

    nlohmann::json csl2 = {
        {"type", "book"},
        {"title", "Argon and the noble gases"},
        {"author", nlohmann::json::array({{{"family","Ramsay"},{"given","William"}}})},
        {"issued", {{"date-parts", nlohmann::json::array({{1904}})}}},
        {"abstract", "A monograph on the inert atmosphere constituents."},
    };
    int64_t id2 = reference_library::add("ramsay1904argon", csl2, {"chemistry"});

    if (id1 == 0 || id2 == 0) return testing::fail("add returned 0");

    auto byk = reference_library::by_citation_key("bauer2026capybara");
    if (!byk || byk->title.find("Capybara") == std::string::npos)
        return testing::fail("by_citation_key wrong");
    if (byk->year != "2026") return testing::fail("year not parsed from issued");
    if (byk->authors.size() != 2) return testing::fail("author split wrong");
    if (byk->container_title != "Journal of Mammalogy")
        return testing::fail("container title parse wrong");

    auto by_type = reference_library::list("book");
    if (by_type.size() != 1 || by_type[0].id != id2)
        return testing::fail("list-by-type wrong");

    auto by_tag = reference_library::list({}, "thermoreg");
    if (by_tag.size() != 1 || by_tag[0].id != id1)
        return testing::fail("list-by-tag wrong");

    auto hits = reference_library::search("capybara thermoregulation");
    if (hits.empty() || hits[0].item.id != id1)
        return testing::fail("FTS did not surface the right item");
    if (hits[0].snippet.find("[[") == std::string::npos)
        return testing::fail("snippet missing highlight");

    // Update preserves citation_key.
    nlohmann::json csl1u = csl1;
    csl1u["title"] = "Capybara thermoregulation in subtropical wetlands (revised)";
    reference_library::update(id1, csl1u, {"animal","thermoreg","revised"});
    auto upd = reference_library::get(id1);
    if (!upd || upd->title.find("revised") == std::string::npos)
        return testing::fail("update did not stick");

    reference_library::forget(id2);
    if (reference_library::get(id2)) return testing::fail("forget did not delete");

    reference_library::close();
    fs::remove_all(root, ec);
    return testing::ok();
}

const int _r = testing::register_test(
    "reference_library",
    "054_reference_library: CSL-JSON store + by-key + by-type + by-tag + FTS",
    &run);

}
