#include "test_runner.hpp"
#include "../modules/1562_netcdf_hdf5_explorer/netcdf_hdf5_explorer.hpp"

namespace {

testing::TestOutcome run_netcdf_hdf5_explorer() {
    netcdf_hdf5_explorer::init();
    auto s = netcdf_hdf5_explorer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    netcdf_hdf5_explorer::shutdown();
    return testing::ok();
}

const int _reg_netcdf_hdf5_explorer = testing::register_test(
    "netcdf_hdf5_explorer",
    "1562_netcdf_hdf5_explorer: stub status check",
    &run_netcdf_hdf5_explorer);

}
