#include "test_runner.hpp"
#include "../modules/1360_metagenome_binner/metagenome_binner.hpp"

namespace {

testing::TestOutcome run_metagenome_binner() {
    metagenome_binner::init();
    auto s = metagenome_binner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    metagenome_binner::shutdown();
    return testing::ok();
}

const int _reg_metagenome_binner = testing::register_test(
    "metagenome_binner",
    "1360_metagenome_binner: stub status check",
    &run_metagenome_binner);

}
