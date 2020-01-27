#include "netcdfReader.h"
#include <cmath>
#include <stdexcept>
#include <cassert>
#include <iostream>

#define NETCDF_ERR(msg, retval) {printf("%s: %s\n", msg, nc_strerror(retval)); throw std::runtime_error(msg);}


/**
 * Construct a netCDF reader
 * @param filename path to netCDF file
 **/
netcdfReader :: netcdfReader ( const char * filename ) : last ( -1 ){
    int retval;
    int varId;
    int numDims;
    
    // Try to open netCDF file
    if ((retval = nc_open(filename, NC_NOWRITE, &netCdfId)) != NC_NOERR)
        NETCDF_ERR ("Cannot open NetCdf file", retval)

    // Try to aquire number of dims
    if ((retval = nc_inq_ndims (netCdfId, &numDims)) != NC_NOERR)
        NETCDF_ERR ("Cannot read dimensions", retval)
    // Expecting time, x and y
    assert ( numDims == 3 );

    // read time dim
    if ((retval = nc_inq_varid (netCdfId, "time", &varId)) != NC_NOERR)
        NETCDF_ERR ("Cannot find variable time", retval)
    if ((retval = nc_inq_vardimid (netCdfId, varId, &timeId)) != NC_NOERR)
        NETCDF_ERR ("Cannot find dimension time", retval)
    if ((retval = nc_inq_dimlen (netCdfId, timeId, &timeLen)) != NC_NOERR)
        NETCDF_ERR ("Cannot read length of dimension time", retval)

    // read x dim
    if ((retval = nc_inq_varid (netCdfId, "x", &varId)) != NC_NOERR)
        NETCDF_ERR ("Cannot find variable x", retval)
    if ((retval = nc_inq_vardimid (netCdfId, varId, &xId)) != NC_NOERR)
        NETCDF_ERR ("Cannot find dimension x", retval)
    if ((retval = nc_inq_dimlen (netCdfId, xId, &xLen)) != NC_NOERR)
        NETCDF_ERR ("Cannot read length of dimension x", retval)

    // read y dim
    if ((retval = nc_inq_varid (netCdfId, "y", &varId)) != NC_NOERR)
        NETCDF_ERR ("Cannot find variable y", retval)
    if ((retval = nc_inq_vardimid (netCdfId, varId, &yId)) != NC_NOERR)
        NETCDF_ERR ("Cannot find dimension y", retval)
    if ((retval = nc_inq_dimlen (netCdfId, yId, &yLen)) != NC_NOERR)
        NETCDF_ERR ("Cannot read length of dimension x", retval)

    // read variables h, hu, hv, b, time
    int dims[] = {timeId, yId, xId};

    if ((retval = nc_inq_varid(netCdfId, "h", &hVar)) != NC_NOERR )
        NETCDF_ERR ("Cannot find variable h ", retval)
    if ((retval = nc_inq_varid(netCdfId, "b", &bVar)) != NC_NOERR )
        NETCDF_ERR ("Cannot find variable b ", retval)
    if ((retval = nc_inq_varid(netCdfId, "hu", &huVar)) != NC_NOERR )
        NETCDF_ERR ("Cannot find variable hu ", retval)
    if ((retval = nc_inq_varid(netCdfId, "hv", &hvVar)) != NC_NOERR )
        NETCDF_ERR ("Cannot find variable hv ", retval)
    
    size_t start[] = { timeLen - 1 };

    int timeVar;
    if ((retval = nc_inq_varid (netCdfId, "time", &timeVar)) != NC_NOERR )
        NETCDF_ERR ("Cannot find variable time", retval)
    if ((retval = nc_get_var1_float(netCdfId, timeVar, start, &maxTime)) != NC_NOERR )
            NETCDF_ERR ("Cannot read time values.", retval)

    std::cout 
        << "  Loaded  " << filename << std::endl 
        << "  Time:   " << maxTime << std::endl 
        << "  Steps:  " << timeLen << std::endl 
        << "  Height: " << yLen << std::endl 
        << "  Width:  " << xLen << std::endl;
}

netcdfReader :: ~netcdfReader () {
    nc_close ( netCdfId );
}


/**
 * Convert bufferType enum to string
 * 
 * @param type type
 **/
std::string typeToString ( bufferType type ) {
    return 
        type == bufferType::H ? "h" :
        type == bufferType::HU? "hu" :
        type == bufferType::HV? "hv":
        type == bufferType::B ? "b" : "unknown type " + (int) type;
}


/**
 * Writes data from netCDF to a buffer.
 * 
 * @param buffer out buffer for data
 * @param type type of buffer to be written
 * @param timeStep time step to be read
 **/
void netcdfReader :: writeBuffer ( float buffer[], bufferType type, int timeStep) const {
    int retval;

    size_t start [] = { (size_t) timeStep, 0, 0 };
    size_t count [] = { 1, yLen,  xLen };

    int varId = 
        type == bufferType :: H  ? hVar :
        type == bufferType :: HU ? huVar:
        type == bufferType :: HV ? hvVar:
        type == bufferType :: B  ? bVar : 0;
    
    if (type != bufferType :: B) 
    {
        if ((retval = nc_get_vara_float(netCdfId, varId, start, count, buffer)) != NC_NOERR )
            NETCDF_ERR ("Cannot read " + typeToString(type) + " values.", retval)
    }
    else {
        if ((retval = nc_get_vara_float(netCdfId, varId, & start [1],  & count [1], buffer)) != NC_NOERR )
            NETCDF_ERR ("Cannot read " + typeToString(type) + " values.", retval)
    }
}


/**
 * Get the timestep from time value in the [0, 1) range
 * @param time time value in the [0, 1) range
 **/
int netcdfReader :: getTimeStep ( float time ) noexcept {
    last = (int) (std::min ( .99f , std::max ( 0.f, time ) ) * timeLen);
    return last;
}

/**
 * Return largest timestep
 **/
int netcdfReader :: getMaxTimeStep() const noexcept {
    return timeLen;
}

/**
 * Return largest simulation time ( value of time var at maxTimeStep)
 **/
float netcdfReader :: getMaxTime () const noexcept {
    return maxTime;
}

/**
 * Get witdh of netCDF arrays
 **/
int netcdfReader :: getWidth () const noexcept {
    return xLen;
}

/**
 * Get height of netCDF arrays
 **/
int netcdfReader :: getHeight () const noexcept {
    return yLen;
}
/**
 * Returns wheather an update to the openGL textures is necessary, depending on 
 * the last time value.
 * 
 * @param time current time in the range [0,1)
 **/
bool netcdfReader :: requiresUpdate ( float time ) const noexcept {
    return (int) (std::min ( 1.f , std::max ( 0.f, time ) ) * timeLen) != last;
}
