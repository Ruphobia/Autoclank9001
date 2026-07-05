#include "netcdf_hdf5_explorer.hpp"

namespace netcdf_hdf5_explorer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: NetCDF/HDF5 explorer (Oceanography and marine science). Awaits wire-up.";
    return s;
}

}
