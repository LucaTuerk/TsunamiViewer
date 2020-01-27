tsunamiviewer
=============

This a utility to view tsunami simulations on spherical world in real time.

## install notes

Make sure to pull all submodules.
In order to build you need:
* OpenGL    ( `apt-get install mesa-utils`)
* GLM       ( `apt-get install libglm-dev`)
* GLUT      ( `apt-get install freeglut3-dev`)
* GLEW      ( `apt-get install libglew-dev` )
* netCDF    ( `apt-get install libnetcdf-dev` )

## build instructions
Building works in the usual CMake way.

* Go the repos root directory and : 
`mkdir build`
`cd build`
`cmake ..`
`make`

## command line usage
The program takes the following command line parameters.
* -f    the filepath to the netCDF file
* -d    the input files dimensions are divided by the           factor for the 
        geometric representation. 

Generally it is recommended to pass a relatively large factor (reduce input width/height to the order of 100s) for -d, as values are interpolated on the graphics card anyway, and performance is greatly improved.

## usage
Simulations are provided in netCDF format, a simple test file is included, and will be loaded if no file is provided.

Make sure to check out the in program help page by pressing `esc`.

When displaying local relative values, the values will be colored relatively to the min/max in this frame.

When displaying global relative values, the values are scaled relative to the min/max of all frames read up to this point. Therefore it is recommended to let the simulation run one time so that values are displayed accurately.