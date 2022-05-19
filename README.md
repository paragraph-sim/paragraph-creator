# paragraph-creator - Tools to create ParaGraphs
This repository contains examples of ParaGraphs - application models described directly in ParaGraph IR. In spirit, they are similar to [SST motifs](https://github.com/sstsimulator/sst-elements/tree/master/src/sst/elements/ember/mpi/motifs). 

Currently available following models:
1. [AllReduce](https://github.com/paragraph-sim/paragraph-creator/tree/main/src/allreduce)
2. [Stencil (Halo-3D)](https://github.com/paragraph-sim/paragraph-creator/tree/main/src/stencil)

## How to install
The C++ projects use [Bazel](https://bazel.build/ "Bazel Build") for building binaries. To install Bazel, follow the directions at [here](https://bazel.build/install "Bazel Install"). You need bazel-3.7.2.
Use the following command to build and test the project 
``
bazel test -c opt ...
```
