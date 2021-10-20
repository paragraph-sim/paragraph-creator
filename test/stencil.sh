#!/bin/bash

set -e

X_NODES=3
Y_NODES=5
Z_NODES=4
ITERATIONS=10
LOOP_COND_SECS=1e-6
COMP_SECS=1e-3
FACE_MSG_SIZE=800
EDGE_MSG_SIZE=80
CORNER_MSG_SIZE=8
ALLREDUCE_SIZE=16
REDUCTION_SECS=1e-7

# Generates the stencil
./stencil \
    ${X_NODES} \
    ${Y_NODES} \
    ${Z_NODES} \
    ${ITERATIONS} \
    ${LOOP_COND_SECS} \
    ${COMP_SECS} \
    ${FACE_MSG_SIZE} \
    ${EDGE_MSG_SIZE} \
    ${CORNER_MSG_SIZE} \
    ${ALLREDUCE_SIZE} \
    ${REDUCTION_SECS} \
    some_cool_stencil.textproto \
    -v 2

# Check that the output file has the right number of nodes
test -f some_cool_stencil.textproto
let EXP=${X_NODES}*${Y_NODES}*${Z_NODES}
ACT=$(grep group_ids some_cool_stencil.textproto | tr -s ' ' | sort | uniq | wc -l | xargs)
if [ "${ACT}" != "${EXP}" ]; then
    echo "Instead of ${EXP} node found, there were ${ACT}"
    exit -1
fi

# Individualizes the stencil
external/paragraph/paragraph/translation/graph_translator \
    --input_graph some_cool_stencil.textproto \
    --translation_config test/unidir_ring_push_translation.json \
    --output_dir . \
    --check_consecutive_natural_processor_ids

# Runs each output through the paragraph simulator
for (( proc = 0; proc < ${EXP}; proc++ )) ; do
    # Runs the simulator
    external/paragraph/paragraph/simulator/simulator \
    --skip_zeros \
    --input_graph some_cool_stencil.${proc}.textproto \
    --log_file some_cool_stencil.${proc}.log
done

