#include "test_runner.hpp"
#include "../modules/631_annotation_store/annotation_store.hpp"

namespace {

testing::TestOutcome run_annotation_store() {
    annotation_store::init();
    auto s = annotation_store::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    annotation_store::shutdown();
    return testing::ok();
}

const int _reg_annotation_store = testing::register_test(
    "annotation_store",
    "631_annotation_store: stub status check",
    &run_annotation_store);

}
