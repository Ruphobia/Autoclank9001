#include "test_runner.hpp"
#include "../modules/289_dialog_scripter/dialog_scripter.hpp"

namespace {

testing::TestOutcome run_dialog_scripter() {
    dialog_scripter::init();
    auto s = dialog_scripter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    dialog_scripter::shutdown();
    return testing::ok();
}

const int _reg_dialog_scripter = testing::register_test(
    "dialog_scripter",
    "289_dialog_scripter: stub status check",
    &run_dialog_scripter);

}
