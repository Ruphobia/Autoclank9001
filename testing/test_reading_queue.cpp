// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for 059_reading_queue.

#include "test_runner.hpp"
#include "../modules/059_reading_queue/reading_queue.hpp"

#include <cstdlib>
#include <filesystem>

namespace {

namespace fs = std::filesystem;

fs::path scratch_root() {
    if (const char * e = std::getenv("TMPDIR")) return fs::path(e) / "tool_test_reading";
    return fs::path("/tmp") / "tool_test_reading";
}

testing::TestOutcome run() {
    fs::path root = scratch_root();
    std::error_code ec;
    fs::remove_all(root, ec);
    fs::create_directories(root);

    reading_queue::open(root.string());

    int64_t a = reading_queue::add(1, "Quick paper",          1, 10);
    int64_t b = reading_queue::add(2, "Medium paper",         2, 30);
    int64_t c = reading_queue::add(3, "Long paper",           3, 90);
    int64_t d = reading_queue::add(4, "Unknown-length paper", 1,  0);
    (void)c; (void)d;

    auto unread = reading_queue::list_by_status(reading_queue::Status::Unread);
    if (unread.size() != 4) return testing::fail("unread count != 4");
    // ordered by priority then id, so a (prio 1, id smallest) is first
    if (unread.front().id != a) return testing::fail("priority order broken");

    auto rec = reading_queue::recommend(2);
    if (rec.size() != 2) return testing::fail("recommend size != 2");
    // First recommendation should still be a (prio 1, eta 10 wins over d's eta 0->huge).
    if (rec[0].id != a)
        return testing::fail("recommend should put a first, got id " + std::to_string(rec[0].id));

    reading_queue::set_status(b, reading_queue::Status::Reading);
    reading_queue::log_session(b, 25, 1700000000);
    auto bg = reading_queue::get(b);
    if (!bg || bg->status != reading_queue::Status::Reading)
        return testing::fail("status not updated");
    if (bg->total_session_minutes != 25)
        return testing::fail("session minutes not accumulated");
    reading_queue::log_session(b, 15, 1700001000);
    bg = reading_queue::get(b);
    if (bg->total_session_minutes != 40)
        return testing::fail("session minutes did not sum");

    reading_queue::set_priority(b, 5);
    if (reading_queue::get(b)->priority != 5)
        return testing::fail("priority not updated");

    reading_queue::set_status(b, reading_queue::Status::Read);
    if (reading_queue::list_by_status(reading_queue::Status::Read).size() != 1)
        return testing::fail("read count != 1");

    reading_queue::forget(a);
    if (reading_queue::get(a)) return testing::fail("forget did not delete");

    reading_queue::close();
    fs::remove_all(root, ec);
    return testing::ok();
}

const int _r = testing::register_test(
    "reading_queue",
    "059_reading_queue: Kanban queue + sessions + recommendation",
    &run);

}
