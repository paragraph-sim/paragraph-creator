#!/bin/bash

set -e

NUM_PROC=32
LOOP_COUNT=2
COMP_DELAY=1e-06
REDUCTION_SIZE=1000000
FORMAT_SIZE=2

# Generates the allreduce
./allreduce \
    --num_proc ${NUM_PROC} \
    --loop_count ${LOOP_COUNT} \
    --time_delay ${COMP_DELAY} \
    --reduction_size ${REDUCTION_SIZE} \
    --format_size ${FORMAT_SIZE} \
    --output_graph allreduce.textproto

# Check that the output file has the right number of nodes
test -f allreduce.textproto
ACT=$(grep group_ids allreduce.textproto | tr -s ' ' | sort | uniq | wc -l | xargs)
if [ "${ACT}" != "${NUM_PROC}" ]; then
    echo "Instead of ${NUM_PROC} node found, there were ${ACT}"
    exit -1
fi

# Check that the output file has the right loop count
BODY_COUNT=$(grep execution_count allreduce.textproto | tr -s ' ' | xargs | awk '{print $4}' | xargs)
if [ "${BODY_COUNT}" != "${LOOP_COUNT}" ]; then
    echo "Instead of ${LOOP_COUNT} iterations while body has ${BODY_COUNT}"
    exit -1
fi
COND_COUNT=$(grep execution_count allreduce.textproto | tr -s ' ' | xargs | awk '{print $8}' | xargs)
if [ "${COND_COUNT}" != "${LOOP_COUNT}" ]; then
    echo "Instead of ${LOOP_COUNT} iterations while conditional has ${COND_COUNT}"
    exit -1
fi

# Check that the output file has the right time delay
ACT=$(grep seconds allreduce.textproto | tr -s ' ' | awk '{print $2}' | xargs)
if [ "${ACT}" != "${COMP_DELAY}" ]; then
    echo "Instead of ${COMP_DELAY} delay found, there were ${ACT}"
    exit -1
fi

# Check that the output file has the right reduction size
ACT=$(grep bytes_out allreduce.textproto | tr -s ' ' | uniq | xargs | awk '{print $2}')
if [ "${ACT}" != "${REDUCTION_SIZE}" ]; then
    echo "Instead of ${REDUCTION_SIZE} reduction size found, there was ${ACT}"
    exit -1
fi

# Check that the output file has the right format size
ACT=$(grep ops allreduce.textproto |  tr -s ' ' | uniq | xargs | awk '{print $2}')
ACT=$((${REDUCTION_SIZE} / ${ACT}))
if [ "${ACT}" != "${FORMAT_SIZE}" ]; then
    echo "Instead of ${FORMAT_SIZE} format size found, there were ${ACT}"
    exit -1
fi

# Individualizes the allreduce
external/paragraph/paragraph/translation/graph_translator \
    --input_graph allreduce.textproto \
    --translation_config test/unidir_ring_push_translation.json\
    --output_dir . \
    --check_consecutive_natural_processor_ids

# Runs each output through the paragraph simulator
for (( proc = 0; proc < ${NUM_PROC}; proc++ )) ; do
    # Runs the simulator
    external/paragraph/paragraph/simulator/simulator \
    --skip_zeros \
    --input_graph allreduce.${proc}.textproto \
    --log_file allreduce.${proc}.log
done

