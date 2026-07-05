#include "test_runner.hpp"
#include "../modules/2041_solunar_tables/solunar_tables.hpp"

namespace {

testing::TestOutcome run_solunar_tables() {
    solunar_tables::init();
    auto s = solunar_tables::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    solunar_tables::shutdown();
    return testing::ok();
}

const int _reg_solunar_tables = testing::register_test(
    "solunar_tables",
    "2041_solunar_tables: stub status check",
    &run_solunar_tables);

}
