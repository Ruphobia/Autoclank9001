#include "test_runner.hpp"
#include "../modules/1832_fermentationlog/fermentationlog.hpp"

namespace {

testing::TestOutcome run_fermentationlog() {
    fermentationlog::init();
    auto s = fermentationlog::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    fermentationlog::shutdown();
    return testing::ok();
}

const int _reg_fermentationlog = testing::register_test(
    "fermentationlog",
    "1832_fermentationlog: stub status check",
    &run_fermentationlog);

}
