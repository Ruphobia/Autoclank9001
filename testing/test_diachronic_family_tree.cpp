#include "test_runner.hpp"
#include "../modules/814_diachronic_family_tree/diachronic_family_tree.hpp"

namespace {

testing::TestOutcome run_diachronic_family_tree() {
    diachronic_family_tree::init();
    auto s = diachronic_family_tree::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    diachronic_family_tree::shutdown();
    return testing::ok();
}

const int _reg_diachronic_family_tree = testing::register_test(
    "diachronic_family_tree",
    "814_diachronic_family_tree: stub status check",
    &run_diachronic_family_tree);

}
