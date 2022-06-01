# AllReduce generator
This ParaGraph implements AllReduce generator. It runs All-Reduce in the loop for `loop_count` iterations. Within each loop, there is a single delay instruction with delay `time_delay` followed by All-Reduce of size `reduction_size`. Two instructions can be overlapped (executed concurrently) based on `comm_overlap` flag. Computation time for All-Reduce is computed based on reduction size and numerical data format size `format_size`.


> Written with [StackEdit](https://bazel.build/install "Bazel Install"). You need bazel-3.7.2.
To build the bridge without linter check, you can use 
``
bazel build allreduce
``

## How to use
You can use this command to generate `all-reduce` among `8` processors and store it into `graph.textproto` ParaGraph textproto format. `all-reduce` parameters correspond to reduction of `1,000,000` bytes in `4`-byte format with non reduction-related delay of `1ms` in a loop with `3` iterations .
``
./bazel-bin/allreduce --num_proc 8 --output_graph graph.textproto --reduction_size 1e6 --format_size 4 --time_delay 1e-6 --loop_count 3
``
Use this command t see generator's flags: 
``
./bazel-bin/allreduce --helpfull
```
