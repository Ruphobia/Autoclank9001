#include "test_runner.hpp"
#include "../modules/535_rental_pro_forma_modeler/rental_pro_forma_modeler.hpp"

namespace {

testing::TestOutcome run_rental_pro_forma_modeler() {
    rental_pro_forma_modeler::init();
    auto s = rental_pro_forma_modeler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    rental_pro_forma_modeler::shutdown();
    return testing::ok();
}

const int _reg_rental_pro_forma_modeler = testing::register_test(
    "rental_pro_forma_modeler",
    "535_rental_pro_forma_modeler: stub status check",
    &run_rental_pro_forma_modeler);

}
