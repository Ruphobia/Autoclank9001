// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for 029_salience: set weights, simulate decay via a
// future `now`, verify ordering and pin behavior.

#include "test_runner.hpp"
#include "../modules/029_salience/salience.hpp"

#include <cmath>
#include <cstdlib>
#include <filesystem>

namespace {

namespace fs = std::filesystem;

fs::path scratch_root() {
    if (const char * e = std::getenv("TMPDIR")) return fs::path(e) / "tool_test_salience";
    return fs::path("/tmp") / "tool_test_salience";
}

testing::TestOutcome run() {
    fs::path root = scratch_root();
    std::error_code ec;
    fs::remove_all(root, ec);
    fs::create_directories(root);

    salience::init();

    const int64_t t0 = 1'700'000'000;   // arbitrary anchor
    salience::set_weight(salience::Scope::Project, 1, 1.0, root.string());
    salience::set_weight(salience::Scope::Project, 2, 1.0, root.string());
    salience::set_weight(salience::Scope::Project, 3, 1.0, root.string());
    salience::set_decay (salience::Scope::Project, 1, 0.10, root.string());
    salience::set_decay (salience::Scope::Project, 2, 0.50, root.string());
    salience::set_decay (salience::Scope::Project, 3, 0.05, root.string());
    salience::touch     (salience::Scope::Project, 1, t0, root.string());
    salience::touch     (salience::Scope::Project, 2, t0, root.string());
    salience::touch     (salience::Scope::Project, 3, t0, root.string());

    // At t0 every entry is at full base; ordering is stable input order.
    auto r0 = salience::rerank(salience::Scope::Project, {1,2,3}, t0, root.string());
    if (r0.size() != 3) return testing::fail("rerank lost ids at t0");

    // Ten days later, entry 2 (decay 0.5) should fall hardest, entry 3
    // (decay 0.05) should stay near its base.
    int64_t t10 = t0 + 10 * 86400;
    auto r10 = salience::rerank(salience::Scope::Project, {1,2,3}, t10, root.string());
    if (r10.empty() || r10.back() != 2)
        return testing::fail("expected entry 2 last after 10 days");
    if (r10.front() != 3)
        return testing::fail("expected entry 3 first after 10 days");

    // Pin entry 2; it should now top the ranking despite its decay.
    salience::pin(salience::Scope::Project, 2, root.string());
    auto rp = salience::rerank(salience::Scope::Project, {1,2,3}, t10, root.string());
    if (rp.front() != 2) return testing::fail("pin did not float entry 2");

    salience::unpin(salience::Scope::Project, 2, root.string());
    auto rpu = salience::rerank(salience::Scope::Project, {1,2,3}, t10, root.string());
    if (rpu.back() != 2) return testing::fail("unpin did not restore decay");

    // touch resets the decay clock.
    salience::touch(salience::Scope::Project, 2, t10, root.string());
    double eff = salience::effective_weight(salience::Scope::Project, 2, t10, root.string());
    if (std::abs(eff - 1.0) > 1e-9)
        return testing::fail("touch did not reset decay: " + std::to_string(eff));

    salience::shutdown();
    fs::remove_all(root, ec);
    return testing::ok();
}

const int _r = testing::register_test(
    "salience",
    "029_salience: weights + decay + pin/touch + rerank",
    &run);

}
