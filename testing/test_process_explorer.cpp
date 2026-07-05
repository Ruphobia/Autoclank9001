#include "test_runner.hpp"
#include "../modules/255_process_explorer/process_explorer.hpp"

namespace {

testing::TestOutcome run_process_explorer() {
    process_explorer::init();
    auto s = process_explorer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    process_explorer::shutdown();
    return testing::ok();
}

const int _reg_process_explorer = testing::register_test(
    "process_explorer",
    "255_process_explorer: stub status check",
    &run_process_explorer);

}
