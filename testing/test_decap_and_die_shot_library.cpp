#include "test_runner.hpp"
#include "../modules/409_decap_and_die_shot_library/decap_and_die_shot_library.hpp"

namespace {

testing::TestOutcome run_decap_and_die_shot_library() {
    decap_and_die_shot_library::init();
    auto s = decap_and_die_shot_library::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    decap_and_die_shot_library::shutdown();
    return testing::ok();
}

const int _reg_decap_and_die_shot_library = testing::register_test(
    "decap_and_die_shot_library",
    "409_decap_and_die_shot_library: stub status check",
    &run_decap_and_die_shot_library);

}
