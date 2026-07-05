// Smoke test for 012_task_scheduler. Builds a small plan with a
// dependency chain and a poison branch, runs it, and verifies that the
// ordering, statuses, and persistence all behave.

#include "test_runner.hpp"
#include "../modules/012_task_scheduler/scheduler.hpp"

#include <cstdlib>
#include <filesystem>

namespace {

namespace fs = std::filesystem;

fs::path scratch_root() {
    if (const char * e = std::getenv("TMPDIR")) return fs::path(e) / "tool_test_scheduler";
    return fs::path("/tmp") / "tool_test_scheduler";
}

testing::TestOutcome run() {
    scheduler::init();

    fs::path root = scratch_root();
    std::error_code ec;
    fs::remove_all(root, ec);
    fs::create_directories(root);

    scheduler::Plan p;
    p.name        = "smoke_plan";
    p.description = "harness round trip";

    // a (ok) -> b (depends a, ok) -> c (depends b, ok). d depends on a
    // failing task; d should auto-skip.
    scheduler::Task a; a.id="a"; a.body="echo a > a.out"; a.timeout_seconds = 5;
    scheduler::Task b; b.id="b"; b.body="cat a.out > b.out"; b.depends_on={"a"}; b.timeout_seconds = 5;
    scheduler::Task c; c.id="c"; c.body="cat b.out > c.out"; c.depends_on={"b"}; c.timeout_seconds = 5;
    scheduler::Task f; f.id="f"; f.body="exit 7";           f.timeout_seconds = 5;
    scheduler::Task d; d.id="d"; d.body="echo never";       d.depends_on={"f"}; d.timeout_seconds = 5;
    p.tasks = {a, b, c, f, d};

    scheduler::save_plan(p, root.string());
    auto names = scheduler::list_plans(root.string());
    if (names.size() != 1 || names[0] != "smoke_plan")
        return testing::fail("list_plans did not return the saved plan");

    size_t ran = scheduler::run_all(p, root.string(), root.string());
    if (ran != 4)
        return testing::fail("expected 4 runs (a,b,c,f), got " + std::to_string(ran));

    auto status_of = [&](const std::string & id) {
        for (const auto & t : p.tasks) if (t.id == id) return t.status;
        return scheduler::Status::Pending;
    };
    if (status_of("a") != scheduler::Status::Done)    return testing::fail("a not Done");
    if (status_of("b") != scheduler::Status::Done)    return testing::fail("b not Done");
    if (status_of("c") != scheduler::Status::Done)    return testing::fail("c not Done");
    if (status_of("f") != scheduler::Status::Failed)  return testing::fail("f not Failed");
    if (status_of("d") != scheduler::Status::Skipped) return testing::fail("d not Skipped");

    if (!fs::exists(root / "c.out")) return testing::fail("c.out not produced");

    // Persistence round trip.
    scheduler::Plan reloaded = scheduler::load_plan("smoke_plan", root.string());
    if (reloaded.tasks.size() != 5) return testing::fail("reloaded plan task count wrong");
    bool any_done = false;
    for (const auto & t : reloaded.tasks) if (t.status == scheduler::Status::Done) any_done = true;
    if (!any_done) return testing::fail("reloaded plan lost status state");

    fs::remove_all(root, ec);
    scheduler::shutdown();
    return testing::ok();
}

const int _r = testing::register_test(
    "scheduler",
    "012_task_scheduler: dependency ordering, poisoned-branch skip, persistence",
    &run);

}
