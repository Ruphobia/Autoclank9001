#include "test_runner.hpp"
#include "../modules/1803_sourdough_starter_logger/sourdough_starter_logger.hpp"

namespace {

testing::TestOutcome run_sourdough_starter_logger() {
    sourdough_starter_logger::init();
    auto s = sourdough_starter_logger::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sourdough_starter_logger::shutdown();
    return testing::ok();
}

const int _reg_sourdough_starter_logger = testing::register_test(
    "sourdough_starter_logger",
    "1803_sourdough_starter_logger: stub status check",
    &run_sourdough_starter_logger);

}
