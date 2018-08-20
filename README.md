# Parallel orientation Invariant Non-parametric Kohonen-map (PINK)

## Requirements

  - CMake (Version >= 2.6.2, http://www.cmake.org)
  - CUDA (recommended)
  - gtest (optional)
  - doxygen (optional)


## Installation

  1) Make build directory and change into it:
  
     mkdir build && cd build

  2) Generate Makefile using cmake:

     cmake -DCMAKE_INSTALL_PREFIX=<INSTALL_PATH> ..

  3) Compile:

     make -j <number of cores>

  4) Execute the Tests

     make test

  5) Generate API documentation (only if doxygen is available)

     make doc

  6) Install

     make install

  7) Set environmental variables

     source <INSTALL_PATH>/bashrc


## Usage

  Please execute 'Pink -h' for current usage and options.


## Python scripts

For conversion and visualization of images some python scripts are available. Some may be incompatible with python2.7.

  numpy2binary.py: Conversion of the python numpy file format into binary input file format for images.

  jpg2binary.py:   Conversion of jpg images into binary input file format for images.
                   Three channels are used for red, green, and blue (RGB) color channels.
                 
  showImages.py:   Visualize binary images file format.

  showSOM.py:      Visualize binary SOM file format.

  showHeatmap.py:  Visualize the mapping result.


## Binary file format description

  Arrays are in FORTRAN notation: First index is the slowest, last index is the fastest.

  1) Input file for images

    (1)(integer) number_of_images
    (1)(integer) number_of_channels
    (1)(integer) width
    (1)(integer) height
    (number_of_images, number_of_channels, width, height)(float) pixels

  2) SOM file

    (1)(integer) number_of_channels
    (1)(integer) SOM_width
    (1)(integer) SOM_height
    (1)(integer) SOM_depth
    (1)(integer) neuron_width
    (1)(integer) neuron_height
    (SOM_width, SOM_height, SOM_depth, number_of_channels, neuron_width, neuron_height)(float) pixels

  3) Mapping result file

    (1)(integer) number_of_images
    (1)(integer) SOM_width
    (1)(integer) SOM_height
    (1)(integer) SOM_depth
    (number_of_images, SOM_width, SOM_height, SOM_depth)(float) euclidian_distance