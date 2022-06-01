# Stencil
This ParaGraph implements stencil generator that implements Halo-3D exchange described in [this paper](https://www.nicm.dev/publication/hxrouting_sc/). See also a [sister library](https://github.com/nicmcd/mkpg) with implementation in SuperSim.

## How to build
The C++ projects use [Bazel](https://bazel.build/ "Bazel Build") for building binaries. To install Bazel, follow the directions at [here](https://bazel.build/install "Bazel Install"). You need bazel-3.7.2.
To build the bridge without linter check, you can use 
``
bazel build stencil
``

## How to use
You can use this command to generate `stencil` ParaGraph with name `pg_27p_stencil`, size `4x4x4`, `2` loop iterations with `0s` loop conditional and `1ms` compute part, with communication message sizes `800` bytes, `80` bytes, and `8` bytes for face, edge, and corner messages respectively, all-reduce size of `8` bytes for the barrier and reduction time `10ns` among `4x4x4=64` processors and store it into `graph.textproto` ParaGraph textproto format.
``
./bazel-bin/stencil -v 1 -n pg_27p_stencil -- 4 4 4 2 0 1e-6 800 80 8 8 1e-8 graph.textproto
``
Use this command t see generator's flags: 
``
./bazel-bin/stencil --help
```
