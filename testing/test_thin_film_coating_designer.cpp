#include "test_runner.hpp"
#include "../modules/875_thin_film_coating_designer/thin_film_coating_designer.hpp"

namespace {

testing::TestOutcome run_thin_film_coating_designer() {
    thin_film_coating_designer::init();
    auto s = thin_film_coating_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    thin_film_coating_designer::shutdown();
    return testing::ok();
}

const int _reg_thin_film_coating_designer = testing::register_test(
    "thin_film_coating_designer",
    "875_thin_film_coating_designer: stub status check",
    &run_thin_film_coating_designer);

}
