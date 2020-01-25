#ifndef _NETCDFREADER_H
#define _NETCDFREADER_H

#include <netcdf.h>
#include <string>
#include <vector>

enum class bufferType {
    H,
    HU,
    HV,
    B
};

class netcdfReader {
    public:
    netcdfReader ( const char * filename );
    ~netcdfReader ();

    void writeBuffer ( float buffer[], bufferType type, int time) const;

    int getTimeStep (float time) noexcept;
    int getMaxTimeStep() const noexcept;
    int getWidth() const noexcept;
    int getHeight() const noexcept;
    float getMaxTime () const noexcept;
    bool requiresUpdate( float time ) const noexcept;
    private:
    float maxTime;
    int last;
    int netCdfId;
    size_t timeLen, xLen, yLen;
    int timeId, xId, yId;
    int hVar, huVar, hvVar, bVar;
};

#endif