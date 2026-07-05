// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for 071_log_viewer.

#include "test_runner.hpp"
#include "../modules/071_log_viewer/log_viewer.hpp"

namespace {

testing::TestOutcome run() {
    log_viewer::init();

    std::string txt =
        R"({"ts":1700000000,"level":"info","msg":"server started","host":"a"})"   "\n"
        R"({"ts":1700000020,"level":"warn","msg":"slow query","host":"a","ms":120})" "\n"
        R"({"ts":1700000040,"level":"error","msg":"db connection lost","host":"a"})" "\n"
        "ts=1700000045 level=error msg=\"db connection lost again\" host=a"      "\n"
        "ts=1700000050 level=error msg=\"connect timeout\" host=b"               "\n"
        "ts=1700000055 level=error msg=\"connect timeout\" host=b"               "\n"
        "ts=1700000059 level=error msg=\"connect timeout\" host=b"               "\n"
        "just a plain message line"                                              "\n";

    auto entries = log_viewer::parse(txt);
    if (entries.size() != 8) return testing::fail("entry count != 8");

    // JSON parse correctness.
    if (entries[0].level != log_viewer::Level::Info ||
        entries[0].message != "server started" ||
        entries[0].ts_unix != 1700000000 ||
        entries[0].fields["host"] != "a")
        return testing::fail("json entry parse wrong");

    // logfmt parse.
    if (entries[3].level != log_viewer::Level::Error ||
        entries[3].message.find("again") == std::string::npos ||
        entries[3].fields["host"] != "a")
        return testing::fail("logfmt entry parse wrong");

    // Plain.
    if (entries.back().message != "just a plain message line")
        return testing::fail("plain entry parse wrong");

    // Filter: errors only.
    log_viewer::Filter f;
    f.level_min = log_viewer::Level::Error;
    auto errs = log_viewer::apply_filter(entries, f);
    if (errs.size() != 5) return testing::fail("error filter count != 5");

    // Filter: contains "timeout"
    log_viewer::Filter ft;
    ft.contains = "TIMEOUT";   // case-insensitive
    auto to_lines = log_viewer::apply_filter(entries, ft);
    if (to_lines.size() != 3) return testing::fail("contains filter count != 3");

    // Filter: field equality.
    log_viewer::Filter fh;
    fh.field_equals["host"] = "b";
    auto host_b = log_viewer::apply_filter(entries, fh);
    if (host_b.size() != 3) return testing::fail("field filter count != 3");

    // Facet counts.
    auto by_level = log_viewer::facet_counts(entries, "_level");
    if (by_level["error"] != 5 || by_level["warn"] != 1 || by_level["info"] != 1)
        return testing::fail("level facet counts wrong");
    auto by_host = log_viewer::facet_counts(entries, "host");
    if (by_host["a"] != 4 || by_host["b"] != 3)
        return testing::fail("host facet counts wrong: a=" +
                             std::to_string(by_host["a"]) + " b=" +
                             std::to_string(by_host["b"]));

    // Error burst within 60s window, threshold 5.
    auto bursts = log_viewer::error_bursts(entries, 60, 5);
    if (bursts.size() != 1)
        return testing::fail("expected 1 burst, got " + std::to_string(bursts.size()));
    if (bursts[0].count != 5)
        return testing::fail("burst count != 5");

    log_viewer::shutdown();
    return testing::ok();
}

const int _r = testing::register_test(
    "log_viewer",
    "071_log_viewer: JSON + logfmt parse + filter + facet + error-burst",
    &run);

}
