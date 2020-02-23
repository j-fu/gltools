OpenGL Graphics with gltools
============================

OpenGL 1.x provided a simple to understand and portable API for 3D graphics  programming.  The  gltools package  accesses these  possibilities for the visualization of results of PDE simulations.

Unfortunately (IMHO), the easy to use and intuitive "Fixed Function Pipeline" API of OpenGL 1.x  has been [deprecated in the OpenGL standard](https://www.khronos.org/opengl/wiki/Legacy_OpenGL).

## Authors: 
  - Jürgen Fuhrmann aka @j-fu 
  - Hartmut Langmach


## Documentation

- [dvi](gltools.dvi)
- [pdf](gltools.pdf)
- [html](gltools.html-dir/index.html)
- [home page](https://www.wias-berlin.de/software/gltools/index.jsp?lang=1) at Weierstrass Insitute

## Installation
Installation should be easy. Just edit the Makefile, tweak the paths if necessary and issue `make lib bin`.

## Time line remarks

The present package provides the source code for the gltools rpm package in the [OpenSUSE science repo](https://software.opensuse.org/package/gltools)  which exists at least since 2012 and was/is maintained by Sebastien Corot. As of 2020-02-22, the sources in the  rpm and in this repository are identical.

The development of gltools took place after the arrival of OpenGL on mainstream computers, [gms](https://github.com/j-fu/gms) was its predecessor. After 2003 it was succeded by the gltools2 C++ code in [pdelib](https://www.wias-berlin.de/software/pdelib/?lang=1) (not released on github).
See also [vtkfig](https://github.com/j-fu/vtkfig) for a vtk based package allowing to use modern OpenGL.
