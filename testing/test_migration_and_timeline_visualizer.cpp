#include "test_runner.hpp"
#include "../modules/2149_migration_and_timeline_visualizer/migration_and_timeline_visualizer.hpp"

namespace {

testing::TestOutcome run_migration_and_timeline_visualizer() {
    migration_and_timeline_visualizer::init();
    auto s = migration_and_timeline_visualizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    migration_and_timeline_visualizer::shutdown();
    return testing::ok();
}

const int _reg_migration_and_timeline_visualizer = testing::register_test(
    "migration_and_timeline_visualizer",
    "2149_migration_and_timeline_visualizer: stub status check",
    &run_migration_and_timeline_visualizer);

}
