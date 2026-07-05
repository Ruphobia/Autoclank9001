#include "test_runner.hpp"
#include "../modules/692_prosopography_database/prosopography_database.hpp"

namespace {

testing::TestOutcome run_prosopography_database() {
    prosopography_database::init();
    auto s = prosopography_database::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    prosopography_database::shutdown();
    return testing::ok();
}

const int _reg_prosopography_database = testing::register_test(
    "prosopography_database",
    "692_prosopography_database: stub status check",
    &run_prosopography_database);

}
